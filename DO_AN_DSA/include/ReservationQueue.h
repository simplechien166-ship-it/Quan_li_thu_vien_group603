#ifndef RESERVATION_QUEUE_H
#define RESERVATION_QUEUE_H

#include "models.h"
#include <string>

class ReservationQueue {
private:
    static const int MAX_QUEUE_SIZE = 100;
    ReservationNode* head;
    ReservationNode* tail;
    int size;

public:
    ReservationQueue();
    ~ReservationQueue();

    // Vô hiệu hóa sao chép nông để tránh rò rỉ bộ nhớ
    ReservationQueue(const ReservationQueue&) = delete;
    ReservationQueue& operator=(const ReservationQueue&) = delete;

    bool push(const std::string& ma_ban_doc); // Enqueue vào cuối - O(1)
    bool pop(std::string& ma_ban_doc_out);    // Dequeue ở đầu - O(1)
    bool front(std::string& ma_ban_doc_out) const;
    bool isEmpty() const;
    bool existsInQueue(const std::string& ma_ban_doc) const; // Tránh xếp hàng trùng
    int getSize() const;
    void clear();
};

#endif // RESERVATION_QUEUE_H