// Demo a race condition where two threads trying to modify a shared value
// synchronized with Peterson's method.
// Implemented the same way as the zybooks does.

#include <atomic>
#include <iostream>
#include <thread>

int counter, will_wait, c1, c2;

// called by thread t1
// increments `counter` 10 million times
void foo() {
  for (int i = 0; i < 1E6; ++i) {
    c1 = 1;
    will_wait = 1;
    // ask GCC and the CPU to not do out-of-order execution
    // this will insert mfence instruction to the assembly on x86
    // https://github.com/mit-pdos/xv6-public/blob/1d19081efbb9517d07c7e6c1a8393c6343ba7817/spinlock.c#L38
    __sync_synchronize();
    while (c2 && (will_wait == 1)) {
      // spin till acquire the lock
    }
    __sync_synchronize();

    // critical section
    counter++;

    // release the lock
    c1 = 0;
  }
}

void bar() {
  for (int i = 0; i < 1E6; ++i) {
    c2 = 1;
    will_wait = 2;
    __sync_synchronize();
    while (c1 && (will_wait == 2)) {
      // spin till acquire the lock
    }
    __sync_synchronize();

    // critical section
    counter++;

    // release the lock
    c2 = 0;
  }
}

int main() {
  // create two threads, which each calls foo once
  counter = 0;
  std::thread t1(foo);
  std::thread t2(bar);
  t1.join();
  t2.join();
  std::cout << counter << std::endl;
  return 0;
}