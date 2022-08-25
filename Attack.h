#pragma once
#include "Transform2D.h"

class AttackFactory
{
public:
	AttackFactory() {}
	virtual ~AttackFactory() {}

	virtual void CreateAttack(const KEngine2D::Transform& origin) = 0;
};