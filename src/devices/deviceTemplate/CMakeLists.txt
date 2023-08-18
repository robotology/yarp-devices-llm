# SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(deviceTemplate
  CATEGORY device
  TYPE DeviceTemplate
  INCLUDE DeviceTemplate.h
  INTERNAL ON
)

if(NOT SKIP_deviceTemplate)
  yarp_add_plugin(yarp_deviceTemplate)

  target_sources(yarp_deviceTemplate
    PRIVATE
      DeviceTemplate.cpp
      DeviceTemplate.h
  )

  target_link_libraries(yarp_deviceTemplate
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )

  yarp_install(
    TARGETS yarp_deviceTemplate
    EXPORT yarp-device-deviceTemplate
    COMPONENT yarp-device-deviceTemplate
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

  set_property(TARGET yarp_deviceTemplate PROPERTY FOLDER "Plugins/Device")
endif()
