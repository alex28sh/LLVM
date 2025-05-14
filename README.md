Project aimed at generating & optimizing LLVM code

### How to build: 
Run ```cmake CMakeLists.txt && make```

### How to run:

1. Convert example from original_programs (or your program) to json, by using
   [L-static-analyzer](https://github.com/alex28sh/L-static-analyzer)
```
$stack run
Type file to parse:
// input file
Type file to save json:
// output file
...
```
2. Run LLVM generator with arguments like 

```
./main
../json_examples/prg3.json     // input file          
../llvm_compiled/prg3.ll       // output file
--optimize                     // optimizations
MyConstantProp                 
MyBranchElim                   
```

### Optimizations supported right now:
* InstCombine - combine redundant instructions
* Reassociate - reassociate commutative expressions in an order that is designed to promote better constant propagation
* GVN - perform global value numbering to eliminate fully and partially redundant instructions
* SimplifyCFG - perform dead code elimination and basic block merging
* MyConstantProp - constant propagation
* MyBranchElim - branch elimination (if statement is always true or always false)

(1 - 4 are taken from the LLVM library, 5 - 6 are custom optimizations)

### Supported language features:
- Operations
```
-,+,*,/,% - arithmetic operations
==,!=,>,>=,<,<= - comparisons
&&,|| - logical operations
```
- IO 
```
read n // n - variable
```
```
write e // e - expression
write (n * 7 + 2) 
```
- Variable declaration (without initialization)
``` 
int a; // integer
int* a; // pointer to integer
```
- Functions
``` 
def gcd(int a, int b) { // types of arguments are required
...
}
```
- While loops
``` 
while(e) {
...
}
```
- If statements
```
if (e) {
}
```
```
if (e) {
} else {
}
```
- Integers
- Pointers and arrays
```
int* a;
a = new int[10]; // array of 10 integers
int** m; 
a = new int*[10]; // array of 10 pointers
m[0] = new int[4];
a[0] = 10;
read a[0];
write m[0][0] + m[0][1];
```

### Example of program (look original_programs):
```
def gcd(int a, int b) {
    while (a != 0 && b != 0) {
        if (a > b) {
            a := a - b;
        } else {
            b := b - a;
        }
    }
    return a + b;
}

def main() {
    int a;
    int b;
    read a;
    read b;
    write gcd(a, b);
    return 0;
}
```

### Additional notes:

For the In-tree integration of ```MyConstantProp``` and ```MyBranchElim``` follow the instructions, described [here](https://github.com/10x-Engineers/tutorial-llvm-pass/blob/main/tutorial_hello.md).
