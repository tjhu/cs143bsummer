// Producer produces `num_data` amount of numbers and put them into the buffer
// one after another. Consumer consumes those numbers and output their sum.

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

#include <boost/program_options.hpp>

namespace po = boost::program_options;

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

int main(int argc, char **argv) {
  // Parse program arguments
  int n, k1, k2, k3;
  po::options_description desc("Program options");
  desc.add_options()("help", "list all flags");
  desc.add_options()("verbose", "produce help message");
  desc.add_options()("n", po::value<int>(&n)->required(),
                     "capacity of the buffer");
  desc.add_options()("k1", po::value<int>(&k1)->required(),
                     "P produces a burst of k1 items, 1 per time unit.");
  desc.add_options()("k2", po::value<int>(&k2)->required(),
                     "After producing an item, P waits for k2 time units until "
                     "the next burst of k1 item.");
  desc.add_options()("k3", po::value<int>(&k3)->required(),
                     "C removes 1 item every k3 time units.");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 0;
  }
  assert(vm.count("n"));

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