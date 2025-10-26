# Assembly-Interpreter

This project began as part of my Assembly course, based on the text book C and C++ Under the Hood, Second Edition.
The assignment was to create an interpreter, based on the assembly compiler we use in class.

## Overview
Expand my existing class project, completing an interpreter that reads and executes encoded assembly files (.e).
These .e files are produced by compiling assembly source files (.a) using the lcc compiler provided by the course.

Currently Supported Instructions:
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

## Usage
Compile:        gcc i1.c -o i1
Run Programs:   ./i1 i1test.e

## Future Plans
I want to grow this project to handle the full instruction set covered in class, and possibly add debugging options to allow for easier testing of assembly programs.
