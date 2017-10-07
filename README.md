# CAN Bootloader #

Fork from outdated source http://www.kreatives-chaos.com/artikel/can-bootloader

With this Can-Bootloader you are able to download your precompiled application (from your [Arduino IDE](https://www.arduino.cc/en/main/software), [WinAVR](http://winavr.sourceforge.net/), etc.) to your decentralized microcontroller over Can-Bus.

The Bootloader required therefore a space of 512 Words.

![](https://github.com/ma-go/CanBootloader/blob/master/Fuses.JPG)

Correspondingly the fuses from an Atmega32 with internal 8Mhz crystal have to be set to

    hfuse 0xDC
    lfuse 0x94

If you have an Atmega32 with an external crystal with an "high freq" (>= 8Mhz) then you have to adopt the fuses regarding this Fuse [calculator](http://www.engbedded.com/fusecalc/) to:

    hfuse 0xDC
    lfuse 0x9E

If you're looking for an brief description about fuse settings, i recommend [this article](http://www.martyncurrey.com/arduino-atmega-328p-fuse-settings/). 

## Functional description Can-Bootloader: ##

After a restart, the AVR waits 0.5s to receive a specific Can-Message from the corresponding python upload script. If the time is elapsed without this message, the Bootloader jumps to the address zero and starts therefore the normal application.

If the Bootloader receives the specific Can-Message within this 0.5s, the transfer from the precompiled user application starts. After an successful upload from your PC to the microcontroller, the new application starts immediately. 


## Functional description PC-Software: ##

The uploading PC-Software is written in Python. 

As interface from your PC to the CAN-Bus, you will need an USB-Can adapter.
Because I have already bought some [Arduino Nanos ](https://store.arduino.cc/usa/arduino-nano) and besides some MCP2515 CAN-Bus Modules with addition TJA1050 Transceiver on board, i decide to build/flash me my own [CAN Debugger](http://www.kreatives-chaos.com/artikel/can_debugger).
The regarding software from https://github.com/latonita/arduino-canbus-monitor with precompiled hex file can be found in the Arduino folder under arduino-canbus-monitor. As you can see in the code, the CAN-Bus speed is set to 125kbps, the Clock speed from the MCP2515 Board is 8MHz, and the **C**hip**S**elect pin is D10.

With your completed CAN-Debugger you can start downloading your first individual application to your decentralized microcontroller.

Therefore go to the command shell and switch to the CanBootloader/bootlaoder_python folder.
Now type in (or copy) subsequent command with your individual BOARD_ID, COM_PORT and FILE.

    $ python bootloader.py -d -i BOARD_ID -p COM_PORT -f FILE.hex

The value from the BOARD_ID must be the BOOTLOADER_BOARD_ID from the previously flashed 
Bootloader at the corresponding microcontroller you want to flash over CAN-Bus.





