#include "RaychelCore/ArgumentParser.h"
#include <cstdlib>

static struct State{
    int i{0};
    float f{0};
    std::string str;

} state;

int main(int argc, char** argv)
{
    Logger::setMinimumLogLevel(Logger::LogLevel::debug);
    Raychel::ArgumentParser ap;

    ap.add_int_arg("iterations", "n", "number of iterations", state.i);

    if(!ap.parse(argc, argv)) {
        return EXIT_FAILURE;
    }

    Logger::log("STATE: ", state.i, " : ", state.f, " : ", state.str, '\n');

    return EXIT_SUCCESS;
}