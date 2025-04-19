#include <fstream>

#include "../include/statement.h"
#include "../include/from_json.h"
#include "../include/generate_llvm.h"

#include <argparse/argparse.hpp>

std::unique_ptr<LLVMContext> TheContext;
std::unique_ptr<Module> TheModule;
std::unique_ptr<IRBuilder<>> Builder;

std::unique_ptr<FunctionPassManager> TheFPM;
std::unique_ptr<LoopAnalysisManager> TheLAM;
std::unique_ptr<FunctionAnalysisManager> TheFAM;
std::unique_ptr<CGSCCAnalysisManager> TheCGAM;
std::unique_ptr<ModuleAnalysisManager> TheMAM;
std::unique_ptr<PassInstrumentationCallbacks> ThePIC;
std::unique_ptr<StandardInstrumentations> TheSI;

void InitializeModule(bool optimize) {
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("my cool jit", *TheContext);
    Builder = std::make_unique<IRBuilder<>>(*TheContext);
    
    TheFPM = std::make_unique<FunctionPassManager>();
    TheLAM = std::make_unique<LoopAnalysisManager>();
    TheFAM = std::make_unique<FunctionAnalysisManager>();
    TheCGAM = std::make_unique<CGSCCAnalysisManager>();
    TheMAM = std::make_unique<ModuleAnalysisManager>();
    ThePIC = std::make_unique<PassInstrumentationCallbacks>();
    TheSI = std::make_unique<StandardInstrumentations>(*TheContext,
                                                       /*DebugLogging*/ true);
    TheSI->registerCallbacks(*ThePIC, TheMAM.get());

    if (optimize) {
        TheFPM->addPass(InstCombinePass());
        TheFPM->addPass(ReassociatePass());
        TheFPM->addPass(GVNPass());
        TheFPM->addPass(SimplifyCFGPass());
        TheFPM->addPass(MyPass());
    }

    PassBuilder PB;
    PB.registerModuleAnalyses(*TheMAM);
    PB.registerFunctionAnalyses(*TheFAM);
    PB.registerCGSCCAnalyses(*TheCGAM);
    PB.registerLoopAnalyses(*TheLAM);
    PB.crossRegisterProxies(*TheLAM, *TheFAM, *TheCGAM, *TheMAM);
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("LLVM");

    program.add_argument("input")
        .help("input file");

    program.add_argument("output")
        .help("output file");

    program.add_argument("--optimize")
        .default_value(false)
        .implicit_value(true)
        .help("optimize");

    program.parse_args(argc, argv);

    std::ifstream f(program.get("input"));
    nlohmann::json data = nlohmann::json::parse(f);
    auto prg = prg_from_json(data);

    InitializeModule(program.get<bool>("optimize"));

    if  (!TheModule) {
        throw std::runtime_error("failed to load module");
    }
    auto PrgIR = prg->codegen();

    std::error_code EC;
    raw_fd_ostream dest(program.get("output"), EC);
    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return 1;
    }
    TheModule->print(dest, nullptr);

    return 0;
}
