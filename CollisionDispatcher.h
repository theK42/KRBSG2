#pragma once
#include "Transform2D.h"
#include "Collision2D.h"
#include <map>
#include <utility>

namespace KEngineCore
{
	class Psychopomp;
}

class EnemyShip;
class Projectile;
class PlayerShip;

class KRBSGCollisionDispatcher : public KEngine2D::CollisionDispatcher
{
public:
	KRBSGCollisionDispatcher();
	virtual ~KRBSGCollisionDispatcher();

	void Init(KEngineCore::Psychopomp * psychopomp);
	void Deinit();

	void AddEnemyShip(EnemyShip* ship, KEngine2D::ColliderHandle handle);
	void AddProjectile(Projectile* shot, KEngine2D::ColliderHandle handle);
	void AddPlayerShip(PlayerShip* ship, KEngine2D::ColliderHandle handle);
	void RemoveItem(int colliderHandle);

	virtual void HandleCollision(KEngine2D::Collision collision) override;
	
private:
	enum Collidable {
		ePlayerShip,
		eEnemyShip,
		eProjectile
	};

	std::map<KEngine2D::ColliderHandle, Collidable> mColliderTypes;
	std::map<KEngine2D::ColliderHandle, PlayerShip*> mPlayerShips;
	std::map<KEngine2D::ColliderHandle, EnemyShip*> mEnemyShips;
	std::map<KEngine2D::ColliderHandle, Projectile*> mProjectiles;

	KEngineCore::Psychopomp *	mPsychopomp{ nullptr };

};

