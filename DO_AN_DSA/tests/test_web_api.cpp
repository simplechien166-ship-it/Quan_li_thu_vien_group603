#include "../include/httplib.h"
#include <iostream>
#include <string>

using namespace std;

// Kiem tra 1 endpoint, tra ve true neu dung status code mong doi, nguoc lai false
bool test_endpoint(httplib::Client& cli, const char* path, int expected_status = 200) {
    auto res = cli.Get(path);
    if (res) {
        if (res->status == expected_status) {
            cout << "[PASS] GET " << path << " (Status " << res->status << ")\n";
            cout << "Body: " << res->body << "\n\n";
            return true;
        }
        else {
            cout << "[FAIL] GET " << path << " (Status " << res->status << ", ky vong " << expected_status << ")\n";
            cout << "Body: " << res->body << "\n\n";
            return false;
        }
    }
    else {
        cout << "[FAIL] GET " << path << " (khong co phan hoi - server co dang chay khong?)\n\n";
        return false;
    }
}

// Nhom test: cac API tra ve danh sach (khong can tham so)
int test_api_lists(httplib::Client& cli) {
    int failures = 0;
    cout << "--- Nhom 1: Cac API danh sach ---\n";
    if (!test_endpoint(cli, "/api/books")) ++failures;
    if (!test_endpoint(cli, "/api/readers")) ++failures;
    if (!test_endpoint(cli, "/api/top10")) ++failures;
    if (!test_endpoint(cli, "/api/overdue")) ++failures;
    return failures;
}

// Nhom test: MC1 - tra cuu sach theo ma (co ton tai / khong ton tai / thieu param)
int test_api_book_lookup(httplib::Client& cli) {
    int failures = 0;
    cout << "--- Nhom 2: MC1 - Tra cuu sach ---\n";
    if (!test_endpoint(cli, "/api/book?id=B100", 200)) ++failures;               // Doi lai ma sach that trong data/books.csv
    if (!test_endpoint(cli, "/api/book?id=KHONG_TON_TAI_999", 404)) ++failures;
    if (!test_endpoint(cli, "/api/book", 404)) ++failures;                      // Thieu param id
    return failures;
}

// Nhom test: lich su muon cua doc gia (thieu id / doc gia khong ton tai)
int test_api_reader_borrows(httplib::Client& cli) {
    int failures = 0;
    cout << "--- Nhom 3: Lich su muon cua doc gia ---\n";
    if (!test_endpoint(cli, "/api/reader-borrows", 400)) ++failures;            // Thieu id
    if (!test_endpoint(cli, "/api/reader-borrows?id=KHONG_TON_TAI_999", 404)) ++failures;
    return failures;
}

int main() {
    cout << "=== BAT DAU KIEM THU WEB API (DO AN DSA) ===\n";
    cout << "(Yeu cau web_server.exe dang chay san o localhost:8080)\n\n";

    httplib::Client cli("localhost", 8080);
  

    int total_failures = 0;
    total_failures += test_api_lists(cli);
    total_failures += test_api_book_lookup(cli);
    total_failures += test_api_reader_borrows(cli);

    cout << "=== HOAN TAT KIEM THU ===\n";
    if (total_failures == 0) {
        cout << "Tat ca test PASS\n";
        return 0;
    }
    else {
        cout << total_failures << " test that bai\n";
        return 1;
    }
}