#include <scratchcpp/costume.h>
#include <scratchcpp/rect.h>
#include <spritehandlermock.h>
#include <enginemock.h>
#include <graphicseffectmock.h>

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
            EXPECT_CALL(m_handler, init(&m_sprite)).Times(1);
            m_sprite.setInterface(&m_handler);
            m_sprite.setEngine(&m_engine);
        }

        Sprite m_sprite;
        SpriteHandlerMock m_handler;
        EngineMock m_engine;
};

TEST_F(ISpriteHandlerTest, Clone)
{
    std::shared_ptr<Sprite> clone;
    Sprite *cloneParam1, *cloneParam2;
    EXPECT_CALL(m_engine, cloneLimit()).Times(2).WillRepeatedly(Return(300));
    EXPECT_CALL(m_engine, cloneCount()).WillOnce(Return(0));
    EXPECT_CALL(m_engine, initClone(_)).WillOnce(SaveArg<0>(&clone));
    EXPECT_CALL(m_engine, moveSpriteBehindOther(_, &m_sprite)).WillOnce(SaveArg<0>(&cloneParam1));
    EXPECT_CALL(m_handler, onCloned(_)).WillOnce(SaveArg<0>(&cloneParam2));
    EXPECT_CALL(m_engine, requestRedraw());

    m_sprite.clone();
    ASSERT_TRUE(clone);
    ASSERT_EQ(cloneParam1, clone.get());
    ASSERT_EQ(cloneParam2, clone.get());

    SpriteHandlerMock cloneHandler;
    EXPECT_CALL(cloneHandler, init(clone.get()));
    clone->setInterface(&cloneHandler);
    EXPECT_CALL(m_engine, deinitClone(clone));
    EXPECT_CALL(cloneHandler, deinitClone());
    clone->deleteClone();
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
    EXPECT_CALL(m_handler, onMoved(0, 0, 189.46, 0));
    EXPECT_CALL(m_handler, onXChanged(189.46)).Times(1);
    EXPECT_CALL(m_handler, fastBoundingRect()).WillOnce(Return(Rect()));
    EXPECT_CALL(m_engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(m_engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(m_engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setX(189.46);

    EXPECT_CALL(m_handler, onMoved(189.46, 0, 284.61, 0));
    EXPECT_CALL(m_handler, onXChanged(284.61)).Times(1);
    EXPECT_CALL(m_engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setX(284.61);
}

TEST_F(ISpriteHandlerTest, Y)
{
    EXPECT_CALL(m_handler, onMoved(0, 0, 0, -153.7)).Times(1);
    EXPECT_CALL(m_handler, onYChanged(-153.7)).Times(1);
    EXPECT_CALL(m_handler, fastBoundingRect()).WillOnce(Return(Rect()));
    EXPECT_CALL(m_engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(m_engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(m_engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setY(-153.7);

    EXPECT_CALL(m_handler, onMoved(0, -153.7, 0, 207.08)).Times(1);
    EXPECT_CALL(m_handler, onYChanged(207.08)).Times(1);
    EXPECT_CALL(m_engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setY(207.08);
}

TEST_F(ISpriteHandlerTest, Position)
{
    EXPECT_CALL(m_handler, onMoved(0, 0, 284.61, 207.08)).Times(1);
    EXPECT_CALL(m_handler, onXChanged(284.61));
    EXPECT_CALL(m_handler, onYChanged(207.08));
    EXPECT_CALL(m_engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setPosition(284.61, 207.08);

    EXPECT_CALL(m_handler, onMoved(284.61, 207.08, -45.9, -20.05)).Times(1);
    EXPECT_CALL(m_handler, onXChanged(-45.9));
    EXPECT_CALL(m_handler, onYChanged(-20.05));
    EXPECT_CALL(m_engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setPosition(-45.9, -20.05);
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

TEST_F(ISpriteHandlerTest, LayerOrder)
{
    EXPECT_CALL(m_handler, onLayerOrderChanged(7)).Times(1);
    m_sprite.setLayerOrder(7);
}

TEST_F(ISpriteHandlerTest, Costume)
{
    auto costume1 = std::make_shared<Costume>("", "", "");
    auto costume2 = std::make_shared<Costume>("", "", "");
    m_sprite.addCostume(costume1);
    m_sprite.addCostume(costume2);

    EXPECT_CALL(m_handler, onCostumeChanged(costume1.get()));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setCostumeIndex(0);

    EXPECT_CALL(m_handler, onCostumeChanged(costume2.get()));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setCostumeIndex(1);
}

TEST_F(ISpriteHandlerTest, GraphicsEffects)
{
    GraphicsEffectMock effect;

    EXPECT_CALL(m_handler, onGraphicsEffectChanged(&effect, 16.7));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setGraphicsEffectValue(&effect, 16.7);

    EXPECT_CALL(m_handler, onGraphicsEffectsCleared());
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.clearGraphicsEffects();
}

TEST_F(ISpriteHandlerTest, BubbleType)
{
    EXPECT_CALL(m_handler, onBubbleTypeChanged(Target::BubbleType::Say));
    m_sprite.setBubbleType(Target::BubbleType::Say);

    EXPECT_CALL(m_handler, onBubbleTypeChanged(Target::BubbleType::Think));
    m_sprite.setBubbleType(Target::BubbleType::Think);
}

TEST_F(ISpriteHandlerTest, BubbleText)
{
    EXPECT_CALL(m_handler, onBubbleTextChanged("test"));
    EXPECT_CALL(m_engine, requestRedraw());
    m_sprite.setBubbleText("test");
}

TEST_F(ISpriteHandlerTest, BoundingRect)
{
    EXPECT_CALL(m_handler, boundingRect()).WillOnce(Return(Rect(-44.6, 89.1, 20.5, -0.48)));
    Rect rect = m_sprite.boundingRect();
    ASSERT_EQ(rect.left(), -44.6);
    ASSERT_EQ(rect.top(), 89.1);
    ASSERT_EQ(rect.right(), 20.5);
    ASSERT_EQ(rect.bottom(), -0.48);
}

TEST_F(ISpriteHandlerTest, FastBoundingRect)
{
    EXPECT_CALL(m_handler, fastBoundingRect()).WillOnce(Return(Rect(-44.6, 89.1, 20.5, -0.48)));
    Rect rect = m_sprite.fastBoundingRect();
    ASSERT_EQ(rect.left(), -44.6);
    ASSERT_EQ(rect.top(), 89.1);
    ASSERT_EQ(rect.right(), 20.5);
    ASSERT_EQ(rect.bottom(), -0.48);
}
