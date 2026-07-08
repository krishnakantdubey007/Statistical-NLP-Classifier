Day 1
- Git setup completed
- Project structure created
- Tokenizer designed
# Day 1

## Completed
- Project structure created
- GitHub repository connected
- Tokenizer module implemented
- Tokenizer tested successfully
- Learned:
  - Header vs Source files
  - Class vs Object
  - Vector
  - const reference
  # Day 2

## Completed

- Implemented DataLoader
- Loaded UCI SMS Spam Collection Dataset (5574 messages)
- Implemented NaiveBayes Class
- Added Training Algorithm
- Added Prediction Algorithm
- Implemented Laplace Smoothing
- Implemented Log Probability
- Improved train() by resetting model state before retraining
- Improved lookup performance using unordered_map::find()

## Learned

- unordered_map
- unordered_set
- Laplace Smoothing
- Log Probability
- Naive Bayes Training
- Naive Bayes Prediction
# Day 3

## Completed

- Completed Naive Bayes Engine
- Implemented Training Algorithm
- Implemented Prediction Algorithm
- Added Interactive Console Menu
- Added Project Information Screen
- Added Model Statistics
- Successfully tested the classifier on real SMS messages
- Verified Spam and Ham predictions using the UCI SMS Spam Collection dataset

## Learned

- Multinomial Naive Bayes
- Laplace Smoothing
- Log Probability
- Object-Oriented Design
- STL (unordered_map, unordered_set, vector)
- Exception Handling
- Interactive Console Programming

V# Version 1.0 Completed
**Date:** 07 July 2026

## Completed Features

- Implemented Tokenizer module
- Implemented DataLoader module
- Implemented Multinomial Naive Bayes classifier
- Added Laplace Smoothing
- Added Interactive Console Menu
- Added Model Statistics
- Added About Project section
- Integrated UCI SMS Spam Collection dataset
- Added README documentation
- Added GitHub screenshots
- Organized project into modular C++17 architecture

## Technologies Used

- C++17
- STL
- Object-Oriented Programming
- Git
- GitHub

## Current Status

Stable Version 1.0 Released

## Next Goals (Version 2.1)

- Calculate accuracy, precision, recall, and F1-score
- Visual confusion matrix in GUI
- Model serialization (saving and loading trained weights)

# Version 2.0 Completed
**Date:** 08 July 2026

## Completed Features

### UI Refactoring (Raylib & RayGUI Desktop Application)
- Replaced the console interface with a professional 5-page desktop GUI application (`gui_main.cpp`).
- Designed a sleek modern dark cyber theme with card hierarchy and custom accent borders.
- Developed a dynamic status-based visual indicator badge system for SPAM and HAM classifications.
- Integrated a live statistics overview screen featuring ratio indicators and responsive tile cards.
- Integrated standard back navigation and exit overlay screens.
- Avoided state conflicts by packing Predict page state values into a testable cohesive structure.
- Addressed cross-page textbox focus states to ensure input boxes clear properly.

### Code Quality & Robustness Upgrades
- Added deep exceptions wrapping around the startup loading/training cycle.
- Fixed a silent class-prior fallback issue for empty whitespace inputs.
- Safeguarded class likelihood calculations against division-by-zero on untrained models.
- Eliminated triple redundant string checks on dataset labels per loop iteration.
- Optimized circle alignment rendering logic using exact float vector coordinates.
- Prevented button collision overlaps on nested page menus using RayGUI lock triggers.

### Custom Typography System
- Set up a dynamic assets resolution path for font lookup handlers.
- Loaded a high-resolution, pixel-perfect copy of the Consolas modern monospace typeface (`consola.ttf`).
- Integrated global font assignments mapping to both standard text and library elements.

## Technologies Used
- C++17
- Raylib 5.x
- RayGUI 4.x
- CMake Build System

## Current Status
Stable Version 2.0 Ready for Release