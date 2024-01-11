// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/entity.h>

#include <vector>

namespace libscratchcpp
{

class Block;
class Sprite;
class Rect;
class MonitorPrivate;

/*! \brief The Monitor class represents a stage monitor. */
class LIBSCRATCHCPP_EXPORT Monitor : public Entity
{
    public:
        enum class Mode
        {
            Default,
            Large,
            Slider,
            List
        };

        Monitor(const std::string &id, const std::string &opcode);
        Monitor(const Monitor &) = delete;

        Mode mode() const;
        void setMode(Mode mode);

        std::shared_ptr<Block> block() const;

        Sprite *sprite() const;
        void setSprite(Sprite *sprite);

        const std::string &opcode() const;

        unsigned int width() const;
        void setWidth(unsigned int width);

        unsigned int height() const;
        void setHeight(unsigned int height);

        int x() const;
        void setX(int x);

        int y() const;
        void setY(int y);

        bool visible() const;
        void setVisible(bool visible);

        double sliderMin() const;
        void setSliderMin(double sliderMin);

        double sliderMax() const;
        void setSliderMax(double sliderMax);

        bool discrete() const;
        void setDiscrete(bool discrete);

        static Rect getInitialPosition(const std::vector<std::shared_ptr<Monitor>> &other, int monitorWidth, int monitorHeight);

    private:
        spimpl::unique_impl_ptr<MonitorPrivate> impl;
};

} // namespace libscratchcpp