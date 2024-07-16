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


// A client sending requests to server in parallel by multiple threads.

#include <turbo/flags/flag.h>
#include <melon/fiber/fiber.h>
#include <turbo/log/logging.h>
#include <melon/utility/string_printf.h>
#include <melon/utility/time.h>
#include <melon/utility/macros.h>
#include <melon/rpc/parallel_channel.h>
#include <melon/rpc/server.h>
#include <halamelon/proto/value_echo.pb.h>
#include <turbo/bootstrap/servlet.h>
#include <sstream>

TURBO_FLAG(int32_t, thread_num, 50, "Number of threads to send requests");
TURBO_FLAG(int32_t, channel_num, 3, "Number of sub channels");
TURBO_FLAG(bool, same_channel, false, "Add the same sub channel multiple times");
TURBO_FLAG(bool, use_fiber, false, "Use fiber to send requests");
TURBO_FLAG(int32_t, attachment_size, 0, "Carry so many byte attachment along with requests");
TURBO_FLAG(int32_t, request_size, 16, "Bytes of each request");
TURBO_FLAG(std::string, connection_type, "", "Connection type. Available values: single, pooled, short");
TURBO_FLAG(std::string, protocol, "melon_std", "Protocol type. Defined in melon/rpc/options.proto");
TURBO_FLAG(std::string, server, "0.0.0.0:8002", "IP Address of server");
TURBO_FLAG(std::string, load_balancer, "", "The algorithm for load balancing");
TURBO_FLAG(int32_t, timeout_ms, 100, "RPC timeout in milliseconds");
TURBO_FLAG(int32_t, max_retry, 3, "Max retries(not including the first RPC)");
TURBO_FLAG(bool, dont_fail, false, "Print fatal when some call failed");
TURBO_FLAG(int32_t, dummy_port, -1, "Launch dummy server at this port");

std::string g_request;
std::string g_attachment;
melon::var::LatencyRecorder g_latency_recorder("client");
melon::var::Adder<int> g_error_count("client_error_count");
melon::var::LatencyRecorder* g_sub_channel_latency = nullptr;

static void* sender(void* arg) {
    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    example::ValueEchoService_Stub stub(static_cast<google::protobuf::RpcChannel*>(arg));

    int log_id = 0;
    while (!melon::IsAskedToQuit()) {
        // We will receive response synchronously, safe to put variables
        // on stack.
        example::ValueEchoRequest request;
        example::ValueEchoResponse response;
        melon::Controller cntl;

        request.set_value(log_id++);
        if (!g_attachment.empty()) {
            // Set attachment which is wired to network directly instead of 
            // being serialized into protobuf messages.
            cntl.request_attachment().append(g_attachment);
        }

        // Because `done'(last parameter) is nullptr, this function waits until
        // the response comes back or error occurs(including timedout).
        stub.Echo(&cntl, &request, &response, nullptr);
        if (!cntl.Failed()) {
            g_latency_recorder << cntl.latency_us();
            for (int i = 0; i < cntl.sub_count(); ++i) {
                if (cntl.sub(i) && !cntl.sub(i)->Failed()) {
                    g_sub_channel_latency[i] << cntl.sub(i)->latency_us();
                }
            }
        } else {
            g_error_count << 1;
            CHECK(melon::IsAskedToQuit() || !turbo::get_flag(FLAGS_dont_fail))
                << "error=" << cntl.ErrorText() << " latency=" << cntl.latency_us();
            // We can't connect to the server, sleep a while. Notice that this
            // is a specific sleeping to prevent this thread from spinning too
            // fast. You should continue the business logic in a production 
            // server rather than sleeping.
            fiber_usleep(50000);
        }
    }
    return nullptr;
}

