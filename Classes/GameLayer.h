/*
 * GameLayer.h
 *
 *  Created on: 2016/07/29
 *      Author: user
 */

#ifndef GAMELAYER_H_
#define GAMELAYER_H_

#include "cocos2d.h"
#include <random>
#include "BallSprite.h"

class GameLayer : public cocos2d::Layer {
protected:
	// ボールチェック方向
	enum class Direction {
		x, y
	};

	// Zオーダー
	enum ZOrder {
		BgForCharacter = 0,
		BgForPuzzle,
		Enemy,
		EnemyHp,
		Char,
		CharHp,
		Ball,
		Level,
		Result,
	};

	std::default_random_engine _engine;		// 乱数生成エンジン
	std::discrete_distribution<int> _distForBall;	// 乱数の分布
	BallSprite* _movingBall;	// 動かしているボール
	bool _movedBall;			// 他ボールとの接触連携
	bool _touchable;			// タップの可否
	int _maxRemovedNo;			// 一度に削除される最大連鎖の番号
	int _chainNumber;			// 連鎖数のカウント
	std::vector<std::map<BallSprite::BallType, int>> _removeNumbers;	// 消去するボールのカウント

	void initBackground();	// 背景の初期化
	void initBalls();		// ボールの初期表示
	BallSprite* newBalls(BallSprite::PositionIndex positionIndex, bool visible);	// 新規ボール作成
	BallSprite* getTouchBall(cocos2d::Point touchPos,
			BallSprite::PositionIndex withoutPosIndex = BallSprite::PositionIndex());	// タッチしたボールを取得
	void movedBall();		// タップ操作によるボールの移動完了時処理
	void checksLinedBalls();	// 一列に並んだボールがあるかチェックする
	bool existsLinedBalls();	// 3個以上並んだボールがあるかチェックする
	cocos2d::Map<int, BallSprite*> getAllBalls();	// 全てのボールのBallTypeを取得
	bool isSameBallType(BallSprite::PositionIndex current, Direction direction);	// 指定方向のボールと同じ色かチェックする
	void initBallParams();	// ボールのパラメータを初期化する
	void checkedBall(BallSprite::PositionIndex current, Direction direction);	// 全ボールに対してボールの並びをチェックする
	void removeAndGenerateBalls();	// ボールの削除とボールの生成
	void generateBalls(int xLineNum, int fallCount);	// ボールを生成する
	void animationBalls();	// ボールの消去と落下アニメーション

public:
	GameLayer();	// コンストラクタ
	virtual bool init();	// 初期化
	CREATE_FUNC(GameLayer);	// create関数生成
	static cocos2d::Scene* createScene();	// シーン生成

	// シングルタップイベント
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* unused_event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* unused_event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* unused_event);
	virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* unused_event);
};

#endif /* GAMELAYER_H_ */
