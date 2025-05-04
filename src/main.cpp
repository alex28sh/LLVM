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

void InitializeModule(const std::vector<std::string>& optPasses) {
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

    std::unordered_map<std::string, std::function<void()>> passMap = {
        {"InstCombine", []() { TheFPM->addPass(InstCombinePass()); }},
        {"Reassociate", []() { TheFPM->addPass(ReassociatePass()); }},
        {"GVN",         []() { TheFPM->addPass(GVNPass()); }},
        {"SimplifyCFG", []() { TheFPM->addPass(SimplifyCFGPass()); }},
        {"MyConstantProp",      []() { TheFPM->addPass(MyConstantProp()); }},
        {"MyBranchElim",        []() { TheFPM->addPass(MyBranchElim()); }},
    };

    // Loop through the provided pass names and add them if found.
    for (const auto& passName : optPasses) {
        auto it = passMap.find(passName);
        if (it != passMap.end()) {
            it->second();
        } else {
            errs() << "Warning: Unknown pass '" << passName << "'\n";
        }
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
        .help("List of passes to perform")
        .nargs(argparse::nargs_pattern::at_least_one);

    program.parse_args(argc, argv);

    std::ifstream f(program.get("input"));
    nlohmann::json data = nlohmann::json::parse(f);
    auto prg = prg_from_json(data);

    std::vector<std::string> optPasses;
    if (program.present("--optimize")) {
        optPasses = program.get<std::vector<std::string>>("--optimize");
    }

    InitializeModule(optPasses);

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
