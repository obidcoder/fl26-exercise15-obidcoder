// thread_safe_vector.hpp: starter code -- please refer to in-class example for ThreadSafeQueue
#ifndef THREAD_SAFE_VECTOR_HPP
#define THREAD_SAFE_VECTOR_HPP

#include <vector>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeVector {
public:
  void push_back(const T& value);
  bool empty() const;
  bool try_pop_back(T& popped_value);
  void wait_and_pop_back(T& popped_value);
private:
  std::vector<T> the_vector;                       
  mutable std::mutex the_mutex;                   
  std::condition_variable the_condition_variable;  
};

template<typename T>
bool ThreadSafeVector<T>::empty() const {
  std::lock_guard<std::mutex> lock(the_mutex); //locks now
  return the_vector.empty();
} //unlocks here, automatically

// push_back(): add, unlock, then notify
template<typename T>
void ThreadSafeVector<T>::push_back(const T& value) {
  std::unique_lock<std::mutex> lock(the_mutex);
  the_vector.push_back(value);
  lock.unlock(); //unlock before notify
  the_condition_variable.notify_one(); //wake one waiting thread
}

// try_pop_back(): take one if there is one, never waits
template<typename T>
bool ThreadSafeVector<T>::try_pop_back(T& popped_value) {
  std::lock_guard<std::mutex> lock(the_mutex);
  if (the_vector.empty()) {
    return false;                                  
  }
  popped_value = the_vector.back();                
  the_vector.pop_back();                           
  return true;
}

// wait_and_pop_back(): sleep until there is data
template<typename T>
void ThreadSafeVector<T>::wait_and_pop_back(T& popped_value) {
  std::unique_lock<std::mutex> lock(the_mutex);
  while (the_vector.empty()) {                    
    the_condition_variable.wait(lock);             
  }
  popped_value = the_vector.back();
  the_vector.pop_back();
}

#endif