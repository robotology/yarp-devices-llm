#include "Module.h"

bool Module::configure(yarp::os::ResourceFinder& rf)
{
    // Opening the llm nwc

    yarp::os::Property prop;
    prop.put("device","LLM_nwc_yarp");
    prop.put("local","/LLM_nwc/rpc");
    prop.put("remote","/yarpgpt/rpc");

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

    std::string question_port_name = rf.check("text_port",yarp::os::Value("/speechTranslate/text:i")).asString();
    question_port_.open(question_port_name);


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

        // yDebug() << answer;
    }

    return true;
}

bool Module::interruptModule()
{
    drv_.close();
    question_port_.interrupt();
    return true;
}

bool Module::close()
{
    question_port_.close();
    drv_.close();
    return true;
}

double Module::getPeriod()
{
    return 0.01;
}

