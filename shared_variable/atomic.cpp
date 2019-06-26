// Demo a race condition where two threads trying to modify a shared value
// without synchronization.

#include <atomic>
#include <iostream>
#include <thread>

std::atomic<int> counter;

// increments `counter` 10 million times
void foo() {
  for (int i = 0; i < 1E7; ++i) {
    // implemented using read-modify-write functions in clang
    // https://llvm.org/docs/Atomics.html#id17
    // https://gcc.gnu.org/wiki/Atomic/GCCMM/LIbrary
    counter++;
  }
}

int main() {
  // create two threads, which each calls foo once
  counter = 0;
  std::thread t1(foo);
  std::thread t2(foo);
  t1.join();
  t2.join();
  std::cout << counter << std::endl;
  return 0;
}