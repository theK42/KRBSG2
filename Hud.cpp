#include "Hud.h"
#include "ScoreKeeper.h"
#include "GameObjectFactory.h"
#include "TextRenderer.h"
#include "StaticTransform2D.h"
#include "FontFactory.h"
#include "UIView.h"
#include "UITextView.h"
#include <assert.h>

#include <iostream>
#include "UIViewFactory.h"

Hud::Hud()
{
}

Hud::~Hud()
{
	Deinit();
}

void Hud::Init(GameObjectFactory* factory, ScoreKeeper* scoreKeeper, KEngineOpenGL::FontFactory* fontFactory, int width, int height, KEngineBasics::UIViewFactory* uiFactory, KEngineCore::LuaContext * parentContext)
{
	assert(mFactory == nullptr);
	mDisposables.Init();
	mFactory = factory;
	mFontFactory = fontFactory;
	mScoreKeeper = scoreKeeper;
	mParentContext = parentContext;
	
	auto* fullScreenRight = uiFactory->CreateStaticLayoutGuide(&mDisposables, 800);
	auto* fullScreenBottom = uiFactory->CreateStaticLayoutGuide(&mDisposables, 600);
	auto* fullScreenOrigin = uiFactory->CreateStaticLayoutGuide(&mDisposables, 0);
	
	KEngineBasics::UIView* hudView = uiFactory->CreateUIView(&mDisposables, nullptr, fullScreenOrigin, fullScreenOrigin, fullScreenRight, fullScreenBottom);
	
	auto* topMargin = uiFactory->CreateParentRelativeLayoutGuide(&mDisposables, KEngineBasics::Height, 0.0, 20);
	auto* rightMargin = uiFactory->CreateParentRelativeLayoutGuide(&mDisposables, KEngineBasics::Width, 1.0, -20);
	auto* bottomContent = uiFactory->CreateContentRelativeLayoutGuide(&mDisposables, topMargin, KEngineBasics::Height, 1, 0);
	auto* leftContent = uiFactory->CreateContentRelativeLayoutGuide(&mDisposables, rightMargin, KEngineBasics::Width, -1.0, 0);

	mScoreTextView = uiFactory->CreateUITextView(&mDisposables, hudView, leftContent, topMargin, rightMargin, bottomContent, HASH("cellphone", 0xAAA18E60), HASH("Textured", 0xDF225E87), "0");


	mScoreKeeper->SetScoreChangeHandler([this](int score, KEngine2D::Point location) {
		std::string s1 = std::to_string(score);
		auto flyoff = mFactory->CreateFlyoff(location, s1, HASH("scoreFlyoff", 0xC5BF99CD), mParentContext);
		std::string s2 = std::to_string(mScoreKeeper->GetScore());
		mScoreTextView->SetText(s2);
	});
}

void Hud::Deinit()
{
	mDisposables.Deinit();
	if (mFactory != nullptr)
	{
		mScoreKeeper->SetScoreChangeHandler(nullptr);
	}
	mFactory = nullptr;
	mScoreKeeper = nullptr;
	mScoreTextView = nullptr;
}
