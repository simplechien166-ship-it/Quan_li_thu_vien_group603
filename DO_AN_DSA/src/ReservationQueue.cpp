#include "../include/ReservationQueue.h"

ReservationQueue::ReservationQueue() : head(nullptr), tail(nullptr), size(0) {}

ReservationQueue::~ReservationQueue() {
    clear();
}

void ReservationQueue::clear() {
    while (head != nullptr) {
        ReservationNode* temp = head;
        head = head->next;
        delete temp;
    }
    tail = nullptr;
    size = 0;
}

bool ReservationQueue::push(const std::string& ma_ban_doc) {
    if (size >= MAX_QUEUE_SIZE || existsInQueue(ma_ban_doc)) return false;

    ReservationNode* newNode = new ReservationNode(ma_ban_doc);
    if (isEmpty()) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    size++;
    return true;
}

bool ReservationQueue::pop(std::string& ma_ban_doc_out) {
    if (isEmpty()) return false;

    ReservationNode* temp = head;
    ma_ban_doc_out = head->ma_ban_doc;
    head = head->next;

    if (head == nullptr) {
        tail = nullptr;
    }

    delete temp;
    size--;
    return true;
}

bool ReservationQueue::front(std::string& ma_ban_doc_out) const {
    if (isEmpty()) return false;
    ma_ban_doc_out = head->ma_ban_doc;
    return true;
}

bool ReservationQueue::isEmpty() const {
    return head == nullptr;
}

bool ReservationQueue::existsInQueue(const std::string& ma_ban_doc) const {
    ReservationNode* curr = head;
    while (curr != nullptr) {
        if (curr->ma_ban_doc == ma_ban_doc) return true;
        curr = curr->next;
    }
    return false;
}

int ReservationQueue::getSize() const {
    return size;
}