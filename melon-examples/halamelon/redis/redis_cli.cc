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


// A melon based command-line interface to talk with redis-server

#include <signal.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <turbo/flags/flag.h>
#include <turbo/log/logging.h>
#include <melon/rpc/channel.h>
#include <melon/rpc/redis/redis.h>
#include <turbo/bootstrap/servlet.h>

TURBO_FLAG(std::string, connection_type, "", "Connection type. Available values: single, pooled, short");
TURBO_FLAG(std::string, server, "127.0.0.1:6379", "IP Address of server");
TURBO_FLAG(int32_t, timeout_ms, 1000, "RPC timeout in milliseconds");
TURBO_FLAG(int32_t, max_retry, 3, "Max retries(not including the first RPC)");

namespace melon {
    const char *logo();
}

// Send `command' to redis-server via `channel'
static bool access_redis(melon::Channel &channel, const char *command) {
    melon::RedisRequest request;
    if (!request.AddCommand(command)) {
        LOG(ERROR) << "Fail to add command";
        return false;
    }
    melon::RedisResponse response;
    melon::Controller cntl;
    channel.CallMethod(nullptr, &cntl, &request, &response, nullptr);
    if (cntl.Failed()) {
        LOG(ERROR) << "Fail to access redis, " << cntl.ErrorText();
        return false;
    } else {
        std::cout << response << std::endl;
        return true;
    }
}

// For freeing the memory returned by readline().
struct Freer {
    void operator()(char *mem) {
        free(mem);
    }
};

static void dummy_handler(int) {}

// The getc for readline. The default getc retries reading when meeting
// EINTR, which is not what we want.
static bool g_canceled = false;

static int cli_getc(FILE *stream) {
    int c = getc(stream);
    if (c == EOF && errno == EINTR) {
        g_canceled = true;
        return '\n';
    }
    return c;
}

int main(int argc, char *argv[]) {
    auto &servlet = turbo::Servlet::instance();
    auto *app = servlet.run_app();
    app->add_option("-c,--connection_type", FLAGS_connection_type, "Connection type. Available values: single, pooled, short");
    app->add_option("-s,--server", FLAGS_server, "IP Address of server");
    app->add_option("-m,--timeout_ms", FLAGS_timeout_ms, "RPC timeout in milliseconds");
    app->add_option("-r,--max_retry", FLAGS_max_retry, "Max retries(not including the first RPC)");
    auto [ext, code] = servlet.run(argc, argv);
    if (ext) {
        return code;
    }
    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    melon::Channel channel;

    // Initialize the channel, nullptr means using default options.
    melon::ChannelOptions options;
    options.protocol = melon::PROTOCOL_REDIS;
    options.connection_type = turbo::get_flag(FLAGS_connection_type);
    options.timeout_ms = turbo::get_flag(FLAGS_timeout_ms)/*milliseconds*/;
    options.max_retry = turbo::get_flag(FLAGS_max_retry);
    if (channel.Init(turbo::get_flag(FLAGS_server).c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    if (argc <= 1) {  // interactive mode
        // We need this dummy signal hander to interrupt getc (and returning
        // EINTR), SIG_IGN did not work.
        signal(SIGINT, dummy_handler);

        // Hook getc of readline.
        rl_getc_function = cli_getc;

        // Print welcome information.
        printf("%s\n", melon::logo());
        printf("This command-line tool mimics the look-n-feel of official "
               "redis-cli, as a demostration of melon's capability of"
               " talking to redis-server. The output and behavior is "
               "not exactly same with the official one.\n\n");

        for (;;) {
            char prompt[64];
            snprintf(prompt, sizeof(prompt), "redis %s> ", turbo::get_flag(FLAGS_server).c_str());
            std::unique_ptr<char, Freer> command(readline(prompt));
            if (command == nullptr || *command == '\0') {
                if (g_canceled) {
                    // No input after the prompt and user pressed Ctrl-C,
                    // quit the CLI.
                    return 0;
                }
                // User entered an empty command by just pressing Enter.
                continue;
            }
            if (g_canceled) {
                // User entered sth. and pressed Ctrl-C, start a new prompt.
                g_canceled = false;
                continue;
            }
            // Add user's command to history so that it's browse-able by
            // UP-key and search-able by Ctrl-R.
            add_history(command.get());

            if (!strcmp(command.get(), "help")) {
                printf("This is a redis CLI written in melon.\n");
                continue;
            }
            if (!strcmp(command.get(), "quit")) {
                // Although quit is a valid redis command, it does not make
                // too much sense to run it in this CLI, just quit.
                return 0;
            }
            access_redis(channel, command.get());
        }
    } else {
        std::string command;
        command.reserve(argc * 16);
        for (int i = 1; i < argc; ++i) {
            if (i != 1) {
                command.push_back(' ');
            }
            command.append(argv[i]);
        }
        if (!access_redis(channel, command.c_str())) {
            return -1;
        }
    }
    return 0;
}
