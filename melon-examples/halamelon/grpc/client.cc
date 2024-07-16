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


// A client sending requests to server every 1 second using grpc.

#include <turbo/flags/flag.h>
#include <turbo/log/logging.h>
#include <melon/utility/time.h>
#include <melon/rpc/channel.h>
#include <halamelon/proto/helloworld.pb.h>
#include <turbo/bootstrap/servlet.h>

TURBO_FLAG(std::string, protocol, "h2:grpc", "Protocol type. Defined in melon/rpc/options.proto");
TURBO_FLAG(std::string, server, "0.0.0.0:50051", "IP Address of server");
TURBO_FLAG(std::string, load_balancer, "", "The algorithm for load balancing");
TURBO_FLAG(int32_t, timeout_ms, 100, "RPC timeout in milliseconds");
TURBO_FLAG(int32_t, max_retry, 3, "Max retries(not including the first RPC)");
TURBO_FLAG(int32_t, interval_ms, 1000, "Milliseconds between consecutive requests");
TURBO_FLAG(bool, gzip, false, "compress body using gzip");
TURBO_DECLARE_FLAG(bool, http_body_compress_threshold);

int main(int argc, char* argv[]) {
    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option("--protocol", FLAGS_protocol, "Protocol type. Defined in melon/rpc/options.proto");
    app->add_option("--server", FLAGS_server, "IP Address of server");
    app->add_option("--load_balancer", FLAGS_load_balancer, "The algorithm for load balancing");
    app->add_option("--timeout_ms", FLAGS_timeout_ms, "RPC timeout in milliseconds");
    app->add_option("--max_retry", FLAGS_max_retry, "Max retries(not including the first RPC)");
    app->add_option("--interval_ms", FLAGS_interval_ms, "Milliseconds between consecutive requests");
    app->add_option_function<bool>("--gzip",[](bool v){turbo::set_flag(&FLAGS_gzip, v);}, "compress body using gzip");
    auto [ext, port] = servlet.run(argc, argv);
    if (ext) {
        return port;
    }
    if(turbo::get_flag(FLAGS_gzip)) {
        turbo::set_flag(&FLAGS_http_body_compress_threshold, true);
    }

    
    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    melon::Channel channel;
    
    // Initialize the channel, NULL means using default options.
    melon::ChannelOptions options;
    options.protocol = turbo::get_flag(FLAGS_protocol);
    options.timeout_ms = turbo::get_flag(FLAGS_timeout_ms)/*milliseconds*/;
    options.max_retry = turbo::get_flag(FLAGS_max_retry);
    if (channel.Init(turbo::get_flag(FLAGS_server).c_str(), turbo::get_flag(FLAGS_load_balancer).c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    helloworld::Greeter_Stub stub(&channel);

    // Send a request and wait for the response every 1 second.
    while (!melon::IsAskedToQuit()) {
        // We will receive response synchronously, safe to put variables
        // on stack.
        helloworld::HelloRequest request;
        helloworld::HelloReply response;
        melon::Controller cntl;

        request.set_name("grpc_req_from_melon");
        if (turbo::get_flag(FLAGS_gzip)) {
            cntl.set_request_compress_type(melon::COMPRESS_TYPE_GZIP);
        }
        // Because `done'(last parameter) is NULL, this function waits until
        // the response comes back or error occurs(including timedout).
        stub.SayHello(&cntl, &request, &response, NULL);
        if (!cntl.Failed()) {
            LOG(INFO) << "Received response from " << cntl.remote_side()
                << " to " << cntl.local_side()
                << ": " << response.message()
                << " latency=" << cntl.latency_us() << "us";
        } else {
            LOG(WARNING) << cntl.ErrorText();
        }
        usleep(turbo::get_flag(FLAGS_interval_ms) * 1000L);
    }

    return 0;
}
