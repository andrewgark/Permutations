#include "tests.h"
#include <iostream>

int main()
{
    ui32 queriesNumber = 500, arraySize = 100;
    GlobalTestResult globalTestResult;
    globalTestResult.execute(arraySize, queriesNumber);
    if (globalTestResult.isSucceeds())
        std::cout << "Testing succeeds\n";
    else
        std::cout << "Testing failed\n";
    return 0;
}
