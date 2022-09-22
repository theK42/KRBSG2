#pragma once
#include "Boundaries2D.h"
#include "HierarchicalTransform2D.h"
#include "SpriteRenderer.h"
#include "SpriteFactory.h"
#include "LuaScheduler.h"
#include "Disposable.h"


class PoolParty;
namespace KEngineCore
{
	class DataTree;
}

struct EnemyShip
{
	void Move(KEngine2D::Point direction);

	KEngine2D::StaticTransform *				mSelfTransform;
	KEngine2D::UpdatingHierarchicalTransform *	mModelTransform;
	KEngine2D::BoundingArea *					mBoundingArea;
	//std::vector<KEngine2D::BoundingBox *>		mBoxes;
	//std::vector<KEngine2D::BoundingCircle *>	mCircles;

	KEngineOpenGL::SpriteGraphic *				mGraphic;
	KEngineCore::ScheduledLuaThread *			mScript;

	KEngineCore::DisposableGroup				mDisposables;

	bool										mInitialized;
};

class EnemyShipSystem : public KEngineCore::LuaLibrary
{
public:
	EnemyShipSystem();
	~EnemyShipSystem();

	void Init(PoolParty* poolParty, KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory, KEngineCore::DataTree * shipDescription);
	void Deinit();

	EnemyShip* CreateEnemyShip(KEngine2D::Point position, const KEngineCore::DataTree * shipDescription);
	void ReleaseEnemyShip(EnemyShip* ship);

	void RegisterLibrary(lua_State* luaState, char const* name = "enemyships");
private:
	PoolParty*								mPoolParty;
	KEngineCore::Pool <EnemyShip>			mEnemyShipPool;

	KEngine2D::HierarchyUpdater* mHierarchySystem{ nullptr };
	KEngineOpenGL::SpriteRenderer* mRenderer{ nullptr };
	KEngineCore::LuaScheduler* mLuaScheduler{ nullptr };
	SpriteFactory* mSpriteFactory{ nullptr };
	KEngineCore::LuaWrapping<EnemyShip>mLuaWrapping;

	KEngineCore::DataTree* mShipDescription{ nullptr };
};



