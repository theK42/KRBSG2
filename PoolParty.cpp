#include "PoolParty.h"

PoolParty::PoolParty()
{
}

PoolParty::~PoolParty()
{
	Deinit();
}

void PoolParty::Init()
{
	mLuaThreadPool.Init();
	mTimePool.Init();
	mStaticTransformPool.Init();
	mHierarchicalTransformPool.Init();
	mMechanicalTransformPool.Init();
	mSpritePool.Init();
}

void PoolParty::Deinit()
{
	mLuaThreadPool.Deinit();	
	mTimePool.Deinit();
	mStaticTransformPool.Deinit();
	mHierarchicalTransformPool.Deinit();
	mMechanicalTransformPool.Deinit();
	mSpritePool.Deinit();
}

KEngineCore::Pool<KEngineCore::ScheduledLuaThread>& PoolParty::GetLuaPool()
{
	return mLuaThreadPool;
}

KEngineCore::Pool<KEngineCore::Timeout>& PoolParty::GetTimePool()
{
	return mTimePool;
}

KEngineCore::Pool<KEngine2D::StaticTransform>& PoolParty::GetStaticPool()
{
	return mStaticTransformPool;
}

KEngineCore::Pool<KEngine2D::UpdatingHierarchicalTransform>& PoolParty::GetHierarchyPool()
{
	return mHierarchicalTransformPool;
}

KEngineCore::Pool<KEngine2D::UpdatingMechanicalTransform>& PoolParty::GetMechanicalPool()
{
	return mMechanicalTransformPool;
}

KEngineCore::Pool<KEngineOpenGL::SpriteGraphic>& PoolParty::GetSpritePool()
{
	return mSpritePool;
}
