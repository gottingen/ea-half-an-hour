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

#[[
carbin_cc_test(
        NAME raw_test
        MODULE base
        SOURCES raw_test.cc
)

carbin_cc_test(
        NAME pass_test
        MODULE base
        SOURCES pass_test.cc
        EXT
)

# base_pass_test_1
carbin_cc_test_ext(
        NAME pass_test
        MODULE base
        ALIAS 1
        PASS_EXP "pass;Passed"
)

# base_pass_test_2
carbin_cc_test_ext(
        NAME pass_test
        MODULE base
        ALIAS 2
        PASS_EXP "pass;Passed"
)
# base_pass_test_foo
carbin_cc_test_ext(
        NAME pass_test
        MODULE base
        ALIAS foo
        PASS_EXP "pass;Passed"
)

# base_pass_test_bar not run
carbin_cc_test_ext(
        NAME pass_test
        MODULE base
        ALIAS bar
        PASS_EXP "pass;Passed"
        DISABLED
)


carbin_cc_test(
        NAME raw_test
        MODULE norun
        SOURCES raw_test.cc
)
]]

find_package(GTest REQUIRED)

carbin_cc_test(
        NAME add_test
        MODULE base
        SOURCES add_test.cc
        LINKS halatest::add GTest::gtest GTest::gtest_main
)


carbin_cc_test(
        NAME args_test
        MODULE base
        SOURCES args_test.cc
        EXT
)

# base_args_test_fail fail
carbin_cc_test_ext(
        NAME args_test
        MODULE base
        ALIAS fail
        FAIL_EXP "[^a-z]Error;ERROR;Failed"
)

# base_args_test_fail fail
carbin_cc_test_ext(
        NAME args_test
        MODULE base
        ALIAS fail_args
        ARGS "Failed"
        FAIL_EXP "[^a-z]Error;ERROR;Failed"
)

# base_args_test_skip fail
carbin_cc_test_ext(
        NAME args_test
        MODULE base
        ALIAS skip_fail
        PASS_EXP "pass;Passed"
        FAIL_EXP "[^a-z]Error;ERROR;Failed"
        SKIP_EXP "[^a-z]Skip" "SKIP" "Skipped"
)

# base_args_test_skip fail
carbin_cc_test_ext(
        NAME args_test
        MODULE base
        ALIAS skip
        ARGS "SKIP"
        PASS_EXP "pass;Passed"
        FAIL_EXP "[^a-z]Error;ERROR;Failed"
        SKIP_EXP "[^a-z]Skip" "SKIP" "Skipped"
)

# base_args_test_skip fail
carbin_cc_test_ext(
        NAME args_test
        MODULE base
        ALIAS skip_pass
        ARGS "Passed"
        PASS_EXP "pass;Passed"
        FAIL_EXP "[^a-z]Error;ERROR;Failed"
        SKIP_EXP "[^a-z]Skip" "SKIP" "Skipped"
)

# base_args_test_skip fail
carbin_cc_test_ext(
        NAME args_test
        MODULE base
        ALIAS skip_diabled
        ARGS "Passed"
        PASS_EXP "pass;Passed"
        FAIL_EXP "[^a-z]Error;ERROR;Failed"
        SKIP_EXP "[^a-z]Skip" "SKIP" "Skipped"
        DISABLE
)

