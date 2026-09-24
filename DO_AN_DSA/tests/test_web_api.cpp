#include "../include/httplib.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

// Test rate limiting: tra ve so luong fail (0 neu PASS)
int test_rate_limit(httplib::Client& cli) {
    std::cout << "--- Nhom 4: Rate Limiting ---\n";

    // Da dong comment 2 dong timeout vi phien ban httplib nay khong ho tro
    // cli.set_connection_timeout_sec(3);
    // cli.set_read_timeout_sec(3);

    int last_status = 0;
    int got_429_at = -1;
    httplib::Headers hdrs = { {"Connection", "close"} }; // force close per request to avoid connection reuse
    for (int i = 0; i < 105; i++) {
        std::cout << "Sending request #" << (i + 1) << "...\n";
        auto res = cli.Get("/api/books", hdrs);
        if (!res) {
            std::cout << "[FAIL] Request #" << i + 1 << " - khong co phan hoi (timeout hay loi ket noi)\n";
            return 1; // treat as failure
        }
        last_status = res->status;
        if (res->status == 429) {
            got_429_at = i + 1;
            break; // early exit on rate limit triggered
        }
        // Nho cho mot chut de khong flood qua nhanh (tuỳ server)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (got_429_at != -1) {
        std::cout << "[PASS] Sau " << got_429_at << " request, server tra ve 429 (rate limit)\n";
        return 0;
    }
    else {
        std::cout << "[FAIL] Khong co 429 sau 105 request, status cuoi = " << last_status << "\n";
        return 1;
    }
}

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

    httplib::Client cli("127.0.0.1", 8080);

    // Da dong comment 2 dong timeout
    // cli.set_connection_timeout_sec(3);
    // cli.set_read_timeout_sec(3);

    int total_failures = 0;
    total_failures += test_api_lists(cli);
    total_failures += test_api_book_lookup(cli);
    total_failures += test_api_reader_borrows(cli);

    // Goi ham test gioi han request - tra ve 0 neu PASS, 1 neu FAIL
    total_failures += test_rate_limit(cli);

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