#include "ProjectileFactory.h"
#include "CollisionDispatcher.h"
#include "SpriteFactory.h"
#include "KRBSG2Constants.h"


ProjectileFactory::ProjectileFactory() : AttackFactory()
{
}

ProjectileFactory::~ProjectileFactory()
{
	Deinit();
}

void ProjectileFactory::Init(PoolParty* poolParty, KEngineCore::LuaScheduler* luaScheduler, KEngineCore::Timer * timer, KEngine2D::HierarchyUpdater* hierarchySystem, KEngine2D::MechanicsUpdater* mechanicsSystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory, KEngine2D::CollisionSystem * collisionSystem, KRBSGCollisionDispatcher* collisionDispatcher, ProjectileDescription blueprint)
{
	assert(mPoolParty == nullptr);
	mPoolParty = poolParty;
	mLuaScheduler = luaScheduler;
	mTimer = timer;
	mHierarchySystem = hierarchySystem;
	mMechanicsSystem = mechanicsSystem;
	mRenderer = renderer;
	mSpriteFactory = spriteFactory;
	mCollisionSystem = collisionSystem;
	mCollisionDispatcher = collisionDispatcher;
	mProjectilePool.Init();
	mBlueprint = blueprint;
}

void ProjectileFactory::Deinit()
{
	mLuaScheduler = nullptr;
	mTimer = nullptr;
	mHierarchySystem = nullptr;
	mMechanicsSystem = nullptr;
	mRenderer = nullptr;
	mSpriteFactory = nullptr;
	auto projectileListCopy = mProjectilesInFlight;
	for (auto& projectile : projectileListCopy)
	{
		projectile->mPosition = mProjectilesInFlight.end();  //Prevent it from breaking the iterator
		RecycleProjectile(projectile);
	}
	mProjectilesInFlight.clear();
	mProjectilePool.Deinit();
	mPoolParty = nullptr;
}

void ProjectileFactory::CreateAttack(const KEngine2D::Transform& origin)
{
	Projectile* projectile = mProjectilePool.GetItem();
	projectile->mProjectileFactory = this;
	projectile->mDisposables.Init();
	projectile->mMover = mPoolParty->GetMechanicalPool().GetItem(&projectile->mDisposables);
	projectile->mMover->Init(mMechanicsSystem, origin, mBlueprint.velocity);

	const int TEMP_DIMENSION = 40;
	int width = TEMP_DIMENSION;
	int height = TEMP_DIMENSION;
	KEngine2D::Point modelUpperLeft = { -width / 2.0f, -height / 2.0f };
	projectile->mModelTransform = mPoolParty->GetHierarchyPool().GetItem(&projectile->mDisposables);
	projectile->mModelTransform->Init(mHierarchySystem, projectile->mMover, modelUpperLeft);
	projectile->mSpriteGraphic = mPoolParty->GetSpritePool().GetItem(&projectile->mDisposables);
	projectile->mSpriteGraphic->Init(mRenderer, mSpriteFactory->GetSprite(mBlueprint.spriteId), projectile->mModelTransform);
	projectile->mLifeTime = mPoolParty->GetTimePool().GetItem(&projectile->mDisposables);

	auto box = mPoolParty->GetBoxPool().GetItem(&projectile->mDisposables);
	box->Init(projectile->mMover, 5, 40);
	auto bounds = mPoolParty->GetBoundsPool().GetItem(&projectile->mDisposables);
	bounds->Init(projectile->mMover);
	bounds->AddBoundingBox(box);
	auto collider = mPoolParty->GetColliderPool().GetItem(&projectile->mDisposables);
	collider->Init(mCollisionSystem, bounds, PlayerProjectileFlag, EnemyShipFlag);


	projectile->mColliderHandle = collider->GetHandle();
	mCollisionDispatcher->AddProjectile(projectile, collider->GetHandle());

	projectile->mLifeTime->Init(mTimer, 10.0, false, [projectile]() 
	{
		projectile->mProjectileFactory->RecycleProjectile(projectile);
	});

	mProjectilesInFlight.push_back(projectile);
	projectile->mPosition = std::next(mProjectilesInFlight.rbegin()).base();
}

void ProjectileFactory::RecycleProjectile(Projectile* projectile)
{
	/*projectile->mSpriteGraphic->Deinit();
	mPoolParty->GetSpritePool().ReleaseItem(projectile->mSpriteGraphic);
	projectile->mSpriteGraphic = nullptr;
	projectile->mModelTransform->Deinit();
	mPoolParty->GetHierarchyPool().ReleaseItem(projectile->mModelTransform);
	projectile->mModelTransform = nullptr;
	projectile->mMover->Deinit();
	mPoolParty->GetMechanicalPool().ReleaseItem(projectile->mMover);
	projectile->mMover = nullptr;
	projectile->mLifeTime->Deinit();
	mPoolParty->GetTimePool().ReleaseItem(projectile->mLifeTime);
	projectile->mLifeTime = nullptr;*/

	mCollisionDispatcher->RemoveItem(projectile->mColliderHandle);


	if (projectile->mPosition != mProjectilesInFlight.end()) {
		mProjectilesInFlight.erase(projectile->mPosition);
		projectile->mPosition = mProjectilesInFlight.end();
	}
	projectile->mDisposables.Deinit();
	mProjectilePool.ReleaseItem(projectile);
}


