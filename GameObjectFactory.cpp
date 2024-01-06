#include "GameObjectFactory.h"
#include "PoolParty.h"
#include "SpriteFactory.h"
#include "ShaderFactory.h"
#include "FontFactory.h"
#include "KRBSG2Constants.h"
#include "CollisionDispatcher.h"
#include "StaticTransform2D.h"
#include "HierarchicalTransform2D.h"
#include "MechanicalTransform2D.h"
#include "SpriteRenderer.h"
#include "LuaContext.h"
#include "DataTree.h"

#ifdef __cpp_lib_math_constants
#include <numbers>
#else //Embarrassing workaround for Android
namespace std {
	namespace numbers {
		const double pi = 3.14159265358979323846;
	}
}
#endif


const char Projectile::MetaName[] = "KRBSG.Projectile";
const char Weapon::MetaName[] = "KRBSG.Weapon";
const char PlayerShip::MetaName[] = "KRBSG.PlayerShip";
const char EnemyShip::MetaName[] = "KRBSG.EnemyShip";
const char Flyoff::MetaName[] = "KRBSG.Flyoff";

static const int MAIN_LAYER = 3;
static const int OVERLAY_LAYER = 4;

Weapon::~Weapon()
{
	Deinit();
}

void Weapon::Init()
{
}

void Weapon::Deinit()
{
}

PlayerShip::~PlayerShip()
{
	Deinit();
}

void PlayerShip::Init()
{
	mDisposables.Init();
}

void PlayerShip::Deinit()
{
	if (mFactory != nullptr)
		mFactory->ReleasePlayerShip(this);
	mFactory = nullptr;
}

void PlayerShip::Move(KEngine2D::Point direction)
{
	mSelfTransform->SetTranslation(mSelfTransform->GetTranslation() + direction);
}

EnemyShip::~EnemyShip()
{
	Deinit();
}

void EnemyShip::Init()
{
	mDisposables.Init();
}

void EnemyShip::Deinit()
{
	if (mFactory != nullptr)
		mFactory->ReleaseEnemyShip(this);
	mFactory = nullptr;
}

void EnemyShip::Move(KEngine2D::Point direction)
{
	mSelfTransform->SetTranslation(mSelfTransform->GetTranslation() + direction);
}

Projectile::~Projectile()
{
	Deinit();
}

void Projectile::Init()
{
	mDisposables.Init();
}

void Projectile::Deinit()
{
	if (mFactory != nullptr)
		mFactory->ReleaseProjectile(this);
	mFactory = nullptr;
}


Flyoff::~Flyoff()
{
	Deinit();
}

void Flyoff::Init()
{
	mDisposables.Init();
}

void Flyoff::Deinit()
{
	if (mFactory != nullptr)
		mFactory->ReleaseFlyoff(this);
	mFactory = nullptr;
}



GameObjectFactory::GameObjectFactory()
{
}

GameObjectFactory::~GameObjectFactory()
{
	Deinit();
}

void GameObjectFactory::Init(PoolParty* poolParty, KEngineCore::LuaScheduler* luaScheduler, KEngineCore::Timer * timer, KEngine2D::HierarchyUpdater* hierarchySystem, KEngine2D::MechanicsUpdater* mechanicsSystem, KEngineOpenGL::ShaderFactory* shaderFactory, KEngineOpenGL::SpriteRenderer* renderer, KEngineOpenGL::TextRenderer * textRenderer, KEngineOpenGL::FontFactory* fontFactory, SpriteFactory* spriteFactory, KEngine2D::CollisionSystem* collisionSystem, KRBSGCollisionDispatcher* collisionDispatcher, KEngineCore::DataTree* dataRoot)
{
	assert(mPoolParty == nullptr);
	mPoolParty = poolParty;
	mLuaScheduler = luaScheduler;
	mTimer = timer;
	mHierarchySystem = hierarchySystem;
	mMechanicsSystem = mechanicsSystem;
	mShaderFactory = shaderFactory;
	mRenderer = renderer;
	mTextRenderer = textRenderer;
	mFontFactory = fontFactory;
	mSpriteFactory = spriteFactory;
	mCollisionSystem = collisionSystem;
	mCollisionDispatcher = collisionDispatcher;
	mDataRoot = dataRoot;
	mProjectilePool.Init();
	mWeaponPool.Init();
	mPlayerShipPool.Init();
	mEnemyShipPool.Init();
	mFlyoffPool.Init();
	mStarfieldPool.Init(1);
	mStarfieldLayerPool.Init();
	mStarfieldChunkPool.Init();
}

