#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <memory>
#include <mutex>
#include <future>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
	T receive();
	void send(T &&msg);

private:
	std::mutex _mutex;
	std::condition_variable _cond;
	std::deque<T> _messages;
};

enum TrafficLightPhase
{
  red,
  green,
};

class TrafficLight: public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    TrafficLightPhase _currentPhase;
    std::shared_ptr<MessageQueue<TrafficLightPhase>> message_queue_;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif
