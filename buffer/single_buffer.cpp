// Producer produces 1 million numbers and put them into the buffer one after
// another. Consumer consumes those numbers and output their sum.
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

// The number of data we produce/consume
const int64_t num_data = 1E6;

// a buffer that holds an integer
class Buffer {
public:
  // Wail till the buffer is not full then append the data into the buffer
  void put(int data) {
    std::unique_lock<std::mutex> lck(mtx_);
    if (is_full()) {
      cv.wait(lck, std::bind(&Buffer::is_not_full, this));
    } 
    data_ = data;
    is_full_ = true;
    cv.notify_one();
  }

  // Wail till the buffer is not empty then get the first data into the buffer
  int get() {
    std::unique_lock<std::mutex> lck(mtx_);
    if (!is_not_empty()) {
      cv.wait(lck, std::bind(&Buffer::is_not_empty, this));
    }
    is_full_ = false;
    cv.notify_one();
    return data_;
  }

  bool is_not_full() { return !is_full(); }

  bool is_not_empty() { return is_full(); }

  bool is_full() { return is_full_; }

private:
  std::mutex mtx_;
  std::condition_variable cv;
  bool is_full_ = false;
  int data_;
};

void producer_fn(Buffer *buffer) {
  for (int i = 0; i < num_data; i++) {
    buffer->put(i + 1);
  }
}

void consumer_fn(Buffer *buffer, uint64_t *result) {
  for (int i = 0; i < num_data; i++) {
    *result += buffer->get();
  }
}

int main() {
  Buffer buffer;
  uint64_t result = 0;

  std::thread producer_thread(std::bind(producer_fn, &buffer));
  std::thread consumer_thread(std::bind(consumer_fn, &buffer, &result));
  producer_thread.join();
  consumer_thread.join();
  std::cout << "expected: " << (num_data + 1) * (num_data / 2)
            << std::endl;
  std::cout << "result: " << result << std::endl;
  return 0;
}