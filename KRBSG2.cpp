#include "KRBSGLuaBinding.h"
#include "ScoreKeeper.h"
#include "Hud.h"
#include "PoolParty.h"
#include "SpriteFactory.h"
#include "UIViewFactory.h"
#include "CollisionDispatcher.h"
#include "Input.h"
#include "Audio.h"
#include "OpenGLUtils.h"
#undef max
#include "SpriteRenderer.h"
#include "ShaderFactory.h"
#include "TextureFactory.h"
#include "FontFactory.h"
#include "StaticTransform2D.h"
#include "HierarchicalTransform2D.h"
#include "MechanicalTransform2D.h"
#include "Collision2D.h"
#include "Tweening.h"
#include "LuaScheduler.h"
#include "Timer.h"
#include "DataTree.h"
#include "Psychopomp.h"
#include <iostream>
#include <limits>
#include <stdio.h>
#include <fstream>


#ifdef __EMSCRIPTEN__
#include <SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#else
#include "SDL.h"
#endif

/* Sets constants */
#define WIDTH 800
#define HEIGHT 600


struct KRBSG2
{
    SDL_Window* window = NULL;
    KEngineCore::Psychopomp         psychopomp;
    KEngineCore::LuaScheduler       luaScheduler;
    KEngineCore::Timer              timer;
    KEngine2D::HierarchyUpdater     hierarchySystem;
    KEngine2D::MechanicsUpdater     mechanicsSystem;


    KEngineOpenGL::SpriteRenderer   renderer;
    KEngineOpenGL::TextRenderer     textRenderer;
    KEngineOpenGL::ShaderFactory    shaderFactory;
    KEngineOpenGL::TextureFactory   textureFactory;
    KEngineOpenGL::FontFactory      fontFactory;

    KEngineBasics::UIViewFactory    uiFactory;
    SpriteFactory                   spriteFactory;
    KRBSGLuaBinding                 coreLuaBinding;
    KEngineBasics::Input            input;
    KEngineBasics::AudioSystem      audio;

    GameObjectFactory               gameObjectFactory;

    PoolParty                       poolParty;

    ScoreKeeper                     scoreKeeper;

    KRBSGCollisionDispatcher        collisionDispatcher;
    KEngine2D::CollisionSystem      collisionSystem;
    KEngineCore::ScheduledLuaThread mainThread;

    Hud                             hud;

    KEngineCore::TweenSystem        tweening;
    KEngine2D::TransformLibrary     transformLib;
    KEngineOpenGL::SpriteLibrary    spriteLib;
    

    KEngineCore::DataTree            dataRoot;
    bool                             loop;
    Uint32                           previousTime;
    
    void Init();
    void Update();
    void Deinit();
};


