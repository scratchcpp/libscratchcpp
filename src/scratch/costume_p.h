// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/broadcast.h>
#include <scratchcpp/iimageformat.h>
#include <unordered_map>

namespace libscratchcpp
{

class IGraphicsEffect;

struct CostumePrivate
{
        CostumePrivate();
        CostumePrivate(const CostumePrivate &) = delete;
        ~CostumePrivate();

        void updateImage();
        void freeImage();

        double oldBitmapResolution = 1;
        double bitmapResolution = 1;
        int rotationCenterX = 0;
        int rotationCenterY = 0;
        std::shared_ptr<IImageFormat> image;
        Rgb **bitmap = nullptr;
        double oldWidth = 0;
        double oldHeight = 0;
        double oldScale = 1;
        double scale = 1;
        bool mirrorHorizontally = false;
        std::unordered_map<IGraphicsEffect *, double> graphicsEffects;
        Broadcast broadcast;
};

} // namespace libscratchcpp
