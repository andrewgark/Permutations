#include "tests.h"
#include <iostream>

int main()
{
    ui32 queriesNumber = 50000;
    GlobalTestResult globalTestResult(queriesNumber);
    if (globalTestResult.succeeds)
        std::cout << "Testing succeeds\n" << std::endl;
    else
        std::cout << "Testing failed\n" << std::endl;
    return 0;
}
