import sys
import shutil 
import fileinput
import os

id = sys.argv[1]
if int(id) < 15:
    id = "0x" + hex(int(id))[2:].zfill(2)
else:
    id = hex(int(id))

shutil.copytree("bootloader_mcp2515", "bootloader_atmega32_12Mhz_mcp2515_8MHz/ID_" + id)

for line in fileinput.input("bootloader_atmega32_12Mhz_mcp2515_8MHz/ID_" + id + "/Makefile", inplace = 1): 
    print line.replace("BOOTLOADER_BOARD_ID = ", "BOOTLOADER_BOARD_ID = " + id),

os.chdir("bootloader_atmega32_12Mhz_mcp2515_8MHz/ID_" + id)
os.system("make clean")
os.system("make all")
os.chdir("..")
shutil.copy2("ID_" + id + "/bootloader.hex", "bootloader" + id)