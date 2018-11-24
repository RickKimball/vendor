# vendor/stm32 
---

Custom stm32 bluepill arduino core

Our goal with this custom core is to provide a custom board we probably won't share with the world. We do this to be able to use the STM core without having to directly modify it or having to write our own. Key to this approach is to be able to use the Arduino IDE board manager package to update the STM core when a new version become available without destroying our modifications.  This is accomplished by referencing the STM32:arduino core in our boards.txt. We create a board entry and a custom variant directory with our custom pin map.

This example core acts as a template for creating a custom core that references the official STM Arduino Core (the HAL based one).  A custom board called 'bluepill' is created. However it uses the STM Core code and many of the tool features. However, we also show how to add additional menu items
and custom tools such as openocd upload, and openocd debugging.  Mostly this core exists to serve as an example of the proper vendor and core reference names used in the boards.txt file.

# install
---

- Use the Arduino board manager and install the STM core.
```
    Follow the instructions at "http://github.com/stm32duino/Arduino_Core_STM32"
    to install using the json file.
```

- Stop the Arduino IDE

- Clone this github into your Arduino hardware environment;

```
    $ mkdir -p $HOME/Arduino/hardware
    $ cd $HOME/Arduino/hardware
    $ git clone https://github.com/RickKimball/vendor.git vendor

    *Note: I used the name 'vendor' here, but you should use your own unique name.
```

- Startup the Arduino IDE and select the Bluepill (kimballsoftare) from
the newly added menu

# stm32/boards.txt
---
(required file)

Put your custom board entry in this file.  This is actually the only required file if you just want to add a new board name with different compile options. However, typically you will also add a variant directory and make pin changes.

This file also contains the references to the STM32:variant package.

This line shows the proper way to reference the ST core for the bluepill board:
```
bluepill.build.core=STM32:arduino
```
Normally the build.core= value would just be 'arduino' By using 'STM32:' it tells the arduino builder to use a different vendor's package.  The vendor name ST chose to use was 'STM32'. I'm not sure why, a better name might have been 'stm' or 'st'.  However, because they used uppercase STM32, that is what you have to use to reference it.

Setting the core to STM32:arduino, tells the arduino builder to use the platform.txt logic defined from the ST core. It uses whatever compiler, linker, and tools that are defined there. However, the big difference is that it will use the build.xxx entries we define in our boards.txt but pass them to the platform.txt in STM32.  We don't have to write any core code, but we can affect the arguments it is provided.

We we go to upload, we have to indicate if we want to use the tools in STM32 also. This line provides that information:

```
bluepill.menu.upload_method.STLinkMethod.upload.tool=STM32:stlink_upload
```
By using 'STM32:stlink_upload', it uses the tools.stlink_upload in the STM32 platform.txt.

However, you can see that I reference other tools that are from our platform.txt:
```
bluepill.menu.upload_method.openocd.upload.tool=openocd
```
I used tool=openocd, that tells the arduino builder to look in our local platform.txt instead of the STM32 one.  In our platform.txt I create a proper entry that will upload using openocd from our $PATH.  I do this to allow me to setup my PATH entry to point at the openocd I want to use before I launch the arduino IDE.

# stm32/variant/*
---
(optional files)

If you create this directory and its files, you can create your own pinmap or deal with board level initialization that will be called by the STM32:arduino core.

In this bluepill example, I added a few defines LED_ON and LED_OFF to show that you need to set the bluepill HIGH to turn it off and LOW to turn it on.

# stm32/platform.txt
---
(optional file)

# stm32/libraries
---
(optional directory)

You can create examples that are specific to your board. I added one called leading_zeros/ that shows how to extended the Arduino <Streaming.h> to print leading zeros.

# stm32/tools
---
(options directory and files)

This is where I added some new tools scripts to provide a way to automatically launch the arm-none-eabi-gdb debugger and openocd as a gdb server connected to an stlink v2 programmer device.  It opens two xterms, one running the openocd gdb server, and a second running the commandline arm-none-eabi-gdb in split layout mode.  It isn't Visual Studio but it is fast if you have spend a littlle time learning the gdb commands.  You can step thorugh the line by line, break at a specific line, and examine memory.





