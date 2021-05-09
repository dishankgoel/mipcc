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

- **Data types**: `int` and single precision `float` variables and `multi-dimensional arrays` with int and float types. The size of arrays should be known at compiler time
- **functions**: Recursive function calls are supported. Any of int, float, Arrays can be passed as arguments. The execution starts from the `main()` function. Functions can also either return or not return a value (VOID type). Arrays are passed by reference to the function.  
- **Global variables**: Global variables can be declared with constant value like in standard C.
- **Scoping**: Variables are scoped according to C standard. There can be many nested scopes inside a function. 
- **Type conversion**: Implicit and explicit type conversion is supported for all expressions, return values of functions etc.  
- **Conditionals**: `if` `else` construct is supported    
- **Expressions**: Arbitrary complex expressions are supported with different comparison, unary and binary operators. Here are the operators supported: ++, --, !, -, *, /, %, +, -, <<, >>, <, >, <=,  >=,  ==,  !=, &, &&, ^, |, ||. Assignment expressions are also supported.
- **Looping**: `while`, `do-while`, `for` constructs are supported. `break` and `continue` are also supported for these constructs.
- **IO capability**: `print` and `scan` functions can be used to perform IO operations. They automatically check the type of the variable and print suitable value. They can have any number of arguments.
- **Comments**: Multi-line comments `/* */` and single line comments `//` are supported
- **Error Reporting**: The parsing is stopped when an error is encontered and suitable error message is printed in RED color.


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
- [Here](https://github.com/alphatron1999/MiniNim): For README structure and code organization 
## Contributers

- Dishank Goel (18110052)
- Sachin Yadav (18110148)
- Raghav Goyal (18110135)