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
#include <melon/fiber/fiber.h>
#include <turbo/log/logging.h>
#include <melon/rpc/selective_channel.h>
#include <melon/rpc/parallel_channel.h>
#include <halamelon/proto/echo.pb.h>
#include <turbo/bootstrap/servlet.h>

TURBO_FLAG(int32_t, thread_num, 50, "Number of threads to send requests");
TURBO_FLAG(bool, use_fiber, false, "Use fiber to send requests");
TURBO_FLAG(int32_t, attachment_size, 0, "Carry so many byte attachment along with requests");
TURBO_FLAG(int32_t, request_size, 16, "Bytes of each request");
TURBO_FLAG(std::string, connection_type, "", "Connection type. Available values: single, pooled, short");
TURBO_FLAG(std::string, protocol, "melon_std", "Protocol type. Defined in melon/rpc/options.proto");
TURBO_FLAG(std::string, starting_server, "0.0.0.0:8114", "IP Address of the first server, port of i-th server is `first-port + i'");
TURBO_FLAG(std::string, load_balancer, "rr", "Name of load balancer");
TURBO_FLAG(int32_t, timeout_ms, 100, "RPC timeout in milliseconds");
TURBO_FLAG(int32_t, backup_ms, -1, "backup timeout in milliseconds");
TURBO_FLAG(int32_t, max_retry, 3, "Max retries(not including the first RPC)");
TURBO_FLAG(bool, dont_fail, false, "Print fatal when some call failed");

std::string g_request;
std::string g_attachment;

