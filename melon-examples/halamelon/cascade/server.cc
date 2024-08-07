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


#include <turbo/bootstrap/servlet.h>
#include <turbo/flags/flag.h>
#include <turbo/log/logging.h>
#include <melon/rpc/channel.h>
#include <melon/rpc/server.h>
#include <halamelon/proto/cascade.pb.h>

TURBO_FLAG(int32_t, timeout_ms, 100, "RPC timeout in milliseconds");
TURBO_FLAG(int32_t, max_retry, 3, "Max retries(not including the first RPC)");

TURBO_FLAG(bool, echo_attachment, true, "Cascade attachment as well");
TURBO_FLAG(int32_t, port, 8000, "TCP Port of this server");
TURBO_FLAG(std::string, server, "", "The server to connect, localhost:FLAGS_port as default");
TURBO_FLAG(std::string, load_balancer, "", "The algorithm for load balancing");
TURBO_FLAG(bool, use_http, false, "Use http protocol to transfer messages");
TURBO_FLAG(int32_t, idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");

melon::Channel channel;

// Your implementation of example::CascadeService
namespace example {
class CascadeCascadeService : public CascadeService {
public:
    CascadeCascadeService() {}
    virtual ~CascadeCascadeService() {}
    virtual void Echo(google::protobuf::RpcController* cntl_base,
                      const CascadeRequest* request,
                      CascadeResponse* response,
                      google::protobuf::Closure* done) {
        // This object helps you to call done->Run() in RAII style. If you need
        // to process the request asynchronously, pass done_guard.release().
        melon::ClosureGuard done_guard(done);

        melon::Controller* cntl =
            static_cast<melon::Controller*>(cntl_base);

        if (request->depth() > 0) {
            CLOGI(cntl) << "I'm about to call myself for another time, depth=" << request->depth();
            example::CascadeService_Stub stub(&channel);
            example::CascadeRequest request2;
            example::CascadeResponse response2;
            melon::Controller cntl2(cntl->inheritable());
            request2.set_message(request->message());
            request2.set_depth(request->depth() - 1);

            cntl2.set_timeout_ms(turbo::get_flag(FLAGS_timeout_ms));
            cntl2.set_max_retry(turbo::get_flag(FLAGS_max_retry));
            stub.Echo(&cntl2, &request2, &response2, NULL);
            if (cntl2.Failed()) {
                CLOGE(&cntl2) << "Fail to send CascadeRequest, " << cntl2.ErrorText();
                cntl->SetFailed(cntl2.ErrorCode(), "%s", cntl2.ErrorText().c_str());
                return;
            }
            response->set_message(response2.message());
        } else {
            CLOGI(cntl) << "I'm the last call";
            response->set_message(request->message());
        }
        
        if (turbo::get_flag(FLAGS_echo_attachment) && !turbo::get_flag(FLAGS_use_http)) {
            // Set attachment which is wired to network directly instead of
            // being serialized into protobuf messages.
            cntl->response_attachment().append(cntl->request_attachment());
        }
    }
};
}  // namespace example

int main(int argc, char* argv[]) {
    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option("-t,--timeout_ms", FLAGS_timeout_ms, "RPC timeout in milliseconds");
    app->add_option("-r,--max_retry", FLAGS_max_retry, "Max retries(not including the first RPC)");
    app->add_option_function<bool>("-a,--echo_attachment", [](bool v) { turbo::set_flag(&FLAGS_echo_attachment,v); }, "Echo attachment as well");
    app->add_option("-p,--port", FLAGS_port, "TCP Port of this server");
    app->add_option("-s,--server", FLAGS_server, "IP Address of server");
    app->add_option("-l,--load_balancer", FLAGS_load_balancer, "The algorithm for load balancing");
    app->add_option_function<bool>("-u,--use_http", [](bool v) { turbo::set_flag(&FLAGS_use_http,v); }, "Use http protocol to transfer messages");
    app->add_option("-i,--idle_timeout_s", FLAGS_idle_timeout_s, "Connection will be closed if there is no read/write operations during the last `idle_timeout_s'");
    auto [ext, code] = servlet.run(argc, argv);
    if (ext) {
        return code;
    }
    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    melon::ChannelOptions coption;
    if (turbo::get_flag(FLAGS_use_http)) {
        coption.protocol = melon::PROTOCOL_HTTP;
    }
    
    // Initialize the channel, NULL means using default options. 
    // options, see `melon/rpc/channel.h'.
    if (turbo::get_flag(FLAGS_server).empty()) {
        if (channel.Init("localhost", turbo::get_flag(FLAGS_port), &coption) != 0) {
            LOG(ERROR) << "Fail to initialize channel";
            return -1;
        }
    } else {
        if (channel.Init(turbo::get_flag(FLAGS_server).c_str(), turbo::get_flag(FLAGS_load_balancer).c_str(), &coption) != 0) {
            LOG(ERROR) << "Fail to initialize channel";
            return -1;
        }
    }

    // Generally you only need one Server.
    melon::Server server;
    // For more options see `melon/rpc/server.h'.
    melon::ServerOptions options;
    options.idle_timeout_sec = turbo::get_flag(FLAGS_idle_timeout_s);

    // Instance of your service.
    example::CascadeCascadeService echo_service_impl;

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
        LOG(ERROR) << "Fail to start CascadeServer";
        return -1;
    }
    
    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();
    return 0;
}
