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
	mBoxPool.Init();
	mCirclePool.Init();
	mBoundsPool.Init();
	mColliderPool.Init();
}

void PoolParty::Deinit()
{
	mLuaThreadPool.Deinit();	
	mTimePool.Deinit();
	mStaticTransformPool.Deinit();
	mHierarchicalTransformPool.Deinit();
	mMechanicalTransformPool.Deinit();
	mSpritePool.Deinit();
	mBoxPool.Deinit();
	mCirclePool.Deinit();
	mBoundsPool.Deinit();
	mColliderPool.Deinit();
}

KEngineCore::RecyclingPool<KEngineCore::ScheduledLuaThread>& PoolParty::GetLuaPool()
{
	return mLuaThreadPool;
}

KEngineCore::RecyclingPool<KEngineCore::Timeout>& PoolParty::GetTimePool()
{
	return mTimePool;
}

KEngineCore::RecyclingPool<KEngine2D::StaticTransform>& PoolParty::GetStaticPool()
{
	return mStaticTransformPool;
}

KEngineCore::RecyclingPool<KEngine2D::UpdatingHierarchicalTransform>& PoolParty::GetHierarchyPool()
{
	return mHierarchicalTransformPool;
}

KEngineCore::RecyclingPool<KEngine2D::UpdatingMechanicalTransform>& PoolParty::GetMechanicalPool()
{
	return mMechanicalTransformPool;
}

KEngineCore::RecyclingPool<KEngineOpenGL::SpriteGraphic>& PoolParty::GetSpritePool()
{
	return mSpritePool;
}

KEngineCore::RecyclingPool<KEngine2D::BoundingBox>& PoolParty::GetBoxPool()
{
	return mBoxPool;
}

KEngineCore::RecyclingPool<KEngine2D::BoundingCircle>& PoolParty::GetCirclePool()
{
	return mCirclePool;
}

KEngineCore::RecyclingPool<KEngine2D::BoundingArea>& PoolParty::GetBoundsPool()
{
	return mBoundsPool;
}

KEngineCore::RecyclingPool<KEngine2D::Collider>& PoolParty::GetColliderPool()
{
	return mColliderPool;
}
