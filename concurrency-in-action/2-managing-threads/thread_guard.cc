#include <iostream>
#include <thread>
#include <exception>

class ThreadGuard {
public:
    explicit ThreadGuard(std::thread& t) : m_thread(t) {}

    ~ThreadGuard() {
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    // Disable copy and move
    ThreadGuard(ThreadGuard const&) = delete;
    ThreadGuard& operator=(ThreadGuard const&) = delete;

private:
    std::thread& m_thread;
};

void task() {
    std::cout << "Thread running\n";
}

int main() {
    std::thread t(task);
    ThreadGuard guard(t);
}
