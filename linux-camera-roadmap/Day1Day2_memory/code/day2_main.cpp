#include <iostream>
#include <memory>

#include "frame.hpp"

void printFrame(const Frame& frame) {
    std::cout << "Resolution: "
              << frame.width << " x "
              << frame.height << '\n';

    std::cout << "Channels: "
              << frame.channels << '\n';

    std::cout << "Memory: "
              << frame.byteSize()
              << " bytes\n";
}

/* RAII 的核心思想：將資源生命週期綁定在物件生命週期上。
資源不只包含記憶體，也包括：檔案, Mutex, Socket, Camera handle, GPU buffer
*/

int main() {
    // frame 的型別是：std::unique_ptr<Frame>
    // 建立 heap 上的 Frame，但 unique_ptr 會在 main() 結束時自動釋放，因此正常情況不會洩漏
    auto frame = std::make_unique<Frame>( 640, 480, 3,1000);
    printFrame(*frame);
    return 0;
}