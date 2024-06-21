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
// Created by jeff on 24-6-14.
//

#include <melon/rpc/restful_service.h>
#include <halakv/restful_service.h>
#include <halakv/kv_service.h>
#include <halakv/web_service.h>
#include <melon/rpc/server.h>
#include "version.h"
#include <halakv/cache.h>
#include <halakv/kv_proxy.h>
DEFINE_string(peers, "127.0.0.1:8018,127.0.0.1:8019,127.0.0.1:8020", "TCP Port of this server");
DEFINE_string(local_peer, "", "TCP Port of this server");
DEFINE_int32(cache_size, 10, "TCP Port of this server");
DEFINE_string(root_path, "www", "TCP Port of this server");
DEFINE_int32(idle_timeout_s, -1, "Connection will be closed if there is no "
                                 "read/write operations during the last `idle_timeout_s'");

DEFINE_string(certificate, "cert.pem", "Certificate file path to enable SSL");
DEFINE_string(private_key, "key.pem", "Private key file path to enable SSL");
DEFINE_string(ciphers, "", "Cipher suite used for SSL connections");


int main(int argc, char* argv[]) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    turbo::setup_color_stderr_sink();
    // Generally you only need one Server.
    melon::Server server;
    halakv::WebServie vue_service(FLAGS_root_path);

    halakv::Cache cache;
    auto rs = cache.init(FLAGS_cache_size);
    if(!rs.ok()) {
        LOG(ERROR) << "init cache failed: " << rs;
        return -1;
    }
    halakv::KvProxy* kv_proxy = halakv::KvProxy::instance();
    rs = kv_proxy->initialize(FLAGS_peers, FLAGS_local_peer, &cache);
    if(!rs.ok()) {
        LOG(ERROR) << "init kv proxy failed: " << rs;
        return -1;
    }
    halakv::CacheService cache_service(&vue_service);
    rs = cache_service.register_server("/ea", &server);
    if(!rs.ok()) {
        LOG(ERROR) << "register server failed: " << rs;
        return -1;
    }

    halakv::KvServiceimpl kv_service;
    if(server.AddService(&kv_service,melon::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add kv service";
        return -1;
    }
    melon::ServerOptions options;
    options.idle_timeout_sec = FLAGS_idle_timeout_s;
    //options.mutable_ssl_options()->default_cert.certificate = FLAGS_certificate;
    //options.mutable_ssl_options()->default_cert.private_key = FLAGS_private_key;
    //options.mutable_ssl_options()->ciphers = FLAGS_ciphers;
    if (server.Start(FLAGS_local_peer.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to start HttpServer";
        return -1;
    }
    server.RunUntilAskedToQuit();
    return 0;
}