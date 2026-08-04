#include <cstdio>
#include <cstdlib>
#include <string_view>

namespace {
void print_version(std::FILE* output) {
  std::fputs("MC-LAB-CORE CLI " MC_LAB_CORE_VERSION "\n",
             output);  // LCOV_EXCL_EXCEPTION_BR_LINE
}

void print_help(std::FILE* output) {
  std::fputs("Usage: mc-lab [option]\n"
             "\n"
             "MC-LAB-CORE command-line interface.\n"
             "\n"
             "Options:\n"
             "  -h, --help       Show this help and exit.\n"
             "      --version    Show the product version and exit.\n",
             output);
}
}  // namespace

int main(int argc, char* argv[]) {
  if (argc == 1) {
    print_help(stdout);
    return EXIT_SUCCESS;
  }

  if (argc == 2) {
    const std::string_view argument{argv[1]};

    if (argument == "-h" || argument == "--help") {

      print_help(stdout);  // LCOV_EXCL_EXCEPTION_BR_LINE
      return EXIT_SUCCESS;
    }

    if (argument == "--version") {
      print_version(stdout);  // LCOV_EXCL_EXCEPTION_BR_LINE
      return EXIT_SUCCESS;
    }
  }

  std::fputs("mc-lab: unsupported command line\n"
             "Run 'mc-lab --help' for usage.\n",
             stderr);

  return EXIT_FAILURE;
}
