#pragma once 
// 同一個 .cpp 在一次編譯過程中，只載入這個 header 一次。
/* 也可以使用 include guard：
    #ifndef FRAME_HPP
    #define FRAME_HPP
    struct Frame {
        int width;
        int height;
    };
    #endif  
*/ 

#include <cstdint>
/* 固定寬度整數型別
    std::uint8_t   8-bit 無號整數   0 到 255
    std::int16_t   16-bit 有號整數  -32768 到 32767
    std::uint32_t  32-bit 無號整數  0 到約 42.9 億
    std::int64_t   64-bit 有號整數  很大的正負整數
    std::uint64_t  64-bit 無號整數  很大的非負整數

    # int long 在不同平台上的大小可能不同。而std::uint64_t 明確表示一個剛好 64-bit 的無號整數
    # 像素資料也常用：std::uint8_t pixel = 255;
*/

#include <vector>

// 表示一張影像，以及它對應的基本資訊與像素資料
struct Frame {
    int width; // 影像寬度
    int height; // 影像高度
    int channels; // 通道數
    std::uint64_t timestamp; // 時間戳記，通常只需要非負數，而且數值可能很大
    std::vector<unsigned char> data; //真正的像素資料

    // 建構函式，建立 Frame 物件時會自動執行
    Frame(int input_width, int input_height, int input_channels, std::uint64_t input_timestamp)
          // member initializer list 成員初始化
        : width(input_width), 
          height(input_height), 
          channels(input_channels), 
          timestamp(input_timestamp),

          // 建立一維data vector，直接配置所需的元素數量
          // 影像概念上是height × width × channels 
          // 但記憶體中會攤平成一條連續的一維資料：[B,G,R, B,G,R, B,G,R, ...]
          /* 輸入資料先轉成 std::size_t : 
             width 原本是 int，但 std::vector 的大小通常使用 std::size_t，
             這是一個無號整數型別，足夠大以容納任何可能的陣列大小。
             因此將 width、height、channels 轉換為 std::size_t，以確保計算結果不會溢位。 */
          data(
              static_cast<std::size_t>(width) *
              static_cast<std::size_t>(height) *
              static_cast<std::size_t>(channels)
          ) { // 所有成員都已經在初始化列表初始化完成，所以建構函式本體不需要再做其他工作。
    }

    std::size_t byteSize() const {
        return data.size();
    }
};

