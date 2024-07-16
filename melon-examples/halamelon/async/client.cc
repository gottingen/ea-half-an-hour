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


// A client sending requests to server asynchronously every 1 second.

#include <turbo/flags/flag.h>
#include <turbo/log/logging.h>
#include <melon/utility/time.h>
#include <melon/rpc/channel.h>
#include <halamelon/proto/echo.pb.h>
#include <turbo/bootstrap/servlet.h>

TURBO_FLAG(bool, send_attachment, true, "Carry attachment along with requests");
TURBO_FLAG(std::string, protocol, "melon_std", "Protocol type. Defined in melon/rpc/options.proto");
TURBO_FLAG(std::string, connection_type, "", "Connection type. Available values: single, pooled, short");
TURBO_FLAG(std::string, server, "0.0.0.0:8003", "IP Address of server");
TURBO_FLAG(std::string, load_balancer, "", "The algorithm for load balancing");
TURBO_FLAG(int32_t, timeout_ms, 100, "RPC timeout in milliseconds");
TURBO_FLAG(int32_t, max_retry, 3, "Max retries(not including the first RPC)");

void HandleEchoResponse(
        melon::Controller* cntl,
        example::EchoResponse* response) {
    // std::unique_ptr makes sure cntl/response will be deleted before returning.
    std::unique_ptr<melon::Controller> cntl_guard(cntl);
    std::unique_ptr<example::EchoResponse> response_guard(response);

    if (cntl->Failed()) {
        LOG(WARNING) << "Fail to send EchoRequest, " << cntl->ErrorText();
        return;
    }
    LOG(INFO) << "Received response from " << cntl->remote_side()
        << ": " << response->message() << " (attached="
        << cntl->response_attachment() << ")"
        << " latency=" << cntl->latency_us() << "us";
}
                        

int main(int argc, char* argv[]) {

    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option_function<bool>("-a,--attachment", [](bool v) { turbo::set_flag(&FLAGS_send_attachment,v); }, "Carry this along with requests");
    app->add_option("-p,--protocol", FLAGS_protocol, "Protocol type. Defined in melon/rpc/options.proto");
    app->add_option("-c,--connection_type", FLAGS_connection_type, "Connection type. Available values: single, pooled, short");
    app->add_option("-s,--server", FLAGS_server, "IP Address of server");
    app->add_option("-l,--load_balancer", FLAGS_load_balancer, "The algorithm for load balancing");
    app->add_option("-t,--timeout_ms", FLAGS_timeout_ms, "RPC timeout in milliseconds");
    app->add_option("-m,--max_retry", FLAGS_max_retry, "Max retries(not including the first RPC)");
    auto [ext, code] = servlet.run(argc, argv);
    if (ext) {
        return code;
    }
    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    melon::Channel channel;

    // Initialize the channel, NULL means using default options.
    melon::ChannelOptions options;
    options.protocol = turbo::get_flag(FLAGS_protocol);
    options.connection_type = turbo::get_flag(FLAGS_connection_type);
    options.timeout_ms = turbo::get_flag(FLAGS_timeout_ms)/*milliseconds*/;
    options.max_retry = turbo::get_flag(FLAGS_max_retry);
    if (channel.Init(turbo::get_flag(FLAGS_server).c_str(), turbo::get_flag(FLAGS_load_balancer).c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    example::EchoService_Stub stub(&channel);
    
    // Send a request and wait for the response every 1 second.
    int log_id = 0;
    while (!melon::IsAskedToQuit()) {
        // Since we are sending asynchronous RPC (`done' is not NULL),
        // these objects MUST remain valid until `done' is called.
        // As a result, we allocate these objects on heap
        example::EchoResponse* response = new example::EchoResponse();
        melon::Controller* cntl = new melon::Controller();

        // Notice that you don't have to new request, which can be modified
        // or destroyed just after stub.Echo is called.
        example::EchoRequest request;
        request.set_message("hello world");

        cntl->set_log_id(log_id ++);  // set by user
        if (turbo::get_flag(FLAGS_send_attachment)) {
            // Set attachment which is wired to network directly instead of 
            // being serialized into protobuf messages.
            cntl->request_attachment().append("foo");
        }

        // We use protobuf utility `NewCallback' to create a closure object
        // that will call our callback `HandleEchoResponse'. This closure
        // will automatically delete itself after being called once
        google::protobuf::Closure* done = melon::NewCallback(
            &HandleEchoResponse, cntl, response);
        stub.Echo(cntl, &request, response, done);

        // This is an asynchronous RPC, so we can only fetch the result
        // inside the callback
        sleep(1);
    }

    LOG(INFO) << "EchoClient is going to quit";
    return 0;
}
