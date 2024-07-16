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

#include <turbo/flags/flag.h>
#include <turbo/log/logging.h>
#include <turbo/strings/str_format.h>
#include <melon/rpc/server.h>
#include <halamelon/proto/value_echo.pb.h>
#include <turbo/bootstrap/servlet.h>

TURBO_FLAG(bool, echo_attachment, true, "Echo attachment as well");
TURBO_FLAG(int32_t, port, 8002, "TCP Port of this server");
TURBO_FLAG(int32_t, idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");
TURBO_FLAG(int32_t, max_concurrency, 0, "Limit of request processing in parallel");

// Your implementation of example::EchoService
class EchoServiceImpl : public example::ValueEchoService {
public:
    EchoServiceImpl() {}
    ~EchoServiceImpl() {}
    void Echo(google::protobuf::RpcController* cntl_base,
              const example::ValueEchoRequest* request,
              example::ValueEchoResponse* response,
              google::protobuf::Closure* done) {
        melon::ClosureGuard done_guard(done);
        melon::Controller* cntl =
            static_cast<melon::Controller*>(cntl_base);

        // Echo request and its attachment
        response->set_value(request->value());
        if (turbo::get_flag(FLAGS_echo_attachment)) {
            cntl->response_attachment().append(cntl->request_attachment());
        }
    }
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
    app->add_option("-c,--max_concurrency", FLAGS_max_concurrency, "Limit of request processing in parallel");
    auto [ext, code] = servlet.run(argc, argv);
    if (ext) {
        return code;
    }
    // Generally you only need one Server.
    melon::Server server;

    // Instance of your service.
    EchoServiceImpl echo_service_impl;

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
    options.max_concurrency = turbo::get_flag(FLAGS_max_concurrency);
    if (server.Start(turbo::get_flag(FLAGS_port), &options) != 0) {
        LOG(ERROR) << "Fail to start EchoServer";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();
    return 0;
}
