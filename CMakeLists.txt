cmake_minimum_required(VERSION 3.31.5)
project(SudokuGame)

set(CMAKE_CXX_STANDARD 17)

# Sử dụng FetchContent để tải SDL2 và SDL2_ttf
include(FetchContent)

# Tải SDL2
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.28.5
)
FetchContent_MakeAvailable(SDL2)

# Tải SDL2_ttf
FetchContent_Declare(
    SDL2_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-2.22.0
)
FetchContent_MakeAvailable(SDL2_ttf)

# Tải SDL2_mixer
FetchContent_Declare(
    SDL2_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG release-2.6.3
)
FetchContent_MakeAvailable(SDL2_mixer)

# Tải SDL2_image
FetchContent_Declare(
    SDL2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-2.8.2
)
FetchContent_MakeAvailable(SDL2_image)

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Board.cpp
    src/Timer.cpp
    src/Storage.cpp
    src/Utils.cpp
    src/Button.cpp
)

add_executable(SudokuGame ${SOURCES})

target_link_libraries(SudokuGame
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    SDL2::SDL2main
    SDL2_mixer::SDL2_mixer
    SDL2_image::SDL2_image
)

target_include_directories(SudokuGame PRIVATE
    ${SDL2_SOURCE_DIR}/include
    ${SDL2_TTF_SOURCE_DIR}/include
    ${SDL2_MIXER_SOURCE_DIR}/include
    ${SDL2_IMAGE_SOURCE_DIR}/include
)

add_custom_command(TARGET SudokuGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:SudokuGame>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/assets/font.ttf
    ${CMAKE_SOURCE_DIR}/assets/sound_game.wav
    ${CMAKE_SOURCE_DIR}/assets/victory_sound.wav
    ${CMAKE_SOURCE_DIR}/assets/saved_game.txt
    ${CMAKE_SOURCE_DIR}/assets/mute.jpg
    $<TARGET_FILE_DIR:SudokuGame>/assets/
)

if (WIN32)
    add_custom_command(TARGET SudokuGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE_DIR:SDL2::SDL2>/SDL2.dll
        $<TARGET_FILE_DIR:SudokuGame>
    )
    add_custom_command(TARGET SudokuGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE_DIR:SDL2_ttf::SDL2_ttf>/SDL2_ttf.dll
        $<TARGET_FILE_DIR:SudokuGame>
    )
    add_custom_command(TARGET SudokuGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE_DIR:SDL2_mixer::SDL2_mixer>/SDL2_mixer.dll
        $<TARGET_FILE_DIR:SudokuGame>
    )
    add_custom_command(TARGET SudokuGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE_DIR:SDL2_image::SDL2_image>/SDL2_image.dll
        $<TARGET_FILE_DIR:SudokuGame>
    )
endif()