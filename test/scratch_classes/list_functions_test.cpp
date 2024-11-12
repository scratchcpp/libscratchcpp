#include <scratchcpp/list.h>
#include <scratch/list_functions.h>

#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(ListFunctionsTest, Clear)
{
    List list("", "");
    list.append(1);
    list.append(2);
    list.append(3);

    list_clear(&list);
}
