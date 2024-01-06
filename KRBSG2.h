#pragma once
#include "KRBSGLuaBinding.h"
#include "PoolParty.h"
#include "SpriteFactory.h"
#include "UIViewFactory.h"
#include "SpriteRenderer.h"
#include "ShaderFactory.h"
#include "TextureFactory.h"
#include "FontFactory.h"
#include "StaticTransform2D.h"
#include "HierarchicalTransform2D.h"
#include "Tweening.h"
#include "LuaScheduler.h"
#include "Timer.h"
#include "DataTree.h"
#include "Psychopomp.h"
#include "Input.h"
#include "Audio.h"
#include "LuaContext.h"
#include "Logger.h"
#if defined(__ANDROID__)
    #include "AndroidLogger.h"
#else
    #include "StdLogger.h"
#endif

class SDL_Window;

struct KRBSG2
{
    SDL_Window* window = NULL;
    
    KEngineCore::Logger             logger;
#if defined(__ANDROID__)
    KEngineBasics::AndroidLogger    androidLogger;
#else
    KEngineBasics::StdLogger        standardLogger;
#endif

    KEngineCore::TimeLibrary        timeLib;
    KEngineBasics::InputLibrary     inputLib;

    KEngineCore::Psychopomp         psychopomp;
    KEngineCore::LuaScheduler       luaScheduler;
    KEngineCore::LuaContext         scriptRunner;
    KEngineCore::Timer              timer;
    KEngine2D::HierarchyUpdater     hierarchySystem;

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

    PoolParty                       poolParty;

    KEngine2D::CollisionSystem      collisionSystem;
    KEngineCore::ScheduledLuaThread mainThread;

    KEngineCore::TweenSystem        tweening;
    KEngine2D::TransformLibrary     transformLib;
    KEngineOpenGL::SpriteLibrary    spriteLib;


    KEngineCore::DataTree            dataRoot;
    bool                             loop;
    Uint32                           previousTime;
    
    int width;
    int height;

    void Init();
    void Update();
    void Deinit();

};
