/*
 * SPDX-FileCopyrightText: 2023-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_GPTDevice_H
#define YARP_GPTDevice_H

#include <yarp/dev/ILLM.h>
#include <yarp/dev/LLM_Message.h>
#include <yarp/dev/DeviceDriver.h>

#include <liboai.h>
#include <nlohmann/json.hpp>
#include <map>

#include "GPTDevice_ParamsParser.h"

using json = nlohmann::json;

using Question = std::string;
using Answer = yarp::dev::LLM_Message;
// using Answer = std::tuple<std::string,std::vector<std::string>,std::vector<std::string>>;

/**
 *  @ingroup dev_impl_other
 *
 * \section GPTDevice
 *
 * \brief `GPTDevice`: A yarp device for interacting with the GPT model
 *
 *  Parameters required by this device are described in class GPTDevice_ParamsParser
 *
 */

class GPTDevice : public yarp::dev::ILLM,
                  public yarp::dev::DeviceDriver,
                  public GPTDevice_ParamsParser
{
public:
    GPTDevice() : ILLM(), m_convo{std::make_unique<liboai::Conversation>()},
                            m_function_called{},
                            m_functions{std::make_unique<liboai::Functions>()},
                            m_convo_length{1},
                            m_offline{false}
    {
    }

    GPTDevice(const GPTDevice &) = delete;
    GPTDevice(GPTDevice &&) noexcept = delete;
    GPTDevice &operator=(const GPTDevice &) = delete;
    GPTDevice &operator=(GPTDevice &&) noexcept = delete;

    ~GPTDevice() override = default;

    // Rpc methods
    yarp::dev::ReturnValue setPrompt(const std::string &prompt) override;

    yarp::dev::ReturnValue readPrompt(std::string &oPrompt) override;

    yarp::dev::ReturnValue ask(const std::string &question, yarp::dev::LLM_Message &oAnswer) override;

    yarp::dev::ReturnValue getConversation(std::vector<yarp::dev::LLM_Message> &oConversation) override;

    yarp::dev::ReturnValue deleteConversation() noexcept override;

    yarp::dev::ReturnValue refreshConversation() noexcept override;

    // Device initialization
    bool open(yarp::os::Searchable &config) override;

    bool close() override;

private:
    // data
    std::unique_ptr<liboai::Conversation> m_convo;
    std::size_t m_convo_length; //How many messages are in conversation

    // function_call data. Mostly kept to display the full conversation
    std::map<std::size_t,Answer> m_function_called;

    // functions
    std::unique_ptr<liboai::Functions> m_functions;

    // configuration
    char *azure_resource;
    char *azure_deployment_id;
    liboai::OpenAI oai;

    // offline mode for testing purpouses
    bool m_offline;

    // model
    std::string m_model;

    // sets OpenAI function
    bool setFunctions(const json& function_json);

    const std::string module_name = "GPTDevice";

};

#endif // YARP_GPTDevice_H
