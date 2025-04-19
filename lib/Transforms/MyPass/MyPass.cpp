#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/Passes/StandardInstrumentations.h"

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

#include  <iostream>

using namespace llvm;

//namespace {

//struct MyPass : public PassInfoMixin<MyPass> {

PreservedAnalyses MyPass::run(Function &F, FunctionAnalysisManager &FAM) {

    std::cout << "MyPass in function: " << F.getName().str() << std::endl;

    return PreservedAnalyses::all();
}
//};
//}

// This part is the new way of registering your pass
//extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
//llvmGetPassPluginInfo() {
//    return {
//        LLVM_PLUGIN_API_VERSION, "MyPass", "v0.1",
//        [](PassBuilder &PB) {
//            PB.registerPipelineParsingCallback(
//              [](StringRef Name, FunctionPassManager &FPM,
//              ArrayRef<PassBuilder::PipelineElement>) {
//                if(Name == "my-pass"){
//                  FPM.addPass(MyPass());
//                  return true;
//                }
//                return false;
//              }
//            );
//        }
//    };
//}