void KRBSG2::Init()
{
    /*
    * Initialises the SDL video subsystem (as well as the events subsystem).
    * Returns 0 on success or a negative error code on failure using SDL_GetError().
    */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        assert(false);
    }
    SDL_GL_LoadLibrary(NULL);
    
    // Request a platform appropriate OpenGL context
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#elif defined(__EMSCRIPTEN__)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
#endif
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
        assert(false);
    }

    auto context = SDL_GL_CreateContext(window);
    if (!context)
    {
        std::cerr << SDL_GetError() << std::endl;
    }

    loop = true;


    KEngineOpenGL::InitializeGlad(SDL_GL_GetProcAddress);
    
    std::ifstream dataStream("KRBSG.dat", std::ios::binary);
    if (!dataStream.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    dataRoot.ReadFromStream(dataStream);
    dataStream.close();

    psychopomp.Init();
    luaScheduler.Init();
    timer.Init(&luaScheduler);
    input.Init(&luaScheduler, &timer);
    audio.Init(&luaScheduler);
    audio.LoadMusic(HASH("gameMusic", 0xBAA31179), "magic_space.mp3");
    audio.LoadSound(HASH("pew", 0x7D5BCA3F), "laser1.wav");
    hierarchySystem.Init();
    mechanicsSystem.Init();
    renderer.Init(WIDTH, HEIGHT, 5); //TODO:  demagic this number?
    textRenderer.Init();
    shaderFactory.Init();
    textureFactory.Init();
    fontFactory.Init(&textureFactory, &shaderFactory);
    
    spriteFactory.Init(&shaderFactory, &textureFactory, &dataRoot);

    KEngineCore::DataTree* fontsData = dataRoot.GetBranch(HASH("sheetName", 0x7E99E530), HASH("Fonts", 0xB2C2C7FF));
    fontFactory.CreateFonts(fontsData);
    
    scoreKeeper.Init(0);

    collisionDispatcher.Init(&psychopomp, &scoreKeeper); 
    collisionSystem.Init(&collisionDispatcher);
    poolParty.Init();
    
	class LuaScheduler;
    tweening.Init(&luaScheduler);
    transformLib.Init(&luaScheduler, &tweening);
    spriteLib.Init(&luaScheduler, &tweening);
    uiFactory.Init(&shaderFactory, &fontFactory, &renderer, &textRenderer, &hierarchySystem, 4);
    gameObjectFactory.Init(&poolParty, &luaScheduler, &timer, &hierarchySystem, &mechanicsSystem, &shaderFactory, &renderer, &textRenderer, &fontFactory, &spriteFactory, &collisionSystem, &collisionDispatcher, &dataRoot);

    hud.Init(&gameObjectFactory, &scoreKeeper, &fontFactory, WIDTH, HEIGHT, &uiFactory);

    coreLuaBinding.Init(luaScheduler.GetMainState(), &luaScheduler, &transformLib, &spriteLib, &psychopomp, &gameObjectFactory, [this]() {loop = false; });

    mainThread.Init(&luaScheduler, "script.lua", true);
        
    input.AddCombinedAxis(HASH("primary", 0x1745EA86));

    input.AddChildAxis(HASH("primary", 0x1745EA86), HASH("primary.horizontal", 0x520B651B), KEngineBasics::AxisType::Horizontal, KEngineBasics::ControllerType::Joystick, 0);
    input.AddChildAxis(HASH("primary", 0x1745EA86), HASH("primary.vertical", 0x6ED1CEFB), KEngineBasics::AxisType::Vertical, KEngineBasics::ControllerType::Joystick, 1);

    input.AddChildAxis(HASH("primary", 0x1745EA86), HASH("primary.horizontal", 0x520B651B), KEngineBasics::AxisType::Horizontal, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_AXIS_LEFTX);
    input.AddChildAxis(HASH("primary", 0x1745EA86), HASH("primary.vertical", 0x6ED1CEFB), KEngineBasics::AxisType::Vertical, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_AXIS_LEFTY);

    input.AddAxisButton(HASH("primary.horizontal", 0x520B651B), HASH("primary.left", 0x174671DF), KEngineBasics::AxisType::Horizontal, -1, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    input.AddAxisButton(HASH("primary.horizontal", 0x520B651B), HASH("primary.right", 0xE1A272AD), KEngineBasics::AxisType::Horizontal, 1, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    input.AddAxisButton(HASH("primary.vertical", 0x6ED1CEFB), HASH("primary.down", 0x71DE068C), KEngineBasics::AxisType::Vertical, -1, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP);
    input.AddAxisButton(HASH("primary.vertical", 0x6ED1CEFB), HASH("primary.up", 0x8840872B), KEngineBasics::AxisType::Vertical, 1, KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN);

    input.AddAxisButton(HASH("primary.horizontal", 0x520B651B), HASH("primary.left", 0x174671DF), KEngineBasics::AxisType::Horizontal, -1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_A);
    input.AddAxisButton(HASH("primary.horizontal", 0x520B651B), HASH("primary.right", 0xE1A272AD), KEngineBasics::AxisType::Horizontal, 1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_D);
    input.AddAxisButton(HASH("primary.vertical", 0x6ED1CEFB), HASH("primary.down", 0x71DE068C), KEngineBasics::AxisType::Vertical, -1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_W);
    input.AddAxisButton(HASH("primary.vertical", 0x6ED1CEFB), HASH("primary.up", 0x8840872B), KEngineBasics::AxisType::Vertical, 1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_S);

    input.AddAxisButton(HASH("primary.horizontal", 0x520B651B), HASH("primary.left", 0x174671DF), KEngineBasics::AxisType::Horizontal, -1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_LEFT);
    input.AddAxisButton(HASH("primary.horizontal", 0x520B651B), HASH("primary.right", 0xE1A272AD), KEngineBasics::AxisType::Horizontal, 1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_RIGHT);
    input.AddAxisButton(HASH("primary.vertical", 0x6ED1CEFB), HASH("primary.down", 0x71DE068C), KEngineBasics::AxisType::Vertical, -1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_UP);
    input.AddAxisButton(HASH("primary.vertical", 0x6ED1CEFB), HASH("primary.up", 0x8840872B), KEngineBasics::AxisType::Vertical, 1, KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_DOWN);

    input.AddButton(HASH("fire", 0x58DE09CF), KEngineBasics::ControllerType::Joystick, 0);
    input.AddButton(HASH("fire", 0x58DE09CF), KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_A);
    input.AddButton(HASH("fire", 0x58DE09CF), KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_SPACE);

    input.AddButton(HASH("pause", 0xD79A92ED), KEngineBasics::ControllerType::Gamepad, SDL_CONTROLLER_BUTTON_START);
    input.AddButton(HASH("pause", 0xD79A92ED), KEngineBasics::ControllerType::Keyboard, SDL_SCANCODE_ESCAPE);

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
    previousTime = SDL_GetTicks();
}

void KRBSG2::Update()
{
    Uint32 currentTime = SDL_GetTicks();

    Uint32 elapsedTime = currentTime - previousTime;
    previousTime = currentTime;
    double elapsedTimeInSeconds = elapsedTime / 1000.0f;
    timer.Update(elapsedTimeInSeconds);
    luaScheduler.Update();
    hierarchySystem.Update(elapsedTimeInSeconds);
    mechanicsSystem.Update(elapsedTimeInSeconds);
    collisionSystem.Update();
    tweening.Update(elapsedTimeInSeconds);
    psychopomp.Update();
    renderer.Render();
    SDL_GL_SwapWindow(window);
    
    SDL_Event event;
    static float maxShort = std::numeric_limits<int16_t>::max();
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
        case SDL_CONTROLLERDEVICEADDED:
            SDL_GameControllerOpen(event.cdevice.which);
            break;
        case SDL_JOYDEVICEADDED:
            SDL_JoystickOpen(event.jdevice.which);
            break;
        case SDL_QUIT:
            loop = false;
            break;
        }
    }
}

