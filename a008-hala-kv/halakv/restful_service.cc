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

#include <halakv/restful_service.h>
#include <turbo/strings/substitute.h>
#include <turbo/strings/match.h>
#include <melon/json2pb/pb_to_json.h>
#include <collie/nlohmann/json.hpp>
#include <halakv/kv.pb.h>

namespace halakv {

    static std::string proto_conversion_err;
    static std::string no_key_err;
    std::once_flag proto_conversion_err_flag;

    void init_error_once() {
        nlohmann::json j;
        j["code"] = turbo::StatusCode::kInternal;
        j["msg"] = "proto to json error";
        j["value"] = "";
        proto_conversion_err = j.dump();
        nlohmann::json nokey;
        nokey["code"] = turbo::StatusCode::kInvalidArgument;
        nokey["msg"] = "no key or value";
        nokey["value"] = "";
        no_key_err = nokey.dump();
    }

    const std::string &get_proto_conversion_err() {
        std::call_once(proto_conversion_err_flag,init_error_once);
        return proto_conversion_err;
    }

    const std::string &get_nokey_err() {
        std::call_once(proto_conversion_err_flag,init_error_once);
        return proto_conversion_err;
    }

    static constexpr std::string_view kTemplate = R"({"code":$0,"msg":"$1", "value":"$2"})";

    void CacheService::process(const melon::RestfulRequest *request, melon::RestfulResponse *response) {
        auto &path = request->unresolved_path();
        response->set_header("Access-Control-Allow-Origin", "*");
        // index
        if (path.empty()) {
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
        if (turbo::starts_with(path, "ui")) {
            _web_service->process(request, response);
            return;
        }
        if (path != "cache") {
            response->set_status_code(404);
            response->set_body("not found");
            return;
        }
        halakv::KvResponse kv_response;
        halakv::KvRequest kv_request;
        response->set_header("Content-Type", "application/json");
        // get method
        if (request->method() == melon::HTTP_METHOD_GET) {
            auto &uri = request->uri();
            auto *key = uri.GetQuery("key");
            if (key == nullptr) {
                response->set_status_code(200);
                kv_response.set_code(static_cast<int>(turbo::StatusCode::kInvalidArgument));
                kv_response.set_value("");
                kv_response.set_message("no key");
                std::string json;
                if (json2pb::ProtoMessageToJson(kv_response, &json)) {
                    response->set_body(json);
                } else {
                    response->set_body(get_proto_conversion_err());
                }
                return;
            }
            // get key from cache
            kv_request.set_key(*key);
            LOG(INFO) << "get key: " << *key;
            _cache_service->get(&kv_request, &kv_response);
            std::string json;
            std::string err;
            response->set_status_code(200);
            if (json2pb::ProtoMessageToJson(kv_response, &json, &err)) {
                LOG(INFO) << "get key: " << *key << "response: " << json;
                response->set_body(json);
            } else {
                LOG(ERROR)<< "error: " << err;
                response->set_body(get_proto_conversion_err());
            }
            return;
        }

        // set method
        if (request->method() == melon::HTTP_METHOD_POST) {
            auto &uri = request->uri();
            auto *key = uri.GetQuery("key");
            auto &value = request->body();
            if (key == nullptr || value.empty()) {
                response->set_status_code(200);
                response->set_body(get_nokey_err());
                return;
            }
            kv_request.set_key(*key);
            kv_request.set_value(value.to_string());
            _cache_service->put(&kv_request, &kv_response);
            response->set_status_code(200);
            std::string json;
            if (json2pb::ProtoMessageToJson(kv_response, &json)) {
                response->set_body(json);
            } else {
                response->set_body(get_proto_conversion_err());
            }
            return;
        }
        response->set_status_code(403);
        response->set_body("{err:not support method}");
    }

}  // namespace halakv