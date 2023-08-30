/*
 * SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <GPTDevice.h>
#include <yarp/os/LogStream.h>

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

    yWarning() << "No system message was found. Set it with setPrompt(string)";

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
