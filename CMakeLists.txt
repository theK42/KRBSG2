cmake_minimum_required(VERSION 3.10)

#set the project name
project(KRBSG2)

function(add_submodule submodule)
    message("adding submodule ${submodule}")
    cmake_path(ABSOLUTE_PATH submodule OUTPUT_VARIABLE absolute_submodule)
    if (NOT EXISTS ${absolute_submodule})
        message("submodule ${submodule} missing, attempting update")

	set (git_cmd "git")
	#set (git_arg "submodule update --init --recursive -- ${submodule}")
	set (git_arg "submodule update")
	execute_process(COMMAND ${git_cmd} submodule update --init --recursive -- ${submodule}
  			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  			RESULT_VARIABLE git_result
  			OUTPUT_VARIABLE git_out)
	
	message(STATUS "git[${git_result}]: ${git_out}")
    endif()
    add_subdirectory(${submodule})
endfunction()

add_submodule(KEngineCore)
add_submodule(KEngine2D)
add_submodule(KEngineOpenGL)
add_submodule(KEngineBasics)

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
Weapon.h
Weapon.cpp
Attack.h
Attack.cpp
ProjectileFactory.h
ProjectileFactory.cpp
PoolParty.h
PoolParty.cpp
script.lua
playerShip.lua
enemyShip.lua
blaster.lua
basic.vert.glsl
textured.frag.glsl
ship.png
blaster.png
)

target_compile_features(KRBSG2 PRIVATE cxx_std_20) 
FIND_PACKAGE(SDL2)
IF (${SDL2_FOUND})
	MESSAGE(STATUS "Found SDL.")
	set(SDL_SHARED ON)
	set(SDL_STATIC OFF)	
	target_link_libraries(KRBSG2 PRIVATE SDL2 SDL2main)
ELSE (${SDL2_FOUND})
	MESSAGE(STATUS "Could not locate SDL.")
	add_submodule(SDL)
	set(SDL_SHARED OFF)
	set(SDL_STATIC ON)	
	target_link_libraries(KRBSG2 PRIVATE SDL2-static SDL2main)
ENDIF (${SDL2_FOUND})

configure_file(script.lua script.lua COPYONLY)
configure_file(playerShip.lua playerShip.lua COPYONLY)
configure_file(enemyShip.lua enemyShip.lua COPYONLY)
configure_file(blaster.lua blaster.lua COPYONLY)
configure_file(basic.vert.glsl basic.vert.glsl COPYONLY)
configure_file(textured.frag.glsl textured.frag.glsl COPYONLY)
configure_file(ship.png ship.png COPYONLY)
configure_file(blaster.png blaster.png COPYONLY)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT KRBSG2)
target_link_libraries(KRBSG2 PRIVATE KEngineCore KEngine2D KEngineBasics KEngineOpenGL SDL2-static SDL2main)