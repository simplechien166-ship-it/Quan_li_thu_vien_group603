#include "../include/CustomMinHeap.h"
#include <algorithm>
#include <cmath>
#include <iomanip>

CustomMinHeap::CustomMinHeap() : size(0) {}

void CustomMinHeap::clear() {
    size = 0;
}

bool CustomMinHeap::isWorse(const Book& a, const Book& b) const {
    if (a.luot_muon != b.luot_muon) {
        return a.luot_muon < b.luot_muon; // Lượt mượn ít hơn thì yếu hơn
    }
    if (a.nam_xuat_ban != b.nam_xuat_ban) {
        return a.nam_xuat_ban < b.nam_xuat_ban; // Năm XB cũ hơn thì yếu hơn
    }
    return a.ma_sach > b.ma_sach; // Mã sách thứ tự từ điển lớn hơn thì yếu hơn
}

void CustomMinHeap::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (isWorse(heap[index], heap[parent])) {
            std::swap(heap[index], heap[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

void CustomMinHeap::heapifyDown(int index) {
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < size && isWorse(heap[left], heap[smallest])) {
            smallest = left;
        }
        if (right < size && isWorse(heap[right], heap[smallest])) {
            smallest = right;
        }

        if (smallest != index) {
            std::swap(heap[index], heap[smallest]);
            index = smallest;
        } else {
            break;
        }
    }
}

void CustomMinHeap::insertOrUpdate(const Book& book) {
    // Kiểm tra xem sách đã có sẵn trong Heap chưa để cập nhật
    for (int i = 0; i < size; ++i) {
        if (heap[i].ma_sach == book.ma_sach) {
            heap[i] = book;
            heapifyUp(i);
            heapifyDown(i);
            return;
        }
    }

    if (size < MAX_K) {
        heap[size] = book;
        size++;
        heapifyUp(size - 1);
    } else {
        // Nếu Heap đã đủ 10 phần tử và cuốn sách mới tốt hơn đỉnh Heap (phần tử kém nhất trong Top 10)
        if (isWorse(heap[0], book)) {
            heap[0] = book; // Thay thế đỉnh Heap
            heapifyDown(0);
        }
    }
}

std::vector<Book> CustomMinHeap::getSortedTop10() const {
    std::vector<Book> result;
    for (int i = 0; i < size; ++i) {
        result.push_back(heap[i]);
    }

    // Sắp xếp giảm dần từ Hạng 1 -> Hạng 10
    std::sort(result.begin(), result.end(), [this](const Book& a, const Book& b) {
        return !isWorse(a, b);
    });

    return result;
}