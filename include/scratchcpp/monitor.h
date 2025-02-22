// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/entity.h>

#include <vector>

namespace libscratchcpp
{

class IMonitorHandler;
class Block;
class Script;
class IExtension;
class Sprite;
class Value;
class Rect;
class MonitorPrivate;

/*! \brief The Monitor class represents a stage monitor. */
class LIBSCRATCHCPP_EXPORT Monitor : public Entity
{
    public:
        friend class Engine;

        enum class Mode
        {
            Default,
            Large,
            Slider,
            List
        };

        Monitor(const std::string &id, const std::string &opcode);
        Monitor(const Monitor &) = delete;

        void setInterface(IMonitorHandler *iface);

        const std::string &name() const;
        void setName(const std::string &name);

        Mode mode() const;
        void setMode(Mode mode);

        std::shared_ptr<Block> block() const;

        std::shared_ptr<Script> script() const;
        void setScript(std::shared_ptr<Script> script);

        IExtension *extension() const;
        void setExtension(IExtension *extension);

        Sprite *sprite() const;
        void setSprite(Sprite *sprite);

        const std::string &opcode() const;

        void updateValue(const Value &value);

        void setValueChangeFunction(MonitorChangeFunc f);
        void changeValue(const Value &newValue);

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

        bool needsAutoPosition() const;
        void autoPosition(const std::vector<std::shared_ptr<Monitor>> &allMonitors);

    private:
        static bool monitorRectsIntersect(const Rect &a, const Rect &b);

        spimpl::unique_impl_ptr<MonitorPrivate> impl;
};

} // namespace libscratchcpp