melon::var::LatencyRecorder g_latency_recorder("client");
melon::var::Adder<int> g_error_count("client_error_count");

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
        stub.Echo(&cntl, &request, &response, NULL);
        const int64_t elp = cntl.latency_us();
        if (!cntl.Failed()) {
            g_latency_recorder << cntl.latency_us();
        } else {
            g_error_count << 1; 
            CHECK(melon::IsAskedToQuit() || !turbo::get_flag(FLAGS_dont_fail))
                << "error=" << cntl.ErrorText() << " latency=" << elp;
            // We can't connect to the server, sleep a while. Notice that this
            // is a specific sleeping to prevent this thread from spinning too // fast. You should continue the business logic in a production 
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
    app->add_option("--connection_type", FLAGS_connection_type, "Connection type. Available values: single, pooled, short");
    app->add_option("-s,--starting_server", FLAGS_starting_server, "IP Address of the first server, port of i-th server is `first-port + i'");
    app->add_option("-l,--load_balancer", FLAGS_load_balancer, "Name of load balancer");
    app->add_option("-p,--protocol", FLAGS_protocol, "Protocol type. Defined in melon/rpc/options.proto");
    app->add_option("-o,--timeout_ms", FLAGS_timeout_ms, "RPC timeout in milliseconds");
    app->add_option("-b,--backup_ms", FLAGS_backup_ms, "backup timeout in milliseconds");
    app->add_option("-r,--max_retry", FLAGS_max_retry, "Max retries(not including the first RPC)");
    app->add_option_function<bool>("--dont_fail", [](bool v) {
        turbo::set_flag(&FLAGS_dont_fail, v);
    }, "Print fatal when some call failed");
    app->add_option("-a,--attachment_size", FLAGS_attachment_size, "Carry so many byte attachment along with requests");
    app->add_option("-q,--request_size", FLAGS_request_size, "Bytes of each request");
    auto [ext, code] = servlet.run(argc, argv);
    if (ext) {
        return code;
    }

    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    melon::SelectiveChannel channel;
    melon::ChannelOptions schan_options;
    schan_options.timeout_ms = turbo::get_flag(FLAGS_timeout_ms);
    schan_options.backup_request_ms = turbo::get_flag(FLAGS_backup_ms);
    schan_options.max_retry = turbo::get_flag(FLAGS_max_retry);
    if (channel.Init(turbo::get_flag(FLAGS_load_balancer).c_str(), &schan_options) != 0) {
        LOG(ERROR) << "Fail to init SelectiveChannel";
        return -1;
    }

    // Add sub channels.
    // ================
    std::vector<melon::ChannelBase*> sub_channels;
    
    // Add an ordinary channel.
    melon::Channel* sub_channel1 = new melon::Channel;
    mutil::EndPoint pt;
    if (str2endpoint(turbo::get_flag(FLAGS_starting_server).c_str(), &pt) != 0 &&
        hostname2endpoint(turbo::get_flag(FLAGS_starting_server).c_str(), &pt) != 0) {
        LOG(ERROR) << "Invalid address=`" << turbo::get_flag(FLAGS_starting_server) << "'";
        return -1;
    }
    melon::ChannelOptions options;
    options.protocol = turbo::get_flag(FLAGS_protocol);
    options.connection_type = turbo::get_flag(FLAGS_connection_type);
    std::ostringstream os;
    os << "list://";
    for (int i = 0; i < 3; ++i) {
        os << mutil::EndPoint(pt.ip, pt.port++) << ",";
    }
    if (sub_channel1->Init(os.str().c_str(), turbo::get_flag(FLAGS_load_balancer).c_str(),
                           &options) != 0) {
        LOG(ERROR) << "Fail to init ordinary channel";
        return -1;
    }
    sub_channels.push_back(sub_channel1);

    // Add a parallel channel.
    melon::ParallelChannel* sub_channel2 = new melon::ParallelChannel;
    melon::ParallelChannelOptions pchan_options;
    pchan_options.fail_limit = 1;
    if (sub_channel2->Init(&pchan_options) != 0) {
        LOG(ERROR) << "Fail to init sub_channel2";
        return -1;
    }
    for (int i = 0; i < 3; ++i) {
        melon::ChannelOptions options;
        options.protocol = turbo::get_flag(FLAGS_protocol);
        options.connection_type = turbo::get_flag(FLAGS_connection_type);
        melon::Channel* c = new melon::Channel;
        if (c->Init(mutil::EndPoint(pt.ip, pt.port++), &options) != 0) {
            LOG(ERROR) << "Fail to init sub channel[" << i << "] of pchan";
            return -1;
        }
        if (sub_channel2->AddChannel(c, melon::OWNS_CHANNEL, NULL, NULL) != 0) {
            LOG(ERROR) << "Fail to add sub channel[" << i << "] into pchan";
            return -1;
        }
    }
    sub_channels.push_back(sub_channel2);

    // Add another selective channel with default options.
    melon::SelectiveChannel* sub_channel3 = new melon::SelectiveChannel;
    if (sub_channel3->Init(turbo::get_flag(FLAGS_load_balancer).c_str(), NULL) != 0) {
        LOG(ERROR) << "Fail to init schan";
        return -1;
    }
    for (int i = 0; i < 3; ++i) {
        melon::Channel* c = new melon::Channel;
        if (i == 0) {
            os.str("");
            os << "list://";
            for (int j = 0; j < 3; ++j) {
                os << mutil::EndPoint(pt.ip, pt.port++) << ",";
            }
            if (c->Init(os.str().c_str(), turbo::get_flag(FLAGS_load_balancer).c_str(),
                        &options) != 0) {
                LOG(ERROR) << "Fail to init sub channel[" << i << "] of schan";
                return -1;
            }
        } else {
            if (c->Init(mutil::EndPoint(pt.ip, pt.port++), &options) != 0) {
                LOG(ERROR) << "Fail to init sub channel[" << i << "] of schan";
                return -1;
            }
        }
        if (sub_channel3->AddChannel(c, NULL)) {
            LOG(ERROR) << "Fail to add sub channel[" << i << "] into schan";
            return -1;
        }
    }
    sub_channels.push_back(sub_channel3);

    // Add all sub channels into schan.
    for (size_t i = 0; i < sub_channels.size(); ++i) {
        // note: we don't need the handle for channel removal;
        if (channel.AddChannel(sub_channels[i], NULL/*note*/) != 0) {
            LOG(ERROR) << "Fail to add sub_channel[" << i << "]";
            return -1;
        }
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
