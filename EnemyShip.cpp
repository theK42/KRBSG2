#include "EnemyShip.h"
#include "SpriteFactory.h"
#include <assert.h>
#include <numbers>

EnemyShip::EnemyShip()
{
	mInitialized = false;
}

EnemyShip::~EnemyShip()
{
	Deinit();
}

const int TEMP_DIMENSION = 40;
const char * TEMP_SCRIPT_NAME = "enemyShip.lua";
void EnemyShip::Init(KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory, KEngine2D::Point position)
{
	int width = TEMP_DIMENSION;
	int height = TEMP_DIMENSION;

	KEngine2D::StaticTransform initialTransform(position);
	//mBoundary.Init(&initialTransform, width, height);
	//mBoundingArea.Init(&initialTransform);
	//mBoundingArea.AddBoundingBox(&mBoundary);

	KEngine2D::Point modelUpperLeft = { -width / 2.0f, -height / 2.0f };
	KEngine2D::StaticTransform modelTransform(modelUpperLeft, std::numbers::pi);

	mModelTransform.Init(hierarchySystem, &mSelfTransform, modelTransform);

	mSelfTransform.SetTranslation(position);

	mGraphic.Init(renderer, spriteFactory->EnemyShipSprite(HASH("Ship", 0x5A02441A)), &mModelTransform);

	mScript.Init(luaScheduler, TEMP_SCRIPT_NAME, false);

	lua_State* scriptState = mScript.GetThreadState();

	lua_checkstack(scriptState, 1);
	lua_pushlightuserdata(scriptState, this);

	mScript.Resume(1);

	mInitialized = true;
}

void EnemyShip::Deinit()
{
	mScript.Deinit();
	mGraphic.Deinit();
	mModelTransform.Deinit();
	//mBoundary.Deinit();
	mInitialized = false;
}

void EnemyShip::Move(KEngine2D::Point direction)
{
	mSelfTransform.SetTranslation(mSelfTransform.GetTranslation() + direction);
}

EnemyShipSystem::EnemyShipSystem()
{
}

EnemyShipSystem::~EnemyShipSystem()
{
}

const int TEMP_POOL_SIZE = 100;
void EnemyShipSystem::Init(KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory)
{
	assert(mLuaScheduler == nullptr);
	mLuaScheduler = luaScheduler;
	mHierarchySystem = hierarchySystem;
	mRenderer = renderer;
	mSpriteFactory = spriteFactory;

	mEnemyShipPool = new EnemyShip[TEMP_POOL_SIZE];
	for (int i = 0; i < TEMP_POOL_SIZE; i++) {
		mFreeEnemyShips.push_back(&mEnemyShipPool[i]);
	}

	RegisterLibrary(luaScheduler->GetMainState());
}

void EnemyShipSystem::Deinit()
{
	mLuaScheduler = nullptr;
	mHierarchySystem = nullptr;
	mRenderer = nullptr;
	mSpriteFactory = nullptr;
	
	delete[] mEnemyShipPool;
	mEnemyShipPool = nullptr;
	mFreeEnemyShips.clear();
}

EnemyShip* EnemyShipSystem::CreateEnemyShip(KEngine2D::Point position)
{
	assert(mEnemyShipPool != nullptr);
	EnemyShip* enemyShip = mFreeEnemyShips.front();
	if (enemyShip) {
		mFreeEnemyShips.pop_front();
		enemyShip->Init(mLuaScheduler, mHierarchySystem, mRenderer, mSpriteFactory, position);
	}
	return enemyShip;
}

void EnemyShipSystem::ReleaseEnemyShip(EnemyShip* ship)
{
	mFreeEnemyShips.push_back(ship);
}

void EnemyShipSystem::RegisterLibrary(lua_State* luaState, char const* name)
{
	mLuaWrapping.Init("KRBSG.EnemyShip");
	auto luaopen_enemyship = [](lua_State* luaState) {
		auto spawnEnemyShip = [](lua_State* luaState) {
			EnemyShipSystem* factory = (EnemyShipSystem*)lua_touserdata(luaState, lua_upvalueindex(1));

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

			EnemyShip* enemyShip = factory->CreateEnemyShip(position);
			return 0;
		};

		auto wrapEnemyShip = [](lua_State* luaState) -> int {
			EnemyShipSystem* enemyShipSystem = (EnemyShipSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
			enemyShipSystem->mLuaWrapping.WrapAndPush(luaState, 1);
			return 1;
		};

		auto move = [](lua_State* luaState) -> int {
			EnemyShipSystem* enemyShipSystem = (EnemyShipSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
			EnemyShip* ship = enemyShipSystem->mLuaWrapping.Unwrap(luaState, 1);
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

		auto despawn = [](lua_State* luaState) -> int {
			EnemyShipSystem* enemyShipSystem = (EnemyShipSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
			EnemyShip* ship = enemyShipSystem->mLuaWrapping.Unwrap(luaState, 1);
			ship->Deinit();
			enemyShipSystem->ReleaseEnemyShip(ship);
			return 0;
		};


		const luaL_Reg enemyShipMethods[] = {
				{"move", move},
				{"despawn", despawn},
				{nullptr, nullptr}
		};

		const luaL_Reg enemyShipLibrary[] = {
			{"spawnEnemyShip", spawnEnemyShip},
			{"wrapEnemyShip", wrapEnemyShip},
			{nullptr, nullptr}
		};

		EnemyShipSystem* enemyShipSystem = (EnemyShipSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
		lua_checkstack(luaState, 5);
		luaL_newmetatable(luaState, enemyShipSystem->mLuaWrapping.GetMetaTableName());
		luaL_newlibtable(luaState, enemyShipMethods);
		lua_pushvalue(luaState, lua_upvalueindex(1));
		luaL_setfuncs(luaState, enemyShipMethods, 1);
		lua_setfield(luaState, -2, "__index");
		lua_pop(luaState, 1);

		luaL_newlibtable(luaState, enemyShipLibrary);
		lua_pushvalue(luaState, lua_upvalueindex(1));
		luaL_setfuncs(luaState, enemyShipLibrary, 1);

		return 1;
	};

	PreloadLibrary(luaState, name, luaopen_enemyship);
}