void GameObjectFactory::Deinit()
{
	mPoolParty = nullptr;
	mLuaScheduler = nullptr;
	mTimer = nullptr;
	mHierarchySystem = nullptr;
	mRenderer = nullptr;
	mSpriteFactory = nullptr;
	mCollisionSystem = nullptr;
	mCollisionDispatcher = nullptr;
	mDataRoot = nullptr;

	mProjectilePool.Clear();
	mPlayerShipPool.Clear();
	mEnemyShipPool.Clear();
	mFlyoffPool.Clear();
	mStarfieldPool.Clear();
	mStarfieldChunkPool.Clear();

	mProjectilePool.Deinit();
	mWeaponPool.Deinit();
	mPlayerShipPool.Deinit();
	mEnemyShipPool.Deinit();
	mFlyoffPool.Deinit();
	mStarfieldPool.Deinit();
	mStarfieldLayerPool.Deinit();
	mStarfieldChunkPool.Deinit();
}

KEngine2D::StaticTransform* GameObjectFactory::CreateStaticTransform(KEngineCore::DisposableGroup* disposables, const KEngine2D::Point& position, double rotation)
{
	KEngine2D::StaticTransform* selfTransform = mPoolParty->GetStaticPool().GetItem(disposables);
	selfTransform = mPoolParty->GetStaticPool().GetItem(disposables);
	*selfTransform = KEngine2D::StaticTransform::Identity();
	selfTransform->SetTranslation(position);
	selfTransform->SetRotation(rotation);
	return selfTransform;
}

KEngineOpenGL::SpriteGraphic* GameObjectFactory::CreateSpriteGraphic(const KEngineOpenGL::Sprite* sprite, KEngineCore::DisposableGroup* disposables, KEngine2D::Transform* selfTransform, int layer)
{
	int width = sprite->width;
	int height = sprite->height;
	KEngine2D::Point modelUpperLeft = { -width / 2.0f, -height / 2.0f };
	KEngine2D::StaticTransform modelTransformTemp(modelUpperLeft);
	auto* modelTransform = mPoolParty->GetHierarchyPool().GetItem(disposables);
	modelTransform->Init(mHierarchySystem, selfTransform, modelTransformTemp);
	auto* graphic = mPoolParty->GetSpritePool().GetItem(disposables);
	graphic->Init(mRenderer, sprite, modelTransform, layer);
	return graphic;
}

KEngineOpenGL::SpriteGraphic * GameObjectFactory::CreateSpriteGraphic(KEngineCore::StringHash& spriteHash, KEngineCore::DisposableGroup* disposables, KEngine2D::Transform* selfTransform, int layer)
{
	const auto* sprite = mSpriteFactory->GetSprite(spriteHash);
	return CreateSpriteGraphic(sprite, disposables, selfTransform, layer);
}


