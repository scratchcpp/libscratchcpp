// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "iprojectreader.h"
#include <nlohmann/json.hpp>

namespace libscratchcpp
{

class Scratch3Reader : public IProjectReader
{
    public:
        bool load() override;
        bool isValid() override;
        void clear() override;
        std::vector<std::shared_ptr<Target>> targets() override;
        std::vector<std::shared_ptr<Broadcast>> broadcasts() override;
        std::vector<std::string> extensions() override;

    private:
        void read();
        nlohmann::json m_json = "";
        std::vector<std::shared_ptr<Target>> m_targets;
        std::vector<std::shared_ptr<Broadcast>> m_broadcasts;
        std::vector<std::string> m_extensions;
};

} // namespace libscratchcpp
