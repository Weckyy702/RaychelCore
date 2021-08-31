#include "RaychelCore/ArgumentParser.h"
#include <cstdlib>

static struct State{

    bool b{false};
    int i{0};
    float f{0};
    std::string str;

} state;

int main(int argc, char** argv)
{
    Raychel::ArgumentParser ap;

    ap.add_bool_arg("bool_arg", state.b);
    ap.add_int_arg("int_arg", state.i);
    ap.add_float_arg("float_arg", state.f);
    ap.add_string_arg("string_arg", state.str);

    if(!ap.parse(argc, argv)) {
        return EXIT_FAILURE;
    }

    Logger::log("STATE: ", state.b, " : ", state.i, " : ", state.f, " : ", state.str, '\n');

    return EXIT_SUCCESS;
}