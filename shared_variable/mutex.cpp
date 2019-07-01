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
    // this thread yields if the lock is acquired already
    // https://github.com/llvm-mirror/libcxx/blob/de635b64f8c4e41da218ff234c8d7d5877f2f1c9/include/mutex#L384
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