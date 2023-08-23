![YARP logo](https://github.com/robotology/yarp/raw/master/doc/images/yarp-robot-24.png?raw=true)
Yarp devices for Large Language Models
=====================

This repository contains the YARP plugin for Large Language Models.

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

Usage
---------

Assuming that the user has already installed [YARP](https://www.github.com/robotology/yarp) and the related LLM devices, one can use this basic configuration example

~~~
yarpserver
~~~

~~~
set -a
source .env
set +a
yarprobotinterface --from assets/LLMDeviceExample.xml
~~~

~~~
yarp rpc /yarpgpt/rpc
>>help
Responses:
  *** Available commands:
  setPrompt
  readPrompt
  ask
  getConversation
  deleteConversation
  help
>>setPrompt "You are a geography expert who is very concise in its answers"
Response: [ok]
>>ask "What is the capital of Italy?"
Response: [ok] Rome. #This answer may vary in the form, hopefully not in the content.
~~~
CI Status
---------

[![Build Status](https://github.com/robotology/yarp-devices-llm/workflows/CI%20Workflow/badge.svg)](https://github.com/robotology/yarp-devices-llm/actions?query=workflow%3A%22CI+Workflow%22)

License
---------

[![License](https://img.shields.io/badge/License-BSD_3--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

This software is distributed under the terms of the BSD-3-Clause.


Maintainers
--------------
This repository is maintained by:

| | |
|:---:|:---:|
| [<img src="https://github.com/fbrand-new.png" width="40">](https://github.com/fbrand-new) | [@fbrand-new](https://github.com/fbrand-new) |
