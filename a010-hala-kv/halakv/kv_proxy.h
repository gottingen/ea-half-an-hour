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
// Created by jeff on 24-6-19.
//

#pragma once
#include <gflags/gflags.h>
#include <turbo/log/logging.h>
#include <melon/rpc/server.h>
#include <halakv/kv.pb.h>
#include <halakv/cache.h>
#include <halakv/router_sender.h>
#include <vector>
#include <string>

namespace halakv {

    class KvProxy {
    public:

        KvProxy() = default;
        ~KvProxy() = default;

        static  KvProxy* instance() {
            static KvProxy _instance;
            return &_instance;
        }

        turbo::Status initialize(const std::string& address, const std::string& local_peer, Cache *cache);

        turbo::Status set(const ::halakv::KvRequest *request,
                 ::halakv::KvResponse *response);

        turbo::Status get(const ::halakv::KvRequest *request,
                 ::halakv::KvResponse *response);

        turbo::Status remove(const ::halakv::KvRequest *request,
                    ::halakv::KvResponse *response);
    private:
        size_t get_peer_index(const std::string_view& key);
    private:
        Cache *_cache;
        std::vector<std::string> _peers;
        std::string _local_peer;
        size_t _peer_index;
        std::hash<std::string_view> _hash;
        std::vector<std::unique_ptr<RouterSender>> _senders;
    };
}  // namespace halakv