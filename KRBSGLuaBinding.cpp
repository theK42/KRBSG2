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


void KRBSGLuaBinding::Init(lua_State* luaState, KEngineCore::LuaScheduler * luaScheduler,
	KEngine2D::HierarchyUpdater* hierarchySystem,
	KEngineOpenGL::SpriteRenderer* renderer,
	SpriteFactory* spriteFactory, std::function<void()> exitFunc) {
	assert(mInstance == nullptr);
	assert(mLuaState == nullptr);
	mInstance = this;
	mLuaState = luaState;
	mLuaScheduler = luaScheduler;
	mHierarchySystem = hierarchySystem;
	mRenderer = renderer;
	mSpriteFactory = spriteFactory;
	mExitFunc = exitFunc;
	RegisterLibrary(luaState);
	
}

void KRBSGLuaBinding::Deinit() {
	if (mInstance == this) {
		mInstance = nullptr;
	}

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
		mHierarchySystem = nullptr;
		mRenderer = nullptr;
		mSpriteFactory = nullptr;
	}
}

void KRBSGLuaBinding::RegisterLibrary(lua_State* luaState, char const* name)
{
	auto luaopen_krbsg = [](lua_State* luaState) {
		auto exit = [](lua_State* luaState) -> int {
			KRBSGLuaBinding* self = (KRBSGLuaBinding*)lua_touserdata(luaState, lua_upvalueindex(1));
			self->RequestExit();
			return 0;
		};

		const luaL_Reg krbsgLibrary[] = {
			{"exit", exit},
			{nullptr, nullptr}
		};

		luaL_newlibtable(luaState, krbsgLibrary);
		lua_pushvalue(luaState, lua_upvalueindex(1));
		luaL_setfuncs(luaState, krbsgLibrary, 1);

		return 1;

		return 1;
	};

	PreloadLibrary(luaState, "krbsg", luaopen_krbsg);

}

void KRBSGLuaBinding::RequestExit() {
	if (mExitFunc)
	{
		mExitFunc();
	}
}