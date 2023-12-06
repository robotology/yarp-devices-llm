# SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_executable(LLMStream Module.h Module.cpp main.cpp)

find_package(YARP 3.8 REQUIRED COMPONENTS os sig dev)
target_link_libraries(LLMStream PRIVATE ${YARP_LIBRARIES})
install(TARGETS LLMStream DESTINATION bin)
