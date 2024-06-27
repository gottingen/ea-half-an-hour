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
// Created by jeff on 24-6-21.
//

#pragma once


#include <melon/rpc/channel.h>
#include <turbo/utility/status.h>
#include <melon/utility/endpoint.h>
#include <melon/rpc/channel.h>
#include <melon/rpc/server.h>
#include <melon/rpc/controller.h>
#include <google/protobuf/descriptor.h>
#include <turbo/strings/substitute.h>
#include <halakv/kv.pb.h>

namespace halakv {

    class RouterSender {
    public:

        static RouterSender *get_instance() {
            static RouterSender ins;
            return &ins;
        }



        static const int kRetryTimes = 3;

        turbo::Status init(const std::string &server);

        RouterSender &set_verbose(bool verbose);

        RouterSender &set_time_out(int time_ms);

        RouterSender &set_connect_time_out(int time_ms);

        RouterSender &set_interval_time(int time_ms);

        RouterSender &set_retry_time(int retry);

        turbo::Status set(const halakv::KvRequest &request, halakv::KvResponse &response, int retry_times);

        turbo::Status get(const halakv::KvRequest &request, halakv::KvResponse &response, int retry_times);

        turbo::Status remove(const halakv::KvRequest &request, halakv::KvResponse &response, int retry_times);

        template<typename Request, typename Response>
        turbo::Status send_request(const std::string &service_name,
                                   const Request &request,
                                   Response &response, int retry_times);

    private:
        bool _verbose{false};
        int _retry_times{kRetryTimes};
        std::string _server;
        int _timeout_ms{300};
        int _connect_timeout_ms{500};
        int _between_meta_connect_error_ms{1000};
    };

    template<typename Request, typename Response>
    turbo::Status RouterSender::send_request(const std::string &service_name,
                                             const Request &request,
                                             Response &response, int retry_times) {
        const ::google::protobuf::ServiceDescriptor *service_desc = halakv::KvService::descriptor();
        const ::google::protobuf::MethodDescriptor *method =
                service_desc->FindMethodByName(service_name);
        if (method == nullptr) {
            LOG_IF(ERROR, _verbose) << "service name not exist, service:" << service_name;
            return turbo::invalid_argument_error(turbo::substitute("service name not exist, service:$0", service_name));
        }
        int retry_time = 0;
        uint64_t log_id = mutil::fast_rand();
        do {
            if (retry_time > 0 && retry_times > 0) {
                fiber_usleep(1000 * _between_meta_connect_error_ms);
            }
            melon::Controller cntl;
            cntl.set_log_id(log_id);
            //store has leader address
            melon::ChannelOptions channel_opt;
            channel_opt.timeout_ms = _timeout_ms;
            channel_opt.connect_timeout_ms = _connect_timeout_ms;
            melon::Channel short_channel;
            if (short_channel.Init(_server.c_str(), &channel_opt) != 0) {
                LOG_IF(WARNING, _verbose) << "connect with router server fail. channel Init fail, leader_addr:" << _server;
                ++retry_time;
                continue;
            }
            short_channel.CallMethod(method, &cntl, &request, &response, nullptr);
            LOG_IF(INFO, _verbose) << "router_req[" << request.ShortDebugString() << "], router_resp["
                                   << response.ShortDebugString() << "]";
            if (cntl.Failed()) {
                LOG_IF(WARNING, _verbose) << "connect with router server fail. send request fail, error:" << cntl.ErrorText() << ", log_id:" << cntl.log_id();
                ++retry_time;
                continue;
            }
            return turbo::OkStatus();
        } while (retry_time < retry_times);
        return turbo::deadline_exceeded_error(turbo::substitute("try times $0 reach max_try $1 and can not get response.", retry_time,
                                                                retry_times));

    }

}
