import os

for x in range(0,256):
    os.system("python2 BuildSingleBootloaderById.py " + str(x))