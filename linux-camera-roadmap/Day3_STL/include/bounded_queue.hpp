#ifndef BOUNDED_QUEUE_HPP
#define BOUNDED_QUEUE_HPP

#include <cstddef>      // std::size_t 
#include <functional>   // std::reference_wrapper std::ref std::cref
#include <optional>     // std::optional std::nullopt -> README.md: std::optional
#include <queue>
#include <stdexcept>    // std::invalid_argument 提供標準例外類別
#include <utility>      // std::move, 
                        // std::forward用於 perfect forwarding, 讓傳入參數保留原本是左值還是右值的特性。

/**
 * @brief 固定容量的單執行緒 FIFO Queue。
 *
 * @tparam T 代表Queue 裡要存放的元素型別。
 *
 * 注意：
 * 1. 此類別不是 thread-safe。
 * 2. Queue 滿時，push() 和 emplace() 會回傳 false。
 * 3. Queue 空時，pop() 會回傳 std::nullopt。
 */
template <typename T>
class BoundedQueue {
public:
    using value_type = T; // value_type 和 T 是同一個型別
    using size_type = std::size_t;

    /**
     * @brief 建立指定容量的 Queue。
     *
     * @param capacity Queue 最大元素數量。
     *
     * @throws std::invalid_argument 當 capacity 為 0。
     */
    explicit BoundedQueue(size_type capacity) // explicit -> README.md: explicit
        : capacity_(capacity) {
        if (capacity_ == 0) {
            // 若使用者給的容量不合法，就丟出例外。
            // throw 表示發生錯誤，將例外傳給呼叫端
            throw std::invalid_argument(
                "BoundedQueue capacity must be greater than zero"
            );
        }
    }
 
    /**
     * @brief 將元素複製至 Queue 尾端。
     *
     * @return true 新增成功。
     * @return false Queue 已滿。
     */
    bool push(const T& value) {
        if (full()) {
            return false;
        }

        queue_.push(value);
        return true;
    }

    /**
     * @brief 將元素移動至 Queue 尾端。
     *
     * @return true 新增成功。
     * @return false Queue 已滿。
     */
    bool push(T&& value) {
        if (full()) {
            return false;
        }

        queue_.push(std::move(value));
        return true;
    }

    /**
     * @brief 直接在 Queue 內部建立元素。
     *
     * @return true 建立成功。
     * @return false Queue 已滿。
     */
    template <typename... Args>
    bool emplace(Args&&... args) {
        if (full()) {
            return false;
        }

        queue_.emplace(std::forward<Args>(args)...);
        return true;
    }

    /**
     * @brief 移除並回傳 Queue 最前面的元素。
     *
     * @return std::optional<T>
     *         Queue 非空時回傳元素，否則回傳 std::nullopt。
     */
    [[nodiscard]] std::optional<T> pop() {
        if (empty()) {
            return std::nullopt;
        }

        T value = std::move(queue_.front());
        queue_.pop();

        return value;
    }

    /**
     * @brief 取得 Queue 最前面的元素。
     *
     * 不會移除元素。
     front() 使用reference_wrapper<T>>
     原因是 std::optional 不能直接儲存 reference：
     std::optional<T&>  // 不合法
     // 所以改用：td::optional<std::reference_wrapper<T>>
        並透過：return std::ref(queue_.front()); 回傳可修改的參考。
     */
    [[nodiscard]]
    std::optional<std::reference_wrapper<T>> front() noexcept {
        if (empty()) {
            return std::nullopt;
        }

        return std::ref(queue_.front());
    }

    /**
     * @brief 取得 Queue 最前面的唯讀元素。
     */
    [[nodiscard]]
    std::optional<std::reference_wrapper<const T>> front() const noexcept {
        if (empty()) {
            return std::nullopt;
        }

        return std::cref(queue_.front());
    }

    /**
     * @brief 取得 Queue 最後面的元素。
     */
    [[nodiscard]]
    std::optional<std::reference_wrapper<T>> back() noexcept {
        if (empty()) {
            return std::nullopt;
        }

        return std::ref(queue_.back());
    }

    /**
     * @brief 取得 Queue 最後面的唯讀元素。
     */
    [[nodiscard]]
    std::optional<std::reference_wrapper<const T>> back() const noexcept {
        if (empty()) {
            return std::nullopt;
        }

        return std::cref(queue_.back());
    }

    /**
     * @brief 判斷 Queue 是否為空。
     */
    [[nodiscard]] bool empty() const noexcept {
        return queue_.empty();
    }

    /**
     * @brief 判斷 Queue 是否已滿。
     */
    [[nodiscard]] bool full() const noexcept {
        return queue_.size() >= capacity_;
    }

    /**
     * @brief 回傳目前元素數量。
     */
    [[nodiscard]] size_type size() const noexcept {
        return queue_.size();
    }

    /**
     * @brief 回傳 Queue 最大容量。
     */
    [[nodiscard]] size_type capacity() const noexcept {
        return capacity_;
    }

    /**
     * @brief 清除 Queue 中的全部元素。
     */
    void clear() noexcept {
        while (!queue_.empty()) {
            queue_.pop();
        }
    }

private:
    std::queue<T> queue_;
    size_type capacity_;
};

#endif  // BOUNDED_QUEUE_HPP