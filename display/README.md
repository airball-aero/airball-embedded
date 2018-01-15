# Airball Display Source

## Introduction

This is the source tree for the Airball display. It is intended to be compiled
on a Linux system. The actual display runs on a Raspberry Pi, and at the moment
the system needs to be compiled from scratch on the Raspberry Pi as well.

## Building

You need to create a subdirectory called `external` and download into
it the following:

```
https://github.com/google/googletest
https://github.com/miloyip/rapidjson
```

To build the system, in the top level directory (here), do:

```
cmake CMakeLists.txt --build build
```

This will create a directory `build` that will contain all the generated
`Makefile`s. Now `cd` into that directory and type:

```
make <target>
```

where your `<target>` is any of the binaries defined in `src/CMakeLists.txt`.