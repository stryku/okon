#include "buffers_queue.hpp"

namespace okon {
buffers_queue::semaphore::semaphore(int count)
  : m_count(count)
{
}

void buffers_queue::semaphore::notify()
{
  std::unique_lock<std::mutex> lock(m_mtx);
  ++m_count;
  m_cv.notify_one();
}

void buffers_queue::semaphore::wait()
{
  std::unique_lock<std::mutex> lock(m_mtx);
  while (m_count == 0) {
    m_cv.wait(lock);
  }
  --m_count;
}

buffers_queue::buffers_queue(unsigned buffer_size, unsigned number_of_buffers)
  : m_buffers{ number_of_buffers, std::vector<uint8_t>(buffer_size) }
  , m_processing_semaphore{ static_cast<int>(number_of_buffers) }
{
}

unsigned buffers_queue::take_for_data_storing()
{
  m_processing_semaphore.wait();
  const auto index_to_return = m_write_buffer_index;
  m_write_buffer_index = next_to(m_write_buffer_index);
  return index_to_return;
}

void buffers_queue::data_storing_ready()
{
  m_storing_semaphore.notify();
}

std::optional<unsigned> buffers_queue::take_for_processing()
{
  m_storing_semaphore.wait();

  if (!m_has_more_data && next_to(m_process_buffer_index) == m_write_buffer_index) {
    return std::nullopt;
  }

  const auto index_to_return = m_process_buffer_index;
  m_process_buffer_index = next_to(m_process_buffer_index);
  return index_to_return;
}

void buffers_queue::processing_ready()
{
  m_processing_semaphore.notify();
}

std::vector<uint8_t>& buffers_queue::access_buffer(unsigned buffer_index)
{
  return m_buffers[buffer_index];
}

void buffers_queue::notify_no_more_data()
{
  m_has_more_data = false;
  m_storing_semaphore.notify();
}

unsigned buffers_queue::next_to(unsigned val)
{
  return (val + 1u) % m_buffers.size();
}
}
