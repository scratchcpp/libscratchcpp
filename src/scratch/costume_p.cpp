// SPDX-License-Identifier: Apache-2.0

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
    unsigned int scaledWidth = image->width() * scale;
    unsigned int scaledHeight = image->height() * scale;

    if (scaledWidth == 0 || scaledHeight == 0) {
        freeImage();
        return;
    }

    if (!bitmap || (scale != oldScale)) {
        freeImage();
        bitmap = static_cast<Rgb **>(malloc(sizeof(Rgb *) * scaledHeight));

        for (unsigned int i = 0; i < scaledHeight; i++)
            bitmap[i] = static_cast<Rgb *>(malloc(sizeof(Rgb) * scaledWidth));

        oldScale = scale;
        oldWidth = scaledWidth;
        oldHeight = scaledHeight;
    }

    for (unsigned int i = 0; i < scaledHeight; i++) {
        for (unsigned int j = 0; j < scaledWidth; j++) {
            Rgb color = image->colorAt(mirrorHorizontally ? (scaledWidth - 1 - j) : j, i, scale);

            // TODO: Apply graphics effects here

            bitmap[i][j] = color;
        }
    }
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
