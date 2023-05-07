#pragma once
#include "Disposable.h"
#include "LuaScheduler.h"
#include "Timer.h"
#include "StaticTransform2D.h"
#include "HierarchicalTransform2D.h"
#include "MechanicalTransform2D.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include "Boundaries2D.h"
#include "Collision2D.h"
#include "LuaContext.h"
#include "Input.h"

class PoolParty
{
public:
	PoolParty();
	~PoolParty();

	void Init();
	void Deinit();

	KEngineCore::RecyclingPool<KEngineCore::ScheduledLuaThread>& GetLuaPool();
	KEngineCore::RecyclingPool<KEngineCore::LuaContext>& GetContextPool();
	KEngineCore::RecyclingPool<KEngineCore::Timeout>& GetTimePool();
	KEngineCore::RecyclingPool<KEngineCore::TimeForwarder>& GetContinuousTimePool();
	KEngineCore::RecyclingPool<KEngineBasics::InputForwarder>& GetInputForwardingPool();
	KEngineCore::RecyclingPool<KEngine2D::StaticTransform>& GetStaticPool();
	KEngineCore::RecyclingPool<KEngine2D::UpdatingHierarchicalTransform>& GetHierarchyPool();
	KEngineCore::RecyclingPool<KEngine2D::UpdatingMechanicalTransform>& GetMechanicalPool();
	KEngineCore::RecyclingPool<KEngineOpenGL::SpriteGraphic>& GetSpritePool();
	KEngineCore::RecyclingPool<KEngineOpenGL::TextSprite>& GetTextPool();
	KEngineCore::RecyclingPool<KEngine2D::BoundingBox>& GetBoxPool();
	KEngineCore::RecyclingPool<KEngine2D::BoundingCircle>& GetCirclePool();
	KEngineCore::RecyclingPool<KEngine2D::BoundingArea>& GetBoundsPool();
	KEngineCore::RecyclingPool<KEngine2D::Collider>& GetColliderPool();

private:
	KEngineCore::RecyclingPool<KEngineCore::ScheduledLuaThread>				mLuaThreadPool;
	KEngineCore::RecyclingPool<KEngineCore::LuaContext>						mContextPool;
	KEngineCore::RecyclingPool<KEngineCore::Timeout>						mTimePool;
	KEngineCore::RecyclingPool<KEngineCore::TimeForwarder>					mContinuousTimePool;
	KEngineCore::RecyclingPool<KEngineBasics::InputForwarder>				mInputForwardingPool;
	KEngineCore::RecyclingPool<KEngine2D::StaticTransform>					mStaticTransformPool;
	KEngineCore::RecyclingPool<KEngine2D::UpdatingHierarchicalTransform>	mHierarchicalTransformPool;
	KEngineCore::RecyclingPool<KEngine2D::UpdatingMechanicalTransform>		mMechanicalTransformPool;
	KEngineCore::RecyclingPool<KEngineOpenGL::SpriteGraphic>				mSpritePool;
	KEngineCore::RecyclingPool<KEngine2D::BoundingBox>						mBoxPool;
	KEngineCore::RecyclingPool<KEngine2D::BoundingCircle>					mCirclePool;
	KEngineCore::RecyclingPool<KEngine2D::BoundingArea>						mBoundsPool;
	KEngineCore::RecyclingPool<KEngine2D::Collider>							mColliderPool;
	KEngineCore::RecyclingPool<KEngineOpenGL::TextSprite>					mTextPool;


};

