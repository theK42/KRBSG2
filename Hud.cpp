#include "Hud.h"
#include "ScoreKeeper.h"
#include "GameObjectFactory.h"
#include "TextRenderer.h"
#include "StaticTransform2D.h"
#include "FontFactory.h"
#include <assert.h>

#include <iostream>

Hud::Hud()
{
}

Hud::~Hud()
{
	Deinit();
}

void Hud::Init(GameObjectFactory* factory, ScoreKeeper* scoreKeeper, KEngineOpenGL::FontFactory* fontFactory)
{
	assert(mFactory == nullptr);
	mDisposables.Init();
	mFactory = factory;
	mFontFactory = fontFactory;
	mScoreKeeper = scoreKeeper;
	KEngine2D::StaticTransform* transform = mFactory->CreateStaticTransform(&mDisposables, { 600, 100 });

	mScoreField = mFactory->CreateTextSprite(&mDisposables, "0000000", HASH("cellphone", 0xAAA18E60), HASH("Textured", 0xDF225E87), transform);
	mFactory->CreateSpriteGraphic(&mScoreField->GetSprite(), &mDisposables, transform, 4); //TODO:  READ THIS FROM SOMEWHERE

	mScoreKeeper->SetScoreChangeHandler([this](int score, KEngine2D::Point location) {
		std::string s1 = std::to_string(score);
		auto flyoff = mFactory->CreateFlyoff(location, s1, HASH("scoreFlyoff", 0xC5BF99CD));
		std::string s2 = std::to_string(mScoreKeeper->GetScore());
		const KEngineOpenGL::FixWidthBitmapFont& font = mFontFactory->GetFont(HASH("cellphone", 0xAAA18E60 ));
		mScoreField->RenderText(s2, font);
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
	mScoreField = nullptr;
}
