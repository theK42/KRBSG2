#include "PlayerShip.h"
#include "SpriteFactory.h"
#include <assert.h>

PlayerShip::PlayerShip()
{
	mInitialized = false;
}

PlayerShip::~PlayerShip()
{
	Deinit();
}

const int TEMP_DIMENSION = 40;
void PlayerShip::Init(KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory, KEngine2D::Point position)
{
	int width = TEMP_DIMENSION;
	int height = TEMP_DIMENSION;

	KEngine2D::StaticTransform initialTransform(position);
	//mBoundary.Init(&initialTransform, width, height);
	//mBoundingArea.Init(&initialTransform);
	//mBoundingArea.AddBoundingBox(&mBoundary);
	
	KEngine2D::Point modelUpperLeft = { -width / 2.0f, -height / 2.0f };
	KEngine2D::StaticTransform modelTransform(modelUpperLeft);

	mModelTransform.Init(hierarchySystem, &mSelfTransform, modelTransform);

	mSelfTransform.SetTranslation(position);

	mGraphic.Init(renderer, spriteFactory->PlayerShipSprite(HASH("Ship", 0x5A02441A)), &mModelTransform);

	mScript.Init(luaScheduler, "playerShip.lua", false);

	lua_State* scriptState = mScript.GetThreadState();

	lua_checkstack(scriptState, 1);
	lua_pushlightuserdata(scriptState, this);

	mScript.Resume(1);

	mInitialized = true;
}

void PlayerShip::Deinit()
{
	mGraphic.Deinit();
	//mBoundary.Deinit();
	mInitialized = false;
}

void PlayerShip::Move(KEngine2D::Point direction)
{
	mSelfTransform.SetTranslation(mSelfTransform.GetTranslation() + direction);
}

PlayerShipSystem::PlayerShipSystem()
{
}

PlayerShipSystem::~PlayerShipSystem()
{
}

void PlayerShipSystem::Init(KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory)
{
	assert(mLuaScheduler == nullptr);
	mLuaScheduler = luaScheduler;
	mHierarchySystem = hierarchySystem; 
	mRenderer = renderer;
	mSpriteFactory = spriteFactory;
	RegisterLibrary(luaScheduler->GetMainState());
}

void PlayerShipSystem::Deinit()
{
	mLuaScheduler = nullptr;
	mHierarchySystem = nullptr;
	mRenderer = nullptr;
	mSpriteFactory = nullptr;
}

PlayerShip* PlayerShipSystem::CreatePlayerShip(KEngine2D::Point position)
{
	mPlayerShip.Init(mLuaScheduler, mHierarchySystem, mRenderer, mSpriteFactory, position);
	return &mPlayerShip;
}

void PlayerShipSystem::RegisterLibrary(lua_State* luaState, char const* name)
{
	mLuaWrapping.Init("KRBSG.PlayerShip");
	auto luaopen_playership = [](lua_State* luaState) {
		auto spawnPlayerShip = [](lua_State* luaState) {
			PlayerShipSystem* factory = (PlayerShipSystem*)lua_touserdata(luaState, lua_upvalueindex(1));

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
	
			PlayerShip* playerShip = factory->CreatePlayerShip(position);
			return 0;
		};

		auto wrapPlayerShip = [](lua_State* luaState) -> int {
			PlayerShipSystem* playerShipSystem = (PlayerShipSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
			playerShipSystem->mLuaWrapping.WrapAndPush(luaState, 1);
			return 1;
		};

		auto move = [](lua_State* luaState) -> int {
			PlayerShipSystem* playerShipSystem = (PlayerShipSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
			PlayerShip* ship = playerShipSystem->mLuaWrapping.Unwrap(luaState, 1);
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

			ship->Move(direction);

			return 0;
		};


		const luaL_Reg playerShipMethods[] = {
				{"move", move},
				{nullptr, nullptr}
		};

		const luaL_Reg playerShipLibrary[] = {
			{"spawnPlayerShip", spawnPlayerShip},
			{"wrapPlayerShip", wrapPlayerShip},
			{nullptr, nullptr}
		};

		PlayerShipSystem* playerShipSystem = (PlayerShipSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
		lua_checkstack(luaState, 5);
		luaL_newmetatable(luaState, playerShipSystem->mLuaWrapping.GetMetaTableName());
		luaL_newlibtable(luaState, playerShipMethods);
		lua_pushvalue(luaState, lua_upvalueindex(1));
		luaL_setfuncs(luaState, playerShipMethods, 1);
		lua_setfield(luaState, -2, "__index");
		lua_pop(luaState, 1);

		luaL_newlibtable(luaState, playerShipLibrary);
		lua_pushvalue(luaState, lua_upvalueindex(1));
		luaL_setfuncs(luaState, playerShipLibrary, 1);

		return 1;
	};

	PreloadLibrary(luaState, name, luaopen_playership);
}
