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


// A server sleeping for even-th requests to trigger backup request of client.

#include <turbo/bootstrap/servlet.h>
#include <turbo/flags/flag.h>
#include <turbo/log/logging.h>
#include <melon/rpc/server.h>
#include <halamelon/proto/backup.pb.h>

TURBO_FLAG(bool, echo_attachment, true, "Echo attachment as well");
TURBO_FLAG(int32_t, port, 8000, "TCP Port of this server");
TURBO_FLAG(int32_t, idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");
TURBO_FLAG(int32_t, sleep_ms, 20, "Sleep so many milliseconds on even-th requests");

// Your implementation of example::EchoService
// Notice that implementing melon::Describable grants the ability to put
// additional information in /status.
namespace example {
class SleepyEchoService : public BackupService
                        , public melon::Describable {
public:
    SleepyEchoService() : _count(0) {}
    virtual ~SleepyEchoService() {}
    virtual void Echo(google::protobuf::RpcController* cntl_base,
                      const BackupRequest* request,
                      BackupResponse* response,
                      google::protobuf::Closure* done) {
        // This object helps you to call done->Run() in RAII style. If you need
        // to process the request asynchronously, pass done_guard.release().
        melon::ClosureGuard done_guard(done);

        melon::Controller* cntl =
            static_cast<melon::Controller*>(cntl_base);

        // The purpose of following logs is to help you to understand
        // how clients interact with servers more intuitively. You should 
        // remove these logs in performance-sensitive servers.
        // The noflush prevents the log from being flushed immediately.
        std::stringstream ss;
        ss << "Received request[index=" << request->index()
                  << "] from " << cntl->remote_side() 
                  << " to " << cntl->local_side();
        // Sleep a while for 0th, 2nd, 4th, 6th ... requests to trigger backup request
        // at client-side.
        bool do_sleep = (_count.fetch_add(1, mutil::memory_order_relaxed) % 2 == 0);
        if (do_sleep) {
            ss << ", sleep " << turbo::get_flag(FLAGS_sleep_ms)
                      << " ms to trigger backup request";
        }
        LOG(INFO)<<ss.str();

        // Fill response.
        response->set_index(request->index());

        if (do_sleep) {
            fiber_usleep(turbo::get_flag(FLAGS_sleep_ms) * 1000);
        }
    }
private:
    mutil::atomic<int> _count;
};
}  // namespace example

int main(int argc, char* argv[]) {
    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option_function<bool>("-a,--echo_attachment", [](bool v) { turbo::set_flag(&FLAGS_echo_attachment,v); }, "Echo attachment as well");
    app->add_option("-p,--port", FLAGS_port, "TCP Port of this server");
    app->add_option("-i,--idle_timeout_s", FLAGS_idle_timeout_s, "Connection will be closed if there is no read/write operations during the last `idle_timeout_s'");
    app->add_option("-s,--sleep_ms", FLAGS_sleep_ms, "Sleep so many milliseconds on even-th requests");
    auto [ext, code] = servlet.run(argc, argv);
    if(ext) {
        return code;
    }
    // Generally you only need one Server.
    melon::Server server;

    // Instance of your service.
    example::SleepyEchoService echo_service_impl;

    // Add the service into server. Notice the second parameter, because the
    // service is put on stack, we don't want server to delete it, otherwise
    // use melon::SERVER_OWNS_SERVICE.
    if (server.AddService(&echo_service_impl, 
                          melon::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add service";
        return -1;
    }

    // Start the server.
    melon::ServerOptions options;
    options.idle_timeout_sec = turbo::get_flag(FLAGS_idle_timeout_s);
    if (server.Start(turbo::get_flag(FLAGS_port), &options) != 0) {
        LOG(ERROR) << "Fail to start EchoServer";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();
    return 0;
}
