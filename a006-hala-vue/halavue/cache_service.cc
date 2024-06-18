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

#include <halavue/cache_service.h>
#include <turbo/strings/substitute.h>
#include <turbo/strings/match.h>
#include <collie/nlohmann/json.hpp>

namespace halavue {

    static constexpr std::string_view kTemplate = R"({"code":$0,"msg":"$1", "value":"$2"})";
    void CacheService::process(const melon::RestfulRequest *request, melon::RestfulResponse *response) {
        auto &path = request->unresolved_path();
        response->set_header("Access-Control-Allow-Origin", "*");
        // index
        if(path.empty()) {
            response->set_status_code(200);
            response->set_header("Content-Type", "text/html");
            response->set_body(R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Cache Service</title>
</head>
<body>
    <br> <a href="/ea/cache">cache</a> </br>
    <br> <a href="/ea/ui">ui</a> </br>
</body>
</html>
            )");
            return;
        }
        if(turbo::starts_with(path, "ui")) {
            _web_service->process(request, response);
            return;
        }
        if(path != "cache") {
            response->set_status_code(404);
            response->set_body("not found");
            return;
        }
        // get method
        response->set_header("Content-Type", "application/json");
        if(request->method() ==melon::HTTP_METHOD_GET) {
            auto &uri = request->uri();
            auto *key = uri.GetQuery("key");
            if(key == nullptr) {
                response->set_status_code(200);
                nlohmann::json j;
                j["code"] = turbo::StatusCode::kInvalidArgument;
                j["msg"] = "no key";
                j["value"] = "";
                response->set_body(j.dump());
                return;
            }
            // get key from cache
            auto r = _cache_service->get(*key);
            if(r.second) {
                response->set_status_code(200);
                nlohmann::json j;
                j["code"] = turbo::StatusCode::kOk;
                j["msg"] = "ok";
                j["value"] = r.first;
                response->set_body(j.dump());
                return;
            } else {
                response->set_status_code(200);
                nlohmann::json j;
                j["code"] = turbo::StatusCode::kNotFound;
                j["msg"] = turbo::str_cat(*key, " not found");
                j["value"] = "";
                response->set_body(j.dump());
                return;
            }
        }

        // set method
        if(request->method() == melon::HTTP_METHOD_POST) {
            auto &uri = request->uri();
            auto *key = uri.GetQuery("key");
            auto &value = request->body();
            if(key == nullptr || value.empty()) {
                response->set_status_code(200);
                nlohmann::json j;
                j["code"] = turbo::StatusCode::kInvalidArgument;
                j["msg"] = "no key or value";
                j["value"] = "";
                response->set_body(j.dump());
                return;
            }
            _cache_service->set(*key, value.to_string());
            response->set_status_code(200);
            nlohmann::json j;
            j["code"] = turbo::StatusCode::kOk;
            j["msg"] = "ok";
            j["value"] = "";
            response->set_body(j.dump());
            return;
        }
        response->set_status_code(403);
        response->set_body("not support method");
    }

}  // namespace halavue