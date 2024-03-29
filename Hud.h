#pragma once
#include "Disposable.h"

namespace KEngineCore
{
	class DataTree;
	class LuaContext;
}

namespace KEngineOpenGL
{
	class TextSprite;
	class FontFactory;
}

namespace KEngineBasics
{
	class UIViewFactory;
	class UITextView;
}

class GameObjectFactory;
class ScoreKeeper;

class Hud
{
public:
	Hud();
	~Hud();

	void Init(GameObjectFactory* factory, ScoreKeeper* scoreKeeper,  KEngineOpenGL::FontFactory * fontFactory, int width, int height, KEngineBasics::UIViewFactory* uiFactory, KEngineCore::LuaContext * parentContext);
	void Deinit();

private:
	ScoreKeeper*					mScoreKeeper{ nullptr };
	GameObjectFactory*				mFactory{ nullptr };
	KEngineOpenGL::FontFactory*		mFontFactory{ nullptr };
	KEngineBasics::UITextView*		mScoreTextView{ nullptr };
	KEngineCore::DisposableGroup	mDisposables;
	KEngineCore::LuaContext*		mParentContext{ nullptr }; //just store pointer to parent context until we have a reason to have our own
};

