#include "KRBSGLuaBinding.h"
#include "SpriteFactory.h"
#include "GameObjectFactory.h"
#include "StaticTransform2D.h"
#include "MechanicalTransform2D.h"
#include "SpriteRenderer.h"
#include "Psychopomp.h"

#include "Lua.hpp"

#include <assert.h> 



KRBSGLuaBinding::KRBSGLuaBinding(void)
{
	mLuaState = nullptr;
}

KRBSGLuaBinding::~KRBSGLuaBinding(void)
{
	Deinit();
}


void KRBSGLuaBinding::Init(lua_State* luaState, KEngineCore::LuaScheduler * luaScheduler, KEngine2D::TransformLibrary* transformLib, KEngineOpenGL::SpriteLibrary* spriteLib, KEngineCore::Psychopomp* psychopomp, GameObjectFactory * gameObjectFactory, std::function<void()> exitFunc) {
	assert(mLuaState == nullptr);
	mLuaState = luaState;
	mLuaScheduler = luaScheduler;
	mTransformLib = transformLib;
	mSpriteLib = spriteLib;
	mPsychopomp = psychopomp;
	mGameObjectFactory = gameObjectFactory;
	mExitFunc = exitFunc;
	RegisterLibrary(luaState);
	
}

void KRBSGLuaBinding::Deinit() {
	if (mLuaState)
	{
		//Honestly unsure if libraries need to be doing this cleanup, but right now none of them are.
		/*
		lua_getglobal(mLuaState, "package");
		lua_getfield(mLuaState, -1, "preload");
		lua_pushnil(mLuaState);
		lua_setfield(mLuaState, -2, "krbsg");
		lua_pop(mLuaState, 2);*/ 
		mLuaState = nullptr;
		mLuaScheduler = nullptr;
		mGameObjectFactory = nullptr;
	}
}

