#include "CollisionDispatcher.h"
#include "PlayerShip.h"
#include "EnemyShip.h"
#include "ProjectileFactory.h"
#include "Psychopomp.h"

KRBSGCollisionDispatcher::KRBSGCollisionDispatcher()
{
}

KRBSGCollisionDispatcher::~KRBSGCollisionDispatcher()
{
	Deinit();
}

void KRBSGCollisionDispatcher::Init(KEngineCore::Psychopomp* psychopomp)
{
	assert(mPsychopomp == nullptr);
	mPsychopomp = psychopomp;
}

void KRBSGCollisionDispatcher::Deinit()
{
	mPsychopomp = nullptr;
	mColliderTypes.clear();
	mPlayerShips.clear();
	mEnemyShips.clear();
	mProjectiles.clear();
}

void KRBSGCollisionDispatcher::AddEnemyShip(EnemyShip* ship, KEngine2D::ColliderHandle handle)
{
	mColliderTypes[handle] = eEnemyShip;
	mEnemyShips[handle] = ship;
}

void KRBSGCollisionDispatcher::AddProjectile(Projectile* shot, KEngine2D::ColliderHandle handle)
{
	mColliderTypes[handle] = eProjectile;
	mProjectiles[handle] = shot;
}

void KRBSGCollisionDispatcher::AddPlayerShip(PlayerShip* ship, KEngine2D::ColliderHandle handle)
{
	mColliderTypes[handle] = ePlayerShip;
	mPlayerShips[handle] = ship;
}

void KRBSGCollisionDispatcher::RemoveItem(int colliderHandle)
{
}

void KRBSGCollisionDispatcher::HandleCollision(KEngine2D::Collision collision)
{
	if (mColliderTypes.contains(collision.first) && mColliderTypes.contains(collision.second))
	{
		Collidable firstType = mColliderTypes[collision.first];
		Collidable secondType = mColliderTypes[collision.second];

		if (firstType == eProjectile && secondType == eEnemyShip)
		{
			Projectile* shot = mProjectiles[collision.first];
			EnemyShip* ship = mEnemyShips[collision.second];
			mPsychopomp->ScheduleAppointment(ship, [ship]() {
				ship->Die();
			});
			mPsychopomp->ScheduleAppointment(shot, [shot]() {
				shot->mProjectileFactory->RecycleProjectile(shot);
			});
		}

		if (firstType == eEnemyShip && secondType == ePlayerShip)
		{
			PlayerShip* ship = mPlayerShips[collision.second];
			mPsychopomp->ScheduleAppointment(ship, [ship]() {
				ship->Move({ 0, -100 });
			});
		}
	}
}

