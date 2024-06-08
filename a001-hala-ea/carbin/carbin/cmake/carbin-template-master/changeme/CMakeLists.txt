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
        NAMESPACE changeme
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
        NAMESPACE changeme
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
        NAMESPACE changeme
        NAME share
        SOURCES
        shared.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        changeme::object
        PLINKS
        ${CARBIN_DEPS_LINK}
        changeme::object
        PUBLIC
)

carbin_cc_interface(
        NAMESPACE changeme
        NAME api
        HEADERS
        api.h
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        PUBLIC
)

carbin_cc_binary(
        NAMESPACE changeme
        NAME shared_main
        SOURCES
        main.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        changeme::share
        LINKS
        ${CARBIN_DEPS_LINK}
        changeme::share
        PUBLIC
)

carbin_cc_object(
        NAMESPACE changeme
        NAME obj1
        SOURCES object1.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        changeme::object
)

carbin_cc_object(
        NAMESPACE changeme
        NAME obj2
        SOURCES object2.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        changeme::object
)

carbin_cc_library(
        NAMESPACE changeme
        NAME obm_source
        OBJECTS obj1 obj2
        SOURCES shared.cc
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        changeme::object
        PLINKS
        ${CARBIN_DEPS_LINK}
        changeme::object
)

carbin_cc_library(
        NAMESPACE changeme
        NAME obm_no_source
        OBJECTS obj1 obj2
        CXXOPTS
        ${CARBIN_CXX_OPTIONS}
        DEPS
        changeme::object
        PLINKS
        ${CARBIN_DEPS_LINK}
        changeme::object
)

###carbin_example