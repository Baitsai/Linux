# STL container 比較

| Container     | Description      | 適用情境     | 常用操作          | 
| ------------- | ---------------- | ----------- | -----------------|
| vector        | 連續記憶體的動態陣列 | 需要快速隨機存取元素 | push_back、pop_back、operator[] |
| deque         | 分段式連續記憶體    | 頭尾都需要新增或刪除 | push_front、push_back |
| queue         | FIFO 容器轉接器    | 工作佇列、影像幀佇列 | push、pop、front |
| unordered_map | Hash Table       | 透過 key 快速查詢 | find、contains、operator[] |

## 時間複雜度

| Container     | 尾端新增    | 頭端刪除     | 隨機存取     | 依 key 查詢   | 
| ------------- | ---------- | ---------- | ----------- | ------------ |
| vector        | 平均 O(1)   | O(n)       | O(1)        | X            |
| deque         | O(1)       | O(1)       | O(1)        | X            |
| queue         | O(1)       | O(1)       | X           | X            |
| unordered_map | X          | X          | X           | 平均 O(1)     |

std::queue 預設使用 std::deque 作為底層容器：
```cpp
std::queue<int> queue;
```
大致相當於：
```cpp
std::queue<int, std::deque<int>> queue;
```

# BoundedQueue

一般的 Queue 可以持續新增元素，直到記憶體不足。
BoundedQueue 則有固定容量：
```cpp
capacity = 3

push(10) → [10]
push(20) → [10, 20]
push(30) → [10, 20, 30]
push(40) → 失敗，Queue 已滿
```
常用在：  
* 相機影像 Frame Buffer
* Producer–Consumer 模型
* 網路封包佇列
* 即時影像處理 pipeline
* 控制記憶體用量

例如相機每秒產生 30 張影像，但影像處理速度只有每秒 20 張。若 Queue 沒有容量限制，尚未處理的影像會持續累積，  
最終可能耗盡記憶體。

單執行緒版本提供：
```cpp
push()
emplace()
pop()
front()
back()
empty()
full()
size()
capacity()
clear()
```
* Queue 滿時，push() 回傳 false
* Queue 空時，pop() 回傳 std::nullopt
* front() 和 back() 使用 std::optional<std::reference_wrapper<...>>
* 容量必須大於零
* 支援泛型型別 T
```cpp
template <typename T> // T 代表之後使用這個類別時，才決定要放什麼型別
class BoundedQueue {
private:
    std::queue<T> queue_;
};
```
typename 也可以寫成 class：
```cpp
template <class T>
class BoundedQueue;
```
* 支援 move semantics(不複製物件擁有的資源，而是把資源的所有權轉移給另一個物)
    這對大型資料很重要，例如：std::string std::vector 影像buffer Frame std::unique_ptr. 
* 不負責多執行緒同步

## Copy 與 Move 的差異

假設有一個很大的影像資料：
```cpp
std::vector<unsigned char> image(1920 * 1080 * 3);
```
### Copy
```cpp
std::vector<unsigned char> copy = image;
```
* 配置一塊新的記憶體
* 把數百萬個元素複製過去
* image 和 copy 各自擁有一份資料
成本通常是 O(n)。  

### Move
```cpp
std::vector<unsigned char> moved = std::move(image);
```
通常不會逐個複製元素，而是把內部指標、容量等資源轉交給 moved。 

移動前：image ─> [大量影像資料].  
移動後：moved ─> [大量影像資料].  
       image ─> 有效但內容未指定的狀態. 

通常只需轉移幾個內部欄位，成本接近 O(1)。  

std::move 本身沒有搬資料，它只是把物件轉換成右值，表示這個物件的資源可以被接手。  
真正執行資源轉移的是該型別的：move constructor 或 move assignment operator
```cpp
T x;
T&& value = std::move(x);
```
此時 value 並不是另一個新物件，它只是 x 的別名。
所以：
```cpp
std::move(value)
```
本質上仍是在把同一個 x 當成右值傳下去。

例如：
```cpp
std::string source = "large data";
std::string destination = std::move(source);

std::move(source) 只是允許 std::string 呼叫移動建構子。
```

## 左值與右值

### 左值 lvalue: 有固定身分、可以定位再次存取的物件
```cpp
std::string name = "Alice"; // name 是左值

queue.push(name); // 此時通常會複製
```
```cpp
int x = 10;
```
x 是左值，因為：
* 有名字
* 可以重複使用
* 可以取地址

### 右值 rvalue: 臨時物件或即將不再使用的值
例如：
* 數字 10 是右值。
* int x = 10 + 20 => 10 + 20產生一個暫時結果 30運算結果也是右值。
* std::string("hello") 這個臨時字串也是右值。

```cpp
queue.push(std::string("Alice")); // std::string("Alice") 是臨時物件，通常會移動。

queue.push(std::move(name)); // 也可以用 std::move 將左值轉成右值
```

