/*
 * SPDX-FileCopyrightText: 2023-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <GPTDevice.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <fstream>
#include <string_view>

using json = nlohmann::json;

bool GPTDevice::open(yarp::os::Searchable &config)
{
    // Azure settings
    azure_api_version = config.check("api_version", yarp::os::Value("2023-07-01-preview")).asString();
    azure_deployment_id = std::getenv("DEPLOYMENT_ID");
    azure_resource = std::getenv("AZURE_RESOURCE");

    if (!azure_resource)
    {
        yWarning() << "Could not read env variable AZURE_RESOURCE. Device set in offline mode";
        m_offline = true;
        return true;
    }

    if (!oai.auth.SetAzureKeyEnv("AZURE_API_KEY"))
    {
        yWarning() << "Invalid or no azure key provided. Device set in offline mode.";
        m_offline = true;
    }

    // Prompt and functions file
    bool has_prompt_file{config.check("prompt_file")};
    yarp::os::ResourceFinder resource_finder;
    std::string prompt_ctx = config.check("prompt_context",yarp::os::Value("GPTDevice")).asString();
    resource_finder.setDefaultContext(prompt_ctx);

    if(has_prompt_file)
    {
        std::string prompt_file_fullpath = resource_finder.findFile(config.find("prompt_file").asString());
        auto stream = std::ifstream(prompt_file_fullpath);
        if (!stream)
        {
            yWarning() << "File:" << prompt_file_fullpath << "does not exist or path is invalid";
        }
        else
        {
            std::ostringstream sstr;
            sstr << stream.rdbuf(); //Reads the entire file into the stringstream
            if(!setPrompt(sstr.str()))
            {
                return false;
            }
        }
    }

    bool has_function_file{config.check("functions_file")};
    std::string json_ctx = config.check("json_context",yarp::os::Value(prompt_ctx)).asString();
    resource_finder.setDefaultContext(json_ctx);
    if(has_function_file)
    {
        std::string functions_file_fullpath = resource_finder.findFile(config.find("functions_file").asString());
        auto stream = std::ifstream(functions_file_fullpath);
        if (!stream)
        {
            yWarning() << "File: " << functions_file_fullpath << "does not exist or path is invalid.";
        }
        else
        {
            // Read the function file into json format
            // yDebug() << functions_file_fullpath;
            json function_js = json::parse(stream);
            if (!setFunctions(function_js))
            {
                return false;
            }
        }
    }

    return true;
}

yarp::dev::ReturnValue GPTDevice::ask(const std::string &question, yarp::dev::LLM_Message &oAnswer)
{
    // Adding prompt to conversation
    m_convo->AddUserData(question);
    m_convo_length += 1;

    if (m_offline)
    {
        yWarning() << "Device in offline mode";
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    // Asking gpt for an answer
    try
    {
        liboai::Response res = oai.Azure->create_chat_completion(
            azure_resource, azure_deployment_id, azure_api_version,
            *m_convo);
        m_convo->Update(res);
    }
    catch (const std::exception &e)
    {
        yError() << e.what() << '\n';
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    if(m_convo->LastResponseIsFunctionCall())
    {
        yDebug() << "Last answer was function call";
        auto str_args = m_convo->GetLastFunctionCallArguments();
        std::string function_call_name = m_convo->GetLastFunctionCallName();
        auto j_args = json::parse(str_args);

        std::vector<std::string> parameters_list;
        std::vector<std::string> arguments_list;

        for(const auto&[key,val]: j_args.items())
        {
            parameters_list.push_back(key);
            arguments_list.push_back(val);
        }

        auto function_call_message = yarp::dev::LLM_Message{"function",
                                        function_call_name,
                                        parameters_list,
                                        arguments_list};

        m_function_called.insert({m_convo_length,function_call_message});

        oAnswer = function_call_message;
    }
    else
    {
        oAnswer = yarp::dev::LLM_Message{"assistant",
                                            m_convo->GetLastResponse(),
                                            std::vector<std::string>(),
                                            std::vector<std::string>()};
    }

    m_convo_length+=1;
    return yarp::dev::ReturnValue_ok;
}

yarp::dev::ReturnValue GPTDevice::setPrompt(const std::string &prompt)
{

    std::string aPrompt;

    if(readPrompt(aPrompt))
    {
        yError() << "A prompt is already set. You must delete conversation first";
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    try
    {
        m_convo->SetSystemData(prompt);
    }
    catch (const std::exception &e)
    {
        yError() << e.what() << '\n';
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    return yarp::dev::ReturnValue_ok;
}

yarp::dev::ReturnValue GPTDevice::readPrompt(std::string &oPrompt)
{
    auto &convo_json = m_convo->GetJSON();
    for (auto &message : convo_json["messages"])
    {
        if (message["role"] == "system")
        {
            oPrompt = message["content"];
            return yarp::dev::ReturnValue_ok;
        }
    }

    return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
}

yarp::dev::ReturnValue GPTDevice::getConversation(std::vector<yarp::dev::LLM_Message> &oConversation)
{
    std::vector<yarp::dev::LLM_Message> conversation;

    auto &convo_json = m_convo->GetJSON();


    if (convo_json["messages"].empty())
    {
        yWarning() << "Conversation is empty!";
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    for (auto &message : convo_json["messages"])
    {
        std::string type = message["role"].get<std::string>();
        std::string content = message["content"].get<std::string>();

        conversation.push_back(yarp::dev::LLM_Message{type, content,std::vector<std::string>(),std::vector<std::string>()});
    }

    // Adding function calls to the conversation
    for(const auto& [i,answer]: m_function_called)
    {
        auto conv_it = conversation.begin();
        conversation.insert(std::next(conv_it,i),answer);
    }

    oConversation = conversation;
    return yarp::dev::ReturnValue_ok;
}

yarp::dev::ReturnValue GPTDevice::deleteConversation() noexcept
{
    // Api does not provide a method to empty the conversation: we are better off if we rebuild an object from scratch
    m_convo.reset(new liboai::Conversation());
    m_convo_length = 0;
    m_function_called.clear();
    return yarp::dev::ReturnValue_ok;
}

yarp::dev::ReturnValue GPTDevice::refreshConversation() noexcept
{
    std::string current_prompt = "";
    this->readPrompt(current_prompt);
    this->deleteConversation();
    this->setPrompt(current_prompt);
    return yarp::dev::ReturnValue_ok;
}

bool GPTDevice::close()
{
    return true;
}

bool GPTDevice::setFunctions(const json& function_json)
{

    for (auto& function: function_json.items())
    {
        if(!function.value().contains("name") || !function.value().contains("description"))
        {
            yError() << "Function missing mandatory parameters <name> and/or <description>";
            return false;
        }

        std::string function_name = function.value()["name"].template get<std::string>();
        std::string function_desc = function.value()["description"].template get<std::string>();

        if(!m_functions->AddFunction(function_name))
        {
            yError() << module_name + "::setFunctions(). Cannot add function.";
            return false;
        }

        if(!m_functions->SetDescription(function_name,function_desc))
        {
            yError() << module_name + "::setFunctions(). Cannot set description";
            return false;
        }

        if(function.value().contains("parameters"))
        {
            auto parameters = function.value()["parameters"]["properties"];
            std::vector<liboai::Functions::FunctionParameter> parameters_vec;
            for(auto& params: parameters.items())
            {
                liboai::Functions::FunctionParameter param;
                param.name = params.key();
                param.description = params.value()["description"];
                param.type = params.value()["type"];
                parameters_vec.push_back(param);
            }
            if(!m_functions->SetParameters(function_name,parameters_vec))
            {
                yError() << module_name + "::setFunction(). Cannot set parameters";
                return false;
            }
        }
    }

    if(!m_convo->SetFunctions(*m_functions))
    {
        yError() << module_name + "::setFunction(). Cannot set function";
        return false;
    }

    return true;
}
