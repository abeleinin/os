#include <vector>
#include <thread>
#include <mutex>
#include <iostream>

struct Node {
    Node(int val) : m_val(val) {}

    Node(int val, Node* next, Node* prev) 
        : m_val(val)
        , m_next(next)
        , m_prev(prev)
    {}

    int m_val;
    Node* m_next;
    Node* m_prev;
    std::mutex m_mutex;
};

class DoubleLinkedList {
public:
    DoubleLinkedList() { 
        m_dummy = new Node(0);
        m_dummy->m_next = m_dummy;
        m_dummy->m_prev = m_dummy;
        m_size = 0;
    }

    ~DoubleLinkedList(); // Probably should clean up the Node's allocated

    void append(int val) {
        if (m_size == 0) {
            std::lock_guard guard(m_dummy->m_mutex);

            Node* new_node = new Node(val, m_dummy, m_dummy);

            m_dummy->m_next = new_node;
            m_dummy->m_prev = new_node;
        } else {
            Node* tail = m_dummy->m_prev;
            std::scoped_lock guard(m_dummy->m_mutex, tail->m_mutex); // C++17 class template argument deduction

            Node* new_node = new Node(val, tail, m_dummy);

            tail->m_next = new_node;
            m_dummy->m_prev = new_node;
        }

        ++m_size;
    }

    void print() const {
        int i = 0;
        Node* curr = m_dummy->m_next;
        while (i < m_size) {
            std::lock_guard guard(curr->m_mutex);
            std::cout << "Node " << i << " = " << curr->m_val << std::endl;
            curr = curr->m_next;
            ++i;
        }
    }

    void reverse_print() const {
        int i = 0;
        Node* curr = m_dummy->m_prev;
        while (i < m_size) {
            std::lock_guard guard(curr->m_mutex);
            std::cout << "Node " << i << " = " << curr->m_val << std::endl;
            curr = curr->m_prev;
            ++i;
        }
    }

private:
    Node* m_dummy;
    size_t m_size = 0;
};

int main() {

    auto dll = new DoubleLinkedList();

    dll->append(10);
    dll->append(20);
    dll->append(30);
    dll->append(40);
    dll->append(50);

    dll->reverse_print();
    // std::vector<std::thread> threads;

    // threads.emplace_back(&DoubleLinkedList::print, std::ref(*dll));
    // threads.emplace_back(&DoubleLinkedList::reverse_print, std::ref(*dll));

    // for (auto& t : threads) {
    //     t.join();
    // }

    return 0;
}
