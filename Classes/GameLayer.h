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
	// �{�[���`�F�b�N����
	enum class Direction {
		x, y
	};

	// Z�I�[�_�[
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

	std::default_random_engine _engine;		// ���������G���W��
	std::discrete_distribution<int> _distForBall;	// �����̕��z
	BallSprite* _movingBall;	// �������Ă���{�[��
	bool _movedBall;			// ���{�[���Ƃ̐ڐG�A�g
	bool _touchable;			// �^�b�v�̉�
	int _maxRemovedNo;			// ��x�ɍ폜�����ő�A���̔ԍ�
	int _chainNumber;			// �A�����̃J�E���g
	std::vector<std::map<BallSprite::BallType, int>> _removeNumbers;	// ��������{�[���̃J�E���g

	void initBackground();	// �w�i�̏�����
	void initBalls();		// �{�[���̏����\��
	BallSprite* newBalls(BallSprite::PositionIndex positionIndex, bool visible);	// �V�K�{�[���쐬
	BallSprite* getTouchBall(cocos2d::Point touchPos,
			BallSprite::PositionIndex withoutPosIndex = BallSprite::PositionIndex());	// �^�b�`�����{�[�����擾
	void movedBall();		// �^�b�v����ɂ��{�[���̈ړ�����������
	void checksLinedBalls();	// ���ɕ��񂾃{�[�������邩�`�F�b�N����
	bool existsLinedBalls();	// 3�ȏ���񂾃{�[�������邩�`�F�b�N����
	cocos2d::Map<int, BallSprite*> getAllBalls();	// �S�Ẵ{�[����BallType���擾
	bool isSameBallType(BallSprite::PositionIndex current, Direction direction);	// �w������̃{�[���Ɠ����F���`�F�b�N����
	void initBallParams();	// �{�[���̃p�����[�^������������
	void checkedBall(BallSprite::PositionIndex current, Direction direction);	// �S�{�[���ɑ΂��ă{�[���̕��т��`�F�b�N����
	void removeAndGenerateBalls();	// �{�[���̍폜�ƃ{�[���̐���
	void generateBalls(int xLineNum, int fallCount);	// �{�[���𐶐�����
	void animationBalls();	// �{�[���̏����Ɨ����A�j���[�V����

public:
	GameLayer();	// �R���X�g���N�^
	virtual bool init();	// ������
	CREATE_FUNC(GameLayer);	// create�֐�����
	static cocos2d::Scene* createScene();	// �V�[������

	// �V���O���^�b�v�C�x���g
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* unused_event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* unused_event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* unused_event);
	virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* unused_event);
};

#endif /* GAMELAYER_H_ */