void KRBSG2::Deinit()
{
    hud.Deinit();
    mainThread.Deinit();
    spriteLib.Deinit();
    transformLib.Deinit();
    tweening.Deinit();
    gameObjectFactory.Deinit();
    renderer.Deinit();
    hierarchySystem.Deinit();
    mechanicsSystem.Deinit();
    coreLuaBinding.Deinit();
    luaScheduler.Deinit();
    audio.Deinit();
    input.Deinit();
    timer.Deinit();
    collisionSystem.Deinit();
    poolParty.Deinit();
    psychopomp.Deinit(); //With strange aeons

    /* Frees memory */
    SDL_DestroyWindow(window);

    /* Shuts down all SDL subsystems */
    SDL_Quit();
}

//Ugh, global?
KRBSG2 krbsg2;

#ifdef __EMSCRIPTEN__
void mainLoop() {
    if (krbsg2.loop)
    {
        krbsg2.Update();
    }
    if (!krbsg2.loop)
    {
        krbsg2.Deinit();
    }
}
#endif

int main(int argc, char** argv)
{
    
#ifdef __EMSCRIPTEN__
   // emscripten_set_mousedown_callback("canvas", nullptr, true,
   //     [](int, const EmscriptenMouseEvent* e, void*)->EM_BOOL {
            if (!krbsg2.loop)
            {
                krbsg2.Init();
        
                emscripten_set_main_loop(mainLoop, 0, 0);
            }
            return true;
 //       });
#else
    
    krbsg2.Init();
    
	while (krbsg2.loop) {
        krbsg2.Update();
	}

    krbsg2.Deinit();
#endif
	return 0;
}
