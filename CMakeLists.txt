cmake_minimum_required(VERSION 3.16)

project(VisionBox VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Qt6, modules nécessaires ---
find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    LinguistTools
    Multimedia
    MultimediaWidgets
)

set(TS_FILES VisionBox_fr_FR.ts)

# --- Tous les fichiers du projet ---
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    webcamdialog.cpp
    webcamdialog.h
    clickablelabel.h
    resources.qrc
    ${TS_FILES}
)

# --- Création de l'exécutable ---
qt_add_executable(VisionBox
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    traitement_image.h
    boite_traitement.h
    boite_traitement.cpp
    panneau_flux_traitements.h
    panneau_flux_traitements.cpp
    traitement_flou.h
    traitement_flou.cpp


)

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

# --- Liaisons Qt ---
target_link_libraries(VisionBox
    PRIVATE
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::MultimediaWidgets
)

# --- Propriétés du binaire ---
set_target_properties(VisionBox PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS VisionBox
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(VisionBox)