void KRBSGLuaBinding::RegisterLibrary(lua_State* luaState, char const* name)
{
	mProjectileWrapping.Init(Projectile::MetaName);
	mWeaponWrapping.Init(Weapon::MetaName);
	mPlayerShipWrapping.Init(PlayerShip::MetaName);
	mEnemyShipWrapping.Init(EnemyShip::MetaName);
	mFlyoffWrapping.Init(Flyoff::MetaName);

	auto luaopen_krbsg = [](lua_State* luaState) {

		auto exit = [](lua_State* luaState) -> int {
			KRBSGLuaBinding* self = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
			self->RequestExit();
			return 0;
		};

		auto log = [](lua_State* luaState) {
			printf(luaL_checkstring(luaState, 1));
			printf("\n");
			return 0;
		};

		auto wrapProjectile = [](lua_State* luaState) -> int {
			KRBSGLuaBinding* self = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
			self->mProjectileWrapping.WrapAndPush(luaState, 1);
			return 1;
		};		
		
		auto wrapWeapon = [](lua_State* luaState) -> int {
			KRBSGLuaBinding* self = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
			self->mWeaponWrapping.WrapAndPush(luaState, 1);
			return 1;
		};

		auto spawnPlayerShip = [](lua_State* luaState) {
			KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));

			KEngineCore::StringHash shipId(luaL_checkstring(luaState, 1));

			if (!lua_istable(luaState, 2)) {
				luaL_error(luaState, "Table required for position");
			}
			lua_getfield(luaState, 2, "x");
			double x = luaL_checknumber(luaState, -1);
			lua_pop(luaState, 1);

			lua_getfield(luaState, 2, "y");
			double y = luaL_checknumber(luaState, -1);
			lua_pop(luaState, 1);

			KEngine2D::Point position = { x, y };

			PlayerShip* playerShip = binding->mGameObjectFactory->CreatePlayerShip(position, shipId);
			return 0;
		};

		auto wrapPlayerShip = [](lua_State* luaState) -> int {
			KRBSGLuaBinding* self = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
			self->mPlayerShipWrapping.WrapAndPush(luaState, 1);
			return 1;
		};

		auto spawnEnemyShip = [](lua_State* luaState) {
			KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));

			KEngineCore::StringHash shipId(luaL_checkstring(luaState, 1));

			if (!lua_istable(luaState, 2)) {
				luaL_error(luaState, "Table required for position");
			}
			lua_getfield(luaState, 2, "x");
			double x = luaL_checknumber(luaState, -1);
			lua_pop(luaState, 1);

			lua_getfield(luaState, 2, "y");
			double y = luaL_checknumber(luaState, -1);
			lua_pop(luaState, 1);

			KEngine2D::Point position = { x, y };

			EnemyShip* enemyShip = binding->mGameObjectFactory->CreateEnemyShip(position, shipId);
			return 0;
		};

		auto wrapEnemyShip = [](lua_State* luaState) -> int {
			KRBSGLuaBinding* self = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
			self->mEnemyShipWrapping.WrapAndPush(luaState, 1);
			return 1;
		};

		auto wrapFlyoff = [](lua_State* luaState) -> int {
			KRBSGLuaBinding* self = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
 			self->mFlyoffWrapping.WrapAndPush(luaState, 1);
			return 1;
		};

		auto spawnStarfield = [](lua_State* luaState) {
			KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
						
			int width = (int)luaL_checkinteger(luaState, 1);
			int height = (int)luaL_checkinteger(luaState, 2);
			Starfield * starfield = binding->mGameObjectFactory->CreateStarfield(width, height);
			
			return 0;
		};

		const luaL_Reg krbsgLibrary[] = {
			{"wrapProjectile", wrapProjectile},
			{"wrapWeapon", wrapWeapon},
			{"spawnPlayerShip", spawnPlayerShip},
			{"wrapPlayerShip", wrapPlayerShip},
			{"spawnEnemyShip", spawnEnemyShip},
			{"wrapEnemyShip", wrapEnemyShip},
			{"wrapFlyoff", wrapFlyoff},
			{"spawnStarfield", spawnStarfield},
			{"log", log },
			{"exit", exit},
			{nullptr, nullptr}
		};

		luaL_newlibtable(luaState, krbsgLibrary);
		lua_pushvalue(luaState, lua_upvalueindex(1));
		luaL_setfuncs(luaState, krbsgLibrary, 1);


		CreateProjectileMetaTable(luaState);
		CreateWeaponMetaTable(luaState);
		CreatePlayerShipMetaTable(luaState);
		CreateEnemyShipMetaTable(luaState);
		CreateFlyoffMetaTable(luaState);

		return 1;
	};


	PreloadLibrary(luaState, "krbsg", luaopen_krbsg);
}

void KRBSGLuaBinding::CreateProjectileMetaTable(lua_State* luaState)
{
	auto setForwardVelocity = [](lua_State* luaState) -> int {
		KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
		Projectile* projectile = binding->mProjectileWrapping.Unwrap(luaState, 1);
		
		double speed = luaL_checknumber(luaState, 2);

		KEngine2D::Point relativeVelocity = { 0, -speed };
		KEngine2D::Point absoluteVelocity = projectile->mMover->LocalToGlobal(relativeVelocity, true);
		projectile->mMover->SetVelocity(absoluteVelocity);
		return 0;
	};

	auto despawn = [](lua_State* luaState) -> int {
		KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
		Projectile* projectile = binding->mProjectileWrapping.Unwrap(luaState, 1);
		binding->mPsychopomp->ScheduleAppointment(projectile, [projectile]() {
			projectile->Deinit();
		});
		return 0;
	};

	const luaL_Reg projectileMethods[] = {
		{"setForwardVelocity", setForwardVelocity},
		{"despawn", despawn},
		{nullptr, nullptr}
	};

	lua_checkstack(luaState, 5);
	luaL_newmetatable(luaState, Projectile::MetaName);
	luaL_newlibtable(luaState, projectileMethods);
	lua_pushvalue(luaState, lua_upvalueindex(1));
	luaL_setfuncs(luaState, projectileMethods, 1);
	lua_setfield(luaState, -2, "__index");
	lua_pop(luaState, 1);
}

