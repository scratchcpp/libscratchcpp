// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "iprojectreader.h"
#include <nlohmann/json.hpp>

#include "zipreader.h"

namespace libscratchcpp
{

class Scratch3Reader : public IProjectReader
{
    public:
        bool load() override;
        bool loadData(const std::string &data) override;
        bool isValid() override;
        void clear() override;
        const std::vector<std::shared_ptr<Target>> &targets() const override;
        const std::vector<std::shared_ptr<Broadcast>> &broadcasts() const override;
        const std::vector<std::shared_ptr<Monitor>> &monitors() const override;
        const std::vector<std::string> &extensions() const override;
        const std::string &userAgent() const override;

    private:
        void read();
        std::unique_ptr<ZipReader> m_zipReader;
        nlohmann::json m_json = "";
        std::vector<std::shared_ptr<Target>> m_targets;
        std::vector<std::shared_ptr<Broadcast>> m_broadcasts;
        std::vector<std::shared_ptr<Monitor>> m_monitors;
        std::vector<std::string> m_extensions;
        std::string m_userAgent;
};

} // namespace libscratchcpp
