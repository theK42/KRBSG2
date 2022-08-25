#pragma once
#include "Boundaries2D.h"
#include "HierarchicalTransform2D.h"
#include "SpriteRenderer.h"
#include "SpriteFactory.h"
#include "LuaScheduler.h"

class WeaponSystem;
class Weapon;

class PlayerShip
{
public:
	PlayerShip();
	~PlayerShip();
	void Init(KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory, KEngine2D::Point position);
	void Deinit();
	void Move(KEngine2D::Point direction);
	const KEngine2D::Transform* GetWeaponAttach() const;
	void AddWeapon(Weapon* weapon, WeaponSystem *system);
private:
	KEngine2D::StaticTransform						mSelfTransform;
	KEngine2D::UpdatingHierarchicalTransform		mModelTransform;
	KEngine2D::UpdatingHierarchicalTransform		mWeaponAttachTransform;
	//KEngine2D::BoundingArea						mBoundingArea;
	//KEngine2D::BoundingBox						mBoundary;
	KEngineOpenGL::SpriteGraphic					mGraphic;
	KEngineCore::ScheduledLuaThread					mScript;
	bool											mInitialized;
	std::vector<std::pair<Weapon*, WeaponSystem*>>	mWeapons;

};

class PlayerShipSystem : public KEngineCore::LuaLibrary
{
public:
	PlayerShipSystem();
	~PlayerShipSystem();

	void Init(KEngineCore::LuaScheduler* luaScheduler, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineOpenGL::SpriteRenderer* renderer, SpriteFactory* spriteFactory, WeaponSystem * weaponSystem);
	void Deinit();

	PlayerShip *CreatePlayerShip(KEngine2D::Point position);

	void RegisterLibrary(lua_State* luaState, char const* name = "playerships");
private:
	PlayerShip mPlayerShip;

	KEngine2D::HierarchyUpdater* mHierarchySystem{ nullptr };
	KEngineOpenGL::SpriteRenderer* mRenderer{ nullptr };
	KEngineCore::LuaScheduler* mLuaScheduler{ nullptr };
	SpriteFactory* mSpriteFactory{ nullptr }; 
	WeaponSystem* mWeaponSystem{ nullptr };
	KEngineCore::LuaWrapping<PlayerShip>mLuaWrapping;
};



