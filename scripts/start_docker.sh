# SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

#!/bin/bash

docker run --privileged --network host --pid host --rm -it -v /usr/local/src/robotology/yarp-devices-llm/.env:/home/user/yarp-devices-llm/.env fbrand-new/yarp-devices-llm:latest bash
