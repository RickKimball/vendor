# Custom stm32 bluepill board based on the official STM Arduino core


The goal of this custom core is to create a custom board we probably won't share with the world. We do this to be able to use the STM core without having to directly modify it or having to write our own. Key to this approach is to be able to use the Arduino IDE board manager to update the STM core when a new version become available without destroying our modifications.  This is accomplished by referencing the STM32:arduino core in our boards.txt. A new board entry is created, a custom variant directory with our custom pin map, and custom tools.

This example core acts as a template for referencing the official STM Arduino Core (the HAL based one).  A custom board called 'bluepill' is created. However it still uses the STM Core code and many of its tools. It also adds additional menu items and custom tools such as openocd upload, and openocd debugging. Lastly, we create some example code that is only shown when our board is selected.  Mostly this core exists to serve as an example of the proper vendor and core reference names used in the boards.txt file.

**Note:** This document just highlights some specific things required to reference the STM core. More detailed information about adding 3rd party hardware and libraries is found in these documents:

* http://github.com/arduino/Arduino/wiki/Arduino-IDE-1.5-3rd-party-Hardware-specification
* https://github.com/arduino/Arduino/wiki/Arduino-IDE-1.6.x-package_index.json-format-specification
* http://github.com/arduino/Arduino/wiki/Arduino-IDE-1.5:-Library-specification


## install

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

## stm32 architecture configuration files

The directory name 'stm32' identifies the architecture of this core.  The architecture name is an arbitrary value however the arduino builder uses to set the global {build.arch} variable.  When you reference other vendors cores like we are doing here the {build.arch} determines which architecture is used in the referenced core.  Library files also use the {build.arch} setting to see if they are compatible. 

### stm32/boards.txt

(required file)

Put your custom board entry in this file.  The stm32/boards.txt file is actually the only required file if you just want to add a new board name with different compile options. However, typically you will also add a variant directory and make pin changes.  In this example the board name used is 'bluepill'

This is where you reference the VENDOR_ID:CORE_ID package. This line shows the correct vendor name and core name to use for the STM core:
```
bluepill.build.core=STM32:arduino
```
The vendor name STM chose to use was 'STM32'. I'm not sure why. A much better name might have been 'stm', 'st', or even 'stm32duino'.  However, because they used uppercase STM32 as the vendor name, that is what you have to use to reference it.  The vendor name ('STM32') and architecture name ('stm32') can be found in the STM core json package file.

After the board manager install its path looks like this:
     /home/user/.arduino15/packages/STM32/hardware/stm32/1.4.0/boards.txt

* where STM32 is the vendor
* where stm32 is the architecture
* where 1.4.0 is the version number

Installing into $HOME/Arduino from github, must look like this: 
     $HOME/Arduino/hardware/STM32/stm32/boards.txt

Note: If you install locally using github, you have to create the proper directories for vendor name and architecture.

Normally, the CORE_ID value for 'build.core=' would just be 'arduino'. By using 'STM32:arduino' we are telling the arduino builder to go elsewhere and use a different vendor's core package.  The arduino builder will look for the vendor package first in the directory $HOME/Arduino/hardware/STM32/stm32/core/arduino. If that doesn't exist, it will look in /home/user/.arduino15/packages/STM32/hardware/stm32/1.4.0/cores/arduino. (where the version is 1.4.0 of in the board manager. * assuming linux names here, it is slightly different path for OSX (/Users/&lt;user&gt;/.arduino15 and Windows *need to look this up *)

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

### stm32/bootloaders/*

(optional directory)

The files in this directory work with entries in boards.txt, platform.txt, and programmer.txt. You must have a tool.erase and tool.bootloader associated with the board for this to work.

As the current STM core doesn't actually support bootloaders or USB with the bluepill boards, I used a simple blinky hex file. However, this lets you exercise the bootloader entries even if it doesn't actually load a bootloader. Someday ;)

### stm32/platform.txt

(optional file)

If this file exists, it creates a new submenu under the Tools/Boards: menu. The submenu shows up as "Custom STM32duino Boards" with one option "BluePill (vendor)".

This file also contains tool entries I added to allow openocd to be used for both upload and debug.  These tools are available because of the additional "Upload using:" menu entries added to the boards.txt file

stm32/platform.txt:
```
tools.openocd.upload.pattern="{path}{cmd}" {upload.verbose} -f interface/{upload.protocol}.cfg -f target/stm32f1x.cfg -c "program {build.path}/{build.project_name}.hex verify reset exit"
```
### stm32/programmers.txt

(optional file)

Here I added an entry for the STM stlink-v2 that points back to the platform.txt openocd tool. Programmer entries here are used when you select the "Upload using Programmer" menu item or hold the Shift key while you press the Upload button. It is also used when you select the "Burn Bootloader" menu item.  Look at the platforms.txt file and find the (tools.openocd.erase tools.openocd.bootloader) and tools.openocd.program to see how the command line is changed to perform each of those specific functions.

### stm32/libraries

(optional directory)

You can create examples that are specific to your board. I added one called leading_zeros/ that shows how to extended the Arduino <Streaming.h> to print leading zeros.

### stm32/tools

(optional directory and files)

This is where I added some new tools scripts to provide a way to automatically launch the arm-none-eabi-gdb debugger and openocd as a gdb server connected to an stlink v2 programmer device.  It opens two xterms, one running the openocd gdb server, and a second running the commandline arm-none-eabi-gdb in split layout mode.  It isn't Visual Studio but it is fast if you spend a little time learning the gdb commands.  You can step thorugh the line by line, break at a specific line, and examine memory.

### stm32/variant/*

(optional files)

If you create this directory and its files, you can define your own pinmap or deal with board level initialization that will be called by the STM32:arduino core.

In this bluepill example, I added a few defines LED_GREEN_ON and LED_GREEN_OFF to show that you need to set the bluepill HIGH to turn it off and LOW to turn it on.
