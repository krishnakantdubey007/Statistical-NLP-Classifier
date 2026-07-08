cmake_minimum_required(VERSION 3.20)

project(StatisticalNLPClassifier
    VERSION 2.0
    DESCRIPTION "Statistical NLP Text Classifier Engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
include_directories(include)
include_directories(gui)
# -------------------------------------------------
# Find Raylib
# -------------------------------------------------
find_package(raylib REQUIRED)

# -------------------------------------------------
# Console Application
# -------------------------------------------------
add_executable(nlp_classifier
    src/main.cpp
    src/DataLoader.cpp
    src/Tokenizer.cpp
    src/NaiveBayes.cpp
)

# -------------------------------------------------
# GUI Application
# -------------------------------------------------
add_executable(nlp_classifier_gui
    src/gui_main.cpp
    src/DataLoader.cpp
    src/Tokenizer.cpp
    src/NaiveBayes.cpp
    gui/Theme.cpp
)
target_link_libraries(nlp_classifier_gui PRIVATE raylib)