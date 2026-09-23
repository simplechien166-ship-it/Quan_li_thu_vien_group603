#include "../include/CustomHashTable.h"
#include "../include/CustomMinHeap.h"
#include "../include/ReservationQueue.h"
#include <iostream>
#include <cassert>

void testHashTable() {
    CustomHashTable<int> table;
    table.insert("K1", 100);
    table.insert("K2", 200);

    assert(table.search("K1") != nullptr && *table.search("K1") == 100);
    assert(table.search("K2") != nullptr && *table.search("K2") == 200);
    assert(table.search("K3") == nullptr);

    std::cout << "[PASS] Unit Test CustomHashTable OK!\n";
}

void testReservationQueue() {
    ReservationQueue q;
    assert(q.push("R001"));
    assert(q.push("R002"));
    assert(!q.push("R001"));

    std::string out;
    assert(q.pop(out) && out == "R001");
    assert(q.pop(out) && out == "R002");
    assert(q.isEmpty());

    for (int i = 0; i < 100; ++i) {
        assert(q.push("R" + std::to_string(i)));
    }
    assert(q.getSize() == 100);
    assert(!q.push("R_OVERFLOW"));

    std::cout << "[PASS] Unit Test ReservationQueue OK!\n";
}

int main() {
    std::cout << "=== RUNNING UNIT TESTS ===\n";
    testHashTable();
    testReservationQueue();
    std::cout << "ALL UNIT TESTS PASSED SUCCESSFULLY!\n";
    return 0;
}