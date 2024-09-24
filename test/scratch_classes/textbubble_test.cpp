#include <scratchcpp/textbubble.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(TextBubbleTest, IsTarget)
{
    TextBubble bubble;
    ASSERT_FALSE(bubble.isTarget());
}

TEST(TextBubbleTest, IsTextBubble)
{
    TextBubble bubble;
    ASSERT_TRUE(bubble.isTextBubble());
}

TEST(TextBubbleTest, BubbleType)
{
    TextBubble bubble;
    ASSERT_EQ(bubble.type(), TextBubble::Type::Say);

    bubble.setType(TextBubble::Type::Think);
    ASSERT_EQ(bubble.type(), TextBubble::Type::Think);

    bubble.setType(TextBubble::Type::Say);
    ASSERT_EQ(bubble.type(), TextBubble::Type::Say);
}

TEST(TextBubbleTest, BubbleText)
{
    TextBubble bubble;
    ASSERT_TRUE(bubble.text().empty());

    bubble.setText("hello");
    ASSERT_EQ(bubble.text(), "hello");

    bubble.setText("world");
    ASSERT_EQ(bubble.text(), "world");

    // longstr.length = 384, should be limited to 330 in bubble text
    std::string longstr =
        "EY8OUNzAqwgh7NRGk5TzCP3dkAhJy9TX"
        "Y9mqKElPjdQpKddYqjyCwUk2hx6YgVZV"
        "6BOdmZGxDMs8Hjv8W9G6j4gTxAWdOkzs"
        "8Ih80xzEDbvLilWsDwoB6FxH2kVVI4xs"
        "IXOETNQ6QMsCKLWc5XjHk2BS9nYvDGpJ"
        "uEmp9zIzFGT1kRSrOlU3ZwnN1YtvqFx"
        "3hkWVNtJ71dQ0PJHhOVQPUy19V01SPu3"
        "KIIS2wdSUVAc4RYMzepSveghzWbdcizy"
        "Tm1KKAj4svu9YoL8b9vsolG8gKunvKO7"
        "MurRKSeUbECELnJEKV6683xCq7RvmjAu"
        "2djZ54apiQc1lTixWns5GoG0SVNuFzHl"
        "q97qUiqiMecjVFM51YVif7c1Stip52Hl";

    bubble.setText(longstr);
    ASSERT_EQ(bubble.text().length(), 330);
    ASSERT_EQ(bubble.text(), longstr.substr(0, 330));

    // Integers should be left unchanged
    bubble.setText("8");
    ASSERT_EQ(bubble.text(), "8");

    bubble.setText("-52");
    ASSERT_EQ(bubble.text(), "-52");

    bubble.setText("0");
    ASSERT_EQ(bubble.text(), "0");

    // Non-integers should be rounded to 2 decimal places (no more, no less), unless they're small enough that rounding would display them as 0.00 (#478)
    bubble.setText("8.324");
    ASSERT_EQ(bubble.text(), "8.32");

    bubble.setText("-52.576");
    ASSERT_EQ(bubble.text(), "-52.58");

    bubble.setText("3.5");
    ASSERT_EQ(bubble.text(), "3.5");

    bubble.setText("0.015");
    ASSERT_EQ(bubble.text(), "0.02");

    bubble.setText("-0.015");
    ASSERT_EQ(bubble.text(), "-0.02");

    bubble.setText("0.005");
    ASSERT_EQ(bubble.text(), "0.005");

    bubble.setText("-0.005");
    ASSERT_EQ(bubble.text(), "-0.005");
}
