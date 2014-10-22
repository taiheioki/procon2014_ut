set(QT_SLIDE_PROJECT "procon2014-qt-slide")

set(QT_SLIDE_SOURCE_FILES
    src/slidemain.cpp
    src/mainslidewindow.cpp
    src/graphicsitemslideblock.cpp
    src/graphicsitemslideboard.cpp
    src/getproblemworker.cpp
    src/postproblemworker.cpp
    include/mainslidewindow.hpp
    include/graphicsitemslideblock.hpp
    include/graphicsitemslideboard.hpp
    include/getproblemworker.hpp
    include/postproblemworker.hpp
)

set(SLIDE_UI_FILES
    src/mainslidewindow.ui
)

include_directories(include)

qt5_wrap_ui(SLIDE_UI_HEADERS ${SLIDE_UI_FILES})
source_group("Generated UI Headers" FILES ${SLIDE_UI_HEADERS})

# Qt executable
add_executable(${QT_SLIDE_PROJECT} MACOSX_BUNDLE ${QT_SLIDE_SOURCE_FILES} ${SLIDE_UI_HEADERS} ${SLIDE_UI_FILES})
target_link_libraries(${QT_SLIDE_PROJECT} ${Boost_LIBRARIES} ${OpenCV_LIBRARIES} slide network)
qt5_use_modules(${QT_SLIDE_PROJECT} Widgets Concurrent)
