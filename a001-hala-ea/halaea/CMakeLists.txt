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
        NAMESPACE halaea
        NAME foo
        SOURCES
        foo.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        PLINKS
        ${CARBIN_DEPS_LINK}
        PUBLIC
)

carbin_cc_library(
        NAMESPACE halaea
        NAME object
        SOURCES
        object.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        PLINKS
        ${CARBIN_DEPS_LINK}
        PUBLIC
)

carbin_cc_library(
        NAMESPACE halaea
        NAME share
        SOURCES
        shared.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        halaea::object
        PLINKS
        ${CARBIN_DEPS_LINK}
        halaea::object
        PUBLIC
)

carbin_cc_interface(
        NAMESPACE halaea
        NAME api
        HEADERS
        api.h
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE halaea
        NAME shared_main
        SOURCES
        main.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        halaea::share
        LINKS
        ${CARBIN_DEPS_LINK}
        halaea::share
        PUBLIC
)

carbin_cc_object(
        NAMESPACE halaea
        NAME obj1
        SOURCES object1.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        halaea::object
)

carbin_cc_object(
        NAMESPACE halaea
        NAME obj2
        SOURCES object2.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        halaea::object
)

carbin_cc_library(
        NAMESPACE halaea
        NAME obm_source
        OBJECTS obj1 obj2
        SOURCES shared.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        halaea::object
        PLINKS
        ${CARBIN_DEPS_LINK}
        halaea::object
)

carbin_cc_library(
        NAMESPACE halaea
        NAME obm_no_source
        OBJECTS obj1 obj2
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        halaea::object
        PLINKS
        ${CARBIN_DEPS_LINK}
        halaea::object
)

###carbin_example