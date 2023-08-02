// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iostream>

#include "../libscratchcpp_global.h"
#include "../scratch/broadcast.h"
#include "../scratch/target.h"

#ifndef NDEBUG
#define READER_STEP(ptr, str) ptr = str
#else
#define READER_STEP(ptr, str)
#endif

namespace libscratchcpp
{

class LIBSCRATCHCPP_EXPORT IProjectReader
{
    public:
        virtual ~IProjectReader() { }

        virtual std::string fileName() const final { return m_fileName; }
        virtual void setFileName(const std::string &fileName) final { m_fileName = fileName; }

        virtual bool load() = 0;
        virtual bool isValid() = 0;
        virtual void clear() = 0;
        virtual std::vector<std::shared_ptr<Target>> targets() = 0;
        virtual std::vector<std::shared_ptr<Broadcast>> broadcasts() = 0;
        virtual std::vector<std::string> extensions() = 0;

    protected:
        virtual void printErr(const std::string &errStr, const char *what) final { std::cerr << "Failed to read project: " << errStr << std::endl << what << std::endl; }

    private:
        std::string m_fileName;
};

} // namespace libscratchcpp
