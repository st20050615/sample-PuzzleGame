/*
 * BallSprite.cpp
 *
 *  Created on: 2016/07/29
 *      Author: user
 */
#include "BallSprite.h"

USING_NS_CC;

// �R���X�g���N�^
BallSprite::BallSprite()
: _removedNo(0)
, _checkedX(false)
, _checkedY(false)
, _fallCount(0)
, _positionIndex(0, 0)
{
}

// �C���X�^���X�쐬
BallSprite* BallSprite::create(BallType type, bool visible) {
	BallSprite *pRet = new BallSprite();
	if (pRet && pRet->init(type, visible)) {
		pRet->autorelease();
		return pRet;
	} else {
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

// ������
bool BallSprite::init(BallType type, bool visible) {
	if (!Sprite::initWithFile(getBallImageFilePath(type))) {
		return false;
	}

	_ballType = type;

	setVisible(visible);

	return true;
}

// �p�����[�^�̃��Z�b�g
void BallSprite::resetParams() {
	_removedNo = 0;
	_checkedX = false;
	_checkedY = false;
	_fallCount = 0;
}

// �����ʒu�ֈړ�
void BallSprite::resetPosition() {
	// �ʒu��ύX����
	setPosition(getPositionForPositionIndex(_positionIndex));
}

// �ʒu�C���f�b�N�X��Ԃ�
BallSprite::PositionIndex BallSprite::getPositionIndex() {
	// �ʒu�C���f�b�N�X��Ԃ�
	return _positionIndex;
}

// �ʒu�C���f�b�N�X��ݒ�
void BallSprite::setPositionIndex(PositionIndex positionIndex) {
	// �ʒu�C���f�b�N�X��ێ�����
	_positionIndex = positionIndex;

	// �^�O���Z�b�g����
	setTag(generateTag(_positionIndex));
}

// �C���f�b�N�X�ƃ^�O�ƈʒu��ύX����
void BallSprite::setPositionIndexAndChangePosition(PositionIndex positionIndex) {
	// �C���f�b�N�X�ƃ^�O��ύX����
	setPositionIndex(positionIndex);

	// �ʒu��ύX����
	resetPosition();
}

// �����A�j���[�V�����Ɨ����A�j���[�V����
void BallSprite::removingAndFallingAnimation(int maxRemovedNo) {
	// �{�[���������A�j���[�V����
	removingAnimation(maxRemovedNo);

	// �{�[���𗎂Ƃ��A�j���[�V����
	fallingAnimation(maxRemovedNo);
}

//�{�[���폜�A�j���[�V����
void BallSprite::removingAnimation(int maxRemovedNo)
{
    if (_removedNo > 0)
    {
        //�{�[���������A�j���[�V����
        auto delay1 = DelayTime::create(ONE_ACTION_TIME * (_removedNo - 1));
        auto fade = FadeTo::create(ONE_ACTION_TIME, 0);
        auto delay2 = DelayTime::create(ONE_ACTION_TIME * (maxRemovedNo - _removedNo));
        auto removeSelf = RemoveSelf::create(false);

        //�A�j���[�V�������s
        runAction(Sequence::create(delay1, fade, delay2, removeSelf, nullptr));
    }
}

//�{�[�������A�j���[�V����
void BallSprite::fallingAnimation(int maxRemovedNo)
{
    if (_fallCount > 0)
    {
        //�{�[���𗎂Ƃ��A�j���[�V����
        setPositionIndex(PositionIndex(_positionIndex.x, _positionIndex.y - _fallCount));

        auto delay = DelayTime::create(ONE_ACTION_TIME * maxRemovedNo);
        auto show = Show::create();
        auto move = MoveTo::create(ONE_ACTION_TIME, getPositionForPositionIndex(getPositionIndex()));
        runAction(Sequence::create(delay, show, move, nullptr));
    }
}

// �{�[���摜�̎擾
std::string BallSprite::getBallImageFilePath(BallType type) {
	// �{�[���^�C�v�ɓK�����摜��Ԃ�
	switch (type) {
		case BallType::Red: return "red.png";
		case BallType::Blue: return "blue.png";
		case BallType::Yellow: return "yellow.png";
		case BallType::Green: return "green.png";
		case BallType::Purple: return "purple.png";
		default: return "pink.png";
	}
}

// �ʒu�C���f�b�N�X����Point���擾
Point BallSprite::getPositionForPositionIndex(PositionIndex positionIndex) {
	return Point(BALL_SIZE * (positionIndex.x - 0.5) + 1,
				BALL_SIZE * (positionIndex.y - 0.5) + 1);
}

// �ʒu�C���f�b�N�X����^�O���擾
int BallSprite::generateTag(PositionIndex positionIndex) {
	return positionIndex.x * 10 + positionIndex.y;
}
