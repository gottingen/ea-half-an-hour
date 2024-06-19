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
        NAMESPACE halakv
        NAME proto
        DEPS
        proto_obj
        OBJECTS
        proto_obj
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        PLINKS
        ${CARBIN_DEPS_LINK}
        PUBLIC
)
###carbin_example

file(COPY ${PROJECT_SOURCE_DIR}/www DESTINATION ${PROJECT_BINARY_DIR})
carbin_cc_binary(
        NAMESPACE halakv
        NAME kv_server
        SOURCES
        cache.cc
        kv_service.cc
        restful_service.cc
        web_service.cc
        server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halakv::proto
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halakv
        NAME kv_cli
        SOURCES
        client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK} halakv::proto
        PUBLIC
)