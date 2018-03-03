<!--
 The MIT License (MIT)

 Copyright (c) 2018, Ihab A.B. Awad

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
-->

# INSTRUCTIONS FOR CROSS COMPILING TO RASPIAN/ARM

## Step by step

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


## Notes

Note how xyz/CMakeFile.txt contains relative references to the tools
location.

See also this writeup:

https://medium.com/@au42/the-useful-raspberrypi-cross-compile-guide-ea56054de187

