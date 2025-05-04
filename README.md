Project aimed at generating & optimizing LLVM code

json_examples are examples from [L-Static-Analyzer](https://github.com/alex28sh/L-static-analyzer)

Run with arguments like 

```
../json_examples/prg3                
../llvm_compiled/llvm3_expr_self_opt.ll
--optimize
```

1) input file
2) output file
3) --optimize - optional flag to perform optimizations (right now only library optimizations, like InstCombinePass, SimplifyCFGPass ...)

For the In-tree integration of ```MyPass``` follow the instructions, described [here](https://github.com/alexjung/Writing-an-LLVM-Pass-using-the-new-PassManager?tab=readme-ov-file).
