//
// Copyright (C) 2024 EA group inc.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//


// A server to receive EchoRequest and send back EchoResponse asynchronously.

#include <turbo/flags/flag.h>
#include <turbo/log/logging.h>
#include <melon/rpc/server.h>
#include <halamelon/proto/echo.pb.h>
#include <turbo/bootstrap/servlet.h>

TURBO_FLAG(bool, echo_attachment, true, "Echo attachment as well");
TURBO_FLAG(int32_t, port, 8002, "TCP Port of this server");
TURBO_FLAG(int32_t, idle_timeout_s, -1, "Connection will be closed if there is no "
                                        "read/write operations during the last `idle_timeout_s'");
TURBO_FLAG(int32_t, max_concurrency, 0, "Limit of request processing in parallel");

mutil::atomic<int> nsd(0);

struct MySessionLocalData {
    MySessionLocalData() : x(123) {
        nsd.fetch_add(1, mutil::memory_order_relaxed);
    }

    ~MySessionLocalData() {
        nsd.fetch_sub(1, mutil::memory_order_relaxed);
    }

    int x;
};

class MySessionLocalDataFactory : public melon::DataFactory {
public:
    void *CreateData() const {
        return new MySessionLocalData;
    }

    void DestroyData(void *d) const {
        delete static_cast<MySessionLocalData *>(d);
    }
};

mutil::atomic<int> ntls(0);

struct MyThreadLocalData {
    MyThreadLocalData() : y(0) {
        ntls.fetch_add(1, mutil::memory_order_relaxed);
    }

    ~MyThreadLocalData() {
        ntls.fetch_sub(1, mutil::memory_order_relaxed);
    }

    static void deleter(void *d) {
        delete static_cast<MyThreadLocalData *>(d);
    }

    int y;
};

class MyThreadLocalDataFactory : public melon::DataFactory {
public:
    void *CreateData() const {
        return new MyThreadLocalData;
    }

    void DestroyData(void *d) const {
        MyThreadLocalData::deleter(d);
    }
};

struct AsyncJob {
    MySessionLocalData *expected_session_local_data;
    int expected_session_value;
    melon::Controller *cntl;
    const example::EchoRequest *request;
    example::EchoResponse *response;
    google::protobuf::Closure *done;

    void run();

    void run_and_delete() {
        run();
        delete this;
    }
};

static void *process_thread(void *args) {
    AsyncJob *job = static_cast<AsyncJob *>(args);
    job->run_and_delete();
    return NULL;
}

// Your implementation of example::EchoService
class EchoServiceWithThreadAndSessionLocal : public example::EchoService {
public:
    EchoServiceWithThreadAndSessionLocal() {
        CHECK_EQ(0, fiber_key_create(&_tls2_key, MyThreadLocalData::deleter));
    }

    ~EchoServiceWithThreadAndSessionLocal() {
        CHECK_EQ(0, fiber_key_delete(_tls2_key));
    };

