Project aimed at generating & optimizing LLVM code

json_examples are examples from [L-Static-Analyzer](https://github.com/alex28sh/L-static-analyzer)

Run with arguments like 

```
../json_examples/prg3                
../llvm_compiled/llvm3_expr_self_opt.ll
--optimize
MyConstantProp
MyBranchElim
```

1) input file
2) output file
3) --optimize - accepts a list of names of optimizations to be applied

Supported language features:
- Functions
- While loops
- If statements

Implemented optimizations:
- Constant propagation
- Branch elimination (if statement is always true or always false)

Example of program (look original_programs):
```
def gcd(a, b) {
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
    var a;
    var b;
    read a;
    read b;
    write gcd(a, b);
    return 0;
}
```

For the In-tree integration of ```MyConstantProp``` and ```MyBranchElim``` follow the instructions, described [here](https://github.com/10x-Engineers/tutorial-llvm-pass/blob/main/tutorial_hello.md).
