// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/igraphicseffect.h>

#include "costume_p.h"

using namespace libscratchcpp;

CostumePrivate::CostumePrivate() :
    broadcast("", "")
{
}

CostumePrivate::~CostumePrivate()
{
    freeImage();
}

void CostumePrivate::updateImage()
{
    unsigned int scaledWidth = image->width() * scale / bitmapResolution;
    unsigned int scaledHeight = image->height() * scale / bitmapResolution;

    if (scaledWidth == 0 || scaledHeight == 0) {
        freeImage();
        return;
    }

    if (!bitmap || (scale != oldScale) || (bitmapResolution != oldBitmapResolution)) {
        freeImage();
        bitmap = static_cast<Rgb **>(malloc(sizeof(Rgb *) * scaledHeight));

        for (unsigned int i = 0; i < scaledHeight; i++)
            bitmap[i] = static_cast<Rgb *>(malloc(sizeof(Rgb) * scaledWidth));

        oldScale = scale;
        oldBitmapResolution = bitmapResolution;
        oldWidth = scaledWidth;
        oldHeight = scaledHeight;
    }

    double actualScale = scale / bitmapResolution;

    for (unsigned int i = 0; i < scaledHeight; i++) {
        for (unsigned int j = 0; j < scaledWidth; j++)
            bitmap[i][j] = image->colorAt(mirrorHorizontally ? (scaledWidth - 1 - j) : j, i, actualScale);
    }

    for (const auto &[effect, value] : graphicsEffects)
        effect->apply(bitmap, scaledWidth, scaledHeight, value);
}

void CostumePrivate::freeImage()
{
    if (bitmap) {
        for (unsigned int i = 0; i < oldHeight; i++)
            free(bitmap[i]);

        free(bitmap);
        bitmap = nullptr;
    }
}
