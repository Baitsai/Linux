# Day 2: Hello Memory

linux-camera-roadmap/  
├─ CMakeLists.txt  
├─ README.md  
├─ notes/  
│   └── Day2.md  
├─ code/  
│   ├── main.cpp  
│   └── memory_demo.cpp ◀︎  
└─ include/  
    └── frame.hpp ◀︎  

This directory contains public C++ header files.

- `frame.hpp`: Defines the `Frame` structure.
- `camera.hpp`: Declares camera-related interfaces.

## Frame struct: include/frame.hpp
雖然影像概念上是：height × width × channels  
但記憶體中會攤平成一條連續的一維資料：[B, G, R, B, G, R, B, G, R, ...]  
這種配置通常比 vector<vector<vector<...>>> 更有效率，也更適合影像處理  
cv::Mat 的底層也通常是一段連續的一維記憶體，但它額外保存了影像形狀、型別、步長等資訊  

例如一張 2 × 2 × 3 的彩色影像，總共有 2 × 2 × 3 = 12 個元素，排列可能是：  
索引：  0 1 2 | 3 4 5 | 6 7 8 | 9 10 11  
資料：  B G R | B G R | B G R | B G  R  
像素：    0       1       2        3  

要存取座標 (x, y) 的第 c 個通道，可以計算一維索引：  
std::size_t index = (static_cast<std::size_t>(y) * width + x) * channels + c;  

unsigned char value = data[index];  

例子  
```cpp
int width = 640;  
int channels = 3;  
int x = 10;  
int y = 20;  
int c = 2;  // 第 2 通道  

std::size_t index = (y * width + x) * channels + c;
```

## 1. Stack

frame 物件本身是區域變數，生命週期由作用域控制。變數離開作用域後會自動銷毀。  

```cpp
{
    Frame frame(640, 480, 3, 1000);
}  // 離開此處後自動銷毀
```

## 2. Heap

Heap 用於動態配置的記憶體。傳統 C++ 使用 `new` 配置，並使用 `delete` 釋放。

```cpp
Frame* frame = new Frame(640, 480, 3, 1000);

delete frame;
frame = nullptr;
```
如果忘記釋放，可能造成 memory leak。

## 3. Pointer

Pointer 儲存另一個物件的記憶體地址。

```cpp
int value = 10;
int* pointer = &value;
```

* `&value`：取得地址
* `pointer`：儲存地址
* `*pointer`：取得該地址中的值

## 4. Reference

Reference 是既有物件的別名。

```cpp
int value = 10;
int& reference = value;
```

修改 `reference` 也會修改 `value`。

## 5. Pointer and Reference

Pointer：

* 可以是 `nullptr`
* 可以改為指向另一個物件
* 使用 `*` 解參考
* 存取成員時使用 `->`

Reference：

* 建立時必須綁定物件
* 一般不應是空的
* 綁定後不能改為代表另一個物件
* 使用方式類似一般變數

## 6. RAII

RAII 是 Resource Acquisition Is Initialization 的縮寫。

資源由物件取得，並在物件生命週期結束時自動釋放。這能降低 memory leak 和資源未釋放的風險。

常見 RAII 類別包括：

* `std::vector`
* `std::string`
* `std::unique_ptr`
* `std::shared_ptr`
* `std::lock_guard`
* 檔案串流

以下也都是 RAII：

vector 離開作用域時，自動釋放內部記憶體: std::vector<int> values(100);
檔案串流物件銷毀時，自動關閉檔案：std::ifstream file("data.txt");
離開作用域時，自動解鎖 mutex: std::lock_guard<std::mutex> lock(mutex);

甚至普通區域物件也是：Frame frame(640, 480, 3, 1000);
frame 離開作用域時，內部的 std::vector 會自動釋放記憶體，這同樣符合 RAII。

可以這樣區分：
RAII          一種資源管理設計原則
unique_ptr    實作 RAII 的工具之一
make_unique   建立 unique_ptr 的推薦方式

## 7. unique_ptr

`std::unique_ptr` 表示資源只有一個擁有者。

```cpp
auto frame = std::make_unique<Frame>();
```

離開作用域時，`Frame` 會自動銷毀。  
C++11 引入的智慧指標，負責獨占管理動態配置的物件。當它離開作用域或被銷毀時，會自動釋放記憶體。  
其擁有權無法共享或複製，只能透過 std::move 轉移，不再使用 new/delete  
兩個 unique_ptr 執行個體之間的擁有權轉移  
linux-camera-roadmap/Day1Day2_memory/notes/unique_ptr.png  

## 8. Common Memory Problems

### Buffer overflow

存取陣列合法範圍以外的位置。用 debugger 觀察 unique_ptr

