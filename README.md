![YARP logo](doc/images/yarp-robot-24.png?raw=true "yarp-devices-llm")
Yarp devices for Large Language Models
=====================

This repository contains the YARP plugin for Large Language Models.

:construction: This repository is currently work in progress.

:construction: The software contained is this repository is currently under testing. 

:construction: APIs may change without any warning.
 
:construction: This code should be not used before its first official release

Documentation
-------------

Documentation of the individual devices is provided in the official Yarp documentation page:
[![YARP documentation](https://img.shields.io/badge/Documentation-yarp.it-19c2d8.svg)](https://yarp.it/latest/group__dev__impl.html)

Prerequisites
-------------

To install GPTDevice we need:
- [nlohmann-json](https://github.com/nlohmann/json)
- [cURL](https://curl.se/)
- [liboai](https://github.com/D7EAD/liboai)

Based on `liboai` recommendations, it is recommended to install `nlohmann-json` and `libcurl` using `vcpkg`.

#### Install vcpkg, cURL and nlohmann-json
~~~
git clone https://github.com/Microsoft/vcpkg.git && \
    cd vcpkg && ./bootstrap-vcpkg.sh -disableMetrics && \
    ./vcpkg integrate install && ./vcpkg install curl && \
    ./vcpkg install nlohmann-json
~~~

#### Install liboai
~~~
git clone https://github.com/D7EAD/liboai.git && \
    cd liboai/liboai && \
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_POSITION_INDEPENDENT_CODE=true && \
    cd build && make -j8 && make install
~~~

Installation
-------------

### Build with pure CMake commands

~~~
# Configure, compile and install
cmake -S. -Bbuild -DCMAKE_INSTALL_PREFIX=<install_prefix> -DCMAKE_TOOLCHAIN_FILE="<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake" -DLIBOAI_INSTALL_PATH="<path-to-liboai>
cmake --build build
cmake --build build --target install
~~~

### Docker

In alternative one can build a docker image by running 
```
cd yarp-devices-llm
docker build -t <my-img-name> .
```

Configuration
-------------

You need to copy `.env-template` into a `.env` file with the correct information regarding your OpenAI deployment. Note that currently `GPTDevice` works only with an Azure instance of GPT.

Remember to source the `.env` file in order to allow the device to connect to your GPT instance.

```
set -a
source .env
set +a
```

CI Status
---------

:construction: This repository is currently work in progress. :construction:

[![Build Status](https://github.com/robotology/yarp-device-template/workflows/CI%20Workflow/badge.svg)](https://github.com/robotology/yarp-device-template/actions?query=workflow%3A%22CI+Workflow%22)

License
---------

:construction: This repository is currently work in progress. :construction:

Maintainers
--------------
This repository is maintained by:

| | |
|:---:|:---:|
| [<img src="https://github.com/fbrand-new.png" width="40">](https://github.com/fbrand-new) | [@fbrand-new](https://github.com/fbrand-new) |
