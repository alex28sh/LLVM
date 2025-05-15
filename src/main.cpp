
#include <argparse/argparse.hpp>
#include <fstream>

#include "../include/from_json.h"
#include "../include/generate_llvm.h"
#include "../include/statement.h"

using std::unique_ptr;

unique_ptr<llvm::LLVMContext> the_context;
unique_ptr<llvm::Module> the_module;
unique_ptr<llvm::IRBuilder<>> builder;

unique_ptr<llvm::FunctionPassManager> the_fpm;
unique_ptr<llvm::LoopAnalysisManager> the_lam;
unique_ptr<llvm::FunctionAnalysisManager> the_fam;
unique_ptr<llvm::CGSCCAnalysisManager> the_cgam;
unique_ptr<llvm::ModuleAnalysisManager> the_mam;
unique_ptr<llvm::PassInstrumentationCallbacks> the_pic;
unique_ptr<llvm::StandardInstrumentations> the_si;

void initialize_module(const std::vector<std::string> &opt_passes) {
  the_context = std::make_unique<llvm::LLVMContext>();
  the_module = std::make_unique<llvm::Module>("my cool jit", *the_context);
  builder = std::make_unique<llvm::IRBuilder<>>(*the_context);

  the_fpm = std::make_unique<llvm::FunctionPassManager>();
  the_lam = std::make_unique<llvm::LoopAnalysisManager>();
  the_fam = std::make_unique<llvm::FunctionAnalysisManager>();
  the_cgam = std::make_unique<llvm::CGSCCAnalysisManager>();
  the_mam = std::make_unique<llvm::ModuleAnalysisManager>();
  the_pic = std::make_unique<llvm::PassInstrumentationCallbacks>();
  the_si =
      std::make_unique<llvm::StandardInstrumentations>(*the_context,
                                                       /*DebugLogging*/ true);
  the_si->registerCallbacks(*the_pic, the_mam.get());

  std::unordered_map<std::string, std::function<void()>> pass_map = {
  {"InstCombine", []() { the_fpm->addPass(llvm::InstCombinePass()); }},
  {"Reassociate", []() { the_fpm->addPass(llvm::ReassociatePass()); }},
  {"GVN", []() { the_fpm->addPass(llvm::GVNPass()); }},
  {"SimplifyCFG", []() { the_fpm->addPass(llvm::SimplifyCFGPass()); }},
  {"MyConstantProp", []() { the_fpm->addPass(llvm::MyConstantProp()); }},
  {"MyBranchElim", []() { the_fpm->addPass(llvm::MyBranchElim()); }},
  {"MyCSE", []() { the_fpm->addPass(llvm::MyCSE()); }},
  };

  // Loop through the provided pass names and add them if found.
  for (const auto &pass_name : opt_passes) {
    if (auto it = pass_map.find(pass_name); it != pass_map.end()) {
      it->second();
    } else {
      llvm::errs() << "Warning: Unknown pass '" << pass_name << "'\n";
    }
  }

  llvm::PassBuilder pb;
  pb.registerModuleAnalyses(*the_mam);
  pb.registerFunctionAnalyses(*the_fam);
  pb.registerCGSCCAnalyses(*the_cgam);
  pb.registerLoopAnalyses(*the_lam);
  pb.crossRegisterProxies(*the_lam, *the_fam, *the_cgam, *the_mam);
}

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("LLVM");

  program.add_argument("input").help("input file");

  program.add_argument("output").help("output file");

  program.add_argument("--optimize")
      .help("List of passes to perform")
      .nargs(argparse::nargs_pattern::at_least_one);

  program.parse_args(argc, argv);

  std::ifstream f(program.get("input"));
  nlohmann::json data = nlohmann::json::parse(f);
  auto prg = prg_from_json(data);

  std::vector<std::string> opt_passes;
  if (program.present("--optimize")) {
    opt_passes = program.get<std::vector<std::string>>("--optimize");
  }

  initialize_module(opt_passes);

  if (!the_module) {
    throw std::runtime_error("failed to load module");
  }
  auto prg_ir = prg->codegen();

  std::error_code ec;
  llvm::raw_fd_ostream dest(program.get("output"), ec);
  if (ec) {
    llvm::errs() << "Could not open file: " << ec.message();
    return 1;
  }
  the_module->print(dest, nullptr);

  return 0;
}
