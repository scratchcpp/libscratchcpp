// SPDX-License-Identifier: Apache-2.0

#include "scratch3reader.h"
#include "../scratch/sprite.h"
#include "../scratch/stage.h"
#include "reader_common.h"

using namespace libscratchcpp;
using json = nlohmann::json;

bool Scratch3Reader::load()
{
    // TODO: Handle JSON exceptions (corrupted project files)
    if (m_json == "")
        read();
    auto project = m_json;

    // targets
    auto targets = project["targets"];
    for (auto jsonTarget : targets) {
        std::shared_ptr<Target> target;

        // isStage
        if (jsonTarget["isStage"])
            target = std::make_shared<Stage>();
        else
            target = std::make_shared<Sprite>();

        // name
        target->setName(jsonTarget["name"]);

        // variables
        auto variables = jsonTarget["variables"];
        for (json::iterator it = variables.begin(); it != variables.end(); ++it) {
            auto varInfo = it.value();
            bool cloudVar = (varInfo.size() >= 3 && varInfo[2]);
            auto variable = std::make_shared<Variable>(it.key(), varInfo[0], Value(jsonToValue(varInfo[1])), cloudVar);
            target->addVariable(variable);
        }

        // lists
        auto lists = jsonTarget["lists"];
        for (json::iterator it = lists.begin(); it != lists.end(); ++it) {
            auto listInfo = it.value();
            auto list = std::make_shared<List>(it.key(), listInfo[0]);
            auto arr = listInfo[1];
            for (auto item : arr)
                list->append(jsonToValue(item));
            target->addList(list);
        }

        // broadcasts
        auto broadcasts = jsonTarget["broadcasts"];
        for (json::iterator it = broadcasts.begin(); it != broadcasts.end(); ++it) {
            auto broadcast = std::make_shared<Broadcast>(it.key(), it.value());
            m_broadcasts.push_back(broadcast);
        }

        // blocks
        auto blocks = jsonTarget["blocks"];
        for (json::iterator it = blocks.begin(); it != blocks.end(); ++it) {
            auto blockInfo = it.value();
            if (blockInfo.is_array()) {
                // TODO: Find out what these "array" blocks that look like input values are for
                // Let's ignore them for now...
                continue;
            }
            auto block = std::make_shared<Block>(it.key(), blockInfo["opcode"]);
            std::string nextId;
            if (!blockInfo["next"].is_null())
                nextId = blockInfo["next"];
            block->setNextId(nextId);
            std::string parentId;
            if (!blockInfo["parent"].is_null())
                parentId = blockInfo["parent"];
            block->setParentId(parentId);

            // inputs
            auto inputs = blockInfo["inputs"];
            for (json::iterator it = inputs.begin(); it != inputs.end(); ++it) {
                auto inputInfo = it.value();
                auto input = std::make_shared<Input>(it.key(), static_cast<Input::Type>(inputInfo[0]));
                auto primary = inputInfo[1];
                if (primary.is_array()) {
                    input->setPrimaryValue(jsonToValue(primary[1]));
                    input->primaryValue()->setType(static_cast<InputValue::Type>(primary[0]));
                } else if (primary.is_null())
                    input->setValueBlockId("");
                else
                    input->setValueBlockId(primary);
                if (inputInfo.size() >= 3) {
                    auto secondary = inputInfo[2];
                    if (secondary.is_array()) {
                        input->setSecondaryValue(jsonToValue(secondary[1]));
                        input->secondaryValue()->setType(static_cast<InputValue::Type>(secondary[0]));
                    }
                }
                block->addInput(input);
            }

            // fields
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

            // shadow
            block->setShadow(blockInfo["shadow"]);

            // topLevel
            block->setTopLevel(blockInfo["topLevel"]);

            target->addBlock(block);
        }

        // TODO: Add comments

        // currentCostume
        target->setCurrentCostume(jsonTarget["currentCostume"]);

        // costumes
        auto costumes = jsonTarget["costumes"];
        for (auto jsonCostume : costumes) {
            Costume costume(jsonCostume["name"], jsonCostume["assetId"], jsonCostume["dataFormat"]);
            if (jsonCostume.contains("bitmapResolution"))
                costume.setBitmapResolution(jsonCostume["bitmapResolution"]);
            costume.setRotationCenterX(jsonCostume["rotationCenterX"]);
            costume.setRotationCenterY(jsonCostume["rotationCenterY"]);
            target->addCostume(costume);
        }

        // sounds
        auto sounds = jsonTarget["sounds"];
        for (auto jsonSound : sounds) {
            Sound sound(jsonSound["name"], jsonSound["assetId"], jsonSound["dataFormat"]);
            sound.setRate(jsonSound["rate"]);
            sound.setSampleCount(jsonSound["sampleCount"]);
            target->addSound(sound);
        }

        // layerOrder
        target->setLayerOrder(jsonTarget["layerOrder"]);

        // volume
        target->setVolume(jsonTarget["volume"]);

        if (target->isStage()) {
            auto stage = std::static_pointer_cast<Stage>(target);

            // tempo
            if (jsonTarget.contains("tempo"))
                stage->setTempo(jsonTarget["tempo"]);

            // videoState
            if (jsonTarget.contains("videoState")) {
                std::string videoState = jsonTarget["videoState"];
                stage->setVideoState(videoState);
            }

            // videoTransparency
            if (jsonTarget.contains("videoTransparency"))
                stage->setVideoTransparency(jsonTarget["videoTransparency"]);

            // textToSpeechLanguage
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
            sprite->setVisible(jsonTarget["visible"]);

            // x
            sprite->setX(jsonTarget["x"]);

            // y
            sprite->setY(jsonTarget["y"]);

            // size
            sprite->setSize(jsonTarget["size"]);

            // direction
            sprite->setDirection(jsonTarget["direction"]);

            // draggable
            sprite->setDraggable(jsonTarget["draggable"]);

            // rotationStyle
            std::string rotationStyle = jsonTarget["rotationStyle"];
            sprite->setRotationStyle(rotationStyle);
        }

        m_targets.push_back(target);
    }

    // extensions
    auto extensions = project["extensions"];
    for (auto extension : extensions)
        m_extensions.push_back(extension);

    // Close the zip file
    zip_close(m_zip);
    return true;
}

bool Scratch3Reader::isValid()
{
    // TODO: Handle JSON exceptions (corrupted project files)
    if (m_json == "")
        read();
    std::string semver = m_json["meta"]["semver"];
    return (semver.substr(0, semver.find(".")) == "3");
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
    // TODO: Check if the file exists
    // Open the zip file
    unsigned char *buf;
    size_t bufsize;
    m_zip = zip_open(fileName().c_str(), 0, 'r');

    // Extract project.json
    zip_entry_open(m_zip, "project.json");
    bufsize = zip_entry_size(m_zip);
    buf = (unsigned char *)calloc(sizeof(unsigned char), bufsize);
    zip_entry_noallocread(m_zip, (void *)buf, bufsize);
    zip_entry_close(m_zip);
    m_json = json::parse(buf);
    free(buf);
}
