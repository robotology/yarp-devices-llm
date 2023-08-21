# SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(GPTDevice
  CATEGORY device
  TYPE GPTDevice
  INCLUDE GPTDevice.h
  INTERNAL ON
)

find_package(CURL)

if(NOT SKIP_GPTDevice)
  yarp_add_plugin(yarp_GPTDevice)

  target_sources(yarp_GPTDevice
    PRIVATE
      GPTDevice.cpp
      GPTDevice.h
  )

  target_link_libraries(yarp_GPTDevice
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      oai
      CURL::libcurl
  )

  target_include_directories(yarp_GPTDevice
    PUBLIC
      "${LIBOAI_INSTALL_PATH}/liboai/include"
      "${CMAKE_CURRENT_SOURCE_DIR}"
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
endif()
