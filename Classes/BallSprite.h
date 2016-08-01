/*
 * BallSprite.h
 *
 *  Created on: 2016/07/29
 *      Author: user
 */

#ifndef BALLSPRITE_H_
#define BALLSPRITE_H_

#include "cocos2d.h"

#define BALL_SIZE 106	// �{�[���̃T�C�Y
#define ONE_ACTION_TIME 0.2		// �{�[���폜�E�����A�N�V�����̎���

class BallSprite : public cocos2d::Sprite {
public:
	// �{�[���̎��
	enum class BallType {
		Blue,	// ��
		Red,	// ��
		Green,	// ��
		Yellow,	// ���F
		Purple,	// ��
		Pink,	// �s���N
	};

	// �ʒu�C���f�b�N�X
	struct PositionIndex {
		// �R���X�g���N�^
		PositionIndex() {
			x = 0;
			y = 0;
		}

		// �R���X�g���N�^
		PositionIndex(int _x, int _y) {
			x = _x;
			y = _y;
		}
		int x;
		int y;
	};

	BallSprite();	// �R���X�g���N�^
	static BallSprite* create(BallType type, bool visible);		// �C���X�^���X����
	virtual bool init(BallType type, bool visible);				// ������

	CC_SYNTHESIZE(int, _removedNo, RemovedNo);	// �{�[���폜�̏���
	CC_SYNTHESIZE(bool, _checkedX, CheckedX);	// �������m�F�t���O
	CC_SYNTHESIZE(bool, _checkedY, CheckedY);	// �c�����m�F�t���O
	CC_SYNTHESIZE(int, _fallCount, FallCount);	// �{�[���������̗����i��
	CC_SYNTHESIZE_READONLY(BallType, _ballType, BallType);	// �{�[���̎��
	CC_PROPERTY(PositionIndex, _positionIndex, PositionIndex);	// �ʒu�C���f�b�N�X

	void setPositionIndexAndChangePosition(PositionIndex positionIndex);	// �ʒu�C���f�b�N�X�̐ݒ肨��шʒu�ύX
	void resetParams();		// �p�����[�^�̃��Z�b�g
	void resetPosition();	// �����ʒu�ֈړ�
	void removingAndFallingAnimation(int maxRemovedNo);		// �����A�j���[�V�����Ɨ����A�j���[�V����

	static std::string getBallImageFilePath(BallType type);	// �{�[���摜�擾
	static cocos2d::Point getPositionForPositionIndex(PositionIndex positionIndex);	// �ʒu�C���f�b�N�X����Point���擾
	static int generateTag(PositionIndex positionIndex);	// �ʒu�C���f�b�N�X����^�O���擾

protected:
	void removingAnimation(int maxRemovedNo);	// �{�[���폜�A�j���[�V����
	void fallingAnimation(int maxRemovedNo);	// �{�[�������A�j���[�V����
};

#endif /* BALLSPRITE_H_ */
