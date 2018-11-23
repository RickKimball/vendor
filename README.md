# vendor 
--------
Custom referencing stm32 bluepill arduino core 

This example core acts as a template for creating a custom core that references the official
STM Arduino Core (the HAL based one).  This core uses many features of the STM Core but also provides
additional features such as openocd upload, and openocd debugging.  Mostly this exists to serve
as an example of the proper vendor and core names used in the boards.txt file.

#Install
--------
- Use the Arduino board manager and following the instructions for installing
the "http://github.com/stm32duino/Arduino_Core_STM32" using the json file:

- Stop the Arduino IDE

- Clone this github into your Arduino hardware environment;

```
    $ cd $HOME/Arduino/hardware
    $ git clone https://github.com/RickKimball/vendor.git vendor
```

- Startup the Arduino IDE and select the Bluepill (kimballsoftare) from
the newly added menu

