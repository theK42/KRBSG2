#pragma once
#include "Collision2D.h"
#include "Disposable.h"
#include "StringHash.h"


namespace KEngineCore
{
	class DataTree;
}

namespace KEngine2D
{
	class Transform;
	class StaticTransform; 
	class HierarchyUpdater;
	class MechanicalTransform; 
	class MechanicsUpdater;
}

namespace KEngineOpenGL
{
	class SpriteGraphic;
	class SpriteRenderer;
	class TextRenderer;
	class Sprite;
	class TextSprite;
	class FontFactory;
	class ShaderFactory;
}

class SpriteFactory;
class PoolParty;
struct ProjectileDescription; 
class KRBSGCollisionDispatcher;
class GameObjectFactory;

struct Projectile
{
	static const char MetaName[];
	~Projectile();
	void Init();
	void Deinit();
	KEngineCore::DisposableGroup	mDisposables;
	GameObjectFactory*				mFactory {nullptr};
	KEngine2D::MechanicalTransform*	mMover{ nullptr };
	KEngine2D::ColliderHandle		mColliderHandle;
};

struct Weapon
{
	static const char MetaName[];
	~Weapon();
	void Init();
	void Deinit();
	GameObjectFactory* mFactory{ nullptr };
	float										mCooldown{ 0.5f };
	float										mDamage{ 1.0f };
	const KEngine2D::Transform*					mTransform{ nullptr };
};

struct PlayerShip
{
	static const char MetaName[];
	~PlayerShip();
	void Init();
	void Deinit();
	void Move(KEngine2D::Point direction);

	KEngineCore::DisposableGroup	mDisposables;
	GameObjectFactory*				mFactory{ nullptr };
	KEngine2D::StaticTransform*		mSelfTransform{ nullptr };
	KEngine2D::Transform*			mWeaponAttach{ nullptr };
	KEngine2D::ColliderHandle		mColliderHandle;
};

struct EnemyShip
{
	static const char MetaName[];
	~EnemyShip();
	void Init();
	void Deinit();
	void Move(KEngine2D::Point direction);

	KEngineCore::DisposableGroup	mDisposables;
	GameObjectFactory*				mFactory{ nullptr };
	KEngine2D::StaticTransform*		mSelfTransform{ nullptr };
	KEngine2D::ColliderHandle		mColliderHandle;
};



struct Flyoff
{
	static const char MetaName[];
	~Flyoff();
	void Init();
	void Deinit();
	KEngineCore::DisposableGroup	mDisposables;
	GameObjectFactory*				mFactory{ nullptr };
	KEngine2D::StaticTransform*		mSelfTransform{ nullptr };
	KEngineOpenGL::SpriteGraphic*	mSprite{ nullptr };
};


class GameObjectFactory
{
public:
	GameObjectFactory();
	~GameObjectFactory();

	void Init(PoolParty* poolParty, KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngine2D::MechanicsUpdater* mechanicsSystem, KEngineOpenGL::ShaderFactory* shaderFactory, KEngineOpenGL::SpriteRenderer* renderer, KEngineOpenGL::TextRenderer* textRenderer, KEngineOpenGL::FontFactory * fontFactory, SpriteFactory* spriteFactory, KEngine2D::CollisionSystem* collisionSystem, KRBSGCollisionDispatcher* collisionDispatcher, KEngineCore::DataTree* dataRoot);
	void Deinit();


	KEngine2D::StaticTransform* CreateStaticTransform(KEngineCore::DisposableGroup* disposables, const KEngine2D::Point& position, double rotation = 0.0);
	KEngineOpenGL::TextSprite* CreateTextSprite(KEngineCore::DisposableGroup* disposables, std::string_view text, KEngineCore::StringHash fontHash, KEngineCore::StringHash shaderId, KEngine2D::Transform* transform);
	KEngineOpenGL::SpriteGraphic* CreateSpriteGraphic(const KEngineOpenGL::Sprite* sprite, KEngineCore::DisposableGroup* disposables, KEngine2D::Transform* selfTransform);
	KEngineOpenGL::SpriteGraphic* CreateSpriteGraphic(KEngineCore::StringHash& spriteHash, KEngineCore::DisposableGroup* disposables, KEngine2D::Transform* selfTransform);
	KEngine2D::ColliderHandle CreateCollider(KEngineCore::DisposableGroup* disposables, KEngine2D::Transform* transform, KEngineCore::StringHash& collisionShapeName, unsigned int flags, unsigned int filters);
	KEngineCore::ScheduledLuaThread* CreateScriptThread(KEngineCore::DisposableGroup* disposables, const std::string_view& scriptName, void* scriptObj);

	Weapon* CreateWeapon(KEngineCore::DisposableGroup* disposables, const KEngine2D::Transform* transform, KEngineCore::StringHash weaponId);

	PlayerShip* CreatePlayerShip(KEngine2D::Point position, KEngineCore::StringHash shipId);
	void ReleasePlayerShip(PlayerShip* ship);

	EnemyShip* CreateEnemyShip(KEngine2D::Point position, KEngineCore::StringHash shipId);
	void ReleaseEnemyShip(EnemyShip* ship);

	Projectile* CreateProjectile(const KEngine2D::Transform& origin, KEngineCore::StringHash projectileDefId);
	void ReleaseProjectile(Projectile* projectile);

	Flyoff* CreateFlyoff(KEngine2D::Point origin, std::string_view text, KEngineCore::StringHash flyoffStyleId);
	void ReleaseFlyoff(Flyoff* flyoff);
private:
	PoolParty*						mPoolParty{ nullptr };
	KEngineCore::LuaScheduler*		mLuaScheduler{ nullptr };
	KEngine2D::HierarchyUpdater*	mHierarchySystem{ nullptr };
	KEngine2D::MechanicsUpdater*	mMechanicsSystem{ nullptr };
	KEngineOpenGL::SpriteRenderer*	mRenderer{ nullptr };
	SpriteFactory*					mSpriteFactory{ nullptr };
	KEngineOpenGL::ShaderFactory*	mShaderFactory{ nullptr };
	KEngineOpenGL::FontFactory*		mFontFactory{ nullptr };
	KEngineOpenGL::TextRenderer*	mTextRenderer{ nullptr };
	KEngine2D::CollisionSystem*		mCollisionSystem{ nullptr };
	KRBSGCollisionDispatcher*		mCollisionDispatcher{ nullptr };
	KEngineCore::DataTree*			mDataRoot{ nullptr };

	KEngineCore::Pool<Projectile>	mProjectilePool;
	KEngineCore::Pool<PlayerShip>	mPlayerShipPool;
	KEngineCore::Pool<EnemyShip>	mEnemyShipPool;
	KEngineCore::Pool<Flyoff>		mFlyoffPool;

	KEngineCore::RecyclingPool<Weapon>	mWeaponPool;

};
