== INSTRUCTIONS FOR CROSS COMPILING TO RASPIAN/ARM ==


1. Step by step

First download the ZIP from this Git repo --

  https://github.com/raspberrypi/tools

Make sure it is expanded under this directory as "./tools-master/".

Make sure the directory "./build/" exists and is empty.

Change to the directory "./build/".

Execute the commands:

  cmake ../xyz/ -DCMAKE_TOOLCHAIN_FILE=../xyz/toolchain-rpi.cmake
  make xyz

Observe that you now have a binary, "xyz", which is a statically
linked ARM binary.


2. Notes

Note how xyz/CMakeFile.txt contains relative references to the tools
location.

See also this writeup:

https://medium.com/@au42/the-useful-raspberrypi-cross-compile-guide-ea56054de187

