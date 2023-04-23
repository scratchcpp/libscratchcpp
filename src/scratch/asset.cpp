// SPDX-License-Identifier: Apache-2.0

#include "asset.h"

using namespace libscratchcpp;

/*! Constructs Asset. */
Asset::Asset(std::string name, std::string id, std::string format) :
    m_name(name),
    m_assetId(id),
    m_dataFormat(format)
{
}

/*! Returns the MD5 hash of the asset file. */
std::string Asset::assetId() const
{
    return m_assetId;
}

/*! Returns the name of the asset. */
std::string Asset::name() const
{
    return m_name;
}

/*! Returns the name of the asset file. */
std::string Asset::md5ext() const
{
    return m_assetId + "." + m_dataFormat;
}

/*! Returns the name of the format of the asset file. */
std::string Asset::dataFormat() const
{
    return m_dataFormat;
}
