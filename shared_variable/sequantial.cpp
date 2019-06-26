// Demo a race condition where two threads trying to modify a shared value
// without synchronization.

#include <iostream>
#include <thread>

int counter;

// increments `counter` 1 million times
void foo() {
  for (int i = 0; i < 1E6; ++i) {
    counter++;
  }
}

int main() {
  // call foo twice
  counter = 0;
  foo();
  foo();
  std::cout << counter << std::endl;
  return 0;
}
