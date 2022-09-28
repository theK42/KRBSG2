#include "EnemyShip.h"
#include "KRBSG2Constants.h"
#include "CollisionDispatcher.h"
#include "SpriteFactory.h"
#include "PoolParty.h"
#include "DataTree.h"
#include <assert.h>
#include <numbers>


void EnemyShip::Move(KEngine2D::Point direction)
{
	mSelfTransform->SetTranslation(mSelfTransform->GetTranslation() + direction);
}

void EnemyShip::Die()
{
	mDisposables.Deinit();
	mInitialized = false;
	mSystem->ReleaseEnemyShip(this);
}

EnemyShipSystem::EnemyShipSystem()
{
}

EnemyShipSystem::~EnemyShipSystem()
{
}

void EnemyShipSystem::Init(PoolParty* poolParty, KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory, KEngine2D::CollisionSystem * collisionSystem, KRBSGCollisionDispatcher * collisionDispatcher, KEngineCore::DataTree* shipDescription, KEngineCore::DataTree* collisionData)
{
	assert(mLuaScheduler == nullptr);
	mPoolParty = poolParty;
	mEnemyShipPool.Init();
	mLuaScheduler = luaScheduler;
	mHierarchySystem = hierarchySystem;
	mRenderer = renderer;
	mSpriteFactory = spriteFactory;
	mCollisionSystem = collisionSystem;
	mCollisionDispatcher = collisionDispatcher;

	mShipDescription = shipDescription;
	mCollisionsData = collisionData;

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

	enemyShip->mSystem = this;

	enemyShip->mSelfTransform = mPoolParty->GetStaticPool().GetItem(&enemyShip->mDisposables);
	*enemyShip->mSelfTransform = KEngine2D::StaticTransform::Identity();
	enemyShip->mSelfTransform->SetTranslation(position);
	enemyShip->mSelfTransform->SetRotation(std::numbers::pi);
	
	const auto * sprite = mSpriteFactory->GetSprite(shipDescription->GetHash(HASH("sprite", 0x351D8F9E)));
	int width = sprite->width;
	int height = sprite->height;
	KEngine2D::Point modelUpperLeft = { -width / 2.0f, -height / 2.0f };    
	KEngine2D::StaticTransform modelTransform(modelUpperLeft);
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

	auto bounds = mPoolParty->GetBoundsPool().GetItem(&enemyShip->mDisposables);
	bounds->Init(enemyShip->mSelfTransform);

	auto collisionShapeName = shipDescription->GetHash(HASH("collider", 0xCA3D45D7));
	KEngineCore::DataTree* collisionData = mCollisionsData->GetBranch(HASH("name", 0x5E237E06), collisionShapeName);

	KEngineCore::DataTree* shapesData = collisionData->GetBranch(HASH("branchName", 0xD7728FA9), HASH("shapes", 0x93DBA512));
	int collisionShapeCount = shapesData->GetNumBranches();
	for (int i = 0; i < collisionShapeCount; i++)
	{
		KEngineCore::DataTree* shapeDescription = shapesData->GetBranch(i);
		
		KEngine2D::StaticTransform tempShapeTransform;
		tempShapeTransform.Init();
		tempShapeTransform.SetTranslation({ shapeDescription->GetFloat(HASH("posX", 0x6852006E)), shapeDescription->GetFloat(HASH("posY", 0x1F5530F8)) });
		tempShapeTransform.SetScale({ shapeDescription->GetFloat(HASH("scaleX", 0x5D8B3757)), shapeDescription->GetFloat(HASH("scaleY", 0x2A8C07C1)) });
		tempShapeTransform.SetRotation(shapeDescription->GetFloat(HASH("rot", 0x1D39A761)));

		auto shapeTransform = mPoolParty->GetHierarchyPool().GetItem(&enemyShip->mDisposables);
		shapeTransform->Init(mHierarchySystem, enemyShip->mSelfTransform, tempShapeTransform);
		
		KEngineCore::StringHash type = shapeDescription->GetHash(HASH("type", 0x8CDE5729));
		static auto box = HASH("box", 0x08A9483A);
		static auto circle = HASH("circle", 0xD4B76579);
		
		//TODO:  turn this damn thing into a switch statement.
		if (type == box) {
			auto* shape = mPoolParty->GetBoxPool().GetItem(&enemyShip->mDisposables);
			shape->Init(shapeTransform, shapeDescription->GetInt(HASH("width", 0x8C1A452F)), shapeDescription->GetInt(HASH("height", 0xF54DE50F)));
			bounds->AddBoundingBox(shape);
		}
		else if (type == circle)
		{
			auto* shape = mPoolParty->GetCirclePool().GetItem(&enemyShip->mDisposables);
			shape->Init(shapeTransform, shapeDescription->GetInt(HASH("radius", 0x3B7C6E5A)));
			bounds->AddBoundingCircle(shape);
		}
		else
		{
			assert(false); //What shape was it then.
		}
	}

	auto collider = mPoolParty->GetColliderPool().GetItem(&enemyShip->mDisposables);
	collider->Init(mCollisionSystem, bounds, EnemyShipFlag, PlayerShipFlag);
	enemyShip->mColliderHandle = collider->GetHandle();
	mCollisionDispatcher->AddEnemyShip(enemyShip, collider->GetHandle());


	enemyShip->mInitialized = true;
	return enemyShip;
}

void EnemyShipSystem::ReleaseEnemyShip(EnemyShip* ship)
{
	mCollisionDispatcher->RemoveItem(ship->mColliderHandle);
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
