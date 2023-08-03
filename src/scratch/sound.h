// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/asset.h>

namespace libscratchcpp
{

/*! \brief The Sound class represents a Scratch sound. */
class LIBSCRATCHCPP_EXPORT Sound : public Asset
{
    public:
        Sound(std::string name, std::string id, std::string format);
        int rate() const;
        void setRate(int newRate);

        int sampleCount() const;
        void setSampleCount(int newSampleCount);

    private:
        int m_rate = 0;
        int m_sampleCount = 0;
};

} // namespace libscratchcpp