KEngine2D::ColliderHandle GameObjectFactory::CreateCollider(KEngineCore::DisposableGroup* disposables, KEngine2D::Transform* transform, KEngineCore::StringHash& collisionShapeName, unsigned int flags, unsigned int filters)
{
	auto bounds = mPoolParty->GetBoundsPool().GetItem(disposables);
	bounds->Init(transform);
	auto* collisionsData = mDataRoot->GetBranch(HASH("sheetName", 0x7E99E530), HASH("CollisionShapes", 0x5EDD4EE1));
	KEngineCore::DataTree* collisionData = collisionsData->GetBranch(HASH("name", 0x5E237E06), collisionShapeName);

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

		auto shapeTransform = mPoolParty->GetHierarchyPool().GetItem(disposables);
		shapeTransform->Init(mHierarchySystem, transform, tempShapeTransform);

		KEngineCore::StringHash type = shapeDescription->GetHash(HASH("type", 0x8CDE5729));
		static auto box = HASH("box", 0x08A9483A);
		static auto circle = HASH("circle", 0xD4B76579);

		//TODO:  turn this damn thing into a switch statement.
		if (type == box) {
			auto* shape = mPoolParty->GetBoxPool().GetItem(disposables);
			shape->Init(shapeTransform, shapeDescription->GetInt(HASH("width", 0x8C1A452F)), shapeDescription->GetInt(HASH("height", 0xF54DE50F)));
			bounds->AddBoundingBox(shape);
		}
		else if (type == circle)
		{
			auto* shape = mPoolParty->GetCirclePool().GetItem(disposables);
			shape->Init(shapeTransform, shapeDescription->GetInt(HASH("radius", 0x3B7C6E5A)));
			bounds->AddBoundingCircle(shape);
		}
		else
		{
			assert(false); //What shape was it then.
		}
	}

	auto collider = mPoolParty->GetColliderPool().GetItem(disposables);
	collider->Init(mCollisionSystem, bounds, flags, filters);
	return collider->GetHandle();
}

KEngineCore::ScheduledLuaThread* GameObjectFactory::CreateScriptThread(KEngineCore::DisposableGroup* disposables, KEngineCore::LuaContext * parentContext, const std::string_view& scriptName, void* scriptObj, const char* objName)
{
	auto* context = mPoolParty->GetContextPool().GetItem(disposables);
	context->Init(mLuaScheduler, parentContext);

	context->AddContextualObject(objName, scriptObj);

	auto* scriptThread = mPoolParty->GetLuaPool().GetItem(disposables);
	
	context->RunScript(scriptName, scriptThread);
	
	return scriptThread; //Might not need this?  Or make more sense to return the context?  
}

KEngineOpenGL::TextSprite* GameObjectFactory::CreateTextSprite(KEngineCore::DisposableGroup* disposables, std::string_view text, KEngineCore::StringHash fontId, KEngineCore::StringHash shaderId, KEngine2D::Transform* transform)
{
	auto* textSprite = mPoolParty->GetTextPool().GetItem(disposables);
	
	const KEngineOpenGL::FixWidthBitmapFont& font = mFontFactory->GetFont(fontId);
	const KEngineOpenGL::ShaderProgram* shaderProgram = mShaderFactory->GetShaderProgram(shaderId);
	
	textSprite->Init(text, font, shaderProgram, mTextRenderer);
	return textSprite;
}

Weapon* GameObjectFactory::CreateWeapon(KEngineCore::DisposableGroup* disposables, const KEngine2D::Transform* transform, KEngineCore::StringHash weaponId, KEngineCore::LuaContext* parentContext)
{
	Weapon* weapon = mWeaponPool.GetItem(disposables);
	auto weaponDescriptions = mDataRoot->GetBranch(HASH("sheetName", 0x7E99E530), HASH("Weapons", 0x9DB3827D));
	auto weaponDescription = weaponDescriptions->GetBranch(HASH("name", 0x5E237E06), weaponId);
	weapon->Init();
	weapon->mTransform = transform;
	weapon->mCooldown = weaponDescription->GetFloat(HASH("cooldown", 0xDE632CD9));
	weapon->mDamage = weaponDescription->GetFloat(HASH("damage", 0x11C8546C));
	weapon->mFactory = this;

	auto scriptName = weaponDescription->GetString(HASH("script", 0x1C81873A));
	auto script = CreateScriptThread(disposables, parentContext, scriptName, weapon, Weapon::MetaName);
	return weapon;
}

