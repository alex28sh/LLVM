#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Function.h"

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