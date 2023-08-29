// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/broadcast.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <scratchcpp/block.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/sound.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/sprite.h>

#include "scratch3reader.h"
#include "reader_common.h"
#include "zipreader.h"

using namespace libscratchcpp;
using json = nlohmann::json;

bool Scratch3Reader::load()
{
    if (m_json == "")
        read();
    auto project = m_json;
    const char *step = "";

    try {
        // targets
        READER_STEP(step, "targets");
        auto targets = project["targets"];
        for (auto jsonTarget : targets) {
            std::shared_ptr<Target> target;

            // isStage
            READER_STEP(step, "target -> isStage");
            if (jsonTarget["isStage"])
                target = std::make_shared<Stage>();
            else
                target = std::make_shared<Sprite>();

            // name
            READER_STEP(step, "target -> name");
            target->setName(jsonTarget["name"]);

            // variables
            READER_STEP(step, "target -> variables");
            auto variables = jsonTarget["variables"];
            for (json::iterator it = variables.begin(); it != variables.end(); ++it) {
                auto varInfo = it.value();
                bool cloudVar = (varInfo.size() >= 3 && varInfo[2]);
                auto variable = std::make_shared<Variable>(it.key(), varInfo[0], Value(jsonToValue(varInfo[1])), cloudVar);
                target->addVariable(variable);
            }

            // lists
            READER_STEP(step, "target -> lists");
            auto lists = jsonTarget["lists"];
            for (json::iterator it = lists.begin(); it != lists.end(); ++it) {
                auto listInfo = it.value();
                auto list = std::make_shared<List>(it.key(), listInfo[0]);
                auto arr = listInfo[1];
                for (auto item : arr)
                    list->push_back(jsonToValue(item));
                target->addList(list);
            }

            // broadcasts
            READER_STEP(step, "target -> broadcasts");
            auto broadcasts = jsonTarget["broadcasts"];
            for (json::iterator it = broadcasts.begin(); it != broadcasts.end(); ++it) {
                auto broadcast = std::make_shared<Broadcast>(it.key(), it.value());
                m_broadcasts.push_back(broadcast);
            }

            // blocks
            READER_STEP(step, "target -> blocks");
            auto blocks = jsonTarget["blocks"];
            for (json::iterator it = blocks.begin(); it != blocks.end(); ++it) {
                auto blockInfo = it.value();

                if (blockInfo.is_array()) {
                    // This is a top level reporter block for a variable/list
                    READER_STEP(step, "target -> block -> top level reporter info");
                    auto block = std::make_shared<Block>(it.key(), "");
                    block->setIsTopLevelReporter(true);
                    InputValue *reporterInfo = block->topLevelReporterInfo();

                    reporterInfo->setValue(jsonToValue(blockInfo[1]));
                    reporterInfo->setValueId(jsonToValue(blockInfo[2]).toString());
                    reporterInfo->setType(static_cast<InputValue::Type>(blockInfo[0]));

                    target->addBlock(block);
                    continue;
                }

                READER_STEP(step, "target -> block -> opcode");
                auto block = std::make_shared<Block>(it.key(), blockInfo["opcode"]);
                std::string nextId;
                READER_STEP(step, "target -> block -> next");
                if (!blockInfo["next"].is_null())
                    nextId = blockInfo["next"];
                block->setNextId(nextId);
                std::string parentId;
                READER_STEP(step, "target -> block -> parent");
                if (!blockInfo["parent"].is_null())
                    parentId = blockInfo["parent"];
                block->setParentId(parentId);

                // inputs
                READER_STEP(step, "target -> block -> inputs");
                auto inputs = blockInfo["inputs"];
                for (json::iterator it = inputs.begin(); it != inputs.end(); ++it) {
                    auto inputInfo = it.value();
                    auto input = std::make_shared<Input>(it.key(), static_cast<Input::Type>(inputInfo[0]));
                    auto primary = inputInfo[1];
                    if (primary.is_array()) {
                        input->setPrimaryValue(jsonToValue(primary[1]));
                        input->primaryValue()->setType(static_cast<InputValue::Type>(primary[0]));
                        if (primary.size() >= 3)
                            input->primaryValue()->setValueId(jsonToValue(primary[2]).toString());
                    } else if (primary.is_null())
                        input->setValueBlockId("");
                    else
                        input->setValueBlockId(primary);
                    if (inputInfo.size() >= 3) {
                        auto secondary = inputInfo[2];
                        if (secondary.is_array()) {
                            input->setSecondaryValue(jsonToValue(secondary[1]));
                            input->secondaryValue()->setType(static_cast<InputValue::Type>(secondary[0]));
                            if (secondary.size() >= 3)
                                input->secondaryValue()->setValueId(jsonToValue(secondary[2]).toString());
                        }
                    }
                    block->addInput(input);
                }

                // fields
                READER_STEP(step, "target -> block -> fields");
                auto fields = blockInfo["fields"];
                for (json::iterator it = fields.begin(); it != fields.end(); ++it) {
                    auto fieldInfo = it.value();
                    std::shared_ptr<Field> field;
                    if (fieldInfo.size() >= 2) {
                        auto valueId = fieldInfo[1];
                        std::string valueIdStr;
                        if (!valueId.is_null())
                            valueIdStr = valueId;
                        field = std::make_shared<Field>(it.key(), jsonToValue(fieldInfo[0]), valueIdStr);
                    } else
                        field = std::make_shared<Field>(it.key(), jsonToValue(fieldInfo[0]));
                    block->addField(field);
                }

                // mutation
                READER_STEP(step, "target -> block -> mutation");
                if (blockInfo.contains("mutation")) {
                    auto mutation = blockInfo["mutation"];
                    READER_STEP(step, "target -> block -> mutation -> hasnext");
                    if (mutation.contains("hasnext"))
                        block->setMutationHasNext(jsonToValue(mutation["hasnext"]).toBool());
                    BlockPrototype *prototype = block->mutationPrototype();
                    READER_STEP(step, "target -> block -> mutation -> proccode");
                    if (mutation.contains("proccode"))
                        prototype->setProcCode(mutation["proccode"].get<std::string>());
                    READER_STEP(step, "target -> block -> mutation -> argumentids");
                    if (mutation.contains("argumentids")) {
                        std::vector<std::string> argIDs;
                        auto argIDsJson = json::parse(mutation["argumentids"].get<std::string>());
                        for (auto arg : argIDsJson)
                            argIDs.push_back(arg.get<std::string>());
                        prototype->setArgumentIds(argIDs);
                    }
                    READER_STEP(step, "target -> block -> mutation -> argumentnames");
                    if (mutation.contains("argumentnames")) {
                        std::vector<std::string> argNames;
                        auto argNamesJson = json::parse(mutation["argumentnames"].get<std::string>());
                        for (auto arg : argNamesJson)
                            argNames.push_back(arg.get<std::string>());
                        prototype->setArgumentNames(argNames);
                    }
                    READER_STEP(step, "target -> block -> mutation -> warp");
                    if (mutation.contains("warp"))
                        prototype->setWarp(jsonToValue(mutation["warp"]).toBool());
                }

                // shadow
                READER_STEP(step, "target -> block -> shadow");
                block->setShadow(blockInfo["shadow"]);

                target->addBlock(block);
            }

            // TODO: Add comments

            // currentCostume
            READER_STEP(step, "target -> currentCostume");
            target->setCurrentCostume(jsonTarget["currentCostume"]);

            // costumes
            READER_STEP(step, "target -> costumes");
            auto costumes = jsonTarget["costumes"];
            for (auto jsonCostume : costumes) {
                READER_STEP(step, "target -> costume -> { name, assetId, dataFormat }");
                auto costume = std::make_shared<Costume>(jsonCostume["name"], jsonCostume["assetId"], jsonCostume["dataFormat"]);
                READER_STEP(step, "target -> costume -> bitmapResolution");
                if (jsonCostume.contains("bitmapResolution"))
                    costume->setBitmapResolution(jsonCostume["bitmapResolution"]);
                READER_STEP(step, "target -> costume -> rotationCenterX");
                costume->setRotationCenterX(jsonCostume["rotationCenterX"]);
                READER_STEP(step, "target -> costume -> rotationCenterY");
                costume->setRotationCenterY(jsonCostume["rotationCenterY"]);
                target->addCostume(costume);
            }

            // sounds
            READER_STEP(step, "target -> sounds");
            auto sounds = jsonTarget["sounds"];
            for (auto jsonSound : sounds) {
                READER_STEP(step, "target -> sound -> { name, assetId, dataFormat }");
                auto sound = std::make_shared<Sound>(jsonSound["name"], jsonSound["assetId"], jsonSound["dataFormat"]);
                READER_STEP(step, "target -> sound -> rate");
                sound->setRate(jsonSound["rate"]);
                READER_STEP(step, "target -> sound -> sampleCount");
                sound->setSampleCount(jsonSound["sampleCount"]);
                target->addSound(sound);
            }

            // layerOrder
            READER_STEP(step, "target -> layerOrder");
            target->setLayerOrder(jsonTarget["layerOrder"]);

            // volume
            READER_STEP(step, "target -> volume");
            target->setVolume(jsonTarget["volume"]);

            if (target->isStage()) {
                auto stage = std::static_pointer_cast<Stage>(target);

                // tempo
                READER_STEP(step, "stage -> tempo");
                if (jsonTarget.contains("tempo"))
                    stage->setTempo(jsonTarget["tempo"]);

                // videoState
                READER_STEP(step, "stage -> videoState");
                if (jsonTarget.contains("videoState")) {
                    std::string videoState = jsonTarget["videoState"];
                    stage->setVideoState(videoState);
                }

                // videoTransparency
                READER_STEP(step, "stage -> videoTransparency");
                if (jsonTarget.contains("videoTransparency"))
                    stage->setVideoTransparency(jsonTarget["videoTransparency"]);

                // textToSpeechLanguage
                READER_STEP(step, "stage -> textToSpeechLanguage");
                if (jsonTarget.contains("textToSpeechLanguage")) {
                    auto lang = jsonTarget["textToSpeechLanguage"];
                    std::string langStr;
                    if (!lang.is_null())
                        langStr = lang;
                    stage->setTextToSpeechLanguage(langStr);
                }
            } else {
                auto sprite = std::static_pointer_cast<Sprite>(target);

                // visible
                READER_STEP(step, "sprite -> visible");
                sprite->setVisible(jsonTarget["visible"]);

                // x
                READER_STEP(step, "sprite -> x");
                sprite->setX(jsonTarget["x"]);

                // y
                READER_STEP(step, "sprite -> y");
                sprite->setY(jsonTarget["y"]);

                // size
                READER_STEP(step, "sprite -> size");
                sprite->setSize(jsonTarget["size"]);

                // direction
                READER_STEP(step, "sprite -> direction");
                sprite->setDirection(jsonTarget["direction"]);

                // draggable
                READER_STEP(step, "sprite -> draggable");
                sprite->setDraggable(jsonTarget["draggable"]);

                // rotationStyle
                READER_STEP(step, "sprite -> rotationStyle");
                std::string rotationStyle = jsonTarget["rotationStyle"];
                sprite->setRotationStyle(rotationStyle);
            }

            m_targets.push_back(target);
        }

        // extensions
        READER_STEP(step, "extensions");
        auto extensions = project["extensions"];
        for (auto extension : extensions)
            m_extensions.push_back(extension);
    } catch (std::exception &e) {
        if (strcmp(step, "") == 0)
            printErr("could not parse project JSON file", e.what());
        else
            printErr(std::string("could not parse ") + step, e.what());

        return false;
    }

    return true;
}

bool Scratch3Reader::isValid()
{
    if (m_json == "")
        read();

    std::string semver;

    try {
        semver = m_json["meta"]["semver"];
    } catch (std::exception &e) {
        printErr("could not find Scratch version", e.what());
        return false;
    }

    return (semver.substr(0, semver.find(".")) == "3");
}

void Scratch3Reader::clear()
{
    m_json = "";
    m_targets.clear();
    m_broadcasts.clear();
    m_extensions.clear();
}

std::vector<std::shared_ptr<Target>> Scratch3Reader::targets()
{
    return m_targets;
}

std::vector<std::shared_ptr<Broadcast>> Scratch3Reader::broadcasts()
{
    return m_broadcasts;
}

std::vector<std::string> Scratch3Reader::extensions()
{
    return m_extensions;
}

void Scratch3Reader::read()
{
    // Read project.json
    ZipReader reader(fileName());
    if (reader.open()) {
        // Parse the JSON
        try {
            m_json = json::parse(reader.readFileToString("project.json"));
        } catch (std::exception &e) {
            printErr("invalid JSON file", e.what());
        }
    } else
        printErr("could not read " + fileName());
}
