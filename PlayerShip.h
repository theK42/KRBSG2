#pragma once
#include "Boundaries2D.h"
#include "HierarchicalTransform2D.h"
#include "SpriteRenderer.h"
#include "SpriteFactory.h"
#include "LuaScheduler.h"

class PlayerShip
{
public:
	PlayerShip();
	~PlayerShip();
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

class PlayerShipSystem : public KEngineCore::LuaLibrary
{
public:
	PlayerShipSystem();
	~PlayerShipSystem();

	void Init(KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory);
	void Deinit();

	PlayerShip *CreatePlayerShip(KEngine2D::Point position);

	void RegisterLibrary(lua_State* luaState, char const* name = "playerships");
private:
	PlayerShip mPlayerShip;

	KEngine2D::HierarchyUpdater* mHierarchySystem{ nullptr };
	KEngineOpenGL::SpriteRenderer* mRenderer{ nullptr };
	KEngineCore::LuaScheduler* mLuaScheduler{ nullptr };
	SpriteFactory* mSpriteFactory{ nullptr };
	KEngineCore::LuaWrapping<PlayerShip>mLuaWrapping;
};



