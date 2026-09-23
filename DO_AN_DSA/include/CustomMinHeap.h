#ifndef CUSTOM_MIN_HEAP_H
#define CUSTOM_MIN_HEAP_H

#include "models.h"
#include <vector>

class CustomMinHeap {
private:
    static const int MAX_K = 10;
    Book heap[MAX_K];
    int size;

    // So sánh: Trả về true nếu A "kém hơn" B (A bị coi là yếu hơn B trong nhóm Top)
    // Tiêu chí: Lượt mượn ít hơn -> NamXB cũ hơn -> Mã sách lớn hơn (theo từ điển)
    bool isWorse(const Book& a, const Book& b) const;

    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    CustomMinHeap();

    void insertOrUpdate(const Book& book);
    std::vector<Book> getSortedTop10() const; // Trả về danh sách Top 10 đã sắp xếp từ cao xuống thấp
    void clear();
};

#endif // CUSTOM_MIN_HEAP_H