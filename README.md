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
