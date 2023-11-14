#include <spritehandlermock.h>
#include <enginemock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::_;
using ::testing::SaveArg;

class ISpriteHandlerTest : public testing::Test
{
    public:
        void SetUp() override
        {
            EXPECT_CALL(m_handler, onSpriteChanged(&m_sprite)).Times(1);
            m_sprite.setInterface(&m_handler);
            m_sprite.setEngine(&m_engine);
        }

        Sprite m_sprite;
        SpriteHandlerMock m_handler;
        EngineMock m_engine;
};

TEST_F(ISpriteHandlerTest, Clone)
{
    Sprite *clone;
    Sprite *cloneArg;
    EXPECT_CALL(m_engine, cloneLimit()).Times(2).WillRepeatedly(Return(300));
    EXPECT_CALL(m_engine, cloneCount()).WillOnce(Return(0));
    EXPECT_CALL(m_engine, initClone(_)).WillOnce(SaveArg<0>(&clone));
    EXPECT_CALL(m_handler, onCloned(_)).WillOnce(SaveArg<0>(&cloneArg));
    EXPECT_CALL(m_engine, requestRedraw());

    m_sprite.clone();
    ASSERT_TRUE(clone);
    ASSERT_EQ(cloneArg, clone);

    // Engine is used during deletion, so let's just set it to nullptr (this is tested elsewhere)
    m_sprite.setEngine(nullptr);
    clone->setEngine(nullptr);
}

TEST_F(ISpriteHandlerTest, Visible)
{
    EXPECT_CALL(m_handler, onVisibleChanged(false)).Times(1);
    m_sprite.setVisible(false);

    EXPECT_CALL(m_handler, onVisibleChanged(true)).Times(1);
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setVisible(true);
}

TEST_F(ISpriteHandlerTest, X)
{
    EXPECT_CALL(m_handler, onXChanged(189.46)).Times(1);
    EXPECT_CALL(m_engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(m_engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(m_engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setX(189.46);

    EXPECT_CALL(m_handler, onXChanged(284.61)).Times(1);
    EXPECT_CALL(m_engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setX(284.61);
}

TEST_F(ISpriteHandlerTest, Y)
{
    EXPECT_CALL(m_handler, onYChanged(-153.7)).Times(1);
    EXPECT_CALL(m_engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(m_engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(m_engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setY(-153.7);

    EXPECT_CALL(m_handler, onYChanged(207.08)).Times(1);
    EXPECT_CALL(m_engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setY(207.08);
}

TEST_F(ISpriteHandlerTest, Size)
{
    EXPECT_CALL(m_handler, onSizeChanged(53.8)).Times(1);
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setSize(53.8);
}

TEST_F(ISpriteHandlerTest, Direction)
{
    EXPECT_CALL(m_handler, onDirectionChanged(179.715)).Times(1);
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setDirection(179.715);
}

TEST_F(ISpriteHandlerTest, RotationStyle)
{
    EXPECT_CALL(m_handler, onRotationStyleChanged(Sprite::RotationStyle::DoNotRotate)).Times(1);
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setRotationStyle(Sprite::RotationStyle::DoNotRotate);

    EXPECT_CALL(m_handler, onRotationStyleChanged(Sprite::RotationStyle::LeftRight)).Times(1);
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setRotationStyle(Sprite::RotationStyle::LeftRight);
}

TEST_F(ISpriteHandlerTest, CostumeData)
{
    // TODO: Add test for costume data
    // EXPECT_CALL(m_handler, onCostumeChanged(...)).Times(1);
}
