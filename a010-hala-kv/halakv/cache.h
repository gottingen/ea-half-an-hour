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
#include <halakv/kv.pb.h>
#include <turbo/container/cache.h>
#include <turbo/utility/status.h>
#include <shared_mutex>

namespace halakv {

    class Cache {
    public:
        Cache()  = default;

        turbo::Status init(int capacity);

        void put(const halakv::KvRequest *request, halakv::KvResponse *response);

        void get(const halakv::KvRequest *request, halakv::KvResponse *response) const;

        void remove(const halakv::KvRequest *request, halakv::KvResponse *response);
    private:
        mutable std::shared_mutex _mutex;
        std::unique_ptr<turbo::LRUCache<std::string, std::string>> _cache;
    };

}  // namespace halakv
