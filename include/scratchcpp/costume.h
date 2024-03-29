// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "spimpl.h"

#include "asset.h"

namespace libscratchcpp
{

class Broadcast;
class CostumePrivate;

/*! \brief The Costume class represents a Scratch costume. */
class LIBSCRATCHCPP_EXPORT Costume : public Asset
{
    public:
        Costume(const std::string &name, const std::string &id, const std::string &format);
        Costume(const Costume &) = delete;

        double bitmapResolution() const;
        void setBitmapResolution(double newBitmapResolution);

        int rotationCenterX() const;
        void setRotationCenterX(int newRotationCenterX);

        int rotationCenterY() const;
        void setRotationCenterY(int newRotationCenterY);

        Broadcast *broadcast();

    private:
        spimpl::unique_impl_ptr<CostumePrivate> impl;
};

} // namespace libscratchcpp
