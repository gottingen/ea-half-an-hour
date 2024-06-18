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

#include <halavue/web_service.h>
#include <alkaid/files/filesystem.h>
#include <turbo/strings/strip.h>

namespace halavue {

    void WebServie::process(const melon::RestfulRequest *request, melon::RestfulResponse *response) const {
        // process request
        auto unresolved_path = request->unresolved_path();
        // path must starts with "ui"
        if(unresolved_path.size() >= 3) {
            unresolved_path = turbo::strip_prefix(unresolved_path, "ui/");
        } else {
            unresolved_path.clear();
        }

        if(unresolved_path.empty()) {
            unresolved_path = "index.html";
        }
        auto path = alkaid::filesystem::path(_root) / unresolved_path;
        LOG(INFO)<<"path: "<<path.string();
        std::error_code ec;
        if(!alkaid::filesystem::exists(path, ec)) {
            response->set_status_code(404);
            response->set_header("Content-Type", "text/html; charset=utf-8");
            response->set_header("Vary", "Accept-Encoding");
            response->set_body("404 not found");
            return;
        }
        auto lfs = alkaid::Filesystem::localfs();
        if(!lfs) {
            response->set_status_code(500);
            response->set_header("Content-Type", "text/html; charset=utf-8");
            response->set_header("Vary", "Accept-Encoding");
            response->set_body("500 internal error");
            return;
        }
        std::string content;
        auto rs =lfs->read_file(path.string(),&content);
        if(!rs.ok()) {
            response->set_status_code(500);
            response->set_header("Content-Type", "text/html; charset=utf-8");
            response->set_header("Vary", "Accept-Encoding");
            response->set_body("500 internal error");
            return;
        }
        std::string ct = "text/html; charset=utf-8";
        if(path.extension() == ".css") {
            ct = "text/css; charset=utf-8";
        } else if(path.extension() == ".js") {
            ct = "application/javascript; charset=utf-8";
        } else if(path.extension() == ".png") {
            ct = "image/png";
        } else if(path.extension() == ".jpg") {
            ct = "image/jpeg";
        } else if(path.extension() == ".gif") {
            ct = "image/gif";
        } else if(path.extension() == ".ico") {
            ct = "image/x-icon";
        }
        LOG(INFO)<<"file path: "<<path.string()<<" content size: "<<content.size();
        response->set_status_code(200);
        response->set_header("Content-Type", ct);
        response->set_header("Vary", "Accept-Encoding");
        response->set_body(content);
    }

}  // namespace halavue