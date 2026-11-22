#include "ThreadPool.hpp"


int main() {
    int* data = new int[100];

    for (int i = 0; i < 100; i++) {
        data[i] = i;
    }

    return 0;
}
