cmake_minimum_required(VERSION 3.10)

#set the project name
project(KRBSG2)

add_subdirectory(KEngineCore)
add_subdirectory(KEngine2D)
add_subdirectory(KEngineOpenGL)
add_subdirectory(KEngineBasics)

add_subdirectory(SDL)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)

add_executable(KRBSG2 
KRBSG2.cpp
KRBSGLuaBinding.h
KRBSGLuaBinding.cpp
SpriteFactory.h
SpriteFactory.cpp
PlayerShip.h
PlayerShip.cpp
EnemyShip.h
EnemyShip.cpp
script.lua
playerShip.lua
enemyShip.lua
basic.vert.glsl
textured.frag.glsl
ship.png
)

target_compile_features(KRBSG2 PRIVATE cxx_std_20) 

configure_file(script.lua script.lua COPYONLY)
configure_file(playerShip.lua playerShip.lua COPYONLY)
configure_file(enemyShip.lua enemyShip.lua COPYONLY)
configure_file(basic.vert.glsl basic.vert.glsl COPYONLY)
configure_file(textured.frag.glsl textured.frag.glsl COPYONLY)
configure_file(ship.png ship.png COPYONLY)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT KRBSG2)
target_link_libraries(KRBSG2 PRIVATE KEngineCore KEngine2D KEngineBasics KEngineOpenGL SDL2-static SDL2main)