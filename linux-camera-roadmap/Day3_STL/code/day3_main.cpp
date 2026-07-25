#include <iostream>
#include <string>

#include "../include/bounded_queue.hpp"

int main() {
    BoundedQueue<std::string> queue(3);

    std::cout << std::boolalpha;

    std::cout << "Push frame_001: "
              << queue.push("frame_001") << '\n';

    std::cout << "Push frame_002: "
              << queue.push("frame_002") << '\n';

    std::cout << "Push frame_003: "
              << queue.push("frame_003") << '\n';

    std::cout << "Push frame_004: "
              << queue.push("frame_004") << '\n';

    std::cout << "Size: "
              << queue.size() << '\n';

    std::cout << "Capacity: "
              << queue.capacity() << '\n';

    std::cout << "Full: "
              << queue.full() << '\n';

    while (auto frame = queue.pop()) {
        std::cout << "Processing: "
                  << *frame << '\n';
    }

    std::cout << "Empty: "
              << queue.empty() << '\n';

    return 0;
}