PlayerShip* GameObjectFactory::CreatePlayerShip(KEngine2D::Point position, KEngineCore::StringHash shipId, KEngineCore::LuaContext* parentContext)
{
	PlayerShip* playerShip = mPlayerShipPool.GetItem();
	playerShip->Init();
	playerShip->mFactory = this;
	playerShip->mSelfTransform = CreateStaticTransform(&playerShip->mDisposables, position);

	auto shipDescriptions = mDataRoot->GetBranch(HASH("sheetName", 0x7E99E530), HASH("PlayerShips", 0x9F959BCA));
	auto shipDescription = shipDescriptions->GetBranch(HASH("name", 0x5E237E06), shipId);
	auto spriteHash = shipDescription->GetHash(HASH("sprite", 0x351D8F9E));
	auto graphic = CreateSpriteGraphic(spriteHash, &playerShip->mDisposables, playerShip->mSelfTransform, MAIN_LAYER);

	auto collisionShapeName = shipDescription->GetHash(HASH("collider", 0xCA3D45D7));
	playerShip->mColliderHandle = CreateCollider(&playerShip->mDisposables, playerShip->mSelfTransform, collisionShapeName, PlayerShipFlag, 0);
	mCollisionDispatcher->AddPlayerShip(playerShip);

	double height = graphic->GetSprite()->height;

	KEngine2D::Point modelUpperCenter = { 0, -height / 2.0f };
	KEngine2D::StaticTransform weaponTransform(modelUpperCenter);
			
	auto weaponAttachTransform = mPoolParty->GetHierarchyPool().GetItem(&playerShip->mDisposables);
	weaponAttachTransform->Init(mHierarchySystem, playerShip->mSelfTransform, weaponTransform);
	playerShip->mWeaponAttach = weaponAttachTransform;

	CreateScriptThread(&playerShip->mDisposables, parentContext, "playerShip.lua", playerShip, PlayerShip::MetaName);

	return playerShip;
}

void GameObjectFactory::ReleasePlayerShip(PlayerShip* ship)
{
	mCollisionDispatcher->RemoveItem(ship->mColliderHandle);
	ship->mDisposables.Deinit();
	mPlayerShipPool.ReleaseItem(ship);
}

EnemyShip* GameObjectFactory::CreateEnemyShip(KEngine2D::Point position, KEngineCore::StringHash shipId, KEngineCore::LuaContext * parentContext)
{
	EnemyShip* enemyShip = mEnemyShipPool.GetItem();
	enemyShip->Init();

	auto disposables = &enemyShip->mDisposables;

	enemyShip->mFactory = this;

	double rotation = std::numbers::pi;

	enemyShip->mSelfTransform = CreateStaticTransform(disposables, position, rotation);

	auto shipDescriptions = mDataRoot->GetBranch(HASH("sheetName", 0x7E99E530), HASH("Enemies", 0xB7BF73A2));
	auto shipDescription = shipDescriptions->GetBranch(HASH("name", 0x5E237E06), shipId);
	auto spriteHash = shipDescription->GetHash(HASH("sprite", 0x351D8F9E));

	CreateSpriteGraphic(spriteHash, disposables, enemyShip->mSelfTransform, MAIN_LAYER);

	auto scriptName = shipDescription->GetString(HASH("script", 0x1C81873A));
	void* scriptObj = enemyShip;
	
	CreateScriptThread(disposables, parentContext, scriptName, scriptObj, EnemyShip::MetaName);

	auto collisionShapeName = shipDescription->GetHash(HASH("collider", 0xCA3D45D7));
	unsigned int flags = EnemyShipFlag;
	unsigned int filters = PlayerShipFlag;

	enemyShip->mColliderHandle = CreateCollider(disposables, enemyShip->mSelfTransform, collisionShapeName, flags, filters);
	mCollisionDispatcher->AddEnemyShip(enemyShip);
	
	return enemyShip;
}

void GameObjectFactory::ReleaseEnemyShip(EnemyShip* ship)
{
	mCollisionDispatcher->RemoveItem(ship->mColliderHandle);
	ship->mDisposables.Deinit();
	mEnemyShipPool.ReleaseItem(ship);
}

