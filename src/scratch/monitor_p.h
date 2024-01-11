// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/monitor.h>

namespace libscratchcpp
{

class Block;
class IRandomGenerator;

struct MonitorPrivate
{
        MonitorPrivate(const std::string &opcode);
        MonitorPrivate(const MonitorPrivate &) = delete;

        Monitor::Mode mode = Monitor::Mode::Default;
        std::shared_ptr<Block> block; // Compiler needs shared_ptr
        unsigned int width = 0;
        unsigned int height = 0;
        int x = 0;
        int y = 0;
        bool visible = true;
        double sliderMin = 0;
        double sliderMax = 0;
        bool discrete = false;
        static IRandomGenerator *rng;
};

} // namespace libscratchcpp
