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

For the In-tree integration of ```MyConstantProp``` follow the instructions, described [here](https://github.com/10x-Engineers/tutorial-llvm-pass/blob/main/tutorial_hello.md).
