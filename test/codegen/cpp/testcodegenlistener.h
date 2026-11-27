// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <gtest/gtest.h>

#include <string>

#include "engine/internal/codegen/cpp/cppcompilercontext.h"

namespace libscratchcpp::test
{

class CppTestCodegenListener : public testing::EmptyTestEventListener
{
    public:
        void OnTestStart(const testing::TestInfo &testInfo) override
        {
            m_testName = std::string(testInfo.test_suite_name()) + "." + testInfo.name();
            m_builderCount = 0;
        }

        void registerProvider()
        {
            CppCompilerContext::setTestCodeIdProvider([this] {
                return m_testName + "#" + std::to_string(m_builderCount++);
            });
        }

    private:
        std::string m_testName;
        unsigned int m_builderCount = 0;
};

} // namespace libscratchcpp::test
