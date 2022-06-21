# Varianta [ZETA] - Packet sniffer

Simple CLI packet sniffer.

### Prerequisites

The things you need before running/compiling the software.

* UNIX based system
* libpcap library
* G++ (C++ code compiler)

### Installation

A step by step guide that will tell you how to get the development environment up and running.

```
$ tar -xvf xvagne10.tar     # (Extract the project files from the tar file.)
$ make                      # (Using UNIX Makefile compile the C code file.)
```

## Usage

The sniffer can be launched with these options:
./ipk-sniffer [-i intrfc | --interface intrfc] {-p port}{[--tcp|-t] [--udp|-u] [--arp] [--icmp] } {-n num}

Make sure to run the program with administrator privileges.

For more information check the program help by running ``./ipk-sniffer -h``

For information about the implementation, check the comments in the source code files (**ipk-sniffer.c, ipk-sniffer.h**) or the manual (**manual.pdf**)

## Author

Dominik Vágner, xvagne10

### Archive contents
Archive xvagne10.tar contains: _Makefile, ipk-sniffer.cpp, ipk-sniffer.h, README.md, manual.pdf_