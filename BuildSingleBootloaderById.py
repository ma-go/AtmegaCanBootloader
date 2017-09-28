import sys
import shutil 
import fileinput
import os

id = sys.argv[1]
if int(id) < 10:
    id = "0" + id;

shutil.copytree("bootloader_mcp2515", "bootloader_atmega32_12Mhz_mcp2515_8MHz/ID_" + id)

for line in fileinput.input("bootloader_atmega32_12Mhz_mcp2515_8MHz/ID_" + id + "/Makefile", inplace = 1): 
    print line.replace("BOOTLOADER_BOARD_ID = 0x", "BOOTLOADER_BOARD_ID = 0x" + id),

os.chdir("bootloader_atmega32_12Mhz_mcp2515_8MHz/ID_" + id)
os.system("make all")