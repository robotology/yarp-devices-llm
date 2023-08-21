/*
 * SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/ILLM.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::GPTDevice_test", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("GPTDevice", "device");

    Network::setLocalMode(true);

    SECTION("Checking the device")
    {
        PolyDriver dd;
        ILLM* illm = nullptr;

        //"Checking opening device"
        {
            Property pcfg;
            pcfg.put("device", "GPTDevice");
            REQUIRE(dd.open(pcfg));
        }

        // Do something
        {
            dd.view(illm);

            std::string prompt = "This is a prompt";
            std::string out_prompt;

            bool ret = illm->readPrompt(out_prompt);
            CHECK(!ret); //Prompt is not set yet

            ret = illm->setPrompt(prompt);
            CHECK(ret);

            ret = illm->readPrompt(out_prompt);
            CHECK(out_prompt == prompt);

            //TODO: add illm->ask test without accessing internet

            std::vector<std::pair<Author,Content>> out_conversation;
            ret = illm->getConversation(out_conversation);
            CHECK(ret); //We have the system message
            CHECK(out_conversation[0].first == "system");
            CHECK(out_conversation[0].second == prompt);

            ret = illm->deleteConversation();
            CHECK(ret);

            // Check that conversation is actually deleted
            ret = illm->getConversation(out_conversation);
            CHECK(!ret);

        }

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
