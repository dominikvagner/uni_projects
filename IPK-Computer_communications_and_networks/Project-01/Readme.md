# HINFOSVC

Simple HTTP client in C, that provides information about the host system.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.


### Prerequisites

The things you need before running/compiling the software.

* The program reads from specific files in **"*/proc*"** folder, for that a system that implements the "procfs" (proc filesystem) is needed.
* GNU Core Utilities
* The **"*sed*"** command
* GCC (C code compiler)
* curl and/or GET command/s

### Installation

A step by step guide that will tell you how to get the development environment up and running.

```
$ unzip xvagne10.zip     # (Extract the project files from the zip file.) 
$ make                   # (Using UNIX Makefile compile the C code file.)
```

## Usage

A few examples of useful commands and/or tasks.
There are 3 different commands (GET request) that are supported:
* **/hostname**, returns the hostname of the host system.
* **/cpu-name**, returns the name of the host system's CPU.
* **/load**, returns the all core average load of the host's CPU in percents.

On all other requests the program returns a code **"*400 Bad Request*"**

```
$ ./hinfosvc 8042 & 
$ curl http://localhost:8042/hostname
$ curl http://localhost:8042/cpu-name
$ curl http://localhost:8042/load
```

or

```
$ ./hinfosvc 8042 & 
$ GET http://localhost:8042/hostname
$ GET http://localhost:8042/cpu-name
$ GET http://localhost:8042/load
```

For more information, check the comments in the source code file. (*hinfosvc.c*)

## Author

Dominik Vágner, xvagne10
