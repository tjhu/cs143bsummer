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
  for (int i = 0; i < 1E7; ++i) {
    c1 = 1;
    will_wait = 1;
    while (c2 && (will_wait==1)) {
      // spin till acquire the lock
    }
    
    // critical section
    counter++;

    // release the lock
    c1 = 0;
  }
}

void bar() {
  for (int i = 0; i < 1E7; ++i) {
    c2 = 1;
    will_wait = 2;
    while (c1 && (will_wait == 2)) {
      // spin till acquire the lock
    }
    
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