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
// Created by jeff on 24-6-22.
//

#include <halakv/router_sender.h>

namespace halakv {

    turbo::Status RouterSender::init(const std::string &server) {
        _server = server;
        return turbo::OkStatus();
    }

    RouterSender &RouterSender::set_verbose(bool verbose) {
        _verbose = verbose;
        return *this;
    }

    RouterSender &RouterSender::set_time_out(int time_ms) {
        _timeout_ms = time_ms;
        return *this;
    }

    RouterSender &RouterSender::set_connect_time_out(int time_ms) {
        _connect_timeout_ms = time_ms;
        return *this;
    }

    RouterSender &RouterSender::set_interval_time(int time_ms) {
        _between_meta_connect_error_ms = time_ms;
        return *this;
    }

    RouterSender &RouterSender::set_retry_time(int retry) {
        _retry_times = retry;
        return *this;
    }

    turbo::Status RouterSender::set(const halakv::KvRequest &request, halakv::KvResponse &response, int retry_times) {
        return send_request("set", request, response, retry_times);
    }

    turbo::Status RouterSender::get(const halakv::KvRequest &request, halakv::KvResponse &response, int retry_times) {
        return send_request("get", request, response, retry_times);
    }

    turbo::Status RouterSender::remove(const halakv::KvRequest &request, halakv::KvResponse &response, int retry_times) {
        return send_request("remove", request, response, retry_times);
    }

}  // halakv

