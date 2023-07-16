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

set(vertexShaderSources ${vertexShaderSources}
    basic.vert.glsl
)

set(fragmentShaderSources ${fragmentShaderSources}
    textured.frag.glsl
)

source_group(shaderSource FILES ${vertexShaderSources} ${fragmentShaderSources})

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten" OR ${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(glsl_version_string "300 es")
else()
    set(glsl_version_string "330")
endif()

foreach(shader IN LISTS vertexShaderSources fragmentShaderSources)
    configure_file(${shader} ${shader})
    set(shaders ${shaders} ${CMAKE_CURRENT_BINARY_DIR}/${shader})
endforeach()

source_group(Resources/shaders FILES ${shaders})

set(dataFiles ${dataFiles}
    KRBSG.dat
)

source_group(Resources/data ${dataFiles})

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

source_group(Resources/textures FILES ${textureFiles})

set(luaScripts  ${luaScripts}
    krbsg2.lua
    game.lua
    playerShip.lua
    enemyShip.lua
    blaster.lua
    blaster_bolt.lua
    Flyoff.lua
)

source_group(Resources/scripts FILES ${luaScripts})

set(soundFiles ${soundFiles}
    laser1.wav
    magic_space.mp3
)

source_group(Resources/audio FILES ${soundFiles})

set(directCopyFiles
    ${directCopyFiles}
    ${dataFiles}
    ${textureFiles}
    ${luaScripts}
    ${soundFiles}
)

set(resourceFiles
    ${dataFiles}
    ${vertexShaders}
    ${fragmentShaders}
    ${textureFiles}
    ${luaScripts}
    ${soundFiles}
    ${shaders}
)

set(codeFiles
    KRBSG2.h
    KRBSG2.cpp
    KRBSG2Constants.h
    KRBSGLuaBinding.h
    KRBSGLuaBinding.cpp
    CollisionDispatcher.h
    CollisionDispatcher.cpp
    GameplaySession.h
    GameplaySession.cpp
    SpriteFactory.h
    SpriteFactory.cpp
    PauseMenu.h
    PauseMenu.cpp
    PoolParty.h
    PoolParty.cpp
    GameObjectFactory.h
    GameObjectFactory.cpp
    ScoreKeeper.h
    ScoreKeeper.cpp
    Hud.h
    Hud.cpp
    ${vertexShaderSources}
    ${fragmentShaderSources}
    ${allFiles}
)

set(allFiles
    ${codeFiles}
    ${resourceFiles}
)

if (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    add_executable(KRBSG2 ${allFiles})
    target_compile_definitions(KRBSG2 PUBLIC KRBSG_EXECUTABLE)
    
    set_target_properties(KRBSG2 PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/plist.in"
        XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER com.kelson.KRBSG2
        RESOURCE "${resourceFiles}")
        
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.kelson.KRBSG2")
else()
    add_executable(KRBSG2 ${allFiles})
    target_compile_definitions(KRBSG2 PUBLIC KRBSG_EXECUTABLE)
endif()

target_compile_features(KRBSG2 PRIVATE cxx_std_20)

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
    foreach(file IN LISTS resourceFiles)
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