Projectile* GameObjectFactory::CreateProjectile(const KEngine2D::Transform & origin, KEngineCore::StringHash projectileDefId, KEngineCore::LuaContext* parentContext)
{
	Projectile* projectile = mProjectilePool.GetItem();
	projectile->Init();

	projectile->mFactory = this;
	
	auto mover = mPoolParty->GetMechanicalPool().GetItem(&projectile->mDisposables);
	projectile->mMover = mover;
	mover->Init(mMechanicsSystem, origin);

	auto projectileDescriptions = mDataRoot->GetBranch(HASH("sheetName", 0x7E99E530), HASH("Projectiles", 0xAEADA67B));
	auto projectileDescription = projectileDescriptions->GetBranch(HASH("name", 0x5E237E06), projectileDefId);
	auto spriteHash = projectileDescription->GetHash(HASH("sprite", 0x351D8F9E));

	CreateSpriteGraphic(spriteHash, &projectile->mDisposables, mover, MAIN_LAYER);

	auto collisionShapeName = projectileDescription->GetHash(HASH("collider", 0xCA3D45D7));
	unsigned int flags = PlayerProjectileFlag;
	unsigned int filters = EnemyShipFlag;
	projectile->mColliderHandle = CreateCollider(&projectile->mDisposables, mover, collisionShapeName, flags, filters);
	mCollisionDispatcher->AddProjectile(projectile);

	CreateScriptThread(&projectile->mDisposables, parentContext, projectileDescription->GetString(HASH("script", 0x1C81873A)), projectile, Projectile::MetaName);
	return projectile;
}

void GameObjectFactory::ReleaseProjectile(Projectile* projectile)
{
	mCollisionDispatcher->RemoveItem(projectile->mColliderHandle);
	projectile->mDisposables.Deinit();
	mProjectilePool.ReleaseItem(projectile);
}

Flyoff* GameObjectFactory::CreateFlyoff(KEngine2D::Point origin, std::string_view text, KEngineCore::StringHash flyoffStyleId, KEngineCore::LuaContext* parentContext)
{
	Flyoff* flyoff = mFlyoffPool.GetItem();
	flyoff->Init();

	flyoff->mFactory = this;
	
	flyoff->mSelfTransform = CreateStaticTransform(&flyoff->mDisposables, origin);
	
	auto flyoffStyleDescriptions = mDataRoot->GetBranch(HASH("sheetName", 0x7E99E530), HASH("FlyoffStyles", 0x8915BE24));
	auto flyoffStyle = flyoffStyleDescriptions->GetBranch(HASH("name", 0x5E237E06), flyoffStyleId);
	auto fontHash = flyoffStyle->GetHash(HASH("font", 0xD09408D2));
	auto shaderHash = flyoffStyle->GetHash(HASH("shader", 0xFFF39E28));
	auto textSprite = CreateTextSprite(&flyoff->mDisposables, text, fontHash, shaderHash, flyoff->mSelfTransform);
	flyoff->mSprite = CreateSpriteGraphic(&textSprite->GetSprite(), &flyoff->mDisposables, flyoff->mSelfTransform, OVERLAY_LAYER);

	CreateScriptThread(&flyoff->mDisposables, parentContext, flyoffStyle->GetString(HASH("script", 0x1C81873A)), flyoff, Flyoff::MetaName);
	return flyoff;
}

void GameObjectFactory::ReleaseFlyoff(Flyoff* flyoff)
{
	flyoff->mDisposables.Deinit();
	mFlyoffPool.ReleaseItem(flyoff);
}

StarfieldChunk* GameObjectFactory::CreateStarfieldChunk(KEngineCore::DataTree * starfieldLayerDefinition, int y, int width, int height, int layer)
{
	auto starfieldChunk = mStarfieldChunkPool.GetItem();

	KEngine2D::StaticTransform startPosition;
	startPosition.Init();
	startPosition.SetTranslation({ 0, (double)y });

	auto mover = mPoolParty->GetMechanicalPool().GetItem(&starfieldChunk->mDisposables);
	mover->Init(mMechanicsSystem, startPosition, { 0, starfieldLayerDefinition->GetFloat(HASH("speed", 0x0F26FEF6)) });

	int starCount = starfieldLayerDefinition->GetInt(HASH("density", 0xC0A86AFA));
	float alpha = starfieldLayerDefinition->GetFloat(HASH("alpha", 0xD0E0396A));
	
	KEngineCore::DataTree* starsData = starfieldLayerDefinition->GetBranch(HASH("branchName", 0xD7728FA9), HASH("stars", 0x011DC02C));


	int starDiversity = starsData->GetNumHashes();
	std::uniform_int_distribution<int> horizontalDist(0, width);	
	std::uniform_int_distribution<int> verticalDist(0, height);
	std::uniform_int_distribution<int> starDist(1, starDiversity - 1); //Lists of Hashes unfortunately have to start at 1

	for (int i = 0; i < starCount; i++)
	{
		int starIndex = starDist(mRandomEngine);
		double localX = horizontalDist(mRandomEngine);
		double localY = verticalDist(mRandomEngine);

		KEngine2D::StaticTransform starTransTemp;
		starTransTemp.Init();
		starTransTemp.SetTranslation({ localX, localY });

		auto* starTrans = mPoolParty->GetHierarchyPool().GetItem(&starfieldChunk->mDisposables);
		starTrans->Init(mHierarchySystem, mover, starTransTemp);

		auto spriteHash = starsData->GetHash(starIndex);
		auto graphic = CreateSpriteGraphic(spriteHash, &starfieldChunk->mDisposables, starTrans, layer);
		graphic->SetAlpha(alpha);
	}
	return starfieldChunk;
}

