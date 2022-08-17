#pragma once

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

class KRBSGLuaBinding
{
public:
	KRBSGLuaBinding(void);
	~KRBSGLuaBinding(void);
	void Init(lua_State* luaState, KEngineCore::LuaScheduler * luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory);
	void Deinit();
	static KRBSGLuaBinding* GetInstance();

	PlayerShip* SpawnPlayerShip(KEngine2D::Point& position);
private:
	lua_State* mLuaState;
	KEngineCore::LuaScheduler* mLuaScheduler;
	KEngine2D::HierarchyUpdater* mHierarchySystem;
	KEngineOpenGL::SpriteRenderer* mRenderer;
	SpriteFactory* mSpriteFactory;

	static KRBSGLuaBinding* mInstance;

};
