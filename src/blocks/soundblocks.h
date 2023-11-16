// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

/*! \brief The SoundBlocks class contains the implementation of sound blocks. */
class SoundBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            VOLUME
        };

        enum Fields
        {

        };

        enum FieldValues
        {

        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileChangeVolumeBy(Compiler *compiler);
        static void compileSetVolumeTo(Compiler *compiler);

        static unsigned int changeVolumeBy(VirtualMachine *vm);
        static unsigned int setVolumeTo(VirtualMachine *vm);
};

} // namespace libscratchcpp
