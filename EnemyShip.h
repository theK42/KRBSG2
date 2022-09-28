#pragma once
#include "Boundaries2D.h"
#include "HierarchicalTransform2D.h"
#include "SpriteRenderer.h"
#include "SpriteFactory.h"
#include "LuaScheduler.h"
#include "Disposable.h"
#include "Collision2D.h"


class KRBSGCollisionDispatcher;
class PoolParty;
namespace KEngineCore
{
	class DataTree;
}


class EnemyShipSystem;

struct EnemyShip
{
	void Move(KEngine2D::Point direction);
	void Die();

	EnemyShipSystem *							mSystem;
	KEngine2D::StaticTransform *				mSelfTransform;
	KEngine2D::UpdatingHierarchicalTransform *	mModelTransform;
	KEngine2D::BoundingArea *					mBoundingArea;

	KEngineOpenGL::SpriteGraphic *				mGraphic;
	KEngineCore::ScheduledLuaThread *			mScript;

	KEngineCore::DisposableGroup				mDisposables;

	KEngine2D::ColliderHandle					mColliderHandle;

	bool										mInitialized;
};

class EnemyShipSystem : public KEngineCore::LuaLibrary
{
public:
	EnemyShipSystem();
	~EnemyShipSystem();

	void Init(PoolParty* poolParty, KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory, KEngine2D::CollisionSystem * collisionSystem, KRBSGCollisionDispatcher * collisionDispatcher,  KEngineCore::DataTree* shipDescription, KEngineCore::DataTree* collisionsData);
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

	KEngine2D::CollisionSystem* mCollisionSystem{ nullptr };
	KRBSGCollisionDispatcher* mCollisionDispatcher{ nullptr };

	KEngineCore::DataTree* mShipDescription{ nullptr };
	KEngineCore::DataTree* mCollisionsData{ nullptr };
};



