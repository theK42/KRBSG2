cmake_minimum_required(VERSION 3.20)

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

set(dataFiles ${dataFiles} 
    KRBSG.dat
)

set(vertexShaders ${vertexShaders}
    basic.vert.glsl
)

set(fragmentShaders ${fragmentShaders}
    textured.frag.glsl
)

set(textureFiles ${textureFiles}
    ship.png
    blaster.png
    charmap-cellphone_white.png
    galaxy.png
    galaxy3.png
    nebula.png
    star1.png
    star2.png
    star3.png
    star4.png
    star5.png
    star6.png
    star7.png
)

set(luaScripts ${luaScripts}
    script.lua
    playerShip.lua
    enemyShip.lua
    blaster.lua
    blaster_bolt.lua
    Flyoff.lua
)

set(soundFiles ${soundFiles}
    laser1.wav
    magic_space.mp3
)

set(codeLikeFiles ${codeLikeFiles}
    ${luaScripts}
    ${vertexShaders}
    ${fragmentShaders}
)

set(directCopyFiles
    ${directCopyFiles}
    ${dataFiles}
    ${textureFiles}
    ${luaScripts}
    ${soundFiles}
)

set(allFiles
    ${dataFiles}
    ${vertexShaders}
    ${fragmentShaders}
    ${textureFiles}
    ${luaScripts}
    ${soundFiles}
)


add_executable(KRBSG2 
KRBSG2.cpp
KRBSG2Constants.h
KRBSGLuaBinding.h
KRBSGLuaBinding.cpp
CollisionDispatcher.h
CollisionDispatcher.cpp
SpriteFactory.h
SpriteFactory.cpp
PoolParty.h
PoolParty.cpp
GameObjectFactory.h
GameObjectFactory.cpp
ScoreKeeper.h
ScoreKeeper.cpp
Hud.h
Hud.cpp
${codeLikeFiles}
)

target_compile_features(KRBSG2 PRIVATE cxx_std_20) 

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(glsl_version_string "300 es")
else()
    set(glsl_version_string "330")
endif()

foreach(shader IN LISTS vertexShaders fragmentShaders)
    configure_file(${shader} ${shader})
endforeach()

foreach(file IN LISTS directCopyFiles)
    configure_file(${file} ${file} COPYONLY)
endforeach()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT KRBSG2)
target_link_libraries(KRBSG2 PRIVATE KEngineCore KEngine2D KEngineBasics KEngineOpenGL)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	target_link_libraries(KRBSG2 PRIVATE "-framework CoreFoundation")
endif()

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(USE_FLAGS "-s USE_MPG123=1 -s USE_SDL=2 -s USE_SDL_MIXER=2 -s SDL2_MIXER_FORMATS=[\"mp3\"]")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")

    list(APPEND USE_FLAGS "-s USE_WEBGL2=1")
    list(APPEND USE_FLAGS "--use-preload-plugins")
    #list(APPEND USE_FLAGS "-gsource-map")
    foreach(file IN LISTS allFiles)
        list(APPEND USE_FLAGS "--preload-file ${file}")
    endforeach()
    list(JOIN USE_FLAGS " " USE_FLAGS)

    message(STATUS "USE_FLAGS is ${USE_FLAGS}")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

set_target_properties(KRBSG2 PROPERTIES 
                      XCODE_GENERATE_SCHEME TRUE
                      XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_custom_command(TARGET KRBSG2 POST_BUILD
  		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:KRBSG2> $<TARGET_FILE_DIR:KRBSG2>
  		COMMAND_EXPAND_LISTS
	)
endif()