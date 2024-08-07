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


// A server to receive HttpRequest and send back HttpResponse.

#include <turbo/flags/flag.h>
#include <turbo/bootstrap/servlet.h>
#include <turbo/log/logging.h>
#include <melon/rpc/server.h>
#include <melon/rpc/restful.h>
#include <melon/json2pb/pb_to_json.h>
#include <halamelon/proto/http.pb.h>

TURBO_FLAG(int32_t, port, 8038, "TCP Port of this server");
TURBO_FLAG(int32_t, idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");

TURBO_FLAG(std::string, certificate, "cert.pem", "Certificate file path to enable SSL");
TURBO_FLAG(std::string, private_key, "key.pem", "Private key file path to enable SSL");
TURBO_FLAG(std::string, ciphers, "", "Cipher suite used for SSL connections");

namespace example {

// Service with static path.
class HttpServiceImpl : public HttpService {
public:
    HttpServiceImpl() {}
    virtual ~HttpServiceImpl() {}
    void Echo(google::protobuf::RpcController* cntl_base,
              const HttpRequest*,
              HttpResponse*,
              google::protobuf::Closure* done) {
        // This object helps you to call done->Run() in RAII style. If you need
        // to process the request asynchronously, pass done_guard.release().
        melon::ClosureGuard done_guard(done);

        melon::Controller* cntl =
            static_cast<melon::Controller*>(cntl_base);

        // optional: set a callback function which is called after response is sent
        // and before cntl/req/res is destructed.
        cntl->set_after_rpc_resp_fn(std::bind(&HttpServiceImpl::CallAfterRpc,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        // Fill response.
        cntl->http_response().set_content_type("text/plain");
        mutil::IOBufBuilder os;
        os << "queries:";
        for (melon::URI::QueryIterator it = cntl->http_request().uri().QueryBegin();
                it != cntl->http_request().uri().QueryEnd(); ++it) {
            os << ' ' << it->first << '=' << it->second;
        }
        os << "\nbody: " << cntl->request_attachment() << '\n';
        os.move_to(cntl->response_attachment());
    }

    // optional
    static void CallAfterRpc(melon::Controller* cntl,
                        const google::protobuf::Message* req,
                        const google::protobuf::Message* res) {
        // at this time res is already sent to client, but cntl/req/res is not destructed
        std::string req_str;
        std::string res_str;
        json2pb::ProtoMessageToJson(*req, &req_str, NULL);
        json2pb::ProtoMessageToJson(*res, &res_str, NULL);
        LOG(INFO) << "req:" << req_str
                    << " res:" << res_str;
    }
};

// Service with dynamic path.
class FileServiceImpl : public FileService {
public:
    FileServiceImpl() {}
    virtual ~FileServiceImpl() {}

    struct Args {
        mutil::intrusive_ptr<melon::ProgressiveAttachment> pa;
    };

    static void* SendLargeFile(void* raw_args) {
        std::unique_ptr<Args> args(static_cast<Args*>(raw_args));
        if (args->pa == NULL) {
            LOG(ERROR) << "ProgressiveAttachment is NULL";
            return NULL;
        }
        for (int i = 0; i < 100; ++i) {
            char buf[16];
            int len = snprintf(buf, sizeof(buf), "part_%d ", i);
            args->pa->Write(buf, len);

            // sleep a while to send another part.
            fiber_usleep(10000);
        }
        return NULL;
    }

    void default_method(google::protobuf::RpcController* cntl_base,
                        const HttpRequest*,
                        HttpResponse*,
                        google::protobuf::Closure* done) {
        melon::ClosureGuard done_guard(done);
        melon::Controller* cntl =
            static_cast<melon::Controller*>(cntl_base);
        const std::string& filename = cntl->http_request().unresolved_path();
        if (filename == "largefile") {
            // Send the "largefile" with ProgressiveAttachment.
            std::unique_ptr<Args> args(new Args);
            args->pa = cntl->CreateProgressiveAttachment();
            fiber_t th;
            fiber_start_background(&th, NULL, SendLargeFile, args.release());
        } else {
            cntl->response_attachment().append("Getting file: ");
            cntl->response_attachment().append(filename);
        }
    }
};

// Restful service. (The service implementation is exactly same with regular
// services, the difference is that you need to pass a `restful_mappings'
// when adding the service into server).
class QueueServiceImpl : public example::QueueService {
public:
    QueueServiceImpl() {}
    virtual ~QueueServiceImpl() {}
    void start(google::protobuf::RpcController* cntl_base,
               const HttpRequest*,
               HttpResponse*,
               google::protobuf::Closure* done) {
        melon::ClosureGuard done_guard(done);
        melon::Controller* cntl =
            static_cast<melon::Controller*>(cntl_base);
        cntl->response_attachment().append("queue started");
    }
    void stop(google::protobuf::RpcController* cntl_base,
              const HttpRequest*,
              HttpResponse*,
              google::protobuf::Closure* done) {
        melon::ClosureGuard done_guard(done);
        melon::Controller* cntl =
            static_cast<melon::Controller*>(cntl_base);
        cntl->response_attachment().append("queue stopped");
    }
    void getstats(google::protobuf::RpcController* cntl_base,
                  const HttpRequest*,
                  HttpResponse*,
                  google::protobuf::Closure* done) {
        melon::ClosureGuard done_guard(done);
        melon::Controller* cntl =
            static_cast<melon::Controller*>(cntl_base);
        const std::string& unresolved_path = cntl->http_request().unresolved_path();
        if (unresolved_path.empty()) {
            cntl->response_attachment().append("Require a name after /stats");
        } else {
            cntl->response_attachment().append("Get stats: ");
            cntl->response_attachment().append(unresolved_path);
        }
    }
};

class HttpSSEServiceImpl : public HttpSSEService {
public:
    HttpSSEServiceImpl() {}
    virtual ~HttpSSEServiceImpl() {}

    struct PredictJobArgs {
        std::vector<uint32_t> input_ids;
        mutil::intrusive_ptr<melon::ProgressiveAttachment> pa;
    };

    static void* Predict(void* raw_args) {
        std::unique_ptr<PredictJobArgs> args(static_cast<PredictJobArgs*>(raw_args));
        if (args->pa == NULL) {
            LOG(ERROR) << "ProgressiveAttachment is NULL";
            return NULL;
        }
        for (int i = 0; i < 100; ++i) {
            char buf[48];
            int len = snprintf(buf, sizeof(buf), "event: foo\ndata: Hello, world! (%d)\n\n", i);
            args->pa->Write(buf, len);

            // sleep a while to send another part.
            fiber_usleep(10000 * 10);
        }
        return NULL;
    }

    void stream(google::protobuf::RpcController* cntl_base,
                const HttpRequest*,
                HttpResponse*,
                google::protobuf::Closure* done) {
        melon::ClosureGuard done_guard(done);
        melon::Controller* cntl =
            static_cast<melon::Controller*>(cntl_base);

        // Send the first SSE response
        cntl->http_response().set_content_type("text/event-stream");
        cntl->http_response().set_status_code(200);
        cntl->http_response().SetHeader("Connection", "keep-alive");
        cntl->http_response().SetHeader("Cache-Control", "no-cache");

        // Send the generated words with progressiveAttachment
        std::unique_ptr<PredictJobArgs> args(new PredictJobArgs);
        args->pa = cntl->CreateProgressiveAttachment();
        args->input_ids = {101, 102};
        fiber_t th;
        fiber_start_background(&th, NULL, Predict, args.release());
    }
};

}  // namespace example

int main(int argc, char* argv[]) {
    // Generally you only need one Server.
    auto *run_app = turbo::Servlet::instance().run_app();
    run_app->add_option("--port", FLAGS_port, FLAGS_port.help());
    run_app->add_option("--idle_timeout_s", FLAGS_idle_timeout_s, FLAGS_idle_timeout_s.help());
    run_app->add_option("--certificate", FLAGS_certificate, FLAGS_certificate.help());
    run_app->add_option("--private_key", FLAGS_private_key, FLAGS_private_key.help());
    run_app->add_option("--ciphers", FLAGS_ciphers, FLAGS_ciphers.help());
    auto [exit, ret] = turbo::Servlet::instance().run(argc, argv);
    if (exit) {
        return ret;
    }
    LOG(INFO)<<"certificate:"<<turbo::get_flag(FLAGS_certificate);
    LOG(INFO)<<"private_key:"<<turbo::get_flag(FLAGS_private_key);
    melon::Server server;
    //turbo::setup_color_stderr_sink();
    example::HttpServiceImpl http_svc;
    example::FileServiceImpl file_svc;
    example::QueueServiceImpl queue_svc;
    example::HttpSSEServiceImpl sse_svc;
    
    // Add services into server. Notice the second parameter, because the
    // service is put on stack, we don't want server to delete it, otherwise
    // use melon::SERVER_OWNS_SERVICE.
    if (server.AddService(&http_svc,
                          melon::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add http_svc";
        return -1;
    }
    if (server.AddService(&file_svc,
                          melon::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add file_svc";
        return -1;
    }
    if (server.AddService(&queue_svc,
                          melon::SERVER_DOESNT_OWN_SERVICE,
                          "/v1/queue/start   => start,"
                          "/v1/queue/stop    => stop,"
                          "/v1/queue/stats/* => getstats") != 0) {
        LOG(ERROR) << "Fail to add queue_svc";
        return -1;
    }
    if (server.AddService(&sse_svc,
                          melon::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add sse_svc";
        return -1;
    }

    // Start the server.
    melon::ServerOptions options;
    options.idle_timeout_sec = turbo::get_flag(FLAGS_idle_timeout_s);
    options.mutable_ssl_options()->default_cert.certificate = turbo::get_flag(FLAGS_certificate);
    options.mutable_ssl_options()->default_cert.private_key = turbo::get_flag(FLAGS_private_key);
    options.mutable_ssl_options()->ciphers = turbo::get_flag(FLAGS_ciphers);
    if (server.Start(turbo::get_flag(FLAGS_port), &options) != 0) {
        LOG(ERROR) << "Fail to start HttpServer";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();
    return 0;
}
