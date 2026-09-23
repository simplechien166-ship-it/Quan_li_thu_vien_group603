#ifndef CUSTOM_HASH_TABLE_H
#define CUSTOM_HASH_TABLE_H

#include <string>
#include <vector>
#include <cstddef>

template <typename T>
struct HashNode {
    std::string key;
    T value;
    HashNode* next;

    HashNode(const std::string& k, const T& v) : key(k), value(v), next(nullptr) {}
};

template <typename T>
class CustomHashTable {
private:
    static const size_t CAPACITY = 10007; // Kích thước mảng băm số nguyên tố
    HashNode<T>* table[CAPACITY];
    size_t count;

    // Thuật toán DJB2 Hash
    size_t hashFunction(const std::string& key) const {
        size_t hash = 5381;
        for (char c : key) {
            hash = ((hash << 5) + hash) + static_cast<size_t>(c); // hash * 33 + c
        }
        return hash % CAPACITY;
    }

public:
    CustomHashTable() : count(0) {
        for (size_t i = 0; i < CAPACITY; ++i) {
            table[i] = nullptr;
        }
    }

    ~CustomHashTable() {
        clear();
    }

    void clear() {
        for (size_t i = 0; i < CAPACITY; ++i) {
            HashNode<T>* curr = table[i];
            while (curr != nullptr) {
                HashNode<T>* temp = curr;
                curr = curr->next;
                delete temp;
            }
            table[i] = nullptr;
        }
        count = 0;
    }

    // Chèn hoặc cập nhật (O(1) trung bình)
    void insert(const std::string& key, const T& value) {
        size_t index = hashFunction(key);
        HashNode<T>* curr = table[index];

        while (curr != nullptr) {
            if (curr->key == key) {
                curr->value = value;
                return;
            }
            curr = curr->next;
        }

        // Chèn vào đầu xích băm O(1)
        HashNode<T>* newNode = new HashNode<T>(key, value);
        newNode->next = table[index];
        table[index] = newNode;
        count++;
    }

    // Tra cứu nhanh O(1) trung bình
    T* search(const std::string& key) {
        size_t index = hashFunction(key);
        HashNode<T>* curr = table[index];

        while (curr != nullptr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    // Xóa phần tử theo Key
    bool remove(const std::string& key) {
        size_t index = hashFunction(key);
        HashNode<T>* curr = table[index];
        HashNode<T>* prev = nullptr;

        while (curr != nullptr) {
            if (curr->key == key) {
                if (prev == nullptr) {
                    table[index] = curr->next;
                } else {
                    prev->next = curr->next;
                }
                delete curr;
                count--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    // Lấy toàn bộ danh sách con trỏ giá trị trong bảng băm
    std::vector<T*> getAllValues() {
        std::vector<T*> values;
        values.reserve(count);
        for (size_t i = 0; i < CAPACITY; ++i) {
            HashNode<T>* curr = table[i];
            while (curr != nullptr) {
                values.push_back(&(curr->value));
                curr = curr->next;
            }
        }
        return values;
    }

    size_t size() const { return count; }
};

#endif // CUSTOM_HASH_TABLE_H