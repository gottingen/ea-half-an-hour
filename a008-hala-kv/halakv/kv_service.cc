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
#include <halakv/kv_service.h>
#include <halakv/cache.h>

namespace halakv {

    void KvServiceimpl::set(::google::protobuf::RpcController *cntl_base,
                            const ::halakv::KvRequest *request,
                            ::halakv::KvResponse *response,
                            ::google::protobuf::Closure *done) {
        melon::ClosureGuard done_guard(done);
        _cache->put(request, response);
    }

    void KvServiceimpl::get(::google::protobuf::RpcController *cntl_base,
                            const ::halakv::KvRequest *request,
                            ::halakv::KvResponse *response,
                            ::google::protobuf::Closure *done) {
        melon::ClosureGuard done_guard(done);
        _cache->get(request, response);
    }

    void KvServiceimpl::remove(::google::protobuf::RpcController *cntl_base,
                            const ::halakv::KvRequest *request,
                            ::halakv::KvResponse *response,
                            ::google::protobuf::Closure *done) {
        melon::ClosureGuard done_guard(done);
        _cache->remove(request, response);
    }

}  // namespace halakv