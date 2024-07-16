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


// A client sending requests to server which will send the request to itself
// again according to the field `depth'

#include <turbo/bootstrap/servlet.h>
#include <turbo/flags/flag.h>
#include <turbo/log/logging.h>
#include <melon/utility/time.h>
#include <melon/fiber/fiber.h>
#include <melon/rpc/channel.h>
#include <melon/rpc/server.h>
#include <halamelon/proto/cascade.pb.h>
#include <melon/var/var.h>
#include <melon/utility/fast_rand.h>

TURBO_FLAG(int32_t, thread_num, 2, "Number of threads to send requests");
TURBO_FLAG(bool, use_fiber, false, "Use fiber to send requests");
TURBO_FLAG(std::string, attachment, "foo", "Carry this along with requests");
TURBO_FLAG(std::string, connection_type, "", "Connection type. Available values: single, pooled, short");
TURBO_FLAG(std::string, server, "0.0.0.0:8000", "IP Address of server");
TURBO_FLAG(std::string, load_balancer, "", "The algorithm for load balancing");
TURBO_FLAG(int32_t, timeout_ms, 100, "RPC timeout in milliseconds");
TURBO_FLAG(int32_t, max_retry, 3, "Max retries(not including the first RPC)");
TURBO_FLAG(std::string, protocol, "melon_std", "Protocol type. Defined in melon/rpc/options.proto");
TURBO_FLAG(int32_t, depth, 0, "number of loop calls");
// Don't send too frequently in this example
TURBO_FLAG(int32_t, sleep_ms, 1000, "milliseconds to sleep after each RPC");
TURBO_FLAG(int32_t, dummy_port, -1, "Launch dummy server at this port");

melon::var::LatencyRecorder g_latency_recorder("client");

void* sender(void* arg) {
    melon::Channel* chan = (melon::Channel*)arg;
    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    example::CascadeService_Stub stub(chan);

    // Send a request and wait for the response every 1 second.
    while (!melon::IsAskedToQuit()) {
        // We will receive response synchronously, safe to put variables
        // on stack.
        example::CascadeRequest request;
        example::CascadeResponse response;
        melon::Controller cntl;

        request.set_message("hello world");
        if (turbo::get_flag(FLAGS_depth) > 0) {
            request.set_depth(turbo::get_flag(FLAGS_depth));
        }

        // Set request_id to be a random string
        cntl.set_request_id(mutil::fast_rand_printable(9));

        // Set attachment which is wired to network directly instead of 
        // being serialized into protobuf messages.
        cntl.request_attachment().append(turbo::get_flag(FLAGS_attachment));

        // Because `done'(last parameter) is NULL, this function waits until
        // the response comes back or error occurs(including timedout).
        stub.Echo(&cntl, &request, &response, NULL);
        if (cntl.Failed()) {
            LOG_EVERY_N_SEC(WARNING, 1) << "Fail to send CascadeRequest, " << cntl.ErrorText();
        } else {
            g_latency_recorder << cntl.latency_us();
        }
        if (turbo::get_flag(FLAGS_sleep_ms) != 0) {
            fiber_usleep(turbo::get_flag(FLAGS_sleep_ms) * 1000L);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {

    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option("-t,--thread_num", FLAGS_thread_num, "Number of threads to send requests");
    app->add_option_function<bool>("-f,--use_fiber", [](bool v) { turbo::set_flag(&FLAGS_use_fiber,v); }, "Use fiber to send requests");
    app->add_option("-a,--attachment", FLAGS_attachment, "Carry this along with requests");
    app->add_option("-c,--connection_type", FLAGS_connection_type, "Connection type. Available values: single, pooled, short");
    app->add_option("-s,--server", FLAGS_server, "IP Address of server");
    app->add_option("-l,--load_balancer", FLAGS_load_balancer, "The algorithm for load balancing");
    app->add_option("-m,--max_retry", FLAGS_max_retry, "Max retries(not including the first RPC)");
    app->add_option("-o,--timeout_ms", FLAGS_timeout_ms, "RPC timeout in milliseconds");
    app->add_option("-p,--protocol", FLAGS_protocol, "Protocol type. Defined in melon/rpc/options.proto");
    app->add_option("-d,--depth", FLAGS_depth, "number of loop calls");
    app->add_option("-e,--sleep_ms", FLAGS_sleep_ms, "milliseconds to sleep after each RPC");
    app->add_option("--dummy_port", FLAGS_dummy_port, "Launch dummy server at this port");
    auto [ext, code] = servlet.run(argc, argv);
    if (ext) {
        return code;
    }
    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    melon::Channel channel;
    melon::ChannelOptions options;
    options.protocol = turbo::get_flag(FLAGS_protocol);
    options.connection_type = turbo::get_flag(FLAGS_connection_type);
    options.timeout_ms = turbo::get_flag(FLAGS_timeout_ms)/*milliseconds*/;
    options.max_retry = turbo::get_flag(FLAGS_max_retry);
    
    // Initialize the channel, NULL means using default options. 
    // options, see `melon/rpc/channel.h'.
    if (channel.Init(turbo::get_flag(FLAGS_server).c_str(), turbo::get_flag(FLAGS_load_balancer).c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

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

    if (turbo::get_flag(FLAGS_dummy_port) >= 0) {
        melon::StartDummyServerAt(turbo::get_flag(FLAGS_dummy_port));
    }

    while (!melon::IsAskedToQuit()) {
        sleep(1);
        LOG(INFO) << "Sending CascadeRequest at qps=" << g_latency_recorder.qps(1)
                  << " latency=" << g_latency_recorder.latency(1);
    }

    LOG(INFO) << "CascadeClient is going to quit";
    for (int i = 0; i < turbo::get_flag(FLAGS_thread_num); ++i) {
        if (!turbo::get_flag(FLAGS_use_fiber)) {
            pthread_join(pids[i], NULL);
        } else {
            fiber_join(bids[i], NULL);
        }
    }
    return 0;
}
