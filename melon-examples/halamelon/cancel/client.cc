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
#include <melon/utility/time.h>
#include <melon/rpc/channel.h>
#include <halamelon/proto/echo.pb.h>


TURBO_FLAG(std::string, protocol, "melon_std", "Protocol type. Defined in melon/rpc/options.proto");
TURBO_FLAG(std::string, connection_type, "", "Connection type. Available values: single, pooled, short");
TURBO_FLAG(std::string, server, "0.0.0.0:8000", "IP Address of server");
TURBO_FLAG(std::string, load_balancer, "", "The algorithm for load balancing");
TURBO_FLAG(int32_t, timeout_ms, 100, "RPC timeout in milliseconds");
TURBO_FLAG(int32_t, max_retry, 3, "Max retries(not including the first RPC)");

// A special done for canceling another RPC.
class CancelRPC : public google::protobuf::Closure {
public:
    explicit CancelRPC(melon::CallId rpc_id) : _rpc_id(rpc_id) {}

    void Run() {
        melon::StartCancel(_rpc_id);
    }

private:
    melon::CallId _rpc_id;
};

int main(int argc, char *argv[]) {

    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option("-p,--protocol", FLAGS_protocol, "Protocol type. Defined in melon/rpc/options.proto");
    app->add_option("-c,--connection_type", FLAGS_connection_type,
                    "Connection type. Available values: single, pooled, short");
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
        example::EchoRequest request1;
        example::EchoResponse response1;
        melon::Controller cntl1;

        example::EchoRequest request2;
        example::EchoResponse response2;
        melon::Controller cntl2;

        request1.set_message("hello1");
        request2.set_message("hello2");

        cntl1.set_log_id(log_id++);  // set by user
        cntl2.set_log_id(log_id++);

        const melon::CallId id1 = cntl1.call_id();
        const melon::CallId id2 = cntl2.call_id();
        CancelRPC done1(id2);
        CancelRPC done2(id1);

        mutil::Timer tm;
        tm.start();
        // Send 2 async calls and join them. They will cancel each other in
        // their done which is run before the RPC being Join()-ed. Canceling
        // a finished RPC has no effect. 
        // For example:
        //  Time       RPC1                      RPC2
        //   1     response1 comes back. 
        //   2     running done1.
        //   3     cancel RPC2   
        //   4                              running done2 (NOTE: done also runs)
        //   5                              cancel RPC1 (no effect)
        stub.Echo(&cntl1, &request1, &response1, &done1);
        stub.Echo(&cntl2, &request2, &response2, &done2);
        melon::Join(id1);
        melon::Join(id2);
        tm.stop();
        if (cntl1.Failed() && cntl2.Failed()) {
            LOG(WARNING) << "Both failed. rpc1:" << cntl1.ErrorText()
                         << ", rpc2: " << cntl2.ErrorText();
        } else if (!cntl1.Failed()) {
            LOG(INFO) << "Received `" << response1.message() << "' from rpc1="
                      << id1.value << '@' << cntl1.remote_side()
                      << " latency=" << tm.u_elapsed() << "us"
                      << " rpc1_latency=" << cntl1.latency_us() << "us"
                      << " rpc2_latency=" << cntl2.latency_us() << "us";
        } else {
            LOG(INFO) << "Received `" << response2.message() << "' from rpc2="
                      << id2.value << '@' << cntl2.remote_side()
                      << " latency=" << tm.u_elapsed() << "us"
                      << " rpc1_latency=" << cntl1.latency_us() << "us"
                      << " rpc2_latency=" << cntl2.latency_us() << "us";
        }
        sleep(1);
    }

    LOG(INFO) << "EchoClient is going to quit";
    return 0;
}
