/*
 * GameLayer.cpp
 *
 *  Created on: 2016/07/29
 *      Author: user
 */
#include "GameLayer.h"

#define BALL_NUM_X 6
#define BALL_NUM_Y 5

USING_NS_CC;

//�R���X�g���N�^
GameLayer::GameLayer()
: _movingBall(nullptr)
, _movedBall(false)
, _touchable(true)
, _maxRemovedNo(0)
, _chainNumber(0)
{
    //��������������ъe�{�[���̏o���̏d�݂��w��
    std::random_device device;
    _engine = std::default_random_engine(device());
    _distForBall = std::discrete_distribution<int>{20, 20, 20, 20, 20, 10};
}

//�V�[������
Scene* GameLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = GameLayer::create();
    scene->addChild(layer);

    return scene;
}

//������
bool GameLayer::init()
{
    if (!Layer::init())
        return false;

    // �V���O���^�b�v�C�x���g�̎擾
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(_swallowsTouches);
    touchListener->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(GameLayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(GameLayer::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(GameLayer::onTouchCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    initBackground(); //�w�i�̏�����
    initBalls(); //�{�[���̏����\��

    return true;
}

//�w�i�̏�����
void GameLayer::initBackground()
{
    //�p�Y�����̔w�i
    auto bgForPuzzle = Sprite::create("Background2.png");
    bgForPuzzle->setAnchorPoint(Point::ZERO);
    bgForPuzzle->setPosition(Point::ZERO);
    addChild(bgForPuzzle, ZOrder::BgForPuzzle);
}

//�{�[���̏����\��
void GameLayer::initBalls()
{
    for (int x = 1; x <= BALL_NUM_X; x++)
    {
        for (int y = 1; y <= BALL_NUM_Y; y++)
        {
            //�{�[���𐶐�����
            newBalls(BallSprite::PositionIndex(x, y), true);
        }
    }
}

//�V�K�{�[���쐬
BallSprite* GameLayer::newBalls(BallSprite::PositionIndex positionIndex, bool visible)
{
    //���݂̃^�O���擾
    int currentTag = BallSprite::generateTag(positionIndex);

    //���������ɁA�����_���Ń^�C�v���擾
    int ballType;
    while (true)
    {
        ballType = _distForBall(_engine);

        if (!visible)
            break;

        //�Ó����̃`�F�b�N�i�{�[�����ׂ荇�킹�ɂȂ�Ȃ��悤�ɂ���j

        //���ׂ̃{�[��
        auto ballX1Tag = currentTag - 10; //1���ׂ�10�������l
        auto ballX2Tag = currentTag - 20; //2���ׂ�20�������l
        auto ballX1 = (BallSprite*)(getChildByTag(ballX1Tag));
        auto ballX2 = (BallSprite*)(getChildByTag(ballX2Tag));

        //���݂̃{�[�����A1���ׂ�2���ׂ̃{�[���Ɠ�������NG
        if (!(ballX1 && ballType == (int)ballX1->getBallType()) ||
            !(ballX2 && ballType == (int)ballX2->getBallType()))
        {
            //���ׂ̃{�[��
            auto ballY1Tag = currentTag - 1; //1���ׂ�1�������l
            auto ballY2Tag = currentTag - 2; //2���ׂ�2�������l
            auto ballY1 = (BallSprite*)(getChildByTag(ballY1Tag));
            auto ballY2 = (BallSprite*)(getChildByTag(ballY2Tag));

            //���݂̃{�[�����A1���ׂ�2���ׂ̃{�[���Ɠ�������NG
            if (!(ballY1 && ballType == (int)ballY1->getBallType()) ||
                !(ballY2 && ballType == (int)ballY2->getBallType()))
            {
                //���ׂƉ��ׂ�����Ȃ��ꍇ�́A���[�v�𔲂���
                break;
            }
        }
    }

    //�{�[���̕\��
    auto ball = BallSprite::create((BallSprite::BallType)ballType, visible);
    ball->setPositionIndexAndChangePosition(positionIndex);
    addChild(ball, ZOrder::Ball);

    return ball;
}

bool GameLayer::onTouchBegan(Touch* touch, Event* unused_event)
{
    if (!_touchable)
        return false;

    _movedBall = false;
    _movingBall = getTouchBall(touch->getLocation());

    if (_movingBall)
        return true;
    else
        return false;
}

void GameLayer::onTouchMoved(Touch* touch, Event* unused_event)
{
    //�X���C�v�ƂƂ��Ƀ{�[�����ړ�����
    _movingBall->setPosition(_movingBall->getPosition() + touch->getDelta());

    auto touchBall = getTouchBall(touch->getLocation(), _movingBall->getPositionIndex());
    if (touchBall && _movingBall != touchBall)
    {
        //�ړ����Ă���{�[�����A�ʂ̃{�[���̈ʒu�Ɉړ�
        _movedBall = true;

        //�ʂ̃{�[���̈ʒu�C���f�b�N�X���擾
        auto touchBallPositionIndex = touchBall->getPositionIndex();

        //�ʂ̃{�[�����ړ����Ă���{�[���̌��̈ʒu�ֈړ�����
        touchBall->setPositionIndexAndChangePosition(_movingBall->getPositionIndex());

        //�ړ����Ă���{�[���̏���ύX
        _movingBall->setPositionIndex(touchBallPositionIndex);
    }
}

void GameLayer::onTouchEnded(Touch* touch, Event* unused_event)
{
    //�^�b�v����ɂ��{�[���̈ړ�����������
    movedBall();
}

void GameLayer::onTouchCancelled(Touch* touch, Event* unused_event)
{
    onTouchEnded(touch, unused_event);
}

//�^�b�v�����ʒu�̃`�F�b�N
BallSprite* GameLayer::getTouchBall(Point touchPos, BallSprite::PositionIndex withoutPosIndex)
{
    for (int x = 1; x <= BALL_NUM_X; x++)
    {
        for (int y = 1; y <= BALL_NUM_Y; y++)
        {
            if (x == withoutPosIndex.x && y == withoutPosIndex.y)
            {
                //�w��ʒu�̃{�[���̏ꍇ�́A�ȉ��̏������s��Ȃ�
                continue;
            }

            //�^�b�v�ʒu�ɂ���{�[�����ǂ����𔻒f����
            int tag = BallSprite::generateTag(BallSprite::PositionIndex(x, y));
            auto ball = (BallSprite*)(getChildByTag(tag));
            if (ball)
            {
                //2�_�Ԃ̋��������߂�
                float distance = ball->getPosition().getDistance(touchPos);

                //�{�[���̓����蔻��͉~�`�B�܂�{�[���̒��S����̔��a�Ŕ��f����
                if (distance <= BALL_SIZE / 2)
                {
                    //�^�b�v�����ʒu�Ƀ{�[�������݂���
                    return ball;
                }
            }
        }
    }

    return nullptr;
}

//�^�b�v����ɂ��{�[���̈ړ�����������
void GameLayer::movedBall()
{
    //�ړ����Ă���{�[����{���̈ʒu�ɖ߂�
    _movingBall->resetPosition();
    _movingBall = nullptr;

    //���ɕ��񂾃{�[�������邩�`�F�b�N����
    _chainNumber = 0;
    _removeNumbers.clear();
    checksLinedBalls();
}

//���ɕ��񂾃{�[�������邩�`�F�b�N����
void GameLayer::checksLinedBalls()
{
    //��ʂ��^�b�v�s�Ƃ���
    _touchable = false;

    if (existsLinedBalls())
    {
        //3�ȏ���񂾃{�[���̑��݂���ꍇ

        //�A���J�E���g�A�b�v
        _chainNumber++;

        //�{�[���̍폜�Ɛ���
        removeAndGenerateBalls();

        //�A�j���[�V������ɍă`�F�b�N
        auto delay = DelayTime::create(ONE_ACTION_TIME * (_maxRemovedNo + 1));
        auto func = CallFunc::create(CC_CALLBACK_0(GameLayer::checksLinedBalls, this));
        auto seq = Sequence::create(delay, func, nullptr);
        runAction(seq);
    }
    else
    {
        //�^�b�v��L���ɂ���
        _touchable = true;
    }
}

//3�ȏ���񂾃{�[���̑��݃`�F�b�N
bool GameLayer::existsLinedBalls()
{
    //�{�[���̃p�����[�^������������
    initBallParams();

    //��������鏇�Ԃ̏�����
    _maxRemovedNo = 0;

    for (int x = 1; x <= BALL_NUM_X; x++)
    {
        for (int y = 1; y <= BALL_NUM_Y; y++)
        {
            //x�����̃{�[�����`�F�b�N
            checkedBall(BallSprite::PositionIndex(x, y), Direction::x);

            //y�����̃{�[�����`�F�b�N
            checkedBall(BallSprite::PositionIndex(x, y), Direction::y);
        }
    }

    //�߂�l�̌���
    return _maxRemovedNo > 0;
}

//�S�Ẵ{�[����BallType���擾
Map<int, BallSprite*> GameLayer::getAllBalls()
{
    auto balls = Map<int, BallSprite*>();

    for (auto object : getChildren())
    {
        auto ball = dynamic_cast<BallSprite*>(object);
        if (ball)
            balls.insert(ball->getTag(), ball);
    }

    return balls;
}

//�w������̃{�[���Ɠ����F���`�F�b�N����
bool GameLayer::isSameBallType(BallSprite::PositionIndex current, Direction direction)
{
    //�S�Ẵ{�[����BallType���擾
    auto allBalls = getAllBalls();

    if (direction == Direction::x)
    {
        if (current.x + 1 > BALL_NUM_X)
            //�񂪑��݂��Ȃ��ꍇ�͔�����
            return false;
    }
    else
    {
        if (current.y + 1 > BALL_NUM_Y)
            //�s�����݂��Ȃ��ꍇ�͔�����
            return false;
    }

    //���݂̃{�[�����擾
    int currentTag = BallSprite::generateTag(BallSprite::PositionIndex(current.x, current.y));
    BallSprite* currentBall = allBalls.at(currentTag);

    //���̃{�[�����擾
    int nextTag;
    if (direction == Direction::x)
        nextTag = BallSprite::generateTag(BallSprite::PositionIndex(current.x + 1, current.y));
    else
        nextTag = BallSprite::generateTag(BallSprite::PositionIndex(current.x, current.y + 1));
    auto nextBall = allBalls.at(nextTag);

    if (currentBall->getBallType() == nextBall->getBallType())
        //���̃{�[��������BallType�ł���
        return true;

    return false;
}

//�{�[���̃p�����[�^������������
void GameLayer::initBallParams()
{
    //�S�Ẵ{�[����BallType���擾
    auto allBalls = getAllBalls();

    for (auto ball : allBalls)
    {
        ball.second->resetParams();
    }
}

//�S�{�[���ɑ΂��ă{�[���̕��т��`�F�b�N����
void GameLayer::checkedBall(BallSprite::PositionIndex current, Direction direction)
{
    //�S�Ẵ{�[����BallType���擾
    auto allBalls = getAllBalls();

    //��������^�O�̐���
    int tag = BallSprite::generateTag(BallSprite::PositionIndex(current.x, current.y));
    BallSprite* ball = allBalls.at(tag);

    //�w������̃`�F�b�N�ς݃t���O���擾
    bool checked;
    if (direction == Direction::x)
        checked = ball->getCheckedX();
    else
        checked = ball->getCheckedY();

    if (!checked)
    {
        int num = 0;

        while (true)
        {
            //�����ʒu���擾
            BallSprite::PositionIndex searchPosition;
            if (direction == Direction::x)
                searchPosition = BallSprite::PositionIndex(current.x + num, current.y);
            else
                searchPosition = BallSprite::PositionIndex(current.x, current.y + num);

            //���̃{�[���Ɠ���ballType���`�F�b�N����
            if (isSameBallType(searchPosition, direction))
            {
                //���̃{�[���Ɠ���ballType
                int nextTag = BallSprite::generateTag(searchPosition);
                auto nextBall = allBalls.at(nextTag);

                //�`�F�b�N�����{�[���̃`�F�b�N�ς݃t���O�𗧂Ă�
                if (direction == Direction::x)
                    nextBall->setCheckedX(true);
                else
                    nextBall->setCheckedY(true);

                num++;
            }
            else
            {
                //���̃{�[�����قȂ�ballType

                if (num >= 2)
                {
                    int removedNo = 0;

                    //��������{�[���̃J�E���g
                    if (_removeNumbers.size() <= _chainNumber)
                    {
                        //�z�񂪑��݂��Ȃ��ꍇ�͒ǉ�����
                        std::map<BallSprite::BallType, int> removeNumber;
                        _removeNumbers.push_back(removeNumber);
                    }
                    _removeNumbers[_chainNumber][ball->getBallType()] += num + 1;

                    //���ł�RemovedNo��������̂����݂��邩�`�F�b�N
                    for (int i = 0; i <= num; i++)
                    {
                    BallSprite::PositionIndex linedPosition;
                        if (direction == Direction::x)
                            linedPosition = BallSprite::PositionIndex(current.x + i, current.y);
                        else
                            linedPosition = BallSprite::PositionIndex(current.x, current.y + i);

                        int linedBallTag = BallSprite::generateTag(linedPosition);
                        auto linedBall = allBalls.at(linedBallTag);
                        if (linedBall->getRemovedNo() > 0)
                        {
                            //���ł�RemovedNo��������̂����݂���̂ŁAremovedNo���擾�����̏������s��
                            removedNo = linedBall->getRemovedNo();
                            break;
                        }
                    }

                    //�������鏇�Ԃ̃J�E���g�A�b�v
                    if (removedNo == 0)
                        removedNo = ++_maxRemovedNo;

                    //3�ȏ����ł����ꍇ�́AreomvedNo���Z�b�g����
                    for (int i = 0; i <= num; i++)
                    {
                        BallSprite::PositionIndex linedPosition;
                        if (direction == Direction::x)
                            linedPosition = BallSprite::PositionIndex(current.x + i, current.y);
                        else
                            linedPosition = BallSprite::PositionIndex(current.x, current.y + i);

                        int linedBallTag = BallSprite::generateTag(linedPosition);
                        auto linedBall = allBalls.at(linedBallTag);
                        linedBall->setRemovedNo(removedNo);
                    }
                }

                break;
            }
        };

        //�w��������`�F�b�N�ς݂Ƃ���
        if (direction == Direction::x)
            ball->setCheckedX(true);
        else
            ball->setCheckedY(true);
    }
}

//�{�[���̍폜�ƃ{�[���̐���
void GameLayer::removeAndGenerateBalls()
{
    //�S�Ẵ{�[����BallType���擾
    auto allBalls = getAllBalls();

    int maxRemovedNo = 0;

    for (int x = 1; x <= BALL_NUM_X; x++)
    {
        int fallCount = 0;

        for (int y = 1; y <= BALL_NUM_Y; y++)
        {
            int tag = BallSprite::generateTag(BallSprite::PositionIndex(x, y));
            auto ball = allBalls.at(tag);

            if (ball) {
                int removedNoForBall = ball->getRemovedNo();

                if (removedNoForBall > 0)
                {
                    //������i�����J�E���g
                    fallCount++;

                    if (removedNoForBall > maxRemovedNo)
                        maxRemovedNo = removedNoForBall;
                }
                else
                {
                    //������i�����Z�b�g
                    ball->setFallCount(fallCount);
                }
            }
        }

        //�{�[���𐶐�����
        generateBalls(x, fallCount);
    }

    //�{�[���̏����������A�j���[�V����
    animationBalls();
}

//�{�[���𐶐�����
void GameLayer::generateBalls(int xLineNum, int fallCount)
{
    for (int i = 1; i <= fallCount; i++)
    {
        //�{�[���𐶐�����
        auto positionIndex = BallSprite::PositionIndex(xLineNum, BALL_NUM_Y + i);
        auto ball = newBalls(positionIndex, false);
        ball->setFallCount(fallCount);
    }
}

//�{�[���̏����Ɨ����A�j���[�V����
void GameLayer::animationBalls()
{
    //�S�Ẵ{�[����BallType���擾
    auto allBalls = getAllBalls();

    for (auto ball : allBalls)
    {
        //�{�[���̃A�j���[�V���������s����
        ball.second->removingAndFallingAnimation(_maxRemovedNo);
    }
}
