#pragma once
#include "Attack.h"
#include "StringHash.h"
#include "Transform2D.h"
#include "Timer.h"
#include "Pool.h"
#include "PoolParty.h"

namespace KEngineCore
{
	class LuaScheduler;
	class SheduledLuaThread;
	class Timer;
	class Timeout;
}

namespace KEngine2D
{
	class HierarchyUpdater; 
	class UpdatingHierarchicalTransform;
	class MechanicsUpdater;
	class UpdatingMechanicalTransform;
}

namespace KEngineOpenGL
{
	class SpriteRenderer;
	class SpriteGraphic;
}

class SpriteFactory;

struct ProjectileDescription
{
	KEngineCore::StringHash	spriteId;
	KEngine2D::Point		velocity;
	float					lifeTime;
};


class ProjectileFactory;

struct Projectile
{
	KEngineCore::DisposableGroup				mDisposables;

	KEngine2D::UpdatingMechanicalTransform*		mMover{ nullptr };
	KEngine2D::UpdatingHierarchicalTransform*	mModelTransform{ nullptr };
	KEngineOpenGL::SpriteGraphic*				mSpriteGraphic{ nullptr };
	KEngineCore::Timeout*						mLifeTime{ nullptr };
	ProjectileFactory*							mProjectileFactory{ nullptr };
	typedef std::list<Projectile*>::iterator Position;
	Position	mPosition;
};

class ProjectileFactory : public AttackFactory
{
public:
	ProjectileFactory();
	virtual ~ProjectileFactory();

	void Init(PoolParty* poolParty, KEngineCore::LuaScheduler* luaScheduler, KEngineCore::Timer * timer, KEngine2D::HierarchyUpdater* hierarchySystem, KEngine2D::MechanicsUpdater* mechanicsSystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory, ProjectileDescription blueprint);

	void Deinit();

	virtual void CreateAttack(const KEngine2D::Transform& origin) override;
	void RecycleProjectile(Projectile* projectile);
private:
	PoolParty*						mPoolParty{ nullptr };
	KEngineCore::LuaScheduler*		mLuaScheduler{ nullptr };
	KEngineCore::Timer*				mTimer{ nullptr };
	KEngine2D::HierarchyUpdater*	mHierarchySystem{ nullptr };
	KEngine2D::MechanicsUpdater*	mMechanicsSystem{ nullptr };
	KEngineOpenGL::SpriteRenderer*	mRenderer{ nullptr };
	SpriteFactory*					mSpriteFactory{ nullptr };

	KEngineCore::Pool<Projectile>   mProjectilePool;
	std::list<Projectile*>			mProjectilesInFlight;
	ProjectileDescription			mBlueprint;

};

