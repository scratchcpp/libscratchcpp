// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/monitor.h>
#include <scratchcpp/block.h>

namespace libscratchcpp
{

class Block;
class IRandomGenerator;

struct MonitorPrivate
{
        MonitorPrivate(const std::string &opcode);
        MonitorPrivate(const MonitorPrivate &) = delete;

        IMonitorHandler *iface = nullptr;
        std::string name;
        Monitor::Mode mode = Monitor::Mode::Default;
        std::shared_ptr<Script> script;
        IExtension *extension = nullptr;
        std::shared_ptr<Block> block;
        MonitorChangeFunc changeFunc = nullptr;
        unsigned int width = 0;
        unsigned int height = 0;
        int x = 0;
        int y = 0;
        bool visible = true;
        double sliderMin = 0;
        double sliderMax = 0;
        bool discrete = false;
        bool needsAutoPosition = true;
        static IRandomGenerator *rng;
};

} // namespace libscratchcpp
