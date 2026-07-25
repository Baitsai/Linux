#include <cstddef>
#include <iostream>

class BadFrame {
public:
    BadFrame(int width, int height, int channels)
        : width_(width),
          height_(height),
          channels_(channels),
          size_(
              static_cast<std::size_t>(width) *
              static_cast<std::size_t>(height) *
              static_cast<std::size_t>(channels)
          ),
          data_(new unsigned char[size_]) {
        std::cout << "Allocated " << size_ << " bytes\n";
    }

    ~BadFrame() {
        std::cout << "BadFrame destroyed\n";

        // 故意漏掉：
        // delete[] data_;
    }

private:
    int width_;
    int height_;
    int channels_;
    std::size_t size_;
    unsigned char* data_;
};

int main() {
    {
        BadFrame frame(640, 480, 3);
    }

    std::cout << "Program finished\n";
    return 0;
}