void KRBSGLuaBinding::CreateWeaponMetaTable(lua_State* luaState)
{
	auto createProjectile = [](lua_State* luaState) -> int {
		KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
		Weapon* weapon = binding->mWeaponWrapping.Unwrap(luaState, 1);
		
		KEngineCore::StringHash projectileId(luaL_checkstring(luaState, 2));

		int damage = (int)luaL_checkinteger(luaState, 3);

		if (!lua_istable(luaState, 4)) {
			luaL_error(luaState, "Table required for offset");
		}
		lua_getfield(luaState, 4, "x");
		double x = luaL_checknumber(luaState, -1);
		lua_pop(luaState, 1);

		lua_getfield(luaState, 4, "y");
		double y = luaL_checknumber(luaState, -1);
		lua_pop(luaState, 1);

		double rotation = luaL_checknumber(luaState, 5);

		KEngine2D::StaticTransform origin;
		origin.Init();
		origin.SetTranslation(weapon->mTransform->LocalToGlobal({ x, y }));
		origin.SetRotation(rotation + weapon->mTransform->GetRotation());

		weapon->mFactory->CreateProjectile(origin, projectileId);
		return 0;
	};


	auto getCooldown = [](lua_State* luaState) -> int {
		KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
		Weapon* weapon = binding->mWeaponWrapping.Unwrap(luaState, 1);
		lua_pushnumber(luaState, weapon->mCooldown);
		return 1;
	};

	auto getDamage = [](lua_State* luaState) -> int {
		KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
		Weapon* weapon = binding->mWeaponWrapping.Unwrap(luaState, 1);
		lua_pushnumber(luaState, weapon->mDamage);
		return 1;
	};

	const luaL_Reg weaponMethods[] = {
		{"createProjectile", createProjectile},
		{"getCooldown", getCooldown},
		{"getDamage", getDamage},
		{nullptr, nullptr}
	};

	lua_checkstack(luaState, 5);
	luaL_newmetatable(luaState, Weapon::MetaName);
	luaL_newlibtable(luaState, weaponMethods);
	lua_pushvalue(luaState, lua_upvalueindex(1));
	luaL_setfuncs(luaState, weaponMethods, 1);
	lua_setfield(luaState, -2, "__index");
	lua_pop(luaState, 1);
}

void KRBSGLuaBinding::CreateEnemyShipMetaTable(lua_State* luaState)
{
	auto move = [](lua_State* luaState) -> int {
		KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
		EnemyShip* ship = binding->mEnemyShipWrapping.Unwrap(luaState, 1);
		if (!lua_istable(luaState, 2)) {
			return luaL_error(luaState, "Table required for direction");
		}
		lua_checkstack(luaState, 2);
		lua_getfield(luaState, 2, "x");
		lua_getfield(luaState, 2, "y");
		double x = luaL_checknumber(luaState, -2);
		double y = luaL_checknumber(luaState, -1);
		lua_pop(luaState, 2);
		KEngine2D::Point direction = { x, y };
		ship->mSelfTransform->SetTranslation(ship->mSelfTransform->GetTranslation() + direction);
		return 0;
	};

	auto despawn = [](lua_State* luaState) -> int {
		KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
		EnemyShip* ship = binding->mEnemyShipWrapping.Unwrap(luaState, 1);
		binding->mPsychopomp->ScheduleAppointment(ship, [ship]() {
			ship->Deinit();
		});
		return 0;
	};

	const luaL_Reg enemyShipMethods[] = {
		{"move", move},
		{"despawn", despawn},
		{nullptr, nullptr}
	};

	lua_checkstack(luaState, 5);
	luaL_newmetatable(luaState, EnemyShip::MetaName);
	luaL_newlibtable(luaState, enemyShipMethods);
	lua_pushvalue(luaState, lua_upvalueindex(1));
	luaL_setfuncs(luaState, enemyShipMethods, 1);
	lua_setfield(luaState, -2, "__index");
	lua_pop(luaState, 1);
}

