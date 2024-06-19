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

#include <melon/rpc/restful_service.h>
#include <halakv/web_service.h>
#include <melon/rpc/server.h>
#include <halakv/cache.h>

namespace halakv {

    class CacheService : public melon::RestfulService {
    public:
        explicit CacheService(Cache *cache_service, const WebServie *ws) : _cache_service(cache_service), _web_service(ws) {}

        void process(const melon::RestfulRequest *request, melon::RestfulResponse *response) override;

    private:
        Cache *_cache_service;
        const WebServie *_web_service;
    };

}  // namespace halavue