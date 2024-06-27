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
#include <halakv/kv_proxy.h>
#include <halakv/kv.pb.h>

namespace halakv {

    static std::string proto_conversion_err;
    static std::string no_key_err;
    std::once_flag proto_conversion_err_flag;

    void init_error_once() {
        nlohmann::json j;
        j["code"] = turbo::StatusCode::kInternal;
        j["message"] = "proto to json error";
        j["value"] = "";
        proto_conversion_err = j.dump();
        nlohmann::json nokey;
        nokey["code"] = turbo::StatusCode::kInvalidArgument;
        nokey["message"] = "no key or value";
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

    void CacheSetProcessor::process(const melon::RestfulRequest *request, melon::RestfulResponse *response) {
        response->set_content_json();
        response->set_access_control_all_allow();
        auto &uri = request->uri();
        auto *key = uri.GetQuery("key");
        auto &value = request->body();
        if (key == nullptr || value.empty()) {
            response->set_status_code(200);
            response->set_body(get_nokey_err());
            return;
        }
        halakv::KvResponse kv_response;
        halakv::KvRequest kv_request;
        kv_request.set_key(*key);
        kv_request.set_value(value.to_string());
        auto rs = KvProxy::instance()->set(&kv_request, &kv_response);
        if (!rs.ok()) {
            response->set_status_code(500);
        } else {
            response->set_status_code(200);
        }
        std::string json;
        if (json2pb::ProtoMessageToJson(kv_response, &json)) {
            response->set_body(json);
        } else {
            response->set_body(get_proto_conversion_err());
        }
    }

    void CacheGetProcessor::process(const melon::RestfulRequest *request, melon::RestfulResponse *response) {
        response->set_content_json();
        response->set_access_control_all_allow();
        halakv::KvResponse kv_response;
        halakv::KvRequest kv_request;
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
        VLOG(20) << "get key: " << *key;
        auto rs = KvProxy::instance()->get(&kv_request, &kv_response);
        if (!rs.ok()) {
            response->set_status_code(500);
        } else {
            response->set_status_code(200);
        }
        std::string json;
        std::string err;
        if (json2pb::ProtoMessageToJson(kv_response, &json, &err)) {
            VLOG(30) << "get key: " << *key << "response: " << json;
            response->set_body(json);
        } else {
            LOG(ERROR)<< "error: " << err;
            response->set_body(get_proto_conversion_err());
        }
    }

    turbo::Status registry_server(melon::Server *server) {
        auto service = melon::RestfulService::instance();
        service->set_processor("/cache/set", std::make_shared<CacheSetProcessor>());
        service->set_processor("/cache/get", std::make_shared<CacheGetProcessor>());
        service->set_not_found_processor(std::make_shared<NotFoundProcessor>());
        service->set_root_processor(std::make_shared<RootProcessor>());
        service->set_mapping_path("ea");
        return service->register_server(server);
    }

}  // namespace halakv