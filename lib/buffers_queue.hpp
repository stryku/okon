#pragma once

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>
#include <vector>

namespace okon {
class buffers_queue
{
private:
  class semaphore
  {
  public:
    explicit semaphore(int count = 0);

    void notify();

    void wait();

  private:
    std::mutex m_mtx;
    std::condition_variable m_cv;
    int m_count;
  };

public:
  explicit buffers_queue(unsigned buffer_size, unsigned number_of_buffers);

  unsigned take_for_data_storing();
  void data_storing_ready();

  std::optional<unsigned> take_for_processing();
  void processing_ready();

  std::vector<uint8_t>& access_buffer(unsigned buffer_index);

  void notify_no_more_data();

private:
  unsigned next_to(unsigned val);

private:
  std::vector<std::vector<uint8_t>> m_buffers;
  semaphore m_storing_semaphore;
  semaphore m_processing_semaphore;
  unsigned m_write_buffer_index{ 0u };
  unsigned m_process_buffer_index{ 0u };
  bool m_has_more_data{ true };
};
}