    void Echo(google::protobuf::RpcController *cntl_base,
              const example::EchoRequest *request,
              example::EchoResponse *response,
              google::protobuf::Closure *done) {
        melon::ClosureGuard done_guard(done);
        melon::Controller *cntl =
                static_cast<melon::Controller *>(cntl_base);

        // Get the session-local data which is created by ServerOptions.session_local_data_factory
        // and reused between different RPC. All session-local data are
        // destroyed upon server destruction.
        MySessionLocalData *sd = static_cast<MySessionLocalData *>(cntl->session_local_data());
        if (sd == NULL) {
            cntl->SetFailed("Require ServerOptions.session_local_data_factory to be"
                            " set with a correctly implemented instance");
            LOG(ERROR) << cntl->ErrorText();
            return;
        }
        const int expected_value = sd->x + (((uintptr_t) cntl) & 0xFFFFFFFF);
        sd->x = expected_value;

        // Get the thread-local data which is created by ServerOptions.thread_local_data_factory
        // and reused between different threads. All thread-local data are 
        // destroyed upon server destruction.
        // "tls" is short for "thread local storage".
        MyThreadLocalData *tls =
                static_cast<MyThreadLocalData *>(melon::thread_local_data());
        if (tls == NULL) {
            cntl->SetFailed("Require ServerOptions.thread_local_data_factory "
                            "to be set with a correctly implemented instance");
            LOG(ERROR) << cntl->ErrorText();
            return;
        }
        tls->y = expected_value;

        // You can create fiber-local data for your own.
        // The interfaces are similar with pthread equivalence:
        //   pthread_key_create  -> fiber_key_create
        //   pthread_key_delete  -> fiber_key_delete
        //   pthread_getspecific -> fiber_getspecific
        //   pthread_setspecific -> fiber_setspecific
        MyThreadLocalData *tls2 =
                static_cast<MyThreadLocalData *>(fiber_getspecific(_tls2_key));
        if (tls2 == NULL) {
            tls2 = new MyThreadLocalData;
            CHECK_EQ(0, fiber_setspecific(_tls2_key, tls2));
        }
        tls2->y = expected_value + 1;

        // sleep awhile to force context switching.
        fiber_usleep(10000);

        // tls is unchanged after context switching.
        CHECK_EQ(tls, melon::thread_local_data());
        CHECK_EQ(expected_value, tls->y);

        CHECK_EQ(tls2, fiber_getspecific(_tls2_key));
        CHECK_EQ(expected_value + 1, tls2->y);

        // Process the request asynchronously.
        AsyncJob *job = new AsyncJob;
        job->expected_session_local_data = sd;
        job->expected_session_value = expected_value;
        job->cntl = cntl;
        job->request = request;
        job->response = response;
        job->done = done;
        fiber_t th;
        CHECK_EQ(0, fiber_start_background(&th, NULL, process_thread, job));

        // We don't want to call done->Run() here, release the guard.
        done_guard.release();

        LOG_EVERY_N_SEC(INFO, 1) << "ntls=" << ntls.load(mutil::memory_order_relaxed)
                                 << " nsd=" << nsd.load(mutil::memory_order_relaxed);
    }

private:
    fiber_key_t _tls2_key;
};

void AsyncJob::run() {
    melon::ClosureGuard done_guard(done);

    // Sleep some time to make sure that Echo() exits.
    fiber_usleep(10000);

    // Still the session-local data that we saw in Echo().
    // This is the major difference between session-local data and thread-local
    // data which was already destroyed upon Echo() exit.
    MySessionLocalData *sd = static_cast<MySessionLocalData *>(cntl->session_local_data());
    CHECK_EQ(expected_session_local_data, sd);
    CHECK_EQ(expected_session_value, sd->x);

    // Echo request and its attachment
    response->set_message(request->message());
    if (turbo::get_flag(FLAGS_echo_attachment)) {
        cntl->response_attachment().append(cntl->request_attachment());
    }
}

int main(int argc, char *argv[]) {
    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option_function<bool>("--echo_attachment", [](bool v) {
        turbo::set_flag(&FLAGS_echo_attachment, v);
    }, "Echo attachment as well");
    app->add_option("--port", FLAGS_port, "TCP Port of this server");
    app->add_option("--idle_timeout_s", FLAGS_idle_timeout_s, "Connection will be closed if there is no "
                                                              "read/write operations during the last `idle_timeout_s'");
    app->add_option("--max_concurrency", FLAGS_max_concurrency, "Limit of request processing in parallel");
    auto [ext, code] = servlet.run(argc, argv);
    if (ext) {
        return code;
    }

    // The factory to create MySessionLocalData. Must be valid when server is running.
    MySessionLocalDataFactory session_local_data_factory;

    MyThreadLocalDataFactory thread_local_data_factory;

    // Generally you only need one Server.
    melon::Server server;
    // For more options see `melon/rpc/server.h'.
    melon::ServerOptions options;
    options.idle_timeout_sec = turbo::get_flag(FLAGS_idle_timeout_s);
    options.max_concurrency = turbo::get_flag(FLAGS_max_concurrency);
    options.session_local_data_factory = &session_local_data_factory;
    options.thread_local_data_factory = &thread_local_data_factory;

    // Instance of your service.
    EchoServiceWithThreadAndSessionLocal echo_service_impl;

    // Add the service into server. Notice the second parameter, because the
    // service is put on stack, we don't want server to delete it, otherwise
    // use melon::SERVER_OWNS_SERVICE.
    if (server.AddService(&echo_service_impl,
                          melon::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add service";
        return -1;
    }

    // Start the server. 
    if (server.Start(turbo::get_flag(FLAGS_port), &options) != 0) {
        LOG(ERROR) << "Fail to start EchoServer";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();
    CHECK_EQ(ntls, 0);
    CHECK_EQ(nsd, 0);
    return 0;
}
