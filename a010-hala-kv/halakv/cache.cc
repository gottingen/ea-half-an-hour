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
#include <halakv/cache.h>

namespace halakv {

    turbo::Status Cache::init(int capacity) {
        _cache = std::make_unique<turbo::LRUCache<std::string, std::string>>(capacity);
        return turbo::OkStatus();
    }

    void Cache::put(const halakv::KvRequest *request, halakv::KvResponse *response) {
        if(!request->has_value()) {
            response->set_code(static_cast<int>(turbo::StatusCode::kInvalidArgument));
            response->set_message("no value");
            return;
        }
        {
            std::unique_lock lock(_mutex);
            _cache->put(request->key(), request->value());
        }
        response->set_code(static_cast<int>(turbo::StatusCode::kOk));
        response->set_message("ok");
    }

    void Cache::get(const halakv::KvRequest *request, halakv::KvResponse *response) const {
        std::shared_lock lock(_mutex);
        auto r = _cache->try_get(request->key());
        if (r.second) {
            response->set_value(*r.first);
            response->set_code(static_cast<int>(turbo::StatusCode::kOk));
            response->set_message("ok");
        } else {
            response->set_code(static_cast<int>(turbo::StatusCode::kNotFound));
            response->set_message("not found");
        }
    }

    void Cache::remove(const halakv::KvRequest *request, halakv::KvResponse *response) {
        std::unique_lock lock(_mutex);
        auto r = _cache->try_get(request->key());
        if (r.second) {
            response->set_value(*r.first);
            response->set_code(static_cast<int>(turbo::StatusCode::kOk));
            response->set_message("ok");
            _cache->remove(request->key());
        } else {
            response->set_code(static_cast<int>(turbo::StatusCode::kNotFound));
            response->set_message("not found");
        }
    }

}  // namespace halakv