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


// A server to receive EchoRequest and send back EchoResponse.

#include <turbo/flags/flag.h>
#include <turbo/log/logging.h>
#include <melon/rpc/server.h>
#include <melon/json2pb/pb_to_json.h>
#include <halamelon/proto/echo.pb.h>
#include <turbo/bootstrap/servlet.h>

TURBO_FLAG(bool, send_attachment, true, "Carry attachment along with response");
TURBO_FLAG(int32_t, port, 8003, "TCP Port of this server");
TURBO_FLAG(int32_t, idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");

// Your implementation of example::EchoService
class EchoServiceImpl : public example::EchoService {
public:
    EchoServiceImpl() {}
    virtual ~EchoServiceImpl() {}
    virtual void Echo(google::protobuf::RpcController* cntl_base,
                      const example::EchoRequest* request,
                      example::EchoResponse* response,
                      google::protobuf::Closure* done) {
        // This object helps you to call done->Run() in RAII style. If you need
        // to process the request asynchronously, pass done_guard.release().
        melon::ClosureGuard done_guard(done);

        melon::Controller* cntl =
            static_cast<melon::Controller*>(cntl_base);

        // optional: set a callback function which is called after response is sent
        // and before cntl/req/res is destructed.
        cntl->set_after_rpc_resp_fn(std::bind(&EchoServiceImpl::CallAfterRpc,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        // The purpose of following logs is to help you to understand
        // how clients interact with servers more intuitively. You should 
        // remove these logs in performance-sensitive servers.
        LOG(INFO) << "Received request[log_id=" << cntl->log_id()
                  << "] from " << cntl->remote_side()
                  << ": " << request->message()
                  << " (attached=" << cntl->request_attachment() << ")";

        // Fill response.
        response->set_message(request->message());

        // You can compress the response by setting Controller, but be aware
        // that compression may be costly, evaluate before turning on.
        // cntl->set_response_compress_type(melon::COMPRESS_TYPE_GZIP);

        if (turbo::get_flag(FLAGS_send_attachment)) {
            // Set attachment which is wired to network directly instead of
            // being serialized into protobuf messages.
            cntl->response_attachment().append("bar");
        }
    }

    // optional
    static void CallAfterRpc(melon::Controller* cntl,
                        const google::protobuf::Message* req,
                        const google::protobuf::Message* res) {
        // at this time res is already sent to client, but cntl/req/res is not destructed
        std::string req_str;
        std::string res_str;
        json2pb::ProtoMessageToJson(*req, &req_str, NULL);
        json2pb::ProtoMessageToJson(*res, &res_str, NULL);
        LOG(INFO) << "req:" << req_str
                    << " res:" << res_str;
    }
};

int main(int argc, char* argv[]) {
    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option_function<bool>("-a,--attachment", [](bool v) { turbo::set_flag(&FLAGS_send_attachment,v); }, "Carry this along with requests");
    app->add_option("-p,--port", FLAGS_port, "TCP Port of this server");
    app->add_option("-i,--idle_timeout_s", FLAGS_idle_timeout_s, "Connection will be closed if there is no read/write operations during the last `idle_timeout_s'");
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
    if (server.Start(turbo::get_flag(FLAGS_port), &options) != 0) {
        LOG(ERROR) << "Fail to start EchoServer";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();
    return 0;
}
