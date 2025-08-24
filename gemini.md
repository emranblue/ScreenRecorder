# Gemini Project Notes: Screen Recorder

This file contains notes for the Gemini agent to quickly understand the Screen Recorder project.

## Project Overview

This is a simple screen recorder application for Linux, built with Qt. It uses the `ffmpeg` command-line tool to perform the actual screen recording. The application provides a GUI for selecting capture modes, audio sources, and other recording options.

## Project Structure

*   `src/`: Contains the C++ source code for the application.
    *   `main.cpp`: The main application entry point and the `ScreenRecorder` class, which is the main window.
    *   `selectionwidget.h` and `selectionwidget.cpp`: The widget used for selecting a screen area to record.
    *   `windowselectiondialog.h` and `windowselectiondialog.cpp`: The dialog for selecting a window to record.
    *   `webcamselectiondialog.h` and `webcamselectiondialog.cpp`: The dialog for selecting a webcam.
    *   `settingsdialog.h` and `settingsdialog.cpp`: The settings dialog for configuring the application.
*   `CMakeLists.txt`: The CMake build script for the project.
*   `README.md`: The user-facing documentation.
*   `gemini.md`: This file.

## Key Classes

*   **`ScreenRecorder`:** The main class, derived from `QWidget`. It handles the main UI, user interactions, and the recording process.
*   **`SettingsDialog`:** A `QDialog` for configuring the application settings. It uses `QSettings` to store the settings.
*   **`SelectionWidget`:** A `QWidget` that allows the user to select a rectangular area on the screen.
*   **`WindowSelectionDialog`:** A `QDialog` that lists the available windows and allows the user to select one to record.
*   **`WebcamSelectionDialog`:** A `QDialog` that lists the available webcams and allows the user to select one.

## Extending the Application

Here are some ideas for extending the application:

*   **More Output Formats:** The application currently only saves recordings in `.mkv` format. You could add options for other formats like `.mp4`, `.avi`, etc.
*   **Video Quality Settings:** Add options for controlling the video quality, such as bitrate, resolution, etc.
*   **Hotkeys:** Add global hotkeys for starting, stopping, and pausing recordings.
*   **Preview:** Show a preview of the recording in the main window.
*   **Cross-Platform Support:** The application is currently Linux-only due to its reliance on `x11grab`. It could be extended to support other platforms like Windows and macOS by using different `ffmpeg` input formats.