### 以Frame為例
```cpp
struct Frame {
    uint64_t timestamp;
    std::vector<unsigned char> data;
};
```
建立一張影像：
```cpp
Frame frame;
frame.timestamp = 12345;
frame.data.resize(1920 * 1080 * 3);

// 複製進 Queue, 這可能複製整個 data buffer。
queue.push(frame); 

// 移動進 Queue, Queue 可以接管frame.data的記憶體，避免複製數百萬個byte
queue.push(std::move(frame));
```
在 Camera Pipeline 中，move semantics 很重要，因為影像資料通常很大

### 套回 BoundedQueue
```cpp
bool push(T&& value) {
    if (full()) {
        return false;
    }

    queue_.push(std::move(value));
    return true;
}
```
呼叫：
```cpp
std::string x = "frame";
queue.push(std::move(x));
```

流程是：
```cpp
std::move(x) //產生右值表達式，因此可以綁定到 T&& value
```

進入函式後，value 是 x 的別名。但直接寫：
```cpp 
queue_.push(value);  
// 此時 value 是具名表達式，所以是左值，底層可能選到複製版本： push(const T&)   
// 原來的x通常不會被搬空，因為實際發生的是複製  
```

因此要把具名左值重新轉成右值，底層才會選push(T&&)：
```cpp 
queue_.push(std::move(value)); // x還是合法物件，但內容變成「有效但未指定」
```
規則
```cpp 
void f(T value);        // 複製一份
void f(T& value);       // 使用並可修改原物件
void f(const T& value); // 使用原物件，但不修改，也避免複製
void f(T&& value);      // 接收可被移動的物件
```

### pass-by-value
也可以只寫一個 pass-by-value 版本：
```cpp
bool push(T value) {
    if (full()) {
        return false;
    }

    queue_.push(std::move(value));
    return true;
}
```
使用時：
```cpp
queue.push(x);             // 先 copy 到 value，再 move 進 queue
queue.push(std::move(x));  // move 到 value，再 move 進 queue
queue.push(T{});           // move
```
不過有一個細節：如果 Queue 已滿，pass-by-value 版本可能已經先複製或移動參數，才進入函式檢查 full()。
原本兩個 reference overload 則可以先檢查 Queue 是否已滿，再決定是否複製或移動。
因此BoundedQueue 寫兩個版本是比較完整且有效率的做法，但不是語法上的必要條件。

## std::unique_ptr 為什麼只能 move？
```cpp
std::unique_ptr<Frame> frame = std::make_unique<Frame>();
```
unique_ptr 表示資源只能有一個擁有者，因此不能複製：
```cpp
auto another = frame;  // 編譯錯誤

auto another = std::move(frame); // 只能轉移所有權：
```
轉移後 frame == nullptr 而 another 成為新的擁有者。

編譯與執行  
```bash
clang++ -std=c++17 \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Werror \
    code/day3_main.cpp \
    -o bounded_queue_demo
```
執行：
```bash
./bounded_queue_demo
```
# std::optional

Queue 為空時，沒有元素可以回傳
1. 回傳特殊數值，例如 nullptr、-1、0、""，但這些值可能是合法的元素，會造成混淆。
2. 回傳 std::optional<T>，當 Queue 為空時回傳 std::nullopt，否則回傳元素。
例如：  
```cpp
auto value = queue.pop();
if (value.has_value()) {
    std::cout << *value << '\n';
    /* 假設 pop() 的宣告是std::optional<T> pop();
        編譯器會將 auto value = queue.pop(); 
        推導成：std::optional<T> value = queue.pop();
        所以value 的型別不是 T，而是std::optional<T> 
        例如：std::optional<int> value;

        要取得其中儲存的 int，可以寫 *value 或者value.value()，
        但如果 Queue 為空，會丟出 std::bad_optional_access 例外。
    */
}
```
可以簡寫成：
```cpp
if (auto value = queue.pop()) {
    std::cout << *value << '\n';
}
```
c++允許直接在 if 條件裡宣告變數, value只作用於這個if 和對應的else裡.  
這比回傳特殊數值更安全，因為 T 不一定有合理的「錯誤值」。

# explicit 

explicit 用來防止編譯器進行不明確的隱式轉型。

沒有 explicit 時，編譯器可能允許：
```cpp
void process(BoundedQueue<int> queue);
process(5);
```
因為 5 被自動轉成 BoundedQueue<int>(5)  
可能讓程式「看起來像傳入整數」，實際上卻偷偷建立了一個 BoundedQueue<int>。

但加上 explicit 後：
```cpp
process(5);  // 編譯錯誤
//必須明確寫成：process(BoundedQueue<int>(5));
```
因此單一參數建構子通常建議加上 explicit，避免意外轉型。



