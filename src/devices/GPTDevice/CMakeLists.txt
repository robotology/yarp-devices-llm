# SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(GPTDevice
  CATEGORY device
  TYPE GPTDevice
  INCLUDE GPTDevice.h
  INTERNAL ON
)

find_package(CURL)
find_package(nlohmann_json CONFIG REQUIRED)

if(NOT SKIP_GPTDevice)
  yarp_add_plugin(yarp_GPTDevice)
  generateDeviceParamsParser(GPTDevice GPTDevice)

  target_sources(yarp_GPTDevice
    PRIVATE
      GPTDevice.cpp
      GPTDevice.h
      GPTDevice_ParamsParser.cpp
      GPTDevice_ParamsParser.h
  )

  cmake_path(SET LIBOAI_INCLUDE_PATH "${LIBOAI_INSTALL_PATH}/liboai/include")
  # cmake_path(SET LIBOAI_LIBRARY_PATH "${LIBOAI_INSTALL_PATH}/liboai/build/Release")

  target_link_directories(yarp_GPTDevice
   PUBLIC
    ${CMAKE_PREFIX_PATH}/lib
  )

  target_include_directories(yarp_GPTDevice
  PUBLIC
    "${LIBOAI_INCLUDE_PATH}"
    ${CMAKE_PREFIX_PATH}/include
    "${CMAKE_CURRENT_SOURCE_DIR}"
  )

  target_link_libraries(yarp_GPTDevice
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      oai
      CURL::libcurl
      nlohmann_json::nlohmann_json
  )

  yarp_install(
    TARGETS yarp_GPTDevice
    EXPORT yarp-device-GPTDevice
    COMPONENT yarp-device-GPTDevice
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

  set_property(TARGET yarp_GPTDevice PROPERTY FOLDER "Plugins/Device")

  message("YARP contexts install dir", ${YARP_CONTEXTS_INSTALL_DIR})
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/prompts/ DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/GPTDevice)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/functions/ DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/GPTDevice)
endif()
