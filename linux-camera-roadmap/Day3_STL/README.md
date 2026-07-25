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