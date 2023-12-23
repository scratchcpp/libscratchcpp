// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iostream>
#include <vector>
#include <memory>

#ifndef NDEBUG
#define READER_STEP(ptr, str) ptr = str
#else
#define READER_STEP(ptr, str)
#endif

namespace libscratchcpp
{

class Target;
class Broadcast;

class IProjectReader
{
    public:
        virtual ~IProjectReader() { }

        virtual const std::string &fileName() const final { return m_fileName; }
        virtual void setFileName(const std::string &fileName) final { m_fileName = fileName; }

        virtual bool load() = 0;
        virtual bool loadData(const std::string &data) = 0;
        virtual bool isValid() = 0;
        virtual void clear() = 0;
        virtual const std::vector<std::shared_ptr<Target>> &targets() = 0;
        virtual const std::vector<std::shared_ptr<Broadcast>> &broadcasts() = 0;
        virtual const std::vector<std::string> &extensions() = 0;

    protected:
        virtual void printErr(const std::string &errStr) final { std::cerr << "Failed to read project: " << errStr << std::endl; }
        virtual void printErr(const std::string &errStr, const char *what) final { std::cerr << "Failed to read project: " << errStr << std::endl << what << std::endl; }

    private:
        std::string m_fileName;
};

} // namespace libscratchcpp
