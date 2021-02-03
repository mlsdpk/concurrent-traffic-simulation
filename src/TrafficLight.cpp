#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <class T>
T MessageQueue<T>::receive()
{
    // perform queue modification under the lock
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_messages.empty(); }); // pass unique lock to condition variable

    // remove last vector element from queue
    T msg = std::move(_messages.back());
    _messages.pop_back();

    return msg; // will not be copied due to return value optimization (RVO) in C++
}

template <class T>
void MessageQueue<T>::send(T &&msg)
{
    // perform vector modification under the lock
    std::lock_guard<std::mutex> uLock(_mutex);

    // add vector to queue
    _messages.push_back(std::move(msg));
    _cond.notify_one(); // notify client after pushing new Vehicle into vector
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    message_queue_ = std::make_shared<MessageQueue<TrafficLightPhase>>();
}

TrafficLight::~TrafficLight() {}

void TrafficLight::waitForGreen()
{
    while (true) {
      auto message = message_queue_->receive();

      if (message == TrafficLightPhase::green) { return; }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // launch cycleThroughPhases function in a thread
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // print id of the current thread
    std::unique_lock<std::mutex> lck(_mtx);
    std::cout << "TrafficLight #" << _id << "::cycleThroughPhases: thread id = " << std::this_thread::get_id() << std::endl;
    lck.unlock();

    // initalize variables
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(4, 6);
    int cycleDuration = distr(eng); // duration of a single simulation cycle in seconds

    // init stop watch
    auto lastUpdate = std::chrono::system_clock::now();
    while (true) {
      // sleep at every iteration to reduce CPU usage
      std::this_thread::sleep_for(std::chrono::milliseconds(1));

      // compute time difference to stop watch
      long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lastUpdate).count();
      if (timeSinceLastUpdate >= cycleDuration) {

        // toggles the current phase of the traffic light between red and green
        _currentPhase = _currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;

        // sends an update method to the message queue using move semantics
        auto msg = _currentPhase;
        auto ftr = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, message_queue_, std::move(msg));
        ftr.wait();

        // randomly select the next simulation cycle
        cycleDuration = distr(eng);

        // reset stop watch for next cycle
        lastUpdate = std::chrono::system_clock::now();
      }
    }
}
