#include "Weapon.h"
#include "Attack.h"
#include <assert.h>

Weapon::Weapon()
{
}

Weapon::~Weapon()
{
	Deinit();
}

void Weapon::Init(AttackFactory* attackFactory, KEngineCore::LuaScheduler* luaScheduler, const KEngine2D::Transform* transform, float cooldown)
{
	assert(mAttackFactory == nullptr);
	mAttackFactory = attackFactory;
	mTransform = transform;
	mCooldown = cooldown;
	mScript.Init(luaScheduler, "blaster.lua", false);
	lua_State* scriptState = mScript.GetThreadState();

	lua_checkstack(scriptState, 1);
	lua_pushlightuserdata(scriptState, this);

	mScript.Resume(1);
}

void Weapon::Deinit()
{
	mScript.Deinit();
	mTransform = nullptr;
	mAttackFactory = nullptr;
}

void Weapon::Fire()
{
	mAttackFactory->CreateAttack(*mTransform);
}

float Weapon::GetCooldown() const
{
	return mCooldown;
}

WeaponSystem::WeaponSystem()
{
}

WeaponSystem::~WeaponSystem()
{
	Deinit();
}

void WeaponSystem::Init(KEngineCore::LuaScheduler* luaScheduler, AttackFactory* attackFactory)
{
	assert(mLuaScheduler == nullptr);
	mLuaScheduler = luaScheduler;
	mAttackFactory = attackFactory;
	mWeaponPool.Init();
	RegisterLibrary(luaScheduler->GetMainState());
}

void WeaponSystem::Deinit()
{
	mLuaScheduler = nullptr;
}

Weapon* WeaponSystem::CreateWeapon(const KEngine2D::Transform* transform, float cooldown)
{
	Weapon* weapon = mWeaponPool.GetItem();
	weapon->Init(mAttackFactory, mLuaScheduler, transform, cooldown);
	return weapon;
}

void WeaponSystem::RecycleWeapon(Weapon* weapon)
{
	mWeaponPool.ReleaseItem(weapon);
}

void WeaponSystem::RegisterLibrary(lua_State* luaState, char const* name)
{
	mLuaWrapping.Init("KRBSG.Weapon");
	auto luaopen_weapon = [](lua_State* luaState) {

		auto wrapWeapon = [](lua_State* luaState) -> int {
			WeaponSystem* weaponSystem = (WeaponSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
			weaponSystem->mLuaWrapping.WrapAndPush(luaState, 1);
			return 1;
		};

		auto fire = [](lua_State* luaState) -> int {
			WeaponSystem* weaponSystem = (WeaponSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
			Weapon* weapon = weaponSystem->mLuaWrapping.Unwrap(luaState, 1);
			weapon->Fire();

			return 0;
		};


		auto getCooldown = [](lua_State* luaState) -> int {
			WeaponSystem* weaponSystem = (WeaponSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
			Weapon* weapon = weaponSystem->mLuaWrapping.Unwrap(luaState, 1);
			lua_pushnumber(luaState, weapon->GetCooldown());

			return 1;
		};

		const luaL_Reg weaponMethods[] = {
				{"getCooldown", getCooldown},
				{"fire", fire},
				{nullptr, nullptr}
		};

		const luaL_Reg weaponLibrary[] = {
			{"wrapWeapon", wrapWeapon},
			{nullptr, nullptr}
		};

		WeaponSystem* weaponSystem = (WeaponSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
		lua_checkstack(luaState, 5);
		luaL_newmetatable(luaState, weaponSystem->mLuaWrapping.GetMetaTableName());
		luaL_newlibtable(luaState, weaponMethods);
		lua_pushvalue(luaState, lua_upvalueindex(1));
		luaL_setfuncs(luaState, weaponMethods, 1);
		lua_setfield(luaState, -2, "__index");
		lua_pop(luaState, 1);

		luaL_newlibtable(luaState, weaponLibrary);
		lua_pushvalue(luaState, lua_upvalueindex(1));
		luaL_setfuncs(luaState, weaponLibrary, 1);

		return 1;
	};

	PreloadLibrary(luaState, name, luaopen_weapon);

}
