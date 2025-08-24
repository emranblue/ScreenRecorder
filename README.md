# Screen Recorder

A simple screen recorder application for Linux, built with Qt.

## Features

*   **Multiple Capture Modes:** Record the full screen, a selected area, or a specific window.
*   **Audio Recording:** Capture audio from your microphone or system audio.
*   **Webcam Overlay:** Include a webcam feed in your recording.
*   **Pause and Resume:** Pause and resume your recording at any time.
*   **Customizable Output:**
    *   Choose the output location for your recordings.
    *   Select from various filename formats, including timestamp-based and sequential naming with a custom prefix.
*   **FPS Selection:** Choose the desired frames per second for your recording.

## Dependencies

*   **Qt6:** The application is built with Qt6. You'll need the `Core`, `Gui`, and `Widgets` modules.
*   **ffmpeg:** The screen recording is done using the `ffmpeg` command-line tool.
*   **A C++17 compiler:** The project is written in C++17.

## Building

1.  **Install Dependencies:** Make sure you have Qt6, ffmpeg, and a C++17 compiler installed on your system.
2.  **Clone the Repository:**

    ```bash
    git clone <repository-url>
    cd screen-recorder
    ```

3.  **Build the Application:**

    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

## Usage

1.  **Run the Application:**

    ```bash
    ./build/ScreenRecorder
    ```

2.  **Configure Settings:**

    *   Click the "Settings" button to open the settings dialog.
    *   Choose the output location for your recordings.
    *   Select a filename format. If you choose the custom sequential format, you can also set a custom prefix.

3.  **Start Recording:**

    *   Select the desired capture mode, audio source, and FPS.
    *   Click the "Start Recording" button.

4.  **Pause and Resume:**

    *   Click the "Pause" button to pause the recording.
    *   Click the "Resume" button to resume the recording.

5.  **Stop Recording:**

    *   Click the "Stop Recording" button to stop the recording. The video file will be saved in the location you specified in the settings.

## Author

This project was developed by Emran Hossain.