void KRBSGLuaBinding::CreateFlyoffMetaTable(lua_State* luaState)
{
	auto getTransform = [](lua_State* luaState) -> int {
		KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
		Flyoff* flyoff = binding->mFlyoffWrapping.Unwrap(luaState, 1);
		
		lua_pushlightuserdata(luaState, flyoff->mSelfTransform);
		binding->mTransformLib->GetTransformWrapping().WrapAndPush(luaState, -1);

		return 1;
	};

	auto getGraphic = [](lua_State* luaState) -> int {
		KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
		Flyoff* flyoff = binding->mFlyoffWrapping.Unwrap(luaState, 1);

		lua_pushlightuserdata(luaState, flyoff->mSprite);
		binding->mSpriteLib->GetSpriteWrapping().WrapAndPush(luaState, -1);
		
		return 1;
	};

	auto despawn = [](lua_State* luaState) -> int {
		KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
		Flyoff* flyoff = binding->mFlyoffWrapping.Unwrap(luaState, 1);
		binding->mPsychopomp->ScheduleAppointment(flyoff, [flyoff]() {
			flyoff->Deinit();
		});
		return 0;
	};

	const luaL_Reg flyoffMethods[] = {
		{"getTransform", getTransform},
		{"getGraphic", getGraphic},
		{"despawn", despawn},
		{nullptr, nullptr}
	};

	lua_checkstack(luaState, 5);
	luaL_newmetatable(luaState, Flyoff::MetaName);
	luaL_newlibtable(luaState, flyoffMethods);
	lua_pushvalue(luaState, lua_upvalueindex(1));
	luaL_setfuncs(luaState, flyoffMethods, 1);
	lua_setfield(luaState, -2, "__index");
	lua_pop(luaState, 1);

}

void KRBSGLuaBinding::CreatePlayerShipMetaTable(lua_State* luaState)
{
	auto move = [](lua_State* luaState) -> int {
		KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
		PlayerShip* ship = binding->mPlayerShipWrapping.Unwrap(luaState, 1);
		if (!lua_istable(luaState, 2)) {
			return luaL_error(luaState, "Table required for direction");
		}
		lua_checkstack(luaState, 2);
		lua_getfield(luaState, 2, "x");
		lua_getfield(luaState, 2, "y");
		double x = luaL_checknumber(luaState, -2);
		double y = luaL_checknumber(luaState, -1);
		lua_pop(luaState, 2);
		KEngine2D::Point direction = { x, y };
		ship->mSelfTransform->SetTranslation(ship->mSelfTransform->GetTranslation() + direction);
		return 0;
	};

	auto addWeapon = [](lua_State* luaState) -> int {
		KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
		PlayerShip* ship = binding->mPlayerShipWrapping.Unwrap(luaState, 1);

		KEngineCore::StringHash weaponId(luaL_checkstring(luaState, 2));

		Weapon * weapon = binding->mGameObjectFactory->CreateWeapon(&ship->mDisposables, ship->mWeaponAttach, weaponId); 
	
		return 0;
	};


	const luaL_Reg playerShipMethods[] = {
			{"move", move},
			{"addWeapon", addWeapon},
			{nullptr, nullptr}
	};


	KRBSGLuaBinding* binding = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
	lua_checkstack(luaState, 5);
	luaL_newmetatable(luaState, binding->mPlayerShipWrapping.GetMetaTableName());
	luaL_newlibtable(luaState, playerShipMethods);
	lua_pushvalue(luaState, lua_upvalueindex(1));
	luaL_setfuncs(luaState, playerShipMethods, 1);
	lua_setfield(luaState, -2, "__index");
	lua_pop(luaState, 1);
}


void KRBSGLuaBinding::RequestExit() {
	if (mExitFunc)
	{
		mExitFunc();
	}
}