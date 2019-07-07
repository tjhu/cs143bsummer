// Producer produces `num_data` amount of numbers and put them into the buffer
// one after another. Consumer consumes those numbers and output their sum.

// Questions:
// 1. Let's say n=1, k1=3, k2=2, k3=2, and both P and C are ready to work at
// t=0. Basically sleep_for or sleep_until
#include "circular_buffer.hpp"

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// The number of data we produce/consume
const int64_t num_data = 1E3;
// Size of one time unit
const auto time_unit_size = std::chrono::milliseconds(1);

class Producer {

public:
  Producer(Buffer *buffer, int k1, int k2)
      : buffer_(buffer), i_(0), k1_(k1), k2_(k2) {}

  void produce(int data) {
    // Produce
    std::this_thread::sleep_for(time_unit_size);
    buffer_->put(data);

    // After producing every k1 items, we take a nap that's k2 long
    if (++i_ % k2_ == 0) {
      std::this_thread::sleep_for(time_unit_size * k2_);
    }
  }

  Buffer *buffer_;
  int i_;
  int k1_;
  int k2_;
};

class Consumer {
public:
  Consumer(Buffer *buffer, int k3) : buffer_(buffer), k3_(k3) {}

  int consume() {
    std::this_thread::sleep_for(time_unit_size * k3_);
    return buffer_->get();
  }

private:
  Buffer *buffer_;
  int k3_;
};

void producer_fn(Buffer *buffer, int k1, int k2) {
  Producer p(buffer, k1, k2);
  for (int i = 0; i < num_data; i++) {
    p.produce(i + 1);
  }
}

void consumer_fn(Buffer *buffer, uint64_t *result, int k3) {
  Consumer c(buffer, k3);
  for (int i = 0; i < num_data; i++) {
    *result += c.consume();
  }
}

// arg1: n, size of the circular array
// arg2: k1, P produces a burst of k1 items, 1 per time unit.
// arg3: k2, Then P waits for k2 time units until the next burst of k1 item.
// arg4: k3, C removes 1 item every k3 time units.
int main(int argc, char **argv) {
  assert(argc == 5);
  int n, k1, k2, k3;
  try {
    n = std::stoi(argv[1]);
    k1 = std::stoi(argv[2]);
    k2 = std::stoi(argv[3]);
    k3 = std::stoi(argv[4]);
  } catch (std::invalid_argument &e) {
    std::cout << "Please provide 4 integers for the arguments" << std::endl;
  }

  Buffer buffer(n);
  uint64_t result = 0;
  std::thread producer_thread(std::bind(producer_fn, &buffer, k1, k2));
  std::thread consumer_thread(std::bind(consumer_fn, &buffer, &result, k3));
  producer_thread.join();
  consumer_thread.join();
  std::cout << "expected: " << (num_data + 1) * (num_data / 2) << std::endl;
  std::cout << "result: " << result << std::endl;
  return 0;
}