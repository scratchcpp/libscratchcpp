// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/asset.h>

namespace libscratchcpp
{

/*! \brief The Costume class represents a Scratch costume. */
class LIBSCRATCHCPP_EXPORT Costume : public Asset
{
    public:
        Costume(std::string name, std::string id, std::string format);
        double bitmapResolution() const;
        void setBitmapResolution(double newBitmapResolution);

        int rotationCenterX() const;
        void setRotationCenterX(int newRotationCenterX);

        int rotationCenterY() const;
        void setRotationCenterY(int newRotationCenterY);

    private:
        double m_bitmapResolution = 1;
        int m_rotationCenterX = 0;
        int m_rotationCenterY = 0;
};

} // namespace libscratchcpp
