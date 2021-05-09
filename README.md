# MIPCC: C-MIPS Compiler 

This is a compiler for C language to MIPS assembly. It implements almost all the syntactic features of C except Dynamic Memory Allocation i.e. malloc(), calloc(), pointers etc and structs. The generated MIPS assembly can be run using SPIM as a simulator. MIPCC uses Flex and Bison to parse the C source code and generate MIPS. 

---
## Requirements

- flex (tested with 2.6.4)
- bison (tested with 3.5.1) 
- SPIM (tested with version 8.0)

To install them, use
```console
foo@bar:~$ sudo apt install flex bison spim
```

## Building 

Run ```make``` in the root directory of the project. This will generate ```mipcc``` executable file which is the compiled compiler. 

## Usage

Here is the help menu
```console
foo@bar:~$ ./mipcc --help
Usage: mipcc [OPTION...] 
A compiler for compiling minimalistic C to MIPS assembly

  -f, --in=FILE              File containing C code
  -o, --out=FILE             Write MIPS assembly to this file
  -r, --run                  Run the Generated MIPS assembly (make sure spim is
                             installed)
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to dishank.goel@iitgn.ac.in.
```
The usage:
```console
foo@bar:~$ ./mipcc --usage
Usage: mipcc [-r?V] [-f FILE] [-o FILE] [--in=FILE] [--out=FILE] [--run]
            [--help] [--usage] [--version] 
```

If ```-f``` is not provided, then mipcc reads code from `stdin` by default. 

### Sample Usage

```console
foo@bar:~$ ./mipcc -r -f examples/type_conversion.c
[*] Starting Parsing...

int main() {

    int x = 3.1415;
    print("x = ", x, "\n");
    print("(float)x = ", (float)x, "\n");
    float pi = x + 0.1415;
    print("pi = ", pi, "\n");
    print("floor(pi) = ", (int)pi, "\n");
    return 0;
    
}

[+] Finished Parsing!
[+] MIPS code written to mips.asm
[*] Running generated MIPS assembly using spim

SPIM Version 8.0 of January 8, 2010
Copyright 1990-2010, James R. Larus.
All Rights Reserved.
See the file README for a full copyright notice.
Loaded: /usr/lib/spim/exceptions.s
x = 3
(float)x = 3.00000000
pi = 3.14150000
floor(pi) = 3
```

To run the generated MIPS assembly without supplying the `-r` option, use the following command (to run mips.asm)

```console
foo@bar:~$ spim -file mips.asm
```
### Sample Programs
There are few programs present in [examples](./examples)

- [`quick_sort.c`](./examples/quick_sort.c): A recursive quick sort implemented in C
- [`knapsack.c`](./examples/knapsack.c): Knapsack problem using Dynamic programming (with 2-dimentional arrays) 
- [`seive.c`](./examples/seive.c): Sieve of Eratosthenes to find out primes less than certain number
- [`sqrt.c`](./examples/sqrt.c): Finding sqrt() of a number
- [`fibonacci.c`](./examples/fibonacci.c): Finding nth fibonacci numbers
- [`fizzbuzz.c`](./examples/fizzbuzz.c): The famous fizzbuzz problem in C
## Features Implemented

Currently, `mipcc` has following notable features:

- **Data types**: `int` and single precision `float` variables and `multi-dimensional arrays` with int and float types are supported. The size of arrays should be known at compiler time since they are stored on the stack. 
- **Functions**: Recursive function calls are supported. Any of the int, float and Arrays can be passed as arguments. The execution starts from the `main()` function. Functions can also either return or not return a value (VOID type).
- **Arrays**: Multi-dimensional int and float arrays are supported. The indexing can be done by arbritary complex expressions. 
- **Passing array to functions**: Arrays are passed by reference to the function. So, the changes made by the function are retained in the original array.   
- **Global variables**: Global variables can be declared with constant value like in standard C.
- **Scoping**: Variables are scoped according to C standard. There can be many nested scopes inside a function. 
- **Type conversion**: Implicit and explicit type conversion is supported for all expressions, return values of functions etc. So, the compiler automatically converts the type of the resulting expression to match context in which the expression is used.  
- **Conditionals**: `if` `else` construct is supported. Nested `else if` ladder is also supported.    
- **Expressions**: Arbitrary complex expressions are supported with different comparison, unary and binary operators. Here are the operators supported: ++, --, !, -, *, /, %, +, -, <<, >>, <, >, <=,  >=,  ==,  !=, &, &&, ^, |, ||. Assignment expressions are also supported.
- **Looping**: `while`, `do-while`, `for` constructs are supported. `break` and `continue` are also supported for these constructs. `for` construct is exactly like in standard C, with initialisation, propagation and termination.
- **IO capability**: `print` and `scan` functions can be used to perform IO operations. They automatically check the type of the variable and print or scan suitable value. They can have any number of arguments i.e. multiple expressions can be printed by a single print call and multiple values can be scanned by a single scan call. 
- **Comments**: Multi-line comments `/* */` and single line comments `//` are supported
- **Error Reporting**: The parsing is stopped when an error is encontered and suitable error message is printed in red color.

