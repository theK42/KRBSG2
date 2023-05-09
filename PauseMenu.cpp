#include "PauseMenu.h"
#include "TextRenderer.h"
#include "StaticTransform2D.h"
#include "FontFactory.h"
#include "UIView.h"
#include "UITextView.h"
#include <assert.h>

#include "UIViewFactory.h"

PauseMenu::PauseMenu()
{
}

PauseMenu::~PauseMenu()
{
	Deinit();
}

void PauseMenu::Init(KEngineOpenGL::FontFactory* fontFactory, int width, int height, KEngineBasics::UIViewFactory* uiFactory, KEngineCore::LuaContext* parentContext)
{
	assert(mParentContext == nullptr);
	mDisposables.Init();
	mParentContext = parentContext;

	auto* fullScreenRight = uiFactory->CreateStaticLayoutGuide(&mDisposables, 800);
	auto* fullScreenBottom = uiFactory->CreateStaticLayoutGuide(&mDisposables, 600);
	auto* fullScreenOrigin = uiFactory->CreateStaticLayoutGuide(&mDisposables, 0);

	KEngineBasics::UIView* hudView = uiFactory->CreateUIView(&mDisposables, nullptr, fullScreenOrigin, fullScreenOrigin, fullScreenRight, fullScreenBottom);

	auto* verticalCenter = uiFactory->CreateParentRelativeLayoutGuide(&mDisposables, KEngineBasics::Height, 0.5, 0);
	auto* horizontalCenter = uiFactory->CreateParentRelativeLayoutGuide(&mDisposables, KEngineBasics::Width, 0.5, 0);
	auto* leftCentered = uiFactory->CreateContentRelativeLayoutGuide(&mDisposables, horizontalCenter, KEngineBasics::Width, -0.5, 0);
	auto* topCentered = uiFactory->CreateContentRelativeLayoutGuide(&mDisposables, verticalCenter, KEngineBasics::Height, -0.5, 0);
	auto* rightCentered = uiFactory->CreateContentRelativeLayoutGuide(&mDisposables, horizontalCenter, KEngineBasics::Width, 0.5, 0);
	auto* bottomCentered = uiFactory->CreateContentRelativeLayoutGuide(&mDisposables, verticalCenter, KEngineBasics::Height, 0.5, 0);
	
	uiFactory->CreateUITextView(&mDisposables, hudView, leftCentered, topCentered, rightCentered, bottomCentered, HASH("cellphone", 0xAAA18E60), HASH("Textured", 0xDF225E87), "Paused");

	/*
		//PAUSE MENU

		UILayout* origin = new UIStaticLayout(0);
		UILayout* fullScreenRight = new UIStaticLayout(800);
		UILayout* fullScreenBottom = new UIStaticLayout(600);

		KEngineBasics::UIView* pauseView = new UIView(); //TODO:  Pool or something
		hudView->Init(nullptr, origin, origin, fullScreenRight, fullScreenBottom);

		UILayout* centeredHorizontal = new UIParentRelativeLayout(Width, 0.5, 0);
		UILayout* centeredContentLeft = new UIContentRelativeLayout(centeredHorizontal, Width, -0.5, 0);
		UILayout* centeredContentRight = new UIContentRelativeLayout(centeredHorizontal, Width, 0.5, 0);

		UILayout* centeredVertical = new UIParentRelativeLayout(Height, 0.5, 0);
		UILayout* centeredContentTop = new UIContentRelativeLayout(centeredVertical, Height, -0.5, 0);
		UILayout* centeredContentBottom = new UIContentRelativeLayout(centeredVertical, Height, 0.5, 0);

		UIView* menuView = new UIView();
		menuView->Init(pauseView, centeredContentLeft, centeredContentTop, centeredContentRight, centeredContentBottom);

		UIButton* resumeButtonView = new UIButton();
		resumeButtonView->Init(menuView, centeredContentLeft, topMargin, centeredContentRight, bottomContent, [this]() {
			Resume();
		});
		UITextView* resumeText = new UITextView();
		resumeText->Init(resumeButtonView, font, shader, "Resume", centeredContentLeft, centeredContentTop, centeredContentRight, centeredContentBottom);


		UILayout* siblingTopMargin = new UISiblingRelativeLayout(Height, 1.0, 40);

		UIButton* optionsButtonView = new UIButton();
		optionsButtonView->Init(menuView, centeredContentLeft, siblingTopMargin, centeredContentRight, bottomContent, [this]() {
			Options();
		});
		UITextView* optionsText = new UITextView();
		optionsText->Init(resumeButtonView, font, shader, "Resume", centeredContentLeft, centeredContentTop, centeredContentRight, centeredContentBottom);

		UIButton* quitButton = new UIButton();

		quitButton->Init(menuView, centeredContentLeft, siblingTopMargin, centeredContentRight, bottomContent, [this]() {
			Quit();
		});

		//OPTIONS MENU
		*/

}

void PauseMenu::Deinit()
{
	mDisposables.Deinit();
}
