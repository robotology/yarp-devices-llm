/*
 * SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <GPTDevice.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <filesystem>
#include <fstream>
// #include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

bool GPTDevice::open(yarp::os::Searchable &config)
{
    // Azure settings
    azure_api_version = config.check("api_version", yarp::os::Value("2023-05-15")).asString();
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
    resource_finder.setDefaultContext("GPTDevice");

    if(has_prompt_file)
    {    
        std::string prompt_file_fullpath = resource_finder.findFile(config.find("prompt_file").asString());
        auto stream = std::ifstream(prompt_file_fullpath);
        if (not stream)
        {
            yWarning() << "File:" << prompt_file_fullpath << "does not exist or path is invalid";
        }
        else
        {
            std::ostringstream sstr;
            sstr << stream.rdbuf(); //Reads the entire file into the stringstream
            yDebug() << "THE PROMPT:" << sstr.str();
            // TODO: uncomment me once done
            // if( not setPrompt(sstr.str()))
            // {
            //     return false;
            // }
        } 
    }

    bool has_function_file{config.check("functions_file")};
    if(has_function_file)
    {
        std::string functions_file_fullpath = resource_finder.findFile(config.find("functions_file").asString());
        auto stream = std::ifstream(functions_file_fullpath);
        if (not stream)
        {
            yWarning() << "File: " << functions_file_fullpath << "does not exist or path is invalid.";
        }
        else
        {
            // Read the function file into json format
            // yDebug() << functions_file_fullpath;
            json function_js = json::parse(stream); 
            // TODO: uncomment me once done
            if (not setFunctions(function_js))
            {
                return false;
            }
        }
    }

    return true;
}

bool GPTDevice::ask(const std::string &question, std::string &oAnswer)
{
    // Adding prompt to conversation
    m_convo->AddUserData(question);

    if (m_offline)
    {
        yWarning() << "Device in offline mode";
        return false;
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
        return false;
    }

    oAnswer = m_convo->GetLastResponse();
    return true;
}

bool GPTDevice::setPrompt(const std::string &prompt)
{

    std::string aPrompt;

    if(readPrompt(aPrompt))
    {
        yError() << "A prompt is already set. You must delete conversation first";
        return false;
    }

    try
    {
        m_convo->SetSystemData(prompt);
    }
    catch (const std::exception &e)
    {
        yError() << e.what() << '\n';
        return false;
    }

    return true;
}

bool GPTDevice::readPrompt(std::string &oPrompt)
{
    auto &convo_json = m_convo->GetJSON();
    for (auto &message : convo_json["messages"])
    {
        if (message["role"] == "system")
        {
            oPrompt = message["content"];
            return true;
        }
    }

    return false;
}

bool GPTDevice::getConversation(std::vector<std::pair<Author, Content>> &oConversation)
{
    std::vector<std::pair<std::string, std::string>> conversation;

    auto &convo_json = m_convo->GetJSON();

    if (convo_json["messages"].empty())
    {
        yWarning() << "Conversation is empty!";
        return false;
    }

    for (auto &message : convo_json["messages"])
    {
        std::string role = message["role"].get<std::string>();
        std::string content = message["content"].get<std::string>();

        conversation.push_back(std::make_pair(role, content));
    }

    oConversation = conversation;
    return true;
}

bool GPTDevice::deleteConversation() noexcept
{
    // Api does not provide a method to empty the conversation: we are better of if we rebuild an object from scratch
    m_convo.reset(new liboai::Conversation());
    return true;
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
        m_functions->AddFunction(function_name);
        m_functions->SetDescription(function_name,function_desc);

        if(function.value().contains("parameters"))
        {
            auto parameters = function.value()["parameters"]["properties"];
            for(auto& params: parameters.items())
            {
                std::cout << params.value();
                liboai::Functions::FunctionParameter param;
                param.name = params.key();
                param.description = params.value()["description"];
                param.type = params.value()["type"];
                m_functions->AppendParameters(function_name,param);
            }
        }
    }

    m_convo->SetFunctions(*m_functions);

    return true;
}