# Usage

Below are instructions on how to run and use the programs.

## Input

For this program to work properly, it is important that the file order is maintained. Inside of "bin", there exists the c files "driver.c", "lex.c", "par.c", and "vm.c", along with their respective header files.

There is also a folder named "rsc". Inside of that folder is where the input file must go.

## Compiling and running

To compile and run the code in a Linux environment, enter the following commands in the terminal.
(It is implied that you have already navigated and set the working directory to the "bin" folder in the project.)

```bash
gcc driver.c lex.c par.c vm.c -o main
./main rsc/lex_input.txt 
```

To compile and run on Windows (PowerShell):
```bash
gcc driver.c lex.c par.c vm.c -o main
./main .\rsc\lex_input.txt
```
"lex_input.txt" is the default input that this project comes with. It is located inside of the "rsc" folder.

## Warnings
If the directory structure and compilation specified above, running the program will result in a segmentation fault.

## Ownership
This project was created by Zach Schickler for HW4 of COP3402, Summer 2019.