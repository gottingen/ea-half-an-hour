#
# Copyright 2023 The titan-search Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
###########################################################################
# below are examples, after initialization, modify it to your own.
###########################################################################
###carbin_example

carbin_cc_library(
        NAMESPACE halamelon
        NAME proto
        OBJECTS proto_object
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        proto_object
        PLINKS
        ${CARBIN_DEPS_LINK}
        PUBLIC
)
#######################
# async echo
carbin_cc_binary(
        NAMESPACE halamelon
        NAME async_echo_server
        SOURCES async/server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)
########################
# cancel
carbin_cc_binary(
        NAMESPACE halamelon
        NAME cancel_client
        SOURCES cancel/client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME cancel_server
        SOURCES cancel/server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

########################
# echo
carbin_cc_binary(
        NAMESPACE halamelon
        NAME echo_client
        SOURCES echo/client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME echo_server
        SOURCES echo/server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

########################
# grpc
carbin_cc_binary(
        NAMESPACE halamelon
        NAME grpc_client
        SOURCES grpc/client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME grpc_server
        SOURCES grpc/server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

########################
# http
file(COPY http/cert.pem DESTINATION ${CMAKE_BINARY_DIR})
file(COPY http/key.pem DESTINATION ${CMAKE_BINARY_DIR})
carbin_cc_binary(
        NAMESPACE halamelon
        NAME http_client
        SOURCES http/http_client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME http_server
        SOURCES http/http_server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME benchmark_http
        SOURCES http/benchmark_http.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

########################
# parallel
carbin_cc_binary(
        NAMESPACE halamelon
        NAME parallel_client
        SOURCES parallel/client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME parallel_server
        SOURCES parallel/server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

########################
# partition
file(COPY partition/server_list DESTINATION ${CMAKE_BINARY_DIR})

carbin_cc_binary(
        NAMESPACE halamelon
        NAME partition_client
        SOURCES partition/client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME partition_server
        SOURCES partition/server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

########################
# redis

carbin_cc_binary(
        NAMESPACE halamelon
        NAME redis_cli
        SOURCES redis/redis_cli.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto readline
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME redis_press
        SOURCES redis/redis_press.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME redis_server
        SOURCES redis/redis_server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

########################
# select

carbin_cc_binary(
        NAMESPACE halamelon
        NAME select_client
        SOURCES select/client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME select_server
        SOURCES select/server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

########################
# session

carbin_cc_binary(
        NAMESPACE halamelon
        NAME session_client
        SOURCES session/client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME session_server
        SOURCES session/server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

########################
# stream

carbin_cc_binary(
        NAMESPACE halamelon
        NAME stream_client
        SOURCES stream/client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME stream_server
        SOURCES stream/server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

########################
# backup

carbin_cc_binary(
        NAMESPACE halamelon
        NAME backup_client
        SOURCES backup/client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME backup_server
        SOURCES backup/server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

########################
# cascade

carbin_cc_binary(
        NAMESPACE halamelon
        NAME cascade_client
        SOURCES cascade/client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halamelon
        NAME cascade_server
        SOURCES cascade/server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halamelon::proto
        PUBLIC
)
