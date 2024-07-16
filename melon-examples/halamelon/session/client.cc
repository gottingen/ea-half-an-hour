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


// A client sending requests to server by multiple threads.

#include <turbo/flags/flag.h>
#include <melon/fiber/fiber.h>
#include <turbo/log/logging.h>
#include <melon/rpc/channel.h>
#include <halamelon/proto/echo.pb.h>
#include <melon/var/var.h>
#include <turbo/bootstrap/servlet.h>

TURBO_FLAG(int32_t, thread_num, 50, "Number of threads to send requests");
TURBO_FLAG(bool, use_fiber, false, "Use fiber to send requests");
TURBO_FLAG(int32_t, attachment_size, 0, "Carry so many byte attachment along with requests");
TURBO_FLAG(int32_t, request_size, 16, "Bytes of each request");
TURBO_FLAG(std::string, protocol, "melon_std", "Protocol type. Defined in melon/rpc/options.proto");
TURBO_FLAG(std::string, connection_type, "", "Connection type. Available values: single, pooled, short");
TURBO_FLAG(std::string, server, "0.0.0.0:8002", "IP Address of server");
TURBO_FLAG(std::string, load_balancer, "", "The algorithm for load balancing");
TURBO_FLAG(int32_t, timeout_ms, 100, "RPC timeout in milliseconds");
TURBO_FLAG(int32_t, max_retry, 3, "Max retries(not including the first RPC)");
TURBO_FLAG(bool, dont_fail, false, "Print fatal when some call failed");

std::string g_request;
std::string g_attachment;

melon::var::LatencyRecorder g_latency_recorder("client");

static void* sender(void* arg) {
    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    example::EchoService_Stub stub(static_cast<google::protobuf::RpcChannel*>(arg));

    int log_id = 0;
    while (!melon::IsAskedToQuit()) {
        // We will receive response synchronously, safe to put variables
        // on stack.
        example::EchoRequest request;
        example::EchoResponse response;
        melon::Controller cntl;

        request.set_message(g_request);
        cntl.set_log_id(log_id++);  // set by user
        if (!g_attachment.empty()) {
            // Set attachment which is wired to network directly instead of 
            // being serialized into protobuf messages.
            cntl.request_attachment().append(g_attachment);
        }

        // Because `done'(last parameter) is NULL, this function waits until
        // the response comes back or error occurs(including timedout).
        const int64_t start_time = mutil::cpuwide_time_us();
        stub.Echo(&cntl, &request, &response, NULL);
        const int64_t end_time = mutil::cpuwide_time_us();
        const int64_t elp = end_time - start_time;
        if (!cntl.Failed()) {
            g_latency_recorder << elp;
        } else {
            CHECK(melon::IsAskedToQuit() || !turbo::get_flag(FLAGS_dont_fail))
                << "error=" << cntl.ErrorText() << " latency=" << elp;
            // We can't connect to the server, sleep a while. Notice that this
            // is a specific sleeping to prevent this thread from spinning too
            // fast. You should continue the business logic in a production 
            // server rather than sleeping.
            fiber_usleep(50000);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option("-t,--thread_num", FLAGS_thread_num, "Number of threads to send requests");
    app->add_option_function<bool>("--use_fiber", [](bool v) {
        turbo::set_flag(&FLAGS_use_fiber, v);
    }, "Use fiber to send requests");
    app->add_option("-a,--attachment_size", FLAGS_attachment_size, "Carry so many byte attachment along with requests");
    app->add_option("-r,--request_size", FLAGS_request_size, "Bytes of each request");
    app->add_option("--protocol", FLAGS_protocol, "Protocol type. Defined in melon/rpc/options.proto");
    app->add_option("--connection_type", FLAGS_connection_type, "Connection type. Available values: single, pooled, short");
    app->add_option("-s,--server", FLAGS_server, "IP Address of server");
    app->add_option("-l,--load_balancer", FLAGS_load_balancer, "The algorithm for load balancing");
    app->add_option("-m,--timeout_ms", FLAGS_timeout_ms, "RPC timeout in milliseconds");
    app->add_option("-r,--max_retry", FLAGS_max_retry, "Max retries(not including the first RPC)");
    app->add_option_function<bool>("--dont_fail", [](bool v) {
        turbo::set_flag(&FLAGS_dont_fail, v);
    }, "Print fatal when some call failed");
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

    if (turbo::get_flag(FLAGS_attachment_size) > 0) {
        g_attachment.resize(turbo::get_flag(FLAGS_attachment_size), 'a');
    }
    if (turbo::get_flag(FLAGS_request_size) <= 0) {
        LOG(ERROR) << "Bad request_size=" << turbo::get_flag(FLAGS_request_size);
        return -1;
    }
    g_request.resize(turbo::get_flag(FLAGS_request_size), 'r');

    std::vector<fiber_t> bids;
    std::vector<pthread_t> pids;
    if (!turbo::get_flag(FLAGS_use_fiber)) {
        pids.resize(turbo::get_flag(FLAGS_thread_num));
        for (int i = 0; i < turbo::get_flag(FLAGS_thread_num); ++i) {
            if (pthread_create(&pids[i], NULL, sender, &channel) != 0) {
                LOG(ERROR) << "Fail to create pthread";
                return -1;
            }
        }
    } else {
        bids.resize(turbo::get_flag(FLAGS_thread_num));
        for (int i = 0; i < turbo::get_flag(FLAGS_thread_num); ++i) {
            if (fiber_start_background(
                    &bids[i], NULL, sender, &channel) != 0) {
                LOG(ERROR) << "Fail to create fiber";
                return -1;
            }
        }
    }

    while (!melon::IsAskedToQuit()) {
        sleep(1);
        LOG(INFO) << "Sending EchoRequest at qps=" << g_latency_recorder.qps(1)
                  << " latency=" << g_latency_recorder.latency(1);
    }

    LOG(INFO) << "EchoClient is going to quit";
    for (int i = 0; i < turbo::get_flag(FLAGS_thread_num); ++i) {
        if (!turbo::get_flag(FLAGS_use_fiber)) {
            pthread_join(pids[i], NULL);
        } else {
            fiber_join(bids[i], NULL);
        }
    }

    return 0;
}