int main(int argc, char* argv[]) {
    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option("-t,--thread_num", FLAGS_thread_num, "Number of threads to send requests");
    app->add_option("-c,--channel_num", FLAGS_channel_num, "Number of sub channels");
    app->add_option_function<bool>("--same_channel", [](bool v) { turbo::set_flag(&FLAGS_same_channel, v); }, "Add the same sub channel multiple times");
    app->add_option_function<bool>("--use_fiber", [](bool v) { turbo::set_flag(&FLAGS_use_fiber, v); }, "Use fiber to send requests");
    app->add_option("-a,--attachment_size", FLAGS_attachment_size, "Carry so many byte attachment along with requests");
    app->add_option("-r,--request_size", FLAGS_request_size, "Bytes of each request");
    app->add_option("-n,--connection_type", FLAGS_connection_type, "Connection type. Available values: single, pooled, short");
    app->add_option("-p,--protocol", FLAGS_protocol, "Protocol type. Defined in melon/rpc/options.proto");
    app->add_option("-s,--server", FLAGS_server, "IP Address of server");
    app->add_option("-l,--load_balancer", FLAGS_load_balancer, "The algorithm for load balancing");
    app->add_option("-o,--timeout_ms", FLAGS_timeout_ms, "RPC timeout in milliseconds");
    app->add_option("-m,--max_retry", FLAGS_max_retry, "Max retries(not including the first RPC)");
    app->add_option_function<bool>("--dont_fail", [](bool v) { turbo::set_flag(&FLAGS_dont_fail, v); }, "Print fatal when some call failed");
    app->add_option("-d,--dummy_port", FLAGS_dummy_port, "Launch dummy server at this port");
    auto [ext, code] = servlet.run(argc, argv);
    if (ext) {
        return code;
    }
    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    melon::ParallelChannel channel;
    melon::ParallelChannelOptions pchan_options;
    pchan_options.timeout_ms = turbo::get_flag(FLAGS_timeout_ms);
    if (channel.Init(&pchan_options) != 0) {
        LOG(ERROR) << "Fail to init ParallelChannel";
        return -1;
    }

    melon::ChannelOptions sub_options;
    sub_options.protocol = turbo::get_flag(FLAGS_protocol);
    sub_options.connection_type = turbo::get_flag(FLAGS_connection_type);
    sub_options.max_retry = turbo::get_flag(FLAGS_max_retry);
    // Setting sub_options.timeout_ms does not work because timeout of sub 
    // channels are disabled in ParallelChannel.

    if (turbo::get_flag(FLAGS_same_channel)) {
        // For melon >= 1.0.155.31351, a sub channel can be added into
        // a ParallelChannel more than once.
        melon::Channel* sub_channel = new melon::Channel;
        // Initialize the channel, nullptr means using default options. 
        // options, see `melon/rpc/channel.h'.
        if (sub_channel->Init(turbo::get_flag(FLAGS_server).c_str(), turbo::get_flag(FLAGS_load_balancer).c_str(), &sub_options) != 0) {
            LOG(ERROR) << "Fail to initialize sub_channel";
            return -1;
        }
        for (int i = 0; i < turbo::get_flag(FLAGS_channel_num); ++i) {
            if (channel.AddChannel(sub_channel, melon::OWNS_CHANNEL,
                                   nullptr, nullptr) != 0) {
                LOG(ERROR) << "Fail to AddChannel, i=" << i;
                return -1;
            }
        }
    } else {
        for (int i = 0; i < turbo::get_flag(FLAGS_channel_num); ++i) {
            melon::Channel* sub_channel = new melon::Channel;
            // Initialize the channel, nullptr means using default options. 
            // options, see `melon/rpc/channel.h'.
            if (sub_channel->Init(turbo::get_flag(FLAGS_server).c_str(), turbo::get_flag(FLAGS_load_balancer).c_str(), &sub_options) != 0) {
                LOG(ERROR) << "Fail to initialize sub_channel[" << i << "]";
                return -1;
            }
            if (channel.AddChannel(sub_channel, melon::OWNS_CHANNEL,
                                   nullptr, nullptr) != 0) {
                LOG(ERROR) << "Fail to AddChannel, i=" << i;
                return -1;
            }
        }
    }

    // Initialize var for sub channel
    g_sub_channel_latency = new melon::var::LatencyRecorder[turbo::get_flag(FLAGS_channel_num)];
    for (int i = 0; i < turbo::get_flag(FLAGS_channel_num); ++i) {
        std::string name;
        mutil::string_printf(&name, "client_sub_%d", i);
        g_sub_channel_latency[i].expose(name);
    }

    if (turbo::get_flag(FLAGS_attachment_size) > 0) {
        g_attachment.resize(turbo::get_flag(FLAGS_attachment_size), 'a');
    }
    if (turbo::get_flag(FLAGS_request_size) <= 0) {
        LOG(ERROR) << "Bad request_size=" << turbo::get_flag(FLAGS_request_size);
        return -1;
    }
    g_request.resize(turbo::get_flag(FLAGS_request_size), 'r');

    if (turbo::get_flag(FLAGS_dummy_port) >= 0) {
        melon::StartDummyServerAt(turbo::get_flag(FLAGS_dummy_port));
    }

    std::vector<fiber_t> bids;
    std::vector<pthread_t> pids;
    if (!turbo::get_flag(FLAGS_use_fiber)) {
        pids.resize(turbo::get_flag(FLAGS_thread_num));
        for (int i = 0; i < turbo::get_flag(FLAGS_thread_num); ++i) {
            if (pthread_create(&pids[i], nullptr, sender, &channel) != 0) {
                LOG(ERROR) << "Fail to create pthread";
                return -1;
            }
        }
    } else {
        bids.resize(turbo::get_flag(FLAGS_thread_num));
        for (int i = 0; i < turbo::get_flag(FLAGS_thread_num); ++i) {
            if (fiber_start_background(
                    &bids[i], nullptr, sender, &channel) != 0) {
                LOG(ERROR) << "Fail to create fiber";
                return -1;
            }
        }
    }

    while (!melon::IsAskedToQuit()) {
        sleep(1);
        std::stringstream ss;
        ss << "Sending EchoRequest at qps=" << g_latency_recorder.qps(1)
                  << " latency=" << g_latency_recorder.latency(1);
        for (int i = 0; i < turbo::get_flag(FLAGS_channel_num); ++i) {
            ss<< " latency_" << i << "="
                      << g_sub_channel_latency[i].latency(1);
        }
        LOG(INFO)<<ss.str();
    }
    
    LOG(INFO) << "EchoClient is going to quit";
    for (int i = 0; i < turbo::get_flag(FLAGS_thread_num); ++i) {
        if (!turbo::get_flag(FLAGS_use_fiber)) {
            pthread_join(pids[i], nullptr);
        } else {
            fiber_join(bids[i], nullptr);
        }
    }

    return 0;
}
