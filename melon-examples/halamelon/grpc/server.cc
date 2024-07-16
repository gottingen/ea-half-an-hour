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


// A server to receive HelloRequest and send back HelloReply

#include <turbo/flags/flag.h>
#include <turbo/log/logging.h>
#include <melon/rpc/server.h>
#include <melon/rpc/restful.h>
#include <halamelon/proto/helloworld.pb.h>
#include <turbo/bootstrap/servlet.h>

TURBO_FLAG(int32_t, port, 50051, "TCP Port of this server");
TURBO_FLAG(int32_t, idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");
TURBO_FLAG(bool, gzip, false, "compress body using gzip");
TURBO_DECLARE_FLAG(bool, http_body_compress_threshold);

class GreeterImpl : public helloworld::Greeter {
public:
    GreeterImpl() {}
    virtual ~GreeterImpl() {}
    void SayHello(google::protobuf::RpcController* cntl_base,
                 const helloworld::HelloRequest* req,
                 helloworld::HelloReply* res,
                 google::protobuf::Closure* done) {
        melon::ClosureGuard done_guard(done);
        melon::Controller* cntl = static_cast<melon::Controller*>(cntl_base);
        if (turbo::get_flag(FLAGS_gzip)) {
            cntl->set_response_compress_type(melon::COMPRESS_TYPE_GZIP);
        }
        res->set_message("Hello " + req->name());
    }
};

int main(int argc, char* argv[]) {

    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option("--port", FLAGS_port, "TCP Port of this server");
    app->add_option("--idle_timeout_s", FLAGS_idle_timeout_s, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");
    app->add_option_function<bool>("--gzip",[](bool v){turbo::set_flag(&FLAGS_gzip, v);}, "compress body using gzip");
    if(turbo::get_flag(FLAGS_gzip)) {
        turbo::set_flag(&FLAGS_http_body_compress_threshold, true);
    }
    auto [ext, code] = servlet.run(argc, argv);
    if (ext) {
        return code;
    }
    // Generally you only need one Server.
    melon::Server server;

    GreeterImpl http_svc;

    // Add services into server. Notice the second parameter, because the
    // service is put on stack, we don't want server to delete it, otherwise
    // use melon::SERVER_OWNS_SERVICE.
    if (server.AddService(&http_svc,
                          melon::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add http_svc";
        return -1;
    }

    // Start the server.
    melon::ServerOptions options;
    options.idle_timeout_sec = turbo::get_flag(FLAGS_idle_timeout_s);
    if (server.Start(turbo::get_flag(FLAGS_port), &options) != 0) {
        LOG(ERROR) << "Fail to start HttpServer";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();
    return 0;
}
