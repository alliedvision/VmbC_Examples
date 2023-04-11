VmbC Examples
=============

Programming examples using the Vimba X C API "VmbC".

Beta Disclaimer
---------------
THE SOFTWARE IS PRELIMINARY AND STILL IN TESTING AND VERIFICATION PHASE AND IS PROVIDED ON AN “AS IS” AND “AS AVAILABLE” BASIS AND IS BELIEVED TO CONTAIN DEFECTS. A PRIMARY PURPOSE OF THIS EARLY ACCESS IS TO OBTAIN FEEDBACK ON PERFORMANCE AND THE IDENTIFICATION OF DEFECT SOFTWARE, HARDWARE AND DOCUMENTATION.

Compile instructions & tool chain requirements
----------------------------------------------

### Requirements

* cmake (>= 3.21)
* Vimba X
* Qt 5.15.x (optional)
* Linux: gcc/g++ (>= 8.4.0)
* Windows: Microsoft Visual Studio (>= 2017)

### 1. Specify VmbAPI and Qt directory

Before running CMake to initialize the project, rename [`CMakeUserPresets.json.TEMPLATE`](./CMakeUserPresets.json.TEMPLATE) to `CMakeUserPresets.json`.

Depending on your desired configuration and platform, you need to specify the path to your VmbAPI and Qt installation directory.

Open the file in an editor and adjust the following variables according to your local setup as follows:

#### Windows

* VmbAPI:

    In the configurePreset `win64`, set the value of `VMB_API_PATH` to your local VmbAPI installation.
    The path must point to the root directory of your VmbAPI installation, e.g.:

    ```
    "VMB_API_PATH": "C:/Program Files/Allied Vision/Vimba X/api/"
    ```

* Qt:

    If you want to build the Qt examples, you need to set the Qt path:

    In the configurePreset `win64`, set the value of `QT_PATH` to your local Qt installation, e.g.:

    ```
    "QT_PATH": "C:/Qt/5.15.2/msvc2019_64"
    ```

#### Linux

Similar to Windows, adjust `VMB_API_PATH` for VmbAPI and `QT_PATH` for Qt for your desired Linux configuration:
* `linux64` (will be inherited by `linux64-debug`)
* `arm64` (will be inherited by `arm64-debug`)


### 2. Configuration
    
Run CMake with your desired configuration preset to create your build system project files.

```$ cmake --preset <PRESET>```

e.g.: ```$ cmake --preset win64```

> <em> All available presets can be listed with:    
    ```$ cmake --list-presets``` </em>

### 3. Compilation

#### Windows

The configuration step will create a Visual Studio Solution `VmbCExamples.sln` in the folder specified in the configurePreset `win64`, the default is `build-win64`.

#### Linux

Makefiles will be created in the specified build folder, e.g. `build-linux64`

#### All platforms

Use the CMake build presets to build a default set of targets for your configuration preset.

```$ cmake --build --preset <PRESET>```

e.g.: ```$ cmake --build --preset win64```

You can adjust the build preset in [`CMakeUserPresets.json`](./CMakeUserPresets.json) to your needs.