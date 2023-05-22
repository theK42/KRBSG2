#include "GameplaySession.h"
#include "PoolParty.h"

#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include "SDL.h"
#endif


const char GameoverCallback::MetaName[] = "KRBSG.GameOverCallback";

GameplaySession::GameplaySession()
{
}

GameplaySession::~GameplaySession()
{
	Deinit();
}

void GameplaySession::Init(KEngineCore::LuaContext* scriptParent, KEngineCore::Timer* rootTimer, KEngineBasics::Input* rootInput, KEngineCore::LuaScheduler* luaScheduler, KEngineCore::Psychopomp* psychopomp, KEngine2D::HierarchyUpdater* hierarchySystem, KEngineBasics::ShaderFactory* shaderFactory, KEngineBasics::SpriteRenderer* renderer, KEngineBasics::TextRenderer* textRenderer, KEngineBasics::FontFactory* fontFactory, SpriteFactory* spriteFactory, KEngineCore::DataTree* dataRoot, PoolParty* poolParty, KEngineBasics::UIViewFactory* uiFactory, const char* scriptName, int width, int height)
{
	mScriptRunner.Init(luaScheduler, scriptParent);

	mTimer.Init(luaScheduler);
	mInput.Init(luaScheduler, &mTimer);

	mInput.AddCombinedAxis(HASH("primary", 0x1745EA86));

	mInput.AddChildAxis(HASH("primary", 0x1745EA86), HASH("primary.horizontal", 0x520B651B), KEngineBasics::AxisType::Horizontal, KEngineBasics::ControllerType::Joystick, 0);
	mInput.AddChildAxis(HASH("primary", 0x1745EA86), HASH("primary.vertical", 0x6ED1CEFB), KEngineBasics::AxisType::Vertical, KEngineBasics::ControllerType::Joystick, 1);

	mInput.AddChildAxis(HASH("primary", 0x1745EA86), HASH("primary.horizontal", 0x520B651B), KEngineBasics::AxisType::Horizontal, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_AXIS_LEFTX);
	mInput.AddChildAxis(HASH("primary", 0x1745EA86), HASH("primary.vertical", 0x6ED1CEFB), KEngineBasics::AxisType::Vertical, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_AXIS_LEFTY);

	mInput.AddAxisButton(HASH("primary.horizontal", 0x520B651B), HASH("primary.left", 0x174671DF), KEngineBasics::AxisType::Horizontal, -1, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	mInput.AddAxisButton(HASH("primary.horizontal", 0x520B651B), HASH("primary.right", 0xE1A272AD), KEngineBasics::AxisType::Horizontal, 1, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	mInput.AddAxisButton(HASH("primary.vertical", 0x6ED1CEFB), HASH("primary.down", 0x71DE068C), KEngineBasics::AxisType::Vertical, -1, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP);
	mInput.AddAxisButton(HASH("primary.vertical", 0x6ED1CEFB), HASH("primary.up", 0x8840872B), KEngineBasics::AxisType::Vertical, 1, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN);

	mInput.AddAxisButton(HASH("primary.horizontal", 0x520B651B), HASH("primary.left", 0x174671DF), KEngineBasics::AxisType::Horizontal, -1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_A);
	mInput.AddAxisButton(HASH("primary.horizontal", 0x520B651B), HASH("primary.right", 0xE1A272AD), KEngineBasics::AxisType::Horizontal, 1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_D);
	mInput.AddAxisButton(HASH("primary.vertical", 0x6ED1CEFB), HASH("primary.down", 0x71DE068C), KEngineBasics::AxisType::Vertical, -1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_W);
	mInput.AddAxisButton(HASH("primary.vertical", 0x6ED1CEFB), HASH("primary.up", 0x8840872B), KEngineBasics::AxisType::Vertical, 1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_S);

	mInput.AddAxisButton(HASH("primary.horizontal", 0x520B651B), HASH("primary.left", 0x174671DF), KEngineBasics::AxisType::Horizontal, -1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_LEFT);
	mInput.AddAxisButton(HASH("primary.horizontal", 0x520B651B), HASH("primary.right", 0xE1A272AD), KEngineBasics::AxisType::Horizontal, 1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_RIGHT);
	mInput.AddAxisButton(HASH("primary.vertical", 0x6ED1CEFB), HASH("primary.down", 0x71DE068C), KEngineBasics::AxisType::Vertical, -1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_UP);
	mInput.AddAxisButton(HASH("primary.vertical", 0x6ED1CEFB), HASH("primary.up", 0x8840872B), KEngineBasics::AxisType::Vertical, 1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_DOWN);

	mInput.AddCursor(HASH("cursor", 0xC57F6A8E), KEngineBasics::ControllerType::Mouse);

	mInput.AddButton(HASH("cursor.primary", 0xB101C609), KEngineBasics::ControllerType::Mouse, 1);

	mInput.AddVirtualAxis(HASH("primary.horizontal", 0x520B651B), HASH("cursor", 0xC57F6A8E), KEngineBasics::AxisType::Horizontal, HASH("cursor.primary", 0xB101C609), 1.0f/80.0f);
	mInput.AddVirtualAxis(HASH("primary.vertical", 0x6ED1CEFB), HASH("cursor", 0xC57F6A8E), KEngineBasics::AxisType::Vertical, HASH("cursor.primary", 0xB101C609), 1.0f / 80.0f);

	mInput.AddButton(HASH("fire", 0x58DE09CF), KEngineBasics::ControllerType::Joystick, 0);
	mInput.AddButton(HASH("fire", 0x58DE09CF), KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_A);
	mInput.AddButton(HASH("fire", 0x58DE09CF), KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_SPACE);
	//mInput.AddButton(HASH("fire", 0x58DE09CF), KEngineBasics::ControllerType::Mouse, 0);

	mScriptRunner.AddContextualObject("time", &mTimer);
	mScriptRunner.AddContextualObject("input", &mInput);

	auto t = poolParty->GetContinuousTimePool().GetItem(&mDisposables);
	t->Init(rootTimer, [this](double delta)
		{
			mTimer.Update(delta);
		}
	);

	t = poolParty->GetContinuousTimePool().GetItem(&mDisposables);
	t->Init(&mTimer, [this](double delta)
		{
			mMechanicsSystem.Update(delta); 
			mCollisionSystem.Update();
		}
	);

	auto iF = poolParty->GetInputForwardingPool().GetItem(&mDisposables);
	iF->Init(rootInput,
		[this](KEngineBasics::ControllerType type, int id, int axisPosition)
		{
			mInput.HandleAxisChange(type, id, axisPosition);
		},
		[this](KEngineBasics::ControllerType type, int id)
		{
			mInput.HandleButtonDown(type, id);
		},
		[this](KEngineBasics::ControllerType type, int id)
		{
			mInput.HandleButtonUp(type, id);
		},
		[this](KEngineBasics::ControllerType type, const KEngine2D::Point& position)
		{
			mInput.HandleCursorPosition(type, position);
		}
	);

	mMechanicsSystem.Init();
	mScoreKeeper.Init(0);
	mCollisionDispatcher.Init(psychopomp, &mScoreKeeper);
	mCollisionSystem.Init(&mCollisionDispatcher);
	mGameObjectFactory.Init(poolParty, luaScheduler, &mTimer, hierarchySystem, &mMechanicsSystem, shaderFactory, renderer, textRenderer, fontFactory, spriteFactory, &mCollisionSystem, &mCollisionDispatcher, dataRoot);
	mHud.Init(&mGameObjectFactory, &mScoreKeeper, fontFactory, width, height, uiFactory, &mScriptRunner);

	mScriptRunner.RunScript(scriptName, &mThread);
}

void GameplaySession::Deinit()
{
	mThread.Deinit();
	mScriptRunner.Deinit();
	mDisposables.Deinit();
	mGameObjectFactory.Deinit();
	mCollisionSystem.Deinit();
	mCollisionDispatcher.Deinit();
	mScoreKeeper.Deinit();
	mMechanicsSystem.Deinit();
	mInput.Deinit();
	mTimer.Deinit();
}

GameObjectFactory* GameplaySession::GetGameObjectFactory()
{
	return &mGameObjectFactory;
}

void GameplaySession::Pause()
{
	mTimer.Pause();
	mInput.Pause();
}

void GameplaySession::Resume()
{
	mTimer.Resume();
	mInput.Resume();
}

void GameplaySession::AddGameoverCallback(GameoverCallback* callback)
{
	mGameOverCallbacks.push_back(callback);
	callback->mPosition = std::next(mGameOverCallbacks.rbegin()).base();
}

void GameplaySession::ClearGameoverCallback(GameoverCallback* callback)
{
	mGameOverCallbacks.erase(callback->mPosition);
	callback->mPosition = mGameOverCallbacks.end();
}

GameoverCallback::GameoverCallback()
{
}

GameoverCallback::~GameoverCallback()
{
	Deinit();
}

void GameoverCallback::Init(GameplaySession* session, std::function<void()> callback)
{
	assert(mCallback == nullptr);
	mCallback = callback;
	mSession = session;
	session->AddGameoverCallback(this);
}

void GameoverCallback::Deinit()
{
	if (mSession != nullptr && mPosition != mSession->mGameOverCallbacks.end())
	{
		mSession->ClearGameoverCallback(this);
	}
	mSession = nullptr;
	mCallback = nullptr;
}