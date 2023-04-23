// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../libscratchcpp_global.h"
#include <string>

namespace libscratchcpp
{

/*! \brief The Asset class represents a Scratch asset, for example a Costume or a Sound. */
class LIBSCRATCHCPP_EXPORT Asset
{
    public:
        Asset(std::string name, std::string id, std::string format);

        std::string assetId() const;

        std::string name() const;

        std::string md5ext() const;

        std::string dataFormat() const;

    private:
        std::string m_assetId;
        std::string m_name;
        std::string m_dataFormat;
};

} // namespace libscratchcpp
