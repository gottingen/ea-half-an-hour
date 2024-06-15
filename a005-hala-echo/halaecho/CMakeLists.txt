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
        NAMESPACE halaecho
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

carbin_cc_binary(
        NAMESPACE halaecho
        NAME echo_cli
        SOURCES
       client.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK}
        halaecho::proto_static
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halaecho
        NAME echo_server
        SOURCES
        server.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        LINKS
        ${CARBIN_DEPS_LINK}
        halaecho::proto_static
        PUBLIC
)


###carbin_example