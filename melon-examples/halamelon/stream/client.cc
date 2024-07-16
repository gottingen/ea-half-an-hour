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


// A client sending requests to server in batch every 1 second.

#include <turbo/flags/flag.h>
#include <turbo/log/logging.h>
#include <melon/rpc/channel.h>
#include <melon/rpc/stream.h>
#include <halamelon/proto/echo.pb.h>
#include <turbo/bootstrap/servlet.h>

TURBO_FLAG(bool, send_attachment, true, "Carry attachment along with requests");
TURBO_FLAG(std::string, connection_type, "", "Connection type. Available values: single, pooled, short");
TURBO_FLAG(std::string, server, "0.0.0.0:8001", "IP Address of server");
TURBO_FLAG(int32_t, timeout_ms, 100, "RPC timeout in milliseconds");
TURBO_FLAG(int32_t, max_retry, 3, "Max retries(not including the first RPC)");

int main(int argc, char* argv[]) {
    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option_function<bool>("send_attachment", [](bool value) {
        turbo::set_flag(&FLAGS_send_attachment, value);
    });
    app->add_option("connection_type", FLAGS_connection_type, "Connection type. Available values: single, pooled, short");
    app->add_option("server", FLAGS_server, "IP Address of server");
    app->add_option("timeout_ms", FLAGS_timeout_ms, "RPC timeout in milliseconds");
    app->add_option("max_retry", FLAGS_max_retry, "Max retries(not including the first RPC)");
    auto [ext, code] = servlet.run(argc, argv);
    if (code != 0) {
        return code;
    }
    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    melon::Channel channel;
        
    // Initialize the channel, NULL means using default options. 
    melon::ChannelOptions options;
    options.protocol = melon::PROTOCOL_MELON_STD;
    options.connection_type = turbo::get_flag(FLAGS_connection_type);
    options.timeout_ms = turbo::get_flag(FLAGS_timeout_ms)/*milliseconds*/;
    options.max_retry = turbo::get_flag(FLAGS_max_retry);
    if (channel.Init(turbo::get_flag(FLAGS_server).c_str(), NULL) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    example::EchoService_Stub stub(&channel);
    melon::Controller cntl;
    melon::StreamId stream;
    if (melon::StreamCreate(&stream, cntl, NULL) != 0) {
        LOG(ERROR) << "Fail to create stream";
        return -1;
    }
    LOG(INFO) << "Created Stream=" << stream;
    example::EchoRequest request;
    example::EchoResponse response;
    request.set_message("I'm a RPC to connect stream");
    stub.Echo(&cntl, &request, &response, NULL);
    if (cntl.Failed()) {
        LOG(ERROR) << "Fail to connect stream, " << cntl.ErrorText();
        return -1;
    }
    
    while (!melon::IsAskedToQuit()) {
        mutil::IOBuf msg1;
        msg1.append("abcdefghijklmnopqrstuvwxyz");
        CHECK_EQ(0, melon::StreamWrite(stream, msg1));
        mutil::IOBuf msg2;
        msg2.append("0123456789");
        CHECK_EQ(0, melon::StreamWrite(stream, msg2));
        sleep(1);
    }

    CHECK_EQ(0, melon::StreamClose(stream));
    LOG(INFO) << "EchoClient is going to quit";
    return 0;
}
