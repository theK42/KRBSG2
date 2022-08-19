#pragma once
#include "LuaLibrary.h"
#include <functional>

struct lua_State;

namespace KEngineCore {
	class LuaScheduler;
}

namespace KEngine2D {
	class PhysicsSystem;
	class HierarchyUpdater;
	struct Point;
}

namespace KEngineOpenGL {
	class SpriteRenderer;
}
class PlayerShip;
class SimpleBox;
class SpriteFactory;

class KRBSGLuaBinding : public KEngineCore::LuaLibrary
{
public:
	KRBSGLuaBinding(void);
	~KRBSGLuaBinding(void);
	void Init(lua_State* luaState, KEngineCore::LuaScheduler * luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory, std::function<void()> exitFunc);
	void Deinit();

	void RegisterLibrary(lua_State* luaState, char const* name = "krbsg");

	void RequestExit();

	static KRBSGLuaBinding* GetInstance();
private:
	lua_State* mLuaState;
	KEngineCore::LuaScheduler* mLuaScheduler;
	KEngine2D::HierarchyUpdater* mHierarchySystem;
	KEngineOpenGL::SpriteRenderer* mRenderer;
	SpriteFactory* mSpriteFactory;
	std::function<void()> mExitFunc;

	static KRBSGLuaBinding* mInstance;

};
