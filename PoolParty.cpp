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
	mContextPool.Init();
	mContinuousTimePool.Init();
	mInputForwardingPool.Init();
	mTimePool.Init();
	mStaticTransformPool.Init();
	mHierarchicalTransformPool.Init();
	mMechanicalTransformPool.Init();
	mSpritePool.Init();
	mBoxPool.Init();
	mCirclePool.Init();
	mBoundsPool.Init();
	mColliderPool.Init();
	mTextPool.Init();
}

void PoolParty::Deinit()
{
	mLuaThreadPool.Deinit();
	mContextPool.Deinit();
	mContinuousTimePool.Deinit();
	mInputForwardingPool.Deinit();
	mTimePool.Deinit();
	mStaticTransformPool.Deinit();
	mHierarchicalTransformPool.Deinit();
	mMechanicalTransformPool.Deinit();
	mSpritePool.Deinit();
	mBoxPool.Deinit();
	mCirclePool.Deinit();
	mBoundsPool.Deinit();
	mColliderPool.Deinit();
	mTextPool.Deinit();
}

KEngineCore::RecyclingPool<KEngineCore::ScheduledLuaThread>& PoolParty::GetLuaPool()
{
	return mLuaThreadPool;
}

KEngineCore::RecyclingPool<KEngineCore::LuaContext>& PoolParty::GetContextPool()
{
	return mContextPool;
}

KEngineCore::RecyclingPool<KEngineCore::Timeout>& PoolParty::GetTimePool()
{
	return mTimePool;
}

KEngineCore::RecyclingPool<KEngineCore::TimeForwarder>& PoolParty::GetContinuousTimePool()
{
	return mContinuousTimePool;
}

KEngineCore::RecyclingPool<KEngineBasics::InputForwarder>& PoolParty::GetInputForwardingPool()
{
	return mInputForwardingPool;
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

KEngineCore::RecyclingPool<KEngineOpenGL::TextSprite>& PoolParty::GetTextPool()
{
	return mTextPool;
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
