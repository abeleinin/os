#include <iostream>
#include <list>

template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input) {
    if (input.empty()) {
        return input;
    }

    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    T const& pivot = *result.begin();

    auto divide_point = std::partition(input.begin(), input.end(),
            [&](T const& t){ return t < pivot; });

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

    auto new_lower(sequential_quick_sort(std::move(lower_part)));
    auto new_higher(sequential_quick_sort(std::move(input)));

    result.splice(result.end(), new_higher);
    result.splice(result.begin(), new_lower);

    return result;
}


int main() {
    std::list<int> lst = {5, 7, 3, 4, 1, 9, 2, 8, 10, 6};

    auto res = sequential_quick_sort(lst);

    for (int el : res) {
        std::cout << el << std::endl;
    }
}