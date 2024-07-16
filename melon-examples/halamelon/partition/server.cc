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

#include <vector>
#include <turbo/flags/flag.h>
#include <melon/utility/time.h>
#include <turbo/log/logging.h>
#include <melon/utility/string_printf.h>
#include <melon/utility/string_splitter.h>
#include <melon/utility/rand_util.h>
#include <melon/rpc/server.h>
#include <turbo/bootstrap/servlet.h>
#include <halamelon/proto/echo.pb.h>

TURBO_FLAG(bool, echo_attachment, true, "Echo attachment as well");
TURBO_FLAG(int32_t, port, 8002, "TCP Port of this server");
TURBO_FLAG(int32_t, idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");
TURBO_FLAG(int32_t, max_concurrency, 0, "Limit of request processing in parallel");
TURBO_FLAG(int32_t, server_num, 1, "Number of servers");
TURBO_FLAG(std::string, sleep_us, "", "Sleep so many microseconds before responding");
TURBO_FLAG(bool, spin, false, "spin rather than sleep");
TURBO_FLAG(double, exception_ratio, 0.1, "Percentage of irregular latencies");
TURBO_FLAG(double, min_ratio, 0.2, "min_sleep / sleep_us");
TURBO_FLAG(double, max_ratio, 10, "max_sleep / sleep_us");

// Your implementation of example::EchoService
class EchoServiceImpl : public example::EchoService {
public:
    EchoServiceImpl() : _index(0) {}
    virtual ~EchoServiceImpl() {}
    void set_index(size_t index, int64_t sleep_us) { 
        _index = index; 
        _sleep_us = sleep_us;
    }
    virtual void Echo(google::protobuf::RpcController* cntl_base,
                      const example::EchoRequest* request,
                      example::EchoResponse* response,
                      google::protobuf::Closure* done) {
        melon::ClosureGuard done_guard(done);
        melon::Controller* cntl =
            static_cast<melon::Controller*>(cntl_base);
        if (_sleep_us > 0) {
            double delay = _sleep_us;
            const double a = turbo::get_flag(FLAGS_exception_ratio) * 0.5;
            if (a >= 0.0001) {
                double x = mutil::RandDouble();
                if (x < a) {
                    const double min_sleep_us = turbo::get_flag(FLAGS_min_ratio) * _sleep_us;
                    delay = min_sleep_us + (_sleep_us - min_sleep_us) * x / a;
                } else if (x + a > 1) {
                    const double max_sleep_us = turbo::get_flag(FLAGS_max_ratio) * _sleep_us;
                    delay = _sleep_us + (max_sleep_us - _sleep_us) * (x + a - 1) / a;
                }
            }
            if (turbo::get_flag(FLAGS_spin)) {
                int64_t end_time = mutil::gettimeofday_us() + (int64_t)delay;
                while (mutil::gettimeofday_us() < end_time) {}
            } else {
                fiber_usleep((int64_t)delay);
            }
        }

        // Echo request and its attachment
        response->set_message(request->message());
        if (turbo::get_flag(FLAGS_echo_attachment)) {
            cntl->response_attachment().append(cntl->request_attachment());
        }
        _nreq << 1;
    }

    size_t num_requests() const { return _nreq.get_value(); }

private:
    size_t _index;
    int64_t _sleep_us;
    melon::var::Adder<size_t> _nreq;
};

int main(int argc, char* argv[]) {
    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option_function<bool>("--echo_attachment", [](bool v) {
        turbo::set_flag(&FLAGS_echo_attachment, v);
    }, "Echo attachment as well");
    app->add_option("-p,--port", FLAGS_port, "TCP Port of this server");
    app->add_option("-i,--idle_timeout_s", FLAGS_idle_timeout_s, "Connection will be closed if there is no "
                     "read/write operations during the last `idle_timeout_s'");
    app->add_option("-n,--server_num", FLAGS_server_num, "Number of servers");
    app->add_option("-s,--sleep_us", FLAGS_sleep_us, "Sleep so many microseconds before responding");
    app->add_option("-S,--spin", FLAGS_spin, "spin rather than sleep");
    app->add_option("-e,--exception_ratio", FLAGS_exception_ratio, "Percentage of irregular latencies");
    app->add_option("-m,--min_ratio", FLAGS_min_ratio, "min_sleep / sleep_us");
    app->add_option("-M,--max_ratio", FLAGS_max_ratio, "max_sleep / sleep_us");
    auto [ext, code] = servlet.run(argc, argv);
    if (ext) {
        return code;
    }
    if (turbo::get_flag(FLAGS_server_num) <= 0) {
        LOG(ERROR) << "server_num must be positive";
        return -1;
    }

    // We need multiple servers in this example.
    melon::Server* servers = new melon::Server[turbo::get_flag(FLAGS_server_num)];
    // For more options see `melon/rpc/server.h'.
    melon::ServerOptions options;
    options.idle_timeout_sec = turbo::get_flag(FLAGS_idle_timeout_s);
    options.max_concurrency = turbo::get_flag(FLAGS_max_concurrency);

    mutil::StringSplitter sp(turbo::get_flag(FLAGS_sleep_us).c_str(), ',');
    std::vector<int64_t> sleep_list;
    for (; sp; ++sp) {
        sleep_list.push_back(strtoll(sp.field(), NULL, 10));
    }
    if (sleep_list.empty()) {
        sleep_list.push_back(0);
    }

    // Instance of your services.
    EchoServiceImpl* echo_service_impls = new EchoServiceImpl[turbo::get_flag(FLAGS_server_num)];
    // Add the service into servers. Notice the second parameter, because the
    // service is put on stack, we don't want server to delete it, otherwise
    // use melon::SERVER_OWNS_SERVICE.
    for (int i = 0; i <turbo::get_flag( FLAGS_server_num); ++i) {
        int64_t sleep_us = sleep_list[(size_t)i < sleep_list.size() ? i : (sleep_list.size() - 1)];
        echo_service_impls[i].set_index(i, sleep_us);
        // will be shown on /version page
        servers[i].set_version(mutil::string_printf(
                    "example/dynamic_partition_echo_c++[%d]", i));
        if (servers[i].AddService(&echo_service_impls[i], 
                                  melon::SERVER_DOESNT_OWN_SERVICE) != 0) {
            LOG(ERROR) << "Fail to add service";
            return -1;
        }
        // Start the server.
        int port = turbo::get_flag(FLAGS_port) + i;
        if (servers[i].Start(port, &options) != 0) {
            LOG(ERROR) << "Fail to start EchoServer";
            return -1;
        }
    }

    // Service logic are running in separate worker threads, for main thread,
    // we don't have much to do, just spinning.
    std::vector<size_t> last_num_requests(turbo::get_flag(FLAGS_server_num));
    while (!melon::IsAskedToQuit()) {
        sleep(1);

        size_t cur_total = 0;
        std::stringstream ss;
        for (int i = 0; i < turbo::get_flag(FLAGS_server_num); ++i) {
            const size_t current_num_requests =
                    echo_service_impls[i].num_requests();
            size_t diff = current_num_requests - last_num_requests[i];
            cur_total += diff;
            last_num_requests[i] = current_num_requests;
            ss << "S[" << i << "]=" << diff << ' ';
        }
        LOG(INFO) <<ss.str()<< "[total=" << cur_total << ']';
    }

    // Don't forget to stop and join the server otherwise still-running
    // worker threads may crash your program.
    for (int i = 0; i < turbo::get_flag(FLAGS_server_num); ++i) {
        servers[i].Stop(0/*not used now*/);
    }
    for (int i = 0; i < turbo::get_flag(FLAGS_server_num); ++i) {
        servers[i].Join();
    }
    delete [] servers;
    delete [] echo_service_impls;
    return 0;
}
