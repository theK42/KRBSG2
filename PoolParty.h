#pragma once
#include "Pool.h"
#include "LuaScheduler.h"
#include "Timer.h"
#include "StaticTransform2D.h"
#include "HierarchicalTransform2D.h"
#include "MechanicalTransform2D.h"
#include "SpriteRenderer.h"


class PoolParty
{
public:
	PoolParty();
	~PoolParty();

	void Init();
	void Deinit();

	KEngineCore::Pool<KEngineCore::ScheduledLuaThread>& GetLuaPool();
	KEngineCore::Pool<KEngineCore::Timeout>& GetTimePool();
	KEngineCore::Pool<KEngine2D::StaticTransform>& GetStaticPool();
	KEngineCore::Pool<KEngine2D::UpdatingHierarchicalTransform>& GetHierarchyPool();
	KEngineCore::Pool<KEngine2D::UpdatingMechanicalTransform>& GetMechanicalPool();
	KEngineCore::Pool<KEngineOpenGL::SpriteGraphic>& GetSpritePool();

private:
	KEngineCore::Pool<KEngineCore::ScheduledLuaThread>			mLuaThreadPool;
	KEngineCore::Pool<KEngineCore::Timeout>						mTimePool;
	KEngineCore::Pool<KEngine2D::StaticTransform>				mStaticTransformPool;
	KEngineCore::Pool<KEngine2D::UpdatingHierarchicalTransform>	mHierarchicalTransformPool;
	KEngineCore::Pool<KEngine2D::UpdatingMechanicalTransform>	mMechanicalTransformPool;
	KEngineCore::Pool<KEngineOpenGL::SpriteGraphic>				mSpritePool;
};

