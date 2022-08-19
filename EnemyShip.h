#pragma once
#include "Boundaries2D.h"
#include "HierarchicalTransform2D.h"
#include "SpriteRenderer.h"
#include "SpriteFactory.h"
#include "LuaScheduler.h"

class EnemyShip
{
public:
	EnemyShip();
	~EnemyShip();
	void Init(KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory, KEngine2D::Point position);
	void Deinit();
	void Move(KEngine2D::Point direction);
private:
	//KEngineBox2D::Box2DTransform				mBoxMechanics;
	KEngine2D::StaticTransform					mSelfTransform;
	KEngine2D::UpdatingHierarchicalTransform	mModelTransform;
	//KEngine2D::BoundingArea						mBoundingArea;
	//KEngine2D::BoundingBox						mBoundary;
	KEngineOpenGL::SpriteGraphic				mGraphic;
	KEngineCore::ScheduledLuaThread				mScript;
	bool										mInitialized;
};

class EnemyShipSystem : public KEngineCore::LuaLibrary
{
public:
	EnemyShipSystem();
	~EnemyShipSystem();

	void Init(KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory);
	void Deinit();

	EnemyShip* CreateEnemyShip(KEngine2D::Point position);
	void ReleaseEnemyShip(EnemyShip* ship);

	void RegisterLibrary(lua_State* luaState, char const* name = "enemyships");
private:
	EnemyShip*								mEnemyShipPool;
	std::list<EnemyShip*>					mFreeEnemyShips;

	KEngine2D::HierarchyUpdater* mHierarchySystem{ nullptr };
	KEngineOpenGL::SpriteRenderer* mRenderer{ nullptr };
	KEngineCore::LuaScheduler* mLuaScheduler{ nullptr };
	SpriteFactory* mSpriteFactory{ nullptr };
	KEngineCore::LuaWrapping<EnemyShip>mLuaWrapping;
};