void GameObjectFactory::ReleaseStarfieldChunk(StarfieldChunk* chunk)
{
	chunk->mDisposables.Deinit();
	mStarfieldChunkPool.ReleaseItem(chunk);
}

StarfieldLayer* GameObjectFactory::CreateStarfieldLayer(KEngineCore::DisposableGroup * disposables, KEngineCore::DataTree* starfieldLayerDefinition, int width, int height, int layer)
{
	auto starfieldLayer = mStarfieldLayerPool.GetItem(disposables);
	starfieldLayer->Init();
	starfieldLayer->mFactory = this;
	starfieldLayer->mStarfieldLayerDescription = starfieldLayerDefinition;
	starfieldLayer->mWidth = width;
	starfieldLayer->mHeight = height;
	starfieldLayer->mLayer = layer;
	starfieldLayer->mChunks.push_back(CreateStarfieldChunk(starfieldLayerDefinition, height, width, height, layer));
	starfieldLayer->mChunks.push_back(CreateStarfieldChunk(starfieldLayerDefinition, 0, width, height, layer));
	starfieldLayer->mChunks.push_back(CreateStarfieldChunk(starfieldLayerDefinition, -height, width, height, layer));
	float speed = starfieldLayerDefinition->GetFloat(HASH("speed", 0x0F26FEF6));
	KEngineCore::Timeout* timeout = mPoolParty->GetTimePool().GetItem(&starfieldLayer->mDisposables);
	timeout->Init(mTimer, height / speed, true, [starfieldLayer]()
	{
		starfieldLayer->mFactory->ReleaseStarfieldChunk(starfieldLayer->mChunks.front());
		starfieldLayer->mChunks.pop_front();
		starfieldLayer->mChunks.push_back(starfieldLayer->mFactory->CreateStarfieldChunk(starfieldLayer->mStarfieldLayerDescription, -starfieldLayer->mHeight, starfieldLayer->mWidth, starfieldLayer->mHeight, starfieldLayer->mLayer));
	});
	return starfieldLayer;
}

Starfield* GameObjectFactory::CreateStarfield(int width, int height)
{
	KEngineCore::DataTree* starfieldDefinition = mDataRoot->GetBranch(HASH("sheetName", 0x7E99E530), HASH("Starfield", 0x7330AFA9));
	auto starfield = mStarfieldPool.GetItem();
	int layerCount = starfieldDefinition->GetNumBranches();
	for (int i = 0; i < layerCount; i++)
	{
		CreateStarfieldLayer(&starfield->mDisposables, starfieldDefinition->GetBranch(i), width, height, i);
	}
	return starfield;
}

StarfieldChunk::~StarfieldChunk()
{
	Deinit();
}

void StarfieldChunk::Init()
{
	mDisposables.Init();
}

void StarfieldChunk::Deinit()
{
	mDisposables.Deinit();
}

StarfieldLayer::~StarfieldLayer()
{
	Deinit();
}

void StarfieldLayer::Init()
{
	mDisposables.Init();
}

void StarfieldLayer::Deinit()
{
	mDisposables.Deinit();
}

Starfield::~Starfield()
{
	Deinit();
}

void Starfield::Init()
{
	mDisposables.Init();
}

void Starfield::Deinit()
{
	mDisposables.Deinit();
}
