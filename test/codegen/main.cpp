#include "../common.h"

#ifdef LIBSCRATCHCPP_CPP_TEST_GENERATED_CODE

#include "generated_cpp_test_code.h"
#include "cpp/testcodegenlistener.h"

int main(int argc, char **argv)
{
    std::filesystem::current_path(DATA_DIR);
    ::testing::InitGoogleTest(&argc, argv);

    libscratchcpp::CppCompilerContext::setDefaultGeneratedCodeFactory(libscratchcpp::createGeneratedCppTestCode);
    auto *listener = new libscratchcpp::test::CppTestCodegenListener();
    listener->registerProvider();
    ::testing::UnitTest::GetInstance()->listeners().Append(listener);

    return RUN_ALL_TESTS();
}

#endif
