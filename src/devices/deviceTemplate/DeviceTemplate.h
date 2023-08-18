/*
 * SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEVICETEMPLATE_H
#define YARP_DEVICETEMPLATE_H

#include <yarp/dev/DeviceDriver.h>

class DeviceTemplate :
        public yarp::dev::DeviceDriver
{
public:
    DeviceTemplate();
    DeviceTemplate(const DeviceTemplate&) = delete;
    DeviceTemplate(DeviceTemplate&&) noexcept = delete;
    DeviceTemplate& operator=(const DeviceTemplate&) = delete;
    DeviceTemplate& operator=(DeviceTemplate&&) noexcept = delete;
    ~DeviceTemplate() override = default;

    // DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;
};

#endif // YARP_DEVICETEMPLATE_H
