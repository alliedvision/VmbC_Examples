VmbC Examples
=============

Compile instructions & tool chain requirements
----------------------------------------------

> #### <em> Please note that the Visual Studio solutions in the corresponding Examples folders are only for deployment purposes. If you want to build the exapmles with a custom build of VmbAPI (e.g. for debugging), please follow the instructions below. </em>

### Requirements

* cmake (>= 3.21)
* VmbAPI Installation directory incl. `FindVmb.cmake` (VmbC, VmbCPP, VmbImageTransform)
* Qt 5.15.x
* Linux: gcc/g++ (>= 8.4.0)
* Windows: Microsoft Visual Studio (>= 2017)

### 1. Specify VmbAPI and Qt directory

Before running CMake to initialize the project, we need to adjust [`CMakeUserPresets.json`](./CMakeUserPresets.json).

Depending on your desired configuration and platform, you need to specify the path to your VmbAPI and Qt installation directory.

Open the file in an editor and adjust the following variables according to your local setup as follows:

#### Windows

* VmbAPI:

    In the configurePreset `win64`, set the value of `VMB_API_PATH` to your local VmbAPI installation.
    The path must point to the root directory of your VmbAPI installation.
    The VmbAPI installation may either be a local directory built with VmbAPI's CMake install target or it may be the API directory of the Vimba X installer, e.g.:

    ```
    "VMB_API_PATH": "C:/Program Files/Allied Vision/Vimba X/api/"
    ```
    The default location is `${sourceDir}/VmbAPI`, which means you may also just unzip the latest `VmbAPI` artifact right into the `VmbC/Examples` directory.

* Qt:

    In addtion, you need to set the Qt search path:

    In the configurePreset `win64`, set the value of `QT_PATH` to your local Qt installation, e.g. `C:/Qt_5_15_x64_Windows` or `"C:/Qt/5.15.2/msvc2019_64"` (depending on whether you are using a self-compiled version of Qt or the official Qt installer):

    ```
    "QT_PATH": "C:/Qt_new/5.15.2/msvc2019_64"
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