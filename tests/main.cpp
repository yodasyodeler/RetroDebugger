#include <DebuggerApi.h>

#include <fmt/core.h>

int main() {

    std::string cmdPrompt;
    GetCommandPrompt(&cmdPrompt);
    fmt::print("Its Here! {}\n", cmdPrompt);

    return 0;
}
