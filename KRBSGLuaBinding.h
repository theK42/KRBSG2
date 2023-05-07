#pragma once
#include "LuaLibrary.h"
#include <functional>
#include "GameObjectFactory.h"

struct lua_State;

namespace KEngineCore {
	class LuaScheduler;
	class Psychopomp;
}

namespace KEngine2D {
	class PhysicsSystem;
	class HierarchyUpdater;
	struct Point;
	class TransformLibrary;
}

namespace KEngineOpenGL {
	class SpriteRenderer;
	class SpriteLibrary;
}
struct KRBSG2;
struct PlayerShip;
class SpriteFactory;
class GameplaySession;

class KRBSGLuaBinding : public KEngineCore::LuaLibrary
{
public:
	KRBSGLuaBinding(void);
	~KRBSGLuaBinding(void);
	void Init(lua_State* luaState, KEngineCore::LuaScheduler * luaScheduler, KEngine2D::TransformLibrary* transformLib, KEngineOpenGL::SpriteLibrary * spriteLib, KEngineCore::Psychopomp * psychopomp, KRBSG2 * app, std::function<void()> exitFunc);
	void Deinit();

	void RegisterLibrary(lua_State* luaState, char const* name = "krbsg");

	void RequestExit();
private:
	lua_State* mLuaState;
	
	static void CreateProjectileMetaTable(lua_State* luaState);
	static void CreateWeaponMetaTable(lua_State* luaState);
	static void CreatePlayerShipMetaTable(lua_State* luaState);
	static void CreateEnemyShipMetaTable(lua_State* luaState);
	static void CreateFlyoffMetaTable(lua_State* luaState);

	GameplaySession*			mGameplaySession;

	KRBSG2*						mApp {nullptr};
	KEngineCore::LuaScheduler *	mLuaScheduler;
	KEngineCore::Psychopomp*	mPsychopomp;

	KEngine2D::TransformLibrary*	mTransformLib;
	KEngineOpenGL::SpriteLibrary*	mSpriteLib;

	std::function<void()> mExitFunc;

	KEngineCore::LuaWrapping<Projectile>mProjectileWrapping;
	KEngineCore::LuaWrapping<EnemyShip>mEnemyShipWrapping;
	KEngineCore::LuaWrapping<Weapon>mWeaponWrapping;
	KEngineCore::LuaWrapping<PlayerShip>mPlayerShipWrapping;
	KEngineCore::LuaWrapping<Flyoff>mFlyoffWrapping;

};
