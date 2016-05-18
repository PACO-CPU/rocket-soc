Rocket SoC application templates
====================

This directory contains ready-to-use templates for developing programs to be
run on the Rocket SoC instantiated on an FPGA.    
To load the program onto the FPGA's memory and execute them, the uart flashing
tool is used.

## basic-application
This is an example application utilizing the UART to read/write. To use this,
simpily copy the entire directory where you need it.   
The included makefile takes care of everything, just run
```
  make
```
to compile the `main.c`. This will produce three files: 
- `main`, the RISC-V executable in ELF format.
- `main.dump`, a dump of all the ELF sections
- `main.bin`, the RISC-V executable in binary format.

The `main.bin` is used to load the program onto an FPGA. To do so, just run:
```
  make run
```
This will reset the FPGA, load the program, execute it and enter an interactive
shell communicating via UART.   
To make sure the program is always up-to-date when testing, it is recommended
to always use a clean build when running:
```
  make clean run
```
This will remove all generated files, re-build them and finally run the 
program.   


### Troubleshooting
By default the UART is expected to be addressable as `/dev/ttyUSB0`. If this
does not work (e.g. you have other UARTs connected to the system), you can
change this by aupplying `UART=<path>` to the make command line, replacing 
`<path>` with the path to the UART block device to use. For example:
```
  make run UART=/dev/ttyUSB1
```


Sometimes the flashing tool does not succeed in synchronizing the resetting
process. In that case just restart the process, it should not occur very often.

