#pragma once

#include "Disposable.h"

namespace KEngineCore
{
	class DataTree;
	class LuaContext;
}

namespace KEngineOpenGL
{
	class FontFactory;
}

namespace KEngineBasics
{
	class UIViewFactory;
}

class PauseMenu
{
public:
	PauseMenu();
	~PauseMenu();

	void Init(KEngineOpenGL::FontFactory* fontFactory, int width, int height, KEngineBasics::UIViewFactory* uiFactory, KEngineCore::LuaContext* parentContext);
	void Deinit();

private:
	KEngineCore::DisposableGroup	mDisposables;
	KEngineCore::LuaContext*		mParentContext{ nullptr }; //just store pointer to parent context until we have a reason to have our own
};
