// Demo a race condition where two threads trying to modify a shared value
// without synchronization.

#include <iostream>
#include <mutex>
#include <thread>

int counter;

// increments `counter` 10 million times
void foo() {
  static std::mutex mtx;
  for (int i = 0; i < 1E7; ++i) {
    mtx.lock();
    counter++;
    mtx.unlock();
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