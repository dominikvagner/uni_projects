# NetFlow Exporter - "flow"

CLI NetFlow v5 exporter for already captured communication.

## Prerequisites

The things you need before running/compiling the software.

* UNIX based system
* libpcap library
* G++ (or any C++ code compiler)

## Installation

A step by step guide that will tell you how to get the development environment up and running.

```
$ tar -xvf xvagne10.tar     # (Extract the project files from the tar file.)
$ make                      # (Using UNIX Makefile compile the C++ code files.)
```

## Usage
Before launching our exporter we need to launch some collector on whose address we \
 will be sending generated NetFlow packets. (`nfcapd` recommended)

The exporter can be launched with these options: \
``./flow [-f <file>] [-c <netflow_collector>[:<port>]] [-a <active_timer>] [-i <inactive_timer>] [-m <count>]``

Example launch: \
``./flow -f capture.pcap -i 15 -m 256``

For more information check the program help by running ``./flow -h`` or ``man -l flow.1``

For information about the implementation, check the comments in the source code files (**flow.c, flow.h**) or the manual (**manual.pdf**)

## Author
Dominik Vágner, xvagne10 \
Created on: 07.11.2022

## Archive contents
Archive xvagne10.tar contains: _Makefile, flow.cpp, flow.h, flow.1, README.md, manual.pdf_

### Earned points
20/20