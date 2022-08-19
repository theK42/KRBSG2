
#include "PlayerShip.h"
#include "EnemyShip.h"
#include "SpriteFactory.h"
#include "SDL.h"
#include "Input.h"
#include "KRBSGLuaBinding.h"
#include "OpenGLUtils.h"
#undef max
#include "SpriteRenderer.h"
#include "ShaderFactory.h"
#include "TextureFactory.h"
#include "StaticTransform2D.h"
#include "HierarchicalTransform2D.h"
#include "LuaScheduler.h"
#include "Timer.h"
#include <iostream>
#include <limits>
#include <stdio.h>

/* Sets constants */
#define WIDTH 800
#define HEIGHT 600

int main(int argc, char** argv)
{
	/* Initialises data */
	SDL_Window* window = NULL;

	/*
	* Initialises the SDL video subsystem (as well as the events subsystem).
	* Returns 0 on success or a negative error code on failure using SDL_GetError().
	*/
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0) {
		fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
		return 1;
	}
	SDL_GL_LoadLibrary(NULL);
	
	// Request an OpenGL 4.5 context (should be core)
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	/* Creates a SDL window */
	window = SDL_CreateWindow("KRBGSG", /* Title of the SDL window */
		SDL_WINDOWPOS_UNDEFINED, /* Position x of the window */
		SDL_WINDOWPOS_UNDEFINED, /* Position y of the window */
		WIDTH, /* Width of the window in pixels */
		HEIGHT, /* Height of the window in pixels */
		SDL_WINDOW_OPENGL); /* Additional flag(s) */
	
	/* Checks if window has been created; if not, exits program */
	if (window == NULL) {
		fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
		return 1;
	}

	auto context = SDL_GL_CreateContext(window);
	if (!context)
	{
		std::cerr << SDL_GetError() << std::endl;
	}

	bool loop = true;


	KEngineOpenGL::InitializeGlad(SDL_GL_GetProcAddress);

	KEngineCore::LuaScheduler       luaScheduler;
	KEngineCore::Timer              timer;
	KEngine2D::HierarchyUpdater     hierarchySystem;


	KEngineOpenGL::SpriteRenderer   renderer;
	KEngineOpenGL::ShaderFactory    shaderFactory;
	KEngineOpenGL::TextureFactory	textureFactory;
	SpriteFactory					spriteFactory;
	KRBSGLuaBinding					coreLuaBinding;
	KEngineBasics::Input			input;

	PlayerShipSystem				playerShipSystem;
	EnemyShipSystem					enemyShipSystem;

	KEngineCore::ScheduledLuaThread mainThread;

	luaScheduler.Init();
	timer.Init(&luaScheduler);
	input.Init(&luaScheduler, &timer);
	hierarchySystem.Init();
	renderer.Init(WIDTH, HEIGHT);
	shaderFactory.Init();
	textureFactory.Init();
	spriteFactory.Init(&shaderFactory, &textureFactory);
	coreLuaBinding.Init(luaScheduler.GetMainState(), &luaScheduler, &hierarchySystem, &renderer, &spriteFactory, [&loop]() {loop = false; });
	playerShipSystem.Init(&luaScheduler, &hierarchySystem, &renderer, &spriteFactory);
	enemyShipSystem.Init(&luaScheduler, &hierarchySystem, &renderer, &spriteFactory);
	mainThread.Init(&luaScheduler, "script.lua", true);


	input.AddCombinedAxis("primary");

	input.AddChildAxis("primary", "primary.horizontal", KEngineBasics::AxisType::Horizontal, KEngineBasics::ControllerType::Joystick, 0);
	input.AddChildAxis("primary", "primary.vertical", KEngineBasics::AxisType::Vertical, KEngineBasics::ControllerType::Joystick, 1);

	input.AddChildAxis("primary", "primary.horizontal", KEngineBasics::AxisType::Horizontal, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_AXIS_LEFTX);
	input.AddChildAxis("primary", "primary.vertical", KEngineBasics::AxisType::Vertical, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_AXIS_LEFTY);

	input.AddAxisButton("primary.horizontal", "primary.left", KEngineBasics::AxisType::Horizontal, -1, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	input.AddAxisButton("primary.horizontal", "primary.right", KEngineBasics::AxisType::Horizontal, 1, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	input.AddAxisButton("primary.vertical", "primary.down", KEngineBasics::AxisType::Vertical, -1, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP);
	input.AddAxisButton("primary.vertical", "primary.up", KEngineBasics::AxisType::Vertical, 1, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN);

	input.AddAxisButton("primary.horizontal", "primary.left", KEngineBasics::AxisType::Horizontal, -1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_A);
	input.AddAxisButton("primary.horizontal", "primary.right", KEngineBasics::AxisType::Horizontal, 1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_D);
	input.AddAxisButton("primary.vertical", "primary.down", KEngineBasics::AxisType::Vertical, -1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_W);
	input.AddAxisButton("primary.vertical", "primary.up", KEngineBasics::AxisType::Vertical, 1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_S);

	input.AddAxisButton("primary.horizontal", "primary.left", KEngineBasics::AxisType::Horizontal, -1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_LEFT);
	input.AddAxisButton("primary.horizontal", "primary.right", KEngineBasics::AxisType::Horizontal, 1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_RIGHT);
	input.AddAxisButton("primary.vertical", "primary.down", KEngineBasics::AxisType::Vertical, -1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_UP);
	input.AddAxisButton("primary.vertical", "primary.up", KEngineBasics::AxisType::Vertical, 1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_DOWN);

	input.AddButton("fire", KEngineBasics::ControllerType::Joystick, 0); 
	input.AddButton("fire", KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_A);
	input.AddButton("fire", KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_SPACE);

	input.AddButton("pause", KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_START);
	input.AddButton("pause", KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_ESCAPE);

	SDL_GameController* controller = NULL;
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			controller = SDL_GameControllerOpen(i);
			if (!controller) {
				fprintf(stderr, "Could not open gamecontroller %i: %s\n", i, SDL_GetError());
			}
		}
		else
		{
			SDL_JoystickOpen(i);
		}
	}

	SDL_Event event;
	Uint32 previousTime = SDL_GetTicks();
	float maxShort = std::numeric_limits<int16_t>::max();

	while (loop) {

		Uint32 currentTime = SDL_GetTicks();

		Uint32 elapsedTime = currentTime - previousTime;
		previousTime = currentTime;
		double elapsedTimeInSeconds = elapsedTime / 1000.0f;
		timer.Update(elapsedTimeInSeconds);
		luaScheduler.Update();
		hierarchySystem.Update(elapsedTimeInSeconds);
		renderer.Render();
		SDL_GL_SwapWindow(window);

		/*check if events are being processed*/
		while (SDL_PollEvent(&event)) {
			/*check if event type is keyboard press*/
			switch (event.type)
			{
			case SDL_CONTROLLERAXISMOTION:
				//TODO:  handle event.caxis.which 
				input.HandleAxisChange(KEngineBasics::ControllerType::Gamepad, event.caxis.axis, event.caxis.value / maxShort);
				break;
			case SDL_JOYAXISMOTION:
				input.HandleAxisChange(KEngineBasics::ControllerType::Joystick, event.jaxis.axis, event.jaxis.value / maxShort);
				break;
			case SDL_KEYDOWN:
				input.HandleButtonDown(KEngineBasics::ControllerType::Keyboard, event.key.keysym.scancode);
				break;
			case SDL_KEYUP:
				input.HandleButtonUp(KEngineBasics::ControllerType::Keyboard, event.key.keysym.scancode);
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				input.HandleButtonDown(KEngineBasics::ControllerType::Gamepad, event.cbutton.button);
				break;
			case SDL_CONTROLLERBUTTONUP:
				input.HandleButtonUp(KEngineBasics::ControllerType::Gamepad, event.cbutton.button);
				break;
			case SDL_JOYBUTTONDOWN:
				input.HandleButtonDown(KEngineBasics::ControllerType::Joystick, event.jbutton.button);
				break;
			case SDL_JOYBUTTONUP:
				input.HandleButtonUp(KEngineBasics::ControllerType::Joystick, event.jbutton.button);
				break;
			case SDL_QUIT:
				loop = false;
				break;
			}		
		}
	}

	mainThread.Deinit();
	enemyShipSystem.Deinit();
	playerShipSystem.Deinit();
	renderer.Deinit();
	hierarchySystem.Deinit();
	coreLuaBinding.Deinit();
	luaScheduler.Deinit();
	input.Deinit();
	timer.Deinit();

	/* Frees memory */
	SDL_DestroyWindow(window);

	/* Shuts down all SDL subsystems */
	SDL_Quit();

	return 0;
}