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


// A client sending requests to server every 1 second.

#include <gflags/gflags.h>
#include <turbo/log/logging.h>
#include <melon/utility/time.h>
#include <melon/rpc/channel.h>
#include <halakv/kv.pb.h>

DEFINE_string(op, "", "Operation type. Available values: set, get, remove");
DEFINE_string(key, "", "Key to operate");
DEFINE_string(value, "", "Value to operate");
DEFINE_string(protocol, "melon_std", "Protocol type. Defined in melon/rpc/options.proto");
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
DEFINE_string(server, "0.0.0.0:8018", "IP Address of server");
DEFINE_int32(timeout_ms, 100, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)"); 
DEFINE_int32(interval_ms, 1000, "Milliseconds between consecutive requests");

int main(int argc, char* argv[]) {
    // Parse gflags. We recommend you to use gflags as well.
    google::ParseCommandLineFlags(&argc, &argv, true);
    
    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    melon::Channel channel;
    
    // Initialize the channel, NULL means using default options.
    melon::ChannelOptions options;
    options.protocol = FLAGS_protocol;
    options.connection_type = FLAGS_connection_type;
    options.timeout_ms = FLAGS_timeout_ms/*milliseconds*/;
    options.max_retry = FLAGS_max_retry;
    if (channel.Init(FLAGS_server.c_str(), "", &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    halakv::KvService_Stub stub(&channel);
    if(FLAGS_op.empty()) {
        LOG(ERROR) << "Please specify operation type";
        return -1;
    }
    if(FLAGS_key.empty()) {
        LOG(ERROR) << "Please specify key";
        return -1;
    }

    if(FLAGS_op == "set") {
        if(FLAGS_value.empty()) {
            LOG(ERROR) << "Please specify value";
            return -1;
        }
        halakv::KvRequest request;
        halakv::KvResponse response;
        melon::Controller cntl;
        request.set_key(FLAGS_key);
        request.set_value(FLAGS_value);
        stub.set(&cntl, &request, &response, NULL);
        if (!cntl.Failed()) {
            LOG(INFO) << "Received response from " << cntl.remote_side()
                << " to " << cntl.local_side()
                << ": " << response.ShortDebugString();
        } else {
            LOG(WARNING) << cntl.ErrorText();
        }
        return 0;
    }
    if(FLAGS_op == "get") {
           halakv::KvRequest request;
            halakv::KvResponse response;
            melon::Controller cntl;
            request.set_key(FLAGS_key);
            stub.get(&cntl, &request, &response, NULL);
            if (!cntl.Failed()) {
                LOG(INFO) << "Received response from " << cntl.remote_side()
                          << " to " << cntl.local_side()
                          << ": " << response.ShortDebugString();
            } else {
                LOG(WARNING) << cntl.ErrorText();
            }
            return 0;
    }

    if(FLAGS_op == "remove") {
        halakv::KvRequest request;
        halakv::KvResponse response;
        melon::Controller cntl;
        request.set_key(FLAGS_key);
        stub.remove(&cntl, &request, &response, NULL);
        if (!cntl.Failed()) {
            LOG(INFO) << "Received response from " << cntl.remote_side()
                << " to " << cntl.local_side()
                << ": " << response.ShortDebugString();
        } else {
            LOG(WARNING) << cntl.ErrorText();
        }
        return 0;
    }
    LOG(ERROR)<< "Invalid operation type";
    return 0;
}
