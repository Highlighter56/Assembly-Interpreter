# Assembly-Interpreter

This project began as part of my Assembly course, based on the text book C and C++ Under the Hood, Second Edition.
The assignment was to recreate a simplified version of the interpreter we use in class.

##Overview
The goal of this project is to build an interpreter that reads and executes encoded assembly files (.e).
These .e files are produced by compiling assembly source files (.a) using the course’s provided compiler, lcc.

My implementation currently supports several instructions, including:
 - br
 - add
 - ld
 - st
 - bl/call/jsr
 - blr/jsrr
 - and
 - ldr
 - str
 - not
 - lea
 - halt
 - nl
 - dout
 - jmp
 - ret
 - brz/bre
 - brn
 - brp


##Usage
Compile:                gcc i1.c -o i1
Run a test program:     ./i1 i1test.e

##Future Plans
I want to expand this project to handle the full instruction set
Implement clearer error messages and debugging options.