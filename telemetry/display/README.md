<!--
 The MIT License (MIT)

 Copyright (c) 2017-2018, Ihab A.B. Awad

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

# Airball Display Source

## Introduction

This is the source tree for the Airball display. It is intended to be compiled
on a Linux system. The actual display runs on a Raspberry Pi, and at the moment
the system needs to be compiled from scratch on the Raspberry Pi as well.

## Building

### Install system dependencies ###

Install the Cairo library (which will pull in X11-related dependencies as well):

```
sudo apt install libcairo2-dev
```

### Clone this repository ###

First you'll need to get this repository on your machine:

```
git clone https://github.com/airball-aero/airball-embedded.git
cd airball-embedded/display
````

Then you will need to create a subdirectory called `external` and clone a couple of dependencies into it:

```
mkdir external
git clone https://github.com/google/googletest external/googletest
git clone https://github.com/miloyip/rapidjson external/rapidjson
```

### Build Airball ###

To build the system, in the top level directory (here), do:

```
mkdir build
cd build
cmake ..
```

This new directory `build` will contain all the generated `Makefile`s. In this
directory, to build a given target, type:

```
make <target>
```

where your `<target>` is any of the binaries defined in `src/CMakeLists.txt`.
