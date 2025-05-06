#pragma once

#include <map>
#include <memory>
#include <string>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"

using std::unique_ptr;

extern unique_ptr<llvm::LLVMContext> the_context;
extern unique_ptr<llvm::Module> the_module;
extern unique_ptr<llvm::IRBuilder<>> builder;
extern std::map<std::string, llvm::Value *> named_values;

extern llvm::Constant *format_str_constant;
extern llvm::ArrayType *format_str_type;
extern llvm::GlobalVariable *format_str_global;

extern unique_ptr<llvm::FunctionPassManager> the_fpm;
extern unique_ptr<llvm::LoopAnalysisManager> the_lam;
extern unique_ptr<llvm::FunctionAnalysisManager> the_fam;
extern unique_ptr<llvm::CGSCCAnalysisManager> the_cgam;
extern unique_ptr<llvm::ModuleAnalysisManager> the_mam;
extern unique_ptr<llvm::PassInstrumentationCallbacks> the_pic;
extern unique_ptr<llvm::StandardInstrumentations> the_si;
