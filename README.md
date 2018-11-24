# vendor/stm32 
--------------

Custom stm32 bluepill arduino core

This example core acts as a template for creating a custom core that references the official
STM Arduino Core (the HAL based one).  This core uses the STM Core code and many of the tool features
but also provides its own additional menu items and toos such as openocd upload, and openocd debugging.
Mostly this core exists to serve as an example of the proper vendor and core reference names used 
in the boards.txt file.

# install
---------

- Use the Arduino board manager and install the STM core.
```
    Follow the instructions at "http://github.com/stm32duino/Arduino_Core_STM32"
    to install using the json file.
```

- Stop the Arduino IDE

- Clone this github into your Arduino hardware environment;

```
    $ cd $HOME/Arduino/hardware
    $ git clone https://github.com/RickKimball/vendor.git vendor

    *Note: I used than name 'vendor' here but you should use your own unique name.
```

- Startup the Arduino IDE and select the Bluepill (kimballsoftare) from
the newly added menu

