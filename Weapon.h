#pragma once
#include "LuaScheduler.h"
#include "Pool.h"


namespace KEngine2D
{
	class Transform;
}

class AttackFactory;

class Weapon
{
public:
	Weapon();
	~Weapon();

	void Init(AttackFactory* attackFactory, KEngineCore::LuaScheduler* luaScheduler, const KEngine2D::Transform * transform, float cooldown);
	void Deinit();

	void Fire();  //Create an Attack
	float GetCooldown() const;
private:
	AttackFactory*								mAttackFactory { nullptr };
	float										mCooldown{ 0.0f };
	KEngineCore::ScheduledLuaThread				mScript;
	const KEngine2D::Transform*					mTransform { nullptr };
};

class WeaponSystem : public KEngineCore::LuaLibrary
{
public:
	WeaponSystem();
	~WeaponSystem();

	void Init(KEngineCore::LuaScheduler* luaScheduler, AttackFactory* attackFactory);
	void Deinit();

	Weapon* CreateWeapon(const KEngine2D::Transform* transform, float cooldown);
	void RecycleWeapon(Weapon * weapon);

	void RegisterLibrary(lua_State* luaState, char const* name = "weapons");

private:
	KEngineCore::LuaWrapping<Weapon>	mLuaWrapping;
	AttackFactory*						mAttackFactory{ nullptr };
	KEngineCore::LuaScheduler*			mLuaScheduler{ nullptr };
	KEngineCore::Pool<Weapon>			mWeaponPool;
};