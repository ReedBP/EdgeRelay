#include "pipeline.h"

#include <iostream>

int main() {
    std::cout << "[EdgeRelay] phase1-pr1 starting pipeline\n";
    edgerelay::run_pipeline(5);
    std::cout << "[EdgeRelay] phase1-pr1 finished\n";
    return 0;
}
