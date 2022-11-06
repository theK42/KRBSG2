#pragma once
#include "Transform2D.h"
#include "Collision2D.h"
#include <map>
#include <utility>

namespace KEngineCore
{
	class Psychopomp;
}

struct EnemyShip;
struct Projectile;
struct PlayerShip;
class ScoreKeeper;

class KRBSGCollisionDispatcher : public KEngine2D::CollisionDispatcher
{
public:
	KRBSGCollisionDispatcher();
	virtual ~KRBSGCollisionDispatcher();

	void Init(KEngineCore::Psychopomp * psychopomp, ScoreKeeper * scorekeeper);
	virtual void Deinit() override;

	void AddEnemyShip(EnemyShip* ship);
	void AddProjectile(Projectile* shot);
	void AddPlayerShip(PlayerShip* ship);
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
	ScoreKeeper*				mScoreKeeper{ nullptr };
};

