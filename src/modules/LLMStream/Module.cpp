#include "Module.h"

bool Module::configure(yarp::os::ResourceFinder& rf)
{
    // Opening the llm nwc
    std::string remote_rpc = rf.check("remote",yarp::os::Value("/LLM_nws/rpc")).asString();

    yarp::os::Property prop;
    prop.put("device","LLM_nwc_yarp");
    prop.put("local","/LLM_nwc/rpc");
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

    std::string question_port_name = rf.check("in_port",yarp::os::Value("/LLMStream/text:i")).asString();
    question_port_.open(question_port_name);

    std::string answer_port_name = rf.check("out_port",yarp::os::Value("/LLMStream/text:o")).asString();
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

        std::string answer;
        illm_->ask(question->toString(),answer);

        auto& res = answer_port_.prepare();
        res.clear();
        res.addString(answer);
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

