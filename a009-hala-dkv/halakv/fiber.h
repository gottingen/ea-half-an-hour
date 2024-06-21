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

#include <melon/fiber/fiber.h>

namespace halakv {

    class Fiber {
    public:
        Fiber() {
        }

        explicit Fiber(const fiber_attr_t *attr) : _attr(attr) {
        }

        void run(const std::function<void()> &call) {
            std::function<void()> *_call = new std::function<void()>;
            *_call = call;
            int ret = fiber_start_background(&_tid, _attr,
                                             [](void *p) -> void * {
                                                 auto call = static_cast<std::function<void()> *>(p);
                                                 (*call)();
                                                 delete call;
                                                 return nullptr;
                                             }, _call);
            if (ret != 0) {
                LOG(ERROR)<< "fiber_start_background fail";
            }
        }

        void run_urgent(const std::function<void()> &call) {
            std::function<void()> *_call = new std::function<void()>;
            *_call = call;
            int ret = fiber_start_urgent(&_tid, _attr,
                                         [](void *p) -> void * {
                                             auto call = static_cast<std::function<void()> *>(p);
                                             (*call)();
                                             delete call;
                                             return nullptr;
                                         }, _call);
            if (ret != 0) {
                LOG(ERROR)<< "fiber_start_urgent fail";
            }
        }

        void join() {
            fiber_join(_tid, nullptr);
        }

        fiber_t id() {
            return _tid;
        }

    private:
        fiber_t _tid;
        const fiber_attr_t *_attr = nullptr;
    };

}  // namespace halakv
