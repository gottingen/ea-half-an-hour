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
// Created by jeff on 24-6-18.
//

#pragma once

#include <turbo/container/cache.h>

namespace halavue {

    class Cache {
    public:
        Cache() = default;

        ~Cache() = default;

        turbo::Status init(int capacity) {
            _cache = std::make_unique<turbo::LRUCache<std::string, std::string>>(capacity);
            return turbo::OkStatus();
        }

        void set(const std::string &key, const std::string &value) {
            _cache->put(key, value);
        }

        std::pair<std::string, bool> get(const std::string &key) {
            auto r = _cache->try_get(key);
            if(r.second) {
                return std::make_pair(*(r.first), true);
            }
            return std::make_pair("", false);
        }

        void remove(const std::string &key) {
            _cache->remove(key);
        }

    private:
        std::unique_ptr<turbo::LRUCache<std::string, std::string>> _cache;
    };
}  // namespace halavue

