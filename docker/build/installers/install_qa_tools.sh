#!/usr/bin/env bash

###############################################################################
# Copyright 2020 The Apollo Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###############################################################################

# Fail on first error.
set -e

cd "$(dirname "${BASH_SOURCE[0]}")"

. /tmp/installers/installer_base.sh

TARGET_ARCH="$(uname -m)"

## NOTE:
## buildifier/buildozer was moved into install_bazel.sh.

apt-get -y update && \
    apt-get -y install \
    cppcheck    \
    shellcheck  \
    lcov        \
    valgrind    \
    libgoogle-perftools4  # gperftools

ln -s /usr/lib/${TARGET_ARCH}-linux-gnu/libprofiler.so.0 /usr/lib/${TARGET_ARCH}-linux-gnu/libprofiler.so

## Pylint
pip3_install pycodestyle
pip3_install pyflakes
pip3_install flake8
# pip3_install pylint

# Clean up cache to reduce layer size.
apt-get clean && \
    rm -rf /var/lib/apt/lists/*
