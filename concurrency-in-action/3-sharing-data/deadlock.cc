#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex mutexA;
std::mutex mutexB;

void thread1() {
    std::lock_guard<std::mutex> lockA(mutexA);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Give thread2 time to lock mutexB
    std::lock_guard<std::mutex> lockB(mutexB); // Deadlock likely here

    std::cout << "Thread 1 acquired both mutexes\n";
}

void thread2() {
    std::lock_guard<std::mutex> lockB(mutexB);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Give thread1 time to lock mutexA
    std::lock_guard<std::mutex> lockA(mutexA); // Deadlock likely here

    std::cout << "Thread 2 acquired both mutexes\n";
}

int main() {
    std::thread t1(thread1);
    std::thread t2(thread2);

    t1.join();
    t2.join();

    std::cout << "Finished execution\n";
    return 0;
}
