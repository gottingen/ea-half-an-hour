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
#include <halakv/kv_proxy.h>
#include <halakv/cache.h>
#include <turbo/strings/str_split.h>
#include <halakv/fiber.h>
#include <melon/rpc/channel.h>
#include <halakv/kv.pb.h>

namespace halakv {

    turbo::Status KvProxy::initialize(const std::string &address, const std::string &local_peer, Cache *cache) {
        _peers = turbo::str_split(address, ",", turbo::SkipEmpty());
        _local_peer = local_peer;
        _cache = cache;
        _peer_index = std::numeric_limits<size_t>::max();
        for (size_t i = 0; i < _peers.size(); i++) {
            if (_peers[i] == _local_peer) {
                _peer_index = i;
                break;
            }
        }
        if (_peer_index == std::numeric_limits<size_t>::max()) {
            return turbo::invalid_argument_error("local peer not found in peers");
        }
        _senders.resize(_peers.size());
        for (size_t i = 0; i < _peers.size(); i++) {
            _senders[i] = std::make_unique<halakv::RouterSender>();
            auto rs = _senders[i]->init(_peers[i]);
            if(!rs.ok()) {
                return rs;
            }
        }
        return turbo::OkStatus();
    }

    turbo::Status KvProxy::set(const ::halakv::KvRequest *request,
                      ::halakv::KvResponse *response) {
        auto index = get_peer_index(request->key());
        LOG(INFO) << "set key: " << request->key()<< " server: "<< _peers[index];
        if (index == _peer_index) {
            _cache->put(request, response);
            return turbo::OkStatus();
        } else {
            turbo::Status rs;
            auto func = [&rs, this, index, request, response]() {
                auto sender = _senders[index].get();
                rs = sender->set(*request, *response, RouterSender::kRetryTimes);
            };
            Fiber fiber;
            fiber.run_urgent(func);
            fiber.join();
        }
        return turbo::OkStatus();
    }

    turbo::Status KvProxy::get(const ::halakv::KvRequest *request,
                      ::halakv::KvResponse *response) {
        auto index = get_peer_index(request->key());
        LOG(INFO) << "get key: " << request->key()<< " server: "<< _peers[index];
        if (index == _peer_index) {
            _cache->get(request, response);
            return turbo::OkStatus();
        } else {
            turbo::Status rs;
            auto func = [&rs, this, index, request, response]() {
                auto sender = _senders[index].get();
                rs = sender->get(*request, *response, RouterSender::kRetryTimes);
            };
            Fiber fiber;
            fiber.run_urgent(func);
            fiber.join();
        }
        return turbo::OkStatus();
    }

    turbo::Status KvProxy::remove(const ::halakv::KvRequest *request,
                         ::halakv::KvResponse *response) {
        auto index = get_peer_index(request->key());
        LOG(INFO) << "remove key: " << request->key()<< " server: "<< _peers[index];
        if (index == _peer_index) {
            _cache->remove(request, response);
            return turbo::OkStatus();
        } else {
            turbo::Status rs;
            auto func = [&rs, this, index, request, response]() {
                auto sender = _senders[index].get();
                rs = sender->remove(*request, *response, RouterSender::kRetryTimes);
            };
            Fiber fiber;
            fiber.run_urgent(func);
            fiber.join();
        }
        return turbo::OkStatus();
    }

    size_t KvProxy::get_peer_index(const std::string_view &key) {
        return _hash(key) % _peers.size();
    }

}  // namespace halakv