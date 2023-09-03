# Raspberry Pi Pico W FreeRTOS Starter - In C/C++

This project is consist of FreeRTOS integration in RPI Pico code. And also have a basic start-up project with a USB-tiny example.
## Setup the RPI Pico environment.
- Follow this video [Setup RPI Pico Compiler](https://youtu.be/mUF9xjDtFfY) to get compile example code first, then start setting up this code. 
- NOTE: It's recommended to use <b>Developer Command Prompt for VS 2019</b> to generate the build directory, then use VS code to compile the modified code. Sometime, VS code failed to generate a proper file path in MakFile. Also copy PATH (use `echo %PATH` command) varible from this console and add it to VS code enviornment variables as PATH variable.
## Important Setup
- We have Used <b>SMP</b> kernel of FreeRTOS. it because we want to use the multi-core of RP2040.
1. Create `rpiPico` folder. Inside folder, do the following steps:
2. Clone [Pico SDK](https://github.com/raspberrypi/pico-sdk.git) and inside the cloned SDK directory, run `git submodule update --init` to init all submodules.
3. Clone [PicoW-FreeRTOS-Template-main](https://github.com/8-DK/PicoW-FreeRTOS-Template-main) and inside the cloned SDK directory, run `git submodule update --init` to init all submodules.

4. Pico SDK should be present on the machine, and its path should be used as an environment variable as `PICO_SDK_PATH` pointing to the cloned Pico SDK dir (Ex. `D:\rpiPico\pico-sdk` )

5. FreeRTOS-Kernel should be present in the machine, and its path should be used as an environment variable as `FREERSTOS_KERNEL_PATH` pointing to the cloned FreeRTOS-Kernel dir (Ex. `D:\rpiPico\PicoW-FreeRTOS-Template-main\FreeRTOS-Kernel`)

These environment variables should be used when calling `CMake` or defined in VSCode(**RECOMENDED**) using [this setup](https://www.youtube.com/watch?v=BAoTBg8MJJ4) that uses the `CMake Tools` extension.
## Setup openocd

## Debug RPI Pico Using VS Code
1. Install Cortex-Debug ectension in VS code 
2. modify <b>Coretex-Debug: Openocd Path</b> to point <b>opeocd.exe</b>
3. Now click on debug symbol > setttings
4. Copy past following json and make changes acordingly

```json
{
    "version": "0.2.0",
    "configurations": [
      {
        "cwd": "${workspaceRoot}",
        "executable": "./build/src/PICOKEY.elf",
        "name": "PICOKEY",
        "request": "launch",
        "type": "cortex-debug",
        "servertype": "openocd",
        "configFiles": [     
          "D:/rpiPico/openocd/tcl/interface/picoprobe.cfg",
          "D:/rpiPico/openocd/tcl/target/rp2040.cfg"
        ],
        "searchDir": [
          "D:/rpiPico/openocd/tcl",
          "D:/rpiPico/openocd"
        ],
        "runToEntryPoint": "main",
        "showDevDebugOutput": "none"
      }]
  } 
  ```

 5. Now using picoprobe you can bedug RPI pico. or create your own using https://github.com/raspberrypi/picoprobe/releases this firmware on other RPI Pico.

## Debug RPI Pico Using QT Creator
This IDE best in my view for debuging. eassy interface for debuging and variable view.

 https://github.com/EdgeTX/edgetx/wiki/Building-and-debugging-with-Qt-Creator