#### CMake 開啟 Debug 模式
在專案根目錄執行：
```cpp
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

也可以在 CMakeLists.txt 明確加入：
```cpp
target_compile_options(hello_opencv
    PRIVATE
    -g
    -O0
)
```

-g：產生除錯資訊  
-O0：關閉最佳化，讓逐行執行較容易理解  

#### macOS使用LLDB Linux使用GDB

執行檔假設叫： build/hello_opencv  
啟動：        lldb ./build/hello_opencv. 
進入 LLDB 後：(lldb) breakpoint set --name main  
             (lldb) run  

如果顯示 Breakpoint 1: 32 locations. 這代表 LLDB 找到 32 個名稱叫 main 的位置  
通常因為正在除錯的執行檔連結了很多函式庫，其中可能包含其他符號或 除錯資訊。  
不一定是錯誤，但對程式來說太寬泛了。改用「檔名 + 行號」會更精確。  
假設 main() 在 day2_main.cpp： (lldb) breakpoint set --file day2_main.cpp --name main. 
或直接指定行號，例如 main() 在第 23 行：(lldb) breakpoint set --file day2_main.cpp --line 23. 

也可以先刪除剛才的 breakpoint：(lldb) breakpoint delete   
再重新設定：(lldb) breakpoint set --file day2_main.cpp --name main. 

在 main() 停下來後程式停在 make_unique 尚未執行的位置：(lldb) next 逐行執行。  

```cpp
(lldb) next
Process 99887 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
    frame #0: 0x0000000100011960 hello_opencv`main at day2_main.cpp:27:16
   24       // frame 的型別是：std::unique_ptr<Frame>
   25       // 建立 heap 上的 Frame，但 unique_ptr 會在 main() 結束時自動釋放，因此正常情況不會洩漏
   26       auto frame = std::make_unique<Frame>( 640, 480, 3,1000);
-> 27       printFrame(*frame);
   28       return 0;
   29   }
Target 0: (hello_opencv) stopped.
```

frame variable frame 查看變數成功建立
```cpp
(lldb) frame variable frame
(std::__unique_if<Frame>::__unique_single) frame = Frame @ 0x0000000144204080 { // heap 位址 0x0000000144204080
  __value_ = 0x0000000144204080 // __value_ 是 unique_ptr 內部保存的原始指標
}
```

unique_ptr 本身不是那個物件，它是一個「擁有者物件」，裡面保存一個普通指標，指向 heap 上真正的物件。

frame.  
└─ 內部保存一個 Frame*.  
    └── 指向 heap 上的 Frame 物件  

假設 LLDB 顯示 __value_ = 0x0000000144204080   
這個位址就是它內部保存的原始指標值 指向真正的 Frame  

執行到 make_unique 後，可查看：(lldb) p frame.get(). 
frame.get() 會回傳 unique_ptr 內部保存的原始指標。  

查看 Frame： (lldb) p *frame  
查看個別成員：
```cpp
(lldb) p frame->width
(lldb) p frame->height
(lldb) p frame->channels
(lldb) p frame->data.size()
```
繼續執行：(lldb) continue  
離開：   (lldb) quit. 

* LLDB / GDB→ 用來停住程式、逐行執行、查看變數
* Valgrind / AddressSanitizer→ 用來偵測記憶體錯誤

#### macOS 用 AddressSanitizer
在 CMakeLists.txt 加入以下 用 day2_heap_overflow.cpp
編譯階段: 這些選項會傳給編譯器，用來編譯 .cpp
```cpp
target_compile_options(hello_opencv
    PRIVATE
    -fsanitize=address /* 在程式裡插入 AddressSanitizer 的檢查機制
                            它會偵測： heap buffer overflow, stack buffer overflow
                            use-after-free, double free, 部分 memory leak, 越界讀寫
                            沒有它時，這行：frame->data[frame->data.size()] = 255;
                            可能看似正常執行，也可能之後才崩潰，錯誤位置不容易找。*/
    -fno-omit-frame-pointer /* 要求編譯器保留 frame pointer, 讓 ASan 顯示更完整、比較準確的呼叫堆疊*/
    -g /* 加入除錯符號 錯誤報告才比較容易顯示: day2_heap_overflow.cpp:35
          而不是只有模糊的記憶體位址：0x10012307c*/
)
```
在連結階段加入 ASan runtime:編譯器雖然已經在 .cpp 中插入檢查程式碼，  
但最後產生執行檔時，還必須連結 ASan 的 runtime library. 

```cpp
target_link_options(hello_opencv
    PRIVATE
    -fsanitize=address
)
```

重新建置：
```cpp
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

執行：./build/hello_opencv
預期會看到類似：
```cpp
==2287==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x000100221800 at pc 0x000100123080 bp 0x00016fceec70 sp 0x00016fceec68
WRITE of size 1 at 0x000100221800 thread T0
    #0 0x10012307c in main day2_heap_overflow.cpp:35
    #1 0x1824cf150  (<unknown module>)
0x000100221800 is located 0 bytes after 921600-byte region [0x000100140800,0x000100221800) => 寫到合法區域的下一個byte
```

