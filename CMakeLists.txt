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
laser1.wav
magic_space.mp3
KRBSG.dat
)

add_custom_command(TARGET KRBSG2 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:KRBSG2> $<TARGET_FILE_DIR:KRBSG2>
  COMMAND_EXPAND_LISTS
)

target_compile_features(KRBSG2 PRIVATE cxx_std_20) 
configure_file(script.lua script.lua COPYONLY)
configure_file(playerShip.lua playerShip.lua COPYONLY)
configure_file(enemyShip.lua enemyShip.lua COPYONLY)
configure_file(blaster.lua blaster.lua COPYONLY)
configure_file(basic.vert.glsl basic.vert.glsl COPYONLY)
configure_file(textured.frag.glsl textured.frag.glsl COPYONLY)
configure_file(ship.png ship.png COPYONLY)
configure_file(blaster.png blaster.png COPYONLY)
configure_file(laser1.wav laser1.wav COPYONLY)
configure_file(magic_space.mp3 magic_space.mp3 COPYONLY)
configure_file(KRBSG.dat KRBSG.dat COPYONLY)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT KRBSG2)
target_link_libraries(KRBSG2 PRIVATE KEngineCore KEngine2D KEngineBasics KEngineOpenGL SDL2main)

