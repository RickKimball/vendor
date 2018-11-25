## Custom stm32 bluepill board based on the official STM Arduino core
---


The goal of this custom core is to create a custom board we probably won't share with the world. We do this to be able to use the STM core without having to directly modify it or having to write our own. Key to this approach is to be able to use the Arduino IDE board manager to update the STM core when a new version become available without destroying our modifications.  This is accomplished by referencing the STM32:arduino core in our boards.txt. A new board entry is created, a custom variant directory with our custom pin map, and custom tools.

This example core acts as a template for referencing the official STM Arduino Core (the HAL based one).  A custom board called 'bluepill' is created. However it still uses the STM Core code and many of its tools. It also adds additional menu items and custom tools such as openocd upload, and openocd debugging. Lastly, we create some example code that is only shown when our board is selected.  Mostly this core exists to serve as an example of the proper vendor and core reference names used in the boards.txt file.

## install
---

* Use the Arduino board manager and install the STM core.

    Follow the "Additional Boards Managers URLs" instructions at http://github.com/stm32duino/Arduino_Core_STM32 using a json file.

* Stop the Arduino IDE

- Clone this github into your Arduino hardware environment;

```
    $ mkdir -p $HOME/Arduino/hardware
    $ cd $HOME/Arduino/hardware
    $ git clone https://github.com/RickKimball/vendor.git vendor

    *Note: I used the name 'vendor' here, but you could/should use your own unique name.
```
* Startup the Arduino IDE and select the BluePill (vendor) from
the newly added menu

## stm32/boards.txt
---
(required file)

Put your custom board entry in this file.  The stm32/boards.txt file is actually the only required file if you just want to add a new board name with different compile options. However, typically you will also add a variant directory and make pin changes.  In this example the board name used is 'bluepill'

This is where you reference the STM32:variant package. This line shows the correct vendor name and core name to use for the STM core:
```
bluepill.build.core=STM32:arduino
```
Normally the 'build.core=' value would just be 'arduino'. By using 'STM32:arduino' we are telling the arduino builder to go elsewhere and use a different vendor's core package.  The vendor name STM chose to use was 'STM32'. I'm not sure why. A better name might have been 'stm' or 'st'.  However, because they used uppercase STM32, that is what you have to use to reference it.  The vendor name can be found in $HOME/.arduino15/packages/ after an install from the board manager.

Setting the build.core to STM32:arduino, tells the arduino builder to use the platform.txt logic defined from the STM core. The builder uses whatever commands are defined for the compiler, linker, and tools in the STM core platform.txt. However, the big difference is that it will use the build.xxx entries we define in our boards.txt and pass them to the platform.txt commands in STM32.  We dont write any core code, instead we use the STM core code but we can affect compiler arguments it is provided from our boards.txt.

When upload is pressed, we also have to indicate if we want to use the tools in STM32. This line provides that information:

```
bluepill.menu.upload_method.STLinkMethod.upload.tool=STM32:stlink_upload
```
By using 'STM32:stlink_upload', it uses the tools.stlink_upload command in the STM32 platform.txt.

However, in this example I also reference other tools that are from our stm32/platform.txt:
```
bluepill.menu.upload_method.openocd.upload.tool=openocd
```
I used upload.tool=openocd, that tells the arduino builder to look in our local platform.txt instead of the STM32 one.  In our platform.txt I create a proper entry that will upload using openocd from our $PATH.  This allows me to setup my PATH entry to point at the openocd I want to use before I launch the arduino IDE.

###stm32/booloaders/*

(optional directory)
The files in this directory work with tools/programmer entries and the boards.txt entries. You must have a erase and bootloader tool associated with the board for this to work.

As the current STM core doesn't actually support any bootloaders with the bluepill boards, I put a blinky hex file. However, this lets you exercise the bootloader entries even if it doesn't actually load a bootloader.

stm32/platform.txt:
```
tools.openocd.upload.pattern="{path}{cmd}" {upload.verbose} -f interface/{upload.protocol}.cfg -f target/stm32f1x.cfg -c "program {build.path}/{build.project_name}.hex verify reset exit"
```

### stm32/platform.txt
---
(optional file)

If this file exists, it creates a new submenu under the Tools/Boards: menu. The submenu shows up as "Custom STM32duino Boards" with one option "BluePill (vendor)".

This file also contains tool entries I added to allow openocd to be used for both upload and debug.  These tools are available because of the additional "Upload using:" menu entries added to the boards.txt file

### stm32/libraries
---
(optional directory)

You can create examples that are specific to your board. I added one called leading_zeros/ that shows how to extended the Arduino <Streaming.h> to print leading zeros.

### stm32/tools
---
(optional directory and files)

This is where I added some new tools scripts to provide a way to automatically launch the arm-none-eabi-gdb debugger and openocd as a gdb server connected to an stlink v2 programmer device.  It opens two xterms, one running the openocd gdb server, and a second running the commandline arm-none-eabi-gdb in split layout mode.  It isn't Visual Studio but it is fast if you spend a little time learning the gdb commands.  You can step thorugh the line by line, break at a specific line, and examine memory.

### stm32/variant/*
---
(optional files)

If you create this directory and its files, you can define your own pinmap or deal with board level initialization that will be called by the STM32:arduino core.

In this bluepill example, I added a few defines LED_GREEN_ON and LED_GREEN_OFF to show that you need to set the bluepill HIGH to turn it off and LOW to turn it on.
