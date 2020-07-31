#include "threadPool.h"

#include <iostream>

using namespace budd;

int f1(int a, int b) {

    for (int i = 0; i < 10; i++) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    return a + b;
}

int main() {

    ThreadPool pool(2);

    auto feture1 = pool.enqueue(f1, 1, 2);
    auto feture2 = pool.enqueue(f1, 3, 4);

    auto res1 = feture1.get();
    auto res2 = feture2.get();

    std::cout << "res1 : " << res1 << std::endl;
    std::cout << "res2 : " << res2 << std::endl;

}