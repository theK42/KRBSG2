#pragma once
#include "Disposable.h"

namespace KEngineCore
{
	class DataTree;
}

namespace KEngineOpenGL
{
	class TextSprite;
	class FontFactory;
}

class GameObjectFactory;
class ScoreKeeper;

class Hud
{
public:
	Hud();
	~Hud();

	void Init(GameObjectFactory* factory, ScoreKeeper* scoreKeeper,  KEngineOpenGL::FontFactory * fontFactory);
	void Deinit();

private:
	ScoreKeeper*				mScoreKeeper{ nullptr };
	GameObjectFactory*			mFactory{ nullptr };
	KEngineOpenGL::FontFactory*	mFontFactory{ nullptr };
	KEngineOpenGL::TextSprite*	mScoreField{ nullptr };
	KEngineCore::DisposableGroup	mDisposables;
};

