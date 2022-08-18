#include "KRBSGLuaBinding.h"
#include "PlayerShip.h"
#include "SpriteFactory.h"

#include "Lua.hpp"

#include <assert.h> 


KRBSGLuaBinding* KRBSGLuaBinding::mInstance = nullptr;

KRBSGLuaBinding* KRBSGLuaBinding::GetInstance() {
	return mInstance;
}

KRBSGLuaBinding::KRBSGLuaBinding(void)
{
	mLuaState = nullptr;
}

KRBSGLuaBinding::~KRBSGLuaBinding(void)
{
	Deinit();
}

int spawnPlayerShip(lua_State* luaState) {
	if (!lua_istable(luaState, 1)) {
		luaL_error(luaState, "Table required for position");
	}
	lua_getfield(luaState, 1, "x");
	double x = luaL_checknumber(luaState, -1);
	lua_pop(luaState, 1);

	lua_getfield(luaState, 1, "y");
	double y = luaL_checknumber(luaState, -1);
	lua_pop(luaState, 1);

	KEngine2D::Point position = { x, y };

	PlayerShip* ship = KRBSGLuaBinding::GetInstance()->SpawnPlayerShip(position);

	lua_pushlightuserdata(luaState, ship); //Probably want a full userdata

	return 1;
}

const struct luaL_Reg krbsgLibrary[] = {
	{ "spawnPlayerShip", spawnPlayerShip },
	{nullptr, nullptr}
};

int luaopen_krbsg(lua_State* luaState) {
	luaL_newlib(luaState, krbsgLibrary);
	return 1;
};

void KRBSGLuaBinding::Init(lua_State* luaState, KEngineCore::LuaScheduler * luaScheduler,
	KEngine2D::HierarchyUpdater* hierarchySystem,
	KEngineOpenGL::SpriteRenderer* renderer,
	SpriteFactory* spriteFactory) {
	assert(mInstance == nullptr);
	mInstance = this;
	mLuaState = luaState;
	mLuaScheduler = luaScheduler;
	mHierarchySystem = hierarchySystem;
	mRenderer = renderer;
	mSpriteFactory = spriteFactory;

	lua_getglobal(luaState, "package");
	lua_getfield(luaState, -1, "preload");
	lua_pushcfunction(luaState, luaopen_krbsg);
	lua_setfield(luaState, -2, "krbsg");
	lua_pop(luaState, 2);
}

void KRBSGLuaBinding::Deinit() {
	if (mInstance == this) {
		mInstance = nullptr;
	}

	lua_getglobal(mLuaState, "package");
	lua_getfield(mLuaState, -1, "preload");
	lua_pushnil(mLuaState);
	lua_setfield(mLuaState, -2, "krbsg");
	lua_pop(mLuaState, 2);
}

PlayerShip* KRBSGLuaBinding::SpawnPlayerShip(KEngine2D::Point& position) {
	PlayerShip* ship = new PlayerShip();
	ship->Init(mLuaScheduler, mHierarchySystem, mRenderer, mSpriteFactory, position);
	return ship;
}