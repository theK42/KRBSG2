#include "EnemyShip.h"
#include "SpriteFactory.h"
#include "PoolParty.h"
#include "DataTree.h"
#include <assert.h>
#include <numbers>


void EnemyShip::Move(KEngine2D::Point direction)
{
	mSelfTransform->SetTranslation(mSelfTransform->GetTranslation() + direction);
}

EnemyShipSystem::EnemyShipSystem()
{
}

EnemyShipSystem::~EnemyShipSystem()
{
}

const int TEMP_POOL_SIZE = 100;
void EnemyShipSystem::Init(PoolParty* poolParty, KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory, KEngineCore::DataTree* shipDescription)
{
	assert(mLuaScheduler == nullptr);
	mPoolParty = poolParty;
	mEnemyShipPool.Init();
	mLuaScheduler = luaScheduler;
	mHierarchySystem = hierarchySystem;
	mRenderer = renderer;
	mSpriteFactory = spriteFactory;
	mShipDescription = shipDescription;

	RegisterLibrary(luaScheduler->GetMainState());
}

void EnemyShipSystem::Deinit()
{
	mLuaScheduler = nullptr;
	mHierarchySystem = nullptr;
	mRenderer = nullptr;
	mSpriteFactory = nullptr;
	mEnemyShipPool.Clear();
	mEnemyShipPool.Deinit();
}

EnemyShip* EnemyShipSystem::CreateEnemyShip(KEngine2D::Point position, const KEngineCore::DataTree * shipDescription)
{
	EnemyShip* enemyShip = mEnemyShipPool.GetItem();

	enemyShip->mSelfTransform = mPoolParty->GetStaticPool().GetItem(&enemyShip->mDisposables);
	*enemyShip->mSelfTransform = KEngine2D::StaticTransform::Identity();
	enemyShip->mSelfTransform->SetTranslation(position);
	
	const auto * sprite = mSpriteFactory->GetSprite(shipDescription->GetHash(HASH("sprite", 0x351D8F9E)));
	int width = sprite->width;
	int height = sprite->height;
	KEngine2D::Point modelUpperLeft = { -width / 2.0f, -height / 2.0f };    
	KEngine2D::StaticTransform modelTransform(modelUpperLeft, std::numbers::pi);
	enemyShip->mModelTransform = mPoolParty->GetHierarchyPool().GetItem(&enemyShip->mDisposables);
	enemyShip->mModelTransform->Init(mHierarchySystem, enemyShip->mSelfTransform, modelTransform);
	enemyShip->mGraphic = mPoolParty->GetSpritePool().GetItem(&enemyShip->mDisposables);
	enemyShip->mGraphic->Init(mRenderer, sprite, enemyShip->mModelTransform);
	
	enemyShip->mScript = mPoolParty->GetLuaPool().GetItem(&enemyShip->mDisposables);
	enemyShip->mScript->Init(mLuaScheduler, shipDescription->GetString(HASH("script", 0x1C81873A)));
	lua_State* scriptState = enemyShip->mScript->GetThreadState();
	lua_checkstack(scriptState, 1);
	lua_pushlightuserdata(scriptState, enemyShip);
	enemyShip->mScript->Resume(1);

	enemyShip->mInitialized = true;
	return enemyShip;
}

void EnemyShipSystem::ReleaseEnemyShip(EnemyShip* ship)
{
	mEnemyShipPool.ReleaseItem(ship);
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

			EnemyShip* enemyShip = factory->CreateEnemyShip(position, factory->mShipDescription);
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
			ship->mDisposables.Deinit();
			ship->mInitialized = false;
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
