import sys
import shutil 
import fileinput
import os

id = sys.argv[1]
if int(id) < 15:
    id = "0x" + hex(int(id))[2:].zfill(2)
else:
    id = hex(int(id))

desFolder = "bootloader_atmega328p_16Mhz_mcp2515_8MHz_CS_B2"
shutil.copytree("bootloader_mcp2515", desFolder + "/ID_" + id)

for line in fileinput.input(desFolder + "/ID_" + id + "/Makefile", inplace = 1): 
    print line.replace("BOOTLOADER_BOARD_ID = ", "BOOTLOADER_BOARD_ID = " + id),

os.chdir(desFolder + "/ID_" + id)
os.system("make clean")
os.system("make all")
os.chdir("..")
shutil.copy2("ID_" + id + "/bootloader.hex", "bootloader" + id)