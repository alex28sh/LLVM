#include <fstream>

#include "../include/statement.h"
#include "../include/from_json.h"
#include "../include/generate_llvm.h"

std::unique_ptr<LLVMContext> TheContext;
std::unique_ptr<Module> TheModule;
std::unique_ptr<IRBuilder<>> Builder;

void InitializeModule() {
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("my cool jit", *TheContext);
    Builder = std::make_unique<IRBuilder<>>(*TheContext);
}

int main(int argc, char* argv[]) {
    std::ifstream f(argv[1]);
    nlohmann::json data = nlohmann::json::parse(f);
    auto prg = prg_from_json(data);

    InitializeModule();

    if  (!TheModule) {
        throw std::runtime_error("failed to load module");
    }
    auto PrgIR = prg->codegen();

    std::error_code EC;
    raw_fd_ostream dest(argv[2], EC);
    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return 1;
    }
    TheModule->print(dest, nullptr);

    return 0;
}