## Implementation details

Here are some implementation specific details in mipcc.

### Symbol Table

There are two type of symbol tables that are maintained. 

1) Local symbol table: Each function has its own symbol table which contain the local variables.  
2) Global symbol table: This stores all data related to functions and global variables.

The Symbol table has several type of information about a variable like
- Name and type of the variable
- Its address on the stack
- The scope
- Array dimensions
- Constant value (if global variable)
- The parent function of variable (if local variable) 
### Expression evaluation

Since mipcc is very basic compiler, it does not perform any optimisations while evaluating the expressions. It performs no register allocation i.e. all the expression results are stored in a new memory location on the stack. 

For `int` expressions, register `$t0` and `$t1` are used.
For `float` expressions, registers `$f0` and `$f1` are used. 
For `arrays`, the actual value is first loaded from the address present in the memory adress pointed by array.  
### Stack Allocation

mipcc uses base pointer `$fp` and stack pointer `$sp` to maintain the activation records. All the addressing is done relative to `$fp` since it does not move around as much as stack pointer. So, each variable has a fixed offset known at compile time from the base pointer. 
At the start of execution, `$fp = $sp`. 

The stack grows from bottom to top i.e. from higher memory address to lower memory address. reducing `$sp` results in allocating new space for variables. 4 bytes are allocated for `int` and `float`, 4*size_of_array bytes are allocated for arrays.

### Function calling

When a function is called, a new activation record is created by pushing the base pointer on the stack so that `$fp = $sp` again. The old `$fp` is stored at `0($sp)` so that when function return, the original activation record is restored.
#### Parameters

Each parameter is pushed on the stack first. In case arrays, the address of the starting of the array is pushed. After the parameters are pushed, old base pointer is pushed. After than new function's activation record starts.

The return address (`$ra`) is stored at `-4($fp)` of the new activation record and popped while returning. 

When a function returns, it restores the old stack pointer, the return address and deallocates its space on the stack by incrementing the stack pointer.

The return value is saved in `$v0` or `$f1` depending on the return type. 

### Array indexing

Multi-dimensional arrays are indexed by adding a suitable offset to the base address of the array. The calculating of the offset is done by knowing the fixed size of the array.
### Print and Scan

Currently, print and scan are not a real functions i.e. they are not invoked in similar manner as other functions, but there definitions are injected wherever they are used.

`print` is used to print any expression or string literals to users using syscalls. It can have any number of arguments, the arguments are printed from left to right.

`scan` is used to read user input for either `int` or `float` variable. It can also have multiple arguments with input being stored from left to right.

## Future Improvements

- Implement Dyanmic memory allocation using syscalls and creating a heap allocation manager
- Implement structs, points, more data types and string operations.
- Bound checking for array index access
- Improve register allocation 
- Ability to import other C files
- Injecting code to detect stack smashing
- Implement ASLR

## References

- [ANSI C11 lexer](http://www.quut.com/c/ANSI-C-grammar-l-2011.html)
- [ANSI C11 grammar](http://www.quut.com/c/ANSI-C-grammar-y-2011.html)
- [SPIM Reference](https://www.cse.iitm.ac.in/~krishna/cs3300/spim_ref.html)
- [Here](https://stackoverflow.com/questions/5812869/access-mips-co-processor-condition-flag-directly): For accessing coprocessor 1 condition flags for floating point comparisons
- [Here](https://www.cse.unsw.edu.au/~cs1521/18s2/notes/C/notes.html): General MIPS architecture
## Contributers

- Dishank Goel (18110052)
- Sachin Yadav (18110148)
- Raghav Goyal (18110135)