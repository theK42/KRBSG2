#pragma once
#include "Disposable.h"
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

	KEngineCore::RecyclingPool<KEngineCore::ScheduledLuaThread>& GetLuaPool();
	KEngineCore::RecyclingPool<KEngineCore::Timeout>& GetTimePool();
	KEngineCore::RecyclingPool<KEngine2D::StaticTransform>& GetStaticPool();
	KEngineCore::RecyclingPool<KEngine2D::UpdatingHierarchicalTransform>& GetHierarchyPool();
	KEngineCore::RecyclingPool<KEngine2D::UpdatingMechanicalTransform>& GetMechanicalPool();
	KEngineCore::RecyclingPool<KEngineOpenGL::SpriteGraphic>& GetSpritePool();

private:
	KEngineCore::RecyclingPool<KEngineCore::ScheduledLuaThread>				mLuaThreadPool;
	KEngineCore::RecyclingPool<KEngineCore::Timeout>						mTimePool;
	KEngineCore::RecyclingPool<KEngine2D::StaticTransform>					mStaticTransformPool;
	KEngineCore::RecyclingPool<KEngine2D::UpdatingHierarchicalTransform>	mHierarchicalTransformPool;
	KEngineCore::RecyclingPool<KEngine2D::UpdatingMechanicalTransform>		mMechanicalTransformPool;
	KEngineCore::RecyclingPool<KEngineOpenGL::SpriteGraphic>				mSpritePool;





};

