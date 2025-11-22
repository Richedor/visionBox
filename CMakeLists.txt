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

# --- OpenCV ---
# OpenCV_DIR est déjà défini dans Qt Creator : D:/opencv/build/x64/vc16/lib
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV Include Dir: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV Libraries  : ${OpenCV_LIBS}")

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


    boite_traitement.h
    boite_traitement.cpp
    panneau_flux_traitements.h
    panneau_flux_traitements.cpp

    resources.qrc
    ${TS_FILES}
)

# --- Création de l'exécutable ---
qt_add_executable(VisionBox
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    traitements/core/flou.h
    traitements/core/flou.cpp
    traitements/core/mosaique.h

    traitements/core/contours.h
    traitements/core/contours.cpp
    traitements/core/mosaique.cpp
    traitements/adaptateurs_qt/adaptateur_flou.h
    traitements/adaptateurs_qt/adaptateur_flou.cpp
    traitements/adaptateurs_qt/adaptateur_mosaique.h
    traitements/adaptateurs_qt/adaptateur_mosaique.cpp
    traitements/adaptateurs_qt/adaptateur_contours.h
    traitements/adaptateurs_qt/adaptateur_contours.cpp
    traitements/adaptateurs_qt/traitement_image.h
    traitements/traitement_factory.h
)

# --- Traductions ---
qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

# --- Inclusions OpenCV ---
target_include_directories(VisionBox
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
)

# --- Liaisons Qt + OpenCV ---
target_link_libraries(VisionBox
    PRIVATE
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::MultimediaWidgets
        ${OpenCV_LIBS}
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
