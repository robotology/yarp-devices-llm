/*
 * SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Module.h"

bool Module::configure(yarp::os::ResourceFinder& rf)
{
    // Opening the llm nwc
    std::string remote_rpc = rf.check("remote",yarp::os::Value("/LLM_nws/rpc")).asString();
    std::string local_name = rf.check("name",yarp::os::Value("GPTDevice")).asString();
    std::string local_rpc = "/"+local_name+"/LLM_nwc/rpc";
    if(rf.check("stream_answer_only"))
    {
        m_stream_answer_only = rf.find("stream_answer_only").asInt32() == 1;
    }

    yarp::os::Property prop;
    prop.put("device","LLM_nwc_yarp");
    prop.put("local",local_rpc);
    prop.put("remote",remote_rpc);

    if(!drv_.open(prop))
    {
        yError() << "Cannot open device LLM_nws_yarp";
        return EXIT_FAILURE;
    };

    if(!drv_.view(illm_))
    {
        yError() << "Cannot set ILLM interface";
        return EXIT_FAILURE;
    }

    std::string question_port_name = "/"+local_name+"/LLMStream/text:i";
    question_port_.open(question_port_name);

    std::string answer_port_name = "/"+local_name+"/LLMStream/text:o";
    answer_port_.open(answer_port_name);

    return true;
}

bool Module::updateModule()
{
    if(auto* question = question_port_.read())
    {
        if(!illm_)
        {
            yError() << "Interface ILLM not set, aborting";
        }

        yarp::dev::LLM_Message answer;
        illm_->ask(question->toString(),answer);

        auto& res = answer_port_.prepare();
        res.clear();
        if(m_stream_answer_only)
        {
            res.addString(answer.content);
        }
        else
        {
            res.copyPortable(answer,res);
        }

        // res.add(answer);
        // res.addString(answer);
        answer_port_.write();
    }

    return true;
}

bool Module::interruptModule()
{
    drv_.close();
    question_port_.interrupt();
    answer_port_.interrupt();
    return true;
}

bool Module::close()
{
    question_port_.close();
    answer_port_.close();
    drv_.close();
    return true;
}

double Module::getPeriod()
{
    return 0.01;
}
