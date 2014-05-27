#include "engine.h"
#include <iostream>

int main()
{
    try {
        cEngine engine;
        engine.run();
        return 0;
    } catch (std::runtime_error e) {
        std::cout << e.what();
        return 1;
    }
}