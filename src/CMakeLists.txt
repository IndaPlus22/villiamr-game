include(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2_IMAGE REQUIRED SDL2_image)

set(SOURCES main.cpp graphicsBase.cpp position.cpp types.cpp)
set(HEADERS graphicsBase.hpp types.hpp position.hpp)
add_executable( ${PROJECT_NAME} ${SOURCES} ${HEADERS})


target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} )
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

