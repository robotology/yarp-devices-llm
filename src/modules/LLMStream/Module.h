/*
 * SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MODULE_H
#define MODULE_H

#include <yarp/os/all.h>
#include <yarp/dev/ILLM.h>
#include <yarp/dev/PolyDriver.h>

class Module : public yarp::os::RFModule
{
    public:
        bool configure(yarp::os::ResourceFinder& rf) override;
        bool updateModule() override;
        bool close() override;
        bool interruptModule() override;
        double getPeriod() override;

    private:

        yarp::os::BufferedPort<yarp::os::Bottle> question_port_;
        yarp::os::BufferedPort<yarp::os::Bottle> answer_port_;
        yarp::dev::PolyDriver drv_;
        yarp::dev::ILLM* illm_;
        bool m_stream_answer_only{false};
};

#endif
