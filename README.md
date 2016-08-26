Happygg
=======

Overview
--------

Happygg is a cross-platform Chinese Chess engine based on Universal Chinese Chess Protocol(UCCI) 3.0.
[![Build Status](https://travis-ci.org/Chaozz/happygg.svg?branch=master)](https://travis-ci.org/Chaozz/happygg)

Structure
-----------

```
+-- src/
|
+-- src_tests/
|   +-- basic_tests/
|   +-- lib/
|   |   +-- googletest/
+-- happygg.cpp
+-- CMakeLists.txt
+-- .travis.yml
```
---
#### Main folders explanation

__src/__ folder

This folder contains the source files. You will find the following folders inside

__src_tests/__ folder

This folder contains the test files. 

- basic_tests/
This folder contains the source code of all tests.
- lib/
Here you will find a copy of Google C++ Testing Framework which is linked to the test project as a library.

__happygg.cpp__

The main entrance of the project.

__CMakeLists.txt__

The configuration file of cmake.

__.travis.yml__

The configuration file of Travis.


Build
-----

> **Important!** To build this project in Windows, make sure to add the `#define WIN32` macro in *./src/pipe.h*.


#### Installing tools

__CMake__ is an open-source, cross-platform family of tools designed to build, test and package software.

And of course you need a cpp compiler (gcc, clang, etc),

#### Once you have all tools installed

- Open a terminal, go to the project folder,

```
cmake .
make
```

If everything goes fine, you should find the executable file *happygg* in the folder.


Use
-----

This project does not contained a GUI client. But you can use any 3rd-party program which support UCCI 3.0 to run the game. 
[ChessQ](https://github.com/walker8088/ChessQ) would be a very good choice.


Testing
-------

The project comes with unit tests, using the [Google C++ Testing Framework](https://github.com/google/googletest).

To run the tests, open a terminal, go to the project folder, and run the command:
```
ctest
```

Continuous Integration
----------------------

### Travis CI

[Travis CI][travis] is a continuous integration service, which can monitor GitHub for new commits
to your repository and execute scripts such as building the app or running tests. This
project contains a Travis configuration file, `.travis.yml`, which will cause Travis to run your
tests when you push to GitHub.

You will need to enable the integration between Travis and GitHub. See the Travis website for more
instruction on how to do this.

[travis]: https://travis-ci.org/






