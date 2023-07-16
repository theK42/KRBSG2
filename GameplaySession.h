#pragma once

#include "LuaContext.h"
#include "Timer.h"
#include "MechanicalTransform2D.h"
#include "GameObjectFactory.h"
#include "Renderers.h"
#include "Input.h"
#include "CollisionDispatcher.h"
#include "ScoreKeeper.h"
#include "Hud.h"

namespace KEngine2D
{
    class HierarchyUpdater;
}

class GameplaySession;

class GameoverCallback
{
public:
    GameoverCallback();
    ~GameoverCallback();

    void Init(GameplaySession* session, std::function<void()> callback);
    void Deinit();

    static const char MetaName[];
private:

    std::list<GameoverCallback*>::iterator	mPosition;
    std::function<void()>			        mCallback{ nullptr };
    GameplaySession*                        mSession{ nullptr };
    friend class GameplaySession;
};


class GameplaySession
{
public:
    GameplaySession();
    ~GameplaySession();

    void Init(KEngineCore::LuaContext* scriptParent, KEngineCore::Timer* rootTimer, KEngineBasics::Input * rootInput, KEngineCore::LuaScheduler* luaScheduler, KEngineCore::Psychopomp* psychopomp, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineGraphics::ShaderFactory* shaderFactory, KEngineGraphics::SpriteRenderer* renderer, KEngineGraphics::TextRenderer* textRenderer, KEngineGraphics::FontFactory* fontFactory, SpriteFactory* spriteFactory, KEngineCore::DataTree* dataRoot, PoolParty* poolParty, KEngineBasics::UIViewFactory * uiFactory, const char* scriptName, int width, int height);
    void Deinit();

    GameObjectFactory* GetGameObjectFactory();

    void Pause();
    void Resume();

 
private:
    void AddGameoverCallback(GameoverCallback* callback);
    void ClearGameoverCallback(GameoverCallback* callback); 

    KEngineCore::LuaContext         mScriptRunner;
    KEngineCore::ScheduledLuaThread mThread;
    KEngineCore::Timer              mTimer;
    KEngineBasics::Input            mInput;
    KEngine2D::MechanicsUpdater     mMechanicsSystem;
    GameObjectFactory               mGameObjectFactory;
    ScoreKeeper                     mScoreKeeper;
    KRBSGCollisionDispatcher        mCollisionDispatcher;
    KEngine2D::CollisionSystem      mCollisionSystem;
    Hud                             mHud;

    KEngineCore::DisposableGroup	mDisposables;
    std::list<GameoverCallback*>   mGameOverCallbacks;

    friend class GameoverCallback;
};

