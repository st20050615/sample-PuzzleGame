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

//コンストラクタ
GameLayer::GameLayer()
: _movingBall(nullptr)
, _movedBall(false)
, _touchable(true)
, _maxRemovedNo(0)
, _chainNumber(0)
{
    //乱数初期化および各ボールの出現の重みを指定
    std::random_device device;
    _engine = std::default_random_engine(device());
    _distForBall = std::discrete_distribution<int>{20, 20, 20, 20, 20, 10};
}

//シーン生成
Scene* GameLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = GameLayer::create();
    scene->addChild(layer);

    return scene;
}

//初期化
bool GameLayer::init()
{
    if (!Layer::init())
        return false;

    // シングルタップイベントの取得
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(_swallowsTouches);
    touchListener->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(GameLayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(GameLayer::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(GameLayer::onTouchCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    initBackground(); //背景の初期化
    initBalls(); //ボールの初期表示

    return true;
}

//背景の初期化
void GameLayer::initBackground()
{
    //パズル部の背景
    auto bgForPuzzle = Sprite::create("Background2.png");
    bgForPuzzle->setAnchorPoint(Point::ZERO);
    bgForPuzzle->setPosition(Point::ZERO);
    addChild(bgForPuzzle, ZOrder::BgForPuzzle);
}

//ボールの初期表示
void GameLayer::initBalls()
{
    for (int x = 1; x <= BALL_NUM_X; x++)
    {
        for (int y = 1; y <= BALL_NUM_Y; y++)
        {
            //ボールを生成する
            newBalls(BallSprite::PositionIndex(x, y), true);
        }
    }
}

//新規ボール作成
BallSprite* GameLayer::newBalls(BallSprite::PositionIndex positionIndex, bool visible)
{
    //現在のタグを取得
    int currentTag = BallSprite::generateTag(positionIndex);

    //乱数を元に、ランダムでタイプを取得
    int ballType;
    while (true)
    {
        ballType = _distForBall(_engine);

        if (!visible)
            break;

        //妥当性のチェック（ボールが隣り合わせにならないようにする）

        //左隣のボール
        auto ballX1Tag = currentTag - 10; //1つ左隣は10引いた値
        auto ballX2Tag = currentTag - 20; //2つ左隣は20引いた値
        auto ballX1 = (BallSprite*)(getChildByTag(ballX1Tag));
        auto ballX2 = (BallSprite*)(getChildByTag(ballX2Tag));

        //現在のボールが、1つ左隣と2つ左隣のボールと同じだとNG
        if (!(ballX1 && ballType == (int)ballX1->getBallType()) ||
            !(ballX2 && ballType == (int)ballX2->getBallType()))
        {
            //下隣のボール
            auto ballY1Tag = currentTag - 1; //1つ下隣は1引いた値
            auto ballY2Tag = currentTag - 2; //2つ下隣は2引いた値
            auto ballY1 = (BallSprite*)(getChildByTag(ballY1Tag));
            auto ballY2 = (BallSprite*)(getChildByTag(ballY2Tag));

            //現在のボールが、1つ下隣と2つ下隣のボールと同じだとNG
            if (!(ballY1 && ballType == (int)ballY1->getBallType()) ||
                !(ballY2 && ballType == (int)ballY2->getBallType()))
            {
                //左隣と下隣が揃わない場合は、ループを抜ける
                break;
            }
        }
    }

    //ボールの表示
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
    //スワイプとともにボールを移動する
    _movingBall->setPosition(_movingBall->getPosition() + touch->getDelta());

    auto touchBall = getTouchBall(touch->getLocation(), _movingBall->getPositionIndex());
    if (touchBall && _movingBall != touchBall)
    {
        //移動しているボールが、別のボールの位置に移動
        _movedBall = true;

        //別のボールの位置インデックスを取得
        auto touchBallPositionIndex = touchBall->getPositionIndex();

        //別のボールを移動しているボールの元の位置へ移動する
        touchBall->setPositionIndexAndChangePosition(_movingBall->getPositionIndex());

        //移動しているボールの情報を変更
        _movingBall->setPositionIndex(touchBallPositionIndex);
    }
}

void GameLayer::onTouchEnded(Touch* touch, Event* unused_event)
{
    //タップ操作によるボールの移動完了時処理
    movedBall();
}

void GameLayer::onTouchCancelled(Touch* touch, Event* unused_event)
{
    onTouchEnded(touch, unused_event);
}

//タップした位置のチェック
BallSprite* GameLayer::getTouchBall(Point touchPos, BallSprite::PositionIndex withoutPosIndex)
{
    for (int x = 1; x <= BALL_NUM_X; x++)
    {
        for (int y = 1; y <= BALL_NUM_Y; y++)
        {
            if (x == withoutPosIndex.x && y == withoutPosIndex.y)
            {
                //指定位置のボールの場合は、以下の処理を行わない
                continue;
            }

            //タップ位置にあるボールかどうかを判断する
            int tag = BallSprite::generateTag(BallSprite::PositionIndex(x, y));
            auto ball = (BallSprite*)(getChildByTag(tag));
            if (ball)
            {
                //2点間の距離を求める
                float distance = ball->getPosition().getDistance(touchPos);

                //ボールの当たり判定は円形。つまりボールの中心からの半径で判断する
                if (distance <= BALL_SIZE / 2)
                {
                    //タップした位置にボールが存在する
                    return ball;
                }
            }
        }
    }

    return nullptr;
}

//タップ操作によるボールの移動完了時処理
void GameLayer::movedBall()
{
    //移動しているボールを本来の位置に戻す
    _movingBall->resetPosition();
    _movingBall = nullptr;

    //一列に並んだボールがあるかチェックする
    _chainNumber = 0;
    _removeNumbers.clear();
    checksLinedBalls();
}

//一列に並んだボールがあるかチェックする
void GameLayer::checksLinedBalls()
{
    //画面をタップ不可とする
    _touchable = false;

    if (existsLinedBalls())
    {
        //3個以上並んだボールの存在する場合

        //連鎖カウントアップ
        _chainNumber++;

        //ボールの削除と生成
        removeAndGenerateBalls();

        //アニメーション後に再チェック
        auto delay = DelayTime::create(ONE_ACTION_TIME * (_maxRemovedNo + 1));
        auto func = CallFunc::create(CC_CALLBACK_0(GameLayer::checksLinedBalls, this));
        auto seq = Sequence::create(delay, func, nullptr);
        runAction(seq);
    }
    else
    {
        //タップを有効にする
        _touchable = true;
    }
}

//3個以上並んだボールの存在チェック
bool GameLayer::existsLinedBalls()
{
    //ボールのパラメータを初期化する
    initBallParams();

    //消去される順番の初期化
    _maxRemovedNo = 0;

    for (int x = 1; x <= BALL_NUM_X; x++)
    {
        for (int y = 1; y <= BALL_NUM_Y; y++)
        {
            //x方向のボールをチェック
            checkedBall(BallSprite::PositionIndex(x, y), Direction::x);

            //y方向のボールをチェック
            checkedBall(BallSprite::PositionIndex(x, y), Direction::y);
        }
    }

    //戻り値の決定
    return _maxRemovedNo > 0;
}

//全てのボールのBallTypeを取得
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

//指定方向のボールと同じ色かチェックする
bool GameLayer::isSameBallType(BallSprite::PositionIndex current, Direction direction)
{
    //全てのボールのBallTypeを取得
    auto allBalls = getAllBalls();

    if (direction == Direction::x)
    {
        if (current.x + 1 > BALL_NUM_X)
            //列が存在しない場合は抜ける
            return false;
    }
    else
    {
        if (current.y + 1 > BALL_NUM_Y)
            //行が存在しない場合は抜ける
            return false;
    }

    //現在のボールを取得
    int currentTag = BallSprite::generateTag(BallSprite::PositionIndex(current.x, current.y));
    BallSprite* currentBall = allBalls.at(currentTag);

    //次のボールを取得
    int nextTag;
    if (direction == Direction::x)
        nextTag = BallSprite::generateTag(BallSprite::PositionIndex(current.x + 1, current.y));
    else
        nextTag = BallSprite::generateTag(BallSprite::PositionIndex(current.x, current.y + 1));
    auto nextBall = allBalls.at(nextTag);

    if (currentBall->getBallType() == nextBall->getBallType())
        //次のボールが同じBallTypeである
        return true;

    return false;
}

//ボールのパラメータを初期化する
void GameLayer::initBallParams()
{
    //全てのボールのBallTypeを取得
    auto allBalls = getAllBalls();

    for (auto ball : allBalls)
    {
        ball.second->resetParams();
    }
}

//全ボールに対してボールの並びをチェックする
void GameLayer::checkedBall(BallSprite::PositionIndex current, Direction direction)
{
    //全てのボールのBallTypeを取得
    auto allBalls = getAllBalls();

    //検索するタグの生成
    int tag = BallSprite::generateTag(BallSprite::PositionIndex(current.x, current.y));
    BallSprite* ball = allBalls.at(tag);

    //指定方向のチェック済みフラグを取得
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
            //検索位置を取得
            BallSprite::PositionIndex searchPosition;
            if (direction == Direction::x)
                searchPosition = BallSprite::PositionIndex(current.x + num, current.y);
            else
                searchPosition = BallSprite::PositionIndex(current.x, current.y + num);

            //次のボールと同じballTypeかチェックする
            if (isSameBallType(searchPosition, direction))
            {
                //次のボールと同じballType
                int nextTag = BallSprite::generateTag(searchPosition);
                auto nextBall = allBalls.at(nextTag);

                //チェックしたボールのチェック済みフラグを立てる
                if (direction == Direction::x)
                    nextBall->setCheckedX(true);
                else
                    nextBall->setCheckedY(true);

                num++;
            }
            else
            {
                //次のボールが異なるballType

                if (num >= 2)
                {
                    int removedNo = 0;

                    //消去するボールのカウント
                    if (_removeNumbers.size() <= _chainNumber)
                    {
                        //配列が存在しない場合は追加する
                        std::map<BallSprite::BallType, int> removeNumber;
                        _removeNumbers.push_back(removeNumber);
                    }
                    _removeNumbers[_chainNumber][ball->getBallType()] += num + 1;

                    //すでにRemovedNoがあるものが存在するかチェック
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
                            //すでにRemovedNoがあるものが存在するので、removedNoを取得し次の処理を行う
                            removedNo = linedBall->getRemovedNo();
                            break;
                        }
                    }

                    //消去する順番のカウントアップ
                    if (removedNo == 0)
                        removedNo = ++_maxRemovedNo;

                    //3個以上並んでいた場合は、reomvedNoをセットする
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

        //指定方向をチェック済みとする
        if (direction == Direction::x)
            ball->setCheckedX(true);
        else
            ball->setCheckedY(true);
    }
}

//ボールの削除とボールの生成
void GameLayer::removeAndGenerateBalls()
{
    //全てのボールのBallTypeを取得
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
                    //落ちる段数をカウント
                    fallCount++;

                    if (removedNoForBall > maxRemovedNo)
                        maxRemovedNo = removedNoForBall;
                }
                else
                {
                    //落ちる段数をセット
                    ball->setFallCount(fallCount);
                }
            }
        }

        //ボールを生成する
        generateBalls(x, fallCount);
    }

    //ボールの消去＆落下アニメーション
    animationBalls();
}

//ボールを生成する
void GameLayer::generateBalls(int xLineNum, int fallCount)
{
    for (int i = 1; i <= fallCount; i++)
    {
        //ボールを生成する
        auto positionIndex = BallSprite::PositionIndex(xLineNum, BALL_NUM_Y + i);
        auto ball = newBalls(positionIndex, false);
        ball->setFallCount(fallCount);
    }
}

//ボールの消去と落下アニメーション
void GameLayer::animationBalls()
{
    //全てのボールのBallTypeを取得
    auto allBalls = getAllBalls();

    for (auto ball : allBalls)
    {
        //ボールのアニメーションを実行する
        ball.second->removingAndFallingAnimation(_maxRemovedNo);
    }
}