### Memory leak

配置記憶體後沒有釋放。  

day2_memory_leak.cpp.  
建構函式用 new 向 heap 申請空間，但解構函式沒有 delete[]. 

##### 用 LeakSanitizer 測試
CMake 已開啟 AddressSanitizer，可以再加入 leak detection：  
```cpp
target_compile_options(hello_opencv PRIVATE
    -fsanitize=address
    -fno-omit-frame-pointer
    -g
)
target_link_options(hello_opencv PRIVATE
    -fsanitize=address
)
```

用我自己的電腦 macOS的leaks 工具和目前啟用了 AddressSanitizer 的執行檔發生衝突。
訊息是：
```cpp
malloc_zone_unregister() failed
realized class ... has corrupt data pointer
```

ASan 會攔截並替換部分 malloc/new/delete 行為；leaks 也需要攔截和追蹤 malloc。  
兩個工具同時操作記憶體配置機制時，可能互相干擾。  
ASan 本身就是透過 runtime instrumentation 偵測非法記憶體存取，  
而 Apple 也把 Address Sanitizer 與 malloc debugging tools 視為不同的診斷方式。  

解法：建立不含 ASan 的版本. 
不用目前有：-fsanitize=address 的 hello_opencv 執行 leaks。  
建立兩個 target：  
```cpp
add_executable(heap_overflow_demo
    Day1Day2_memory/code/day2_heap_overflow.cpp
)
target_include_directories(heap_overflow_demo PRIVATE
    Day1Day2_memory/include
)
target_compile_options(heap_overflow_demo PRIVATE
    -fsanitize=address
    -fno-omit-frame-pointer
    -g
    -O0
)
target_link_options(heap_overflow_demo PRIVATE
    -fsanitize=address
)
add_executable(memory_leak_demo
    Day1Day2_memory/code/day2_memory_leak.cpp
)
target_include_directories(memory_leak_demo PRIVATE
    Day1Day2_memory/include
)
target_compile_options(memory_leak_demo PRIVATE
    -g
    -O0
)
```

然後完整刪掉舊 build：
```cpp
    rm -rf build
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build
    ```
確認 leak demo 沒有 ASan：otool -L ./build/memory_leak_demo | grep -i asan  
沒有輸出後，再測：  
leaks --atExit -- ./build/memory_leak_demo.  

結果：leaks抓到故意製造的記憶體洩漏.
```cpp
leaks Report Version: 4.0, multi-line stacks
Process 7105: 188 nodes malloced for 943 KB
Process 7105: 1 leak for 950272 total leaked bytes.
```

總共有 1 個 leak, 洩漏約 950,272 bytes, 約等於 928 KiB.  
原本配置的是：640 × 480 × 3 = 921600 bytes, 報告稍微更大，是因為配置器可能會加上對齊或額外管理空間.  

關鍵 stack：ROOT LEAK: <malloc in BadFrame::BadFrame(int, int, int)>  
以及：  
day2_memory_leak.cpp:36  main. 
day2_memory_leak.cpp:15  BadFrame::BadFrame(...). 

代表 main() 第 36 行建立 BadFrame, BadFrame 建構函式第 15 行執行 new[].  
程式結束前，這塊記憶體都沒有被 delete[], leaks 因此把它判定為 root leak.  

這一行：1 (928K) ROOT LEAK: <malloc in BadFrame::BadFrame(int, int, int) 0x140008000>.  
表示那塊洩漏記憶體起始位址是0x140008000  

所以現在已驗證 建構函式配置 heap 記憶體  
→ 解構函式有執行. 
→ 但沒有 delete[]. 
→ leaks 找到未釋放區塊. 

修正方式：
```cpp
~BadFrame() {
    delete[] data_;
    std::cout << "BadFrame destroyed\n";
}
```
重新編譯後再執行：
```cpp
cmake --build build
leaks --atExit -- ./build/memory_leak_demo
```
修正成功時，應該會接近：Process ...: 0 leaks for 0 total leaked bytes.

### Dangling pointer

指標還保存著某個記憶體位址，但那塊記憶體已經失效或被釋放。

int* value = new int(10);
delete value;

// value 仍保存舊地址，但該地址已不再屬於它
std::cout << *value << '\n';  // 錯誤：use-after-free

這時 value 就是 dangling pointer。

* Memory leak → 記憶體仍存在，但已經沒有可靠的方法釋放它
* Dangling pointer→ 記憶體已失效，但指標還指向那裡

### Null pointer dereference

解參考 `nullptr`。

### Double free

同一塊記憶體被釋放兩次。
