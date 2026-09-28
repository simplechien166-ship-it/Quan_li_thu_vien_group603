#include "../include/httplib.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

// Kiem tra gioi han request (Rate limiting): tra ve so luong loi (0 neu PASS)
int test_rate_limit(httplib::Client& cli) {
    std::cout << "--- Nhom 4: Kiem tra Rate Limiting (Chong Spam) ---\n";

    int last_status = 0;
    int got_429_at = -1;

    // Da them Token vao day de qua duoc tram gac Auth (Xac thuc)
    httplib::Headers hdrs = {
        {"Connection", "close"}, // ep dong ket noi sau moi request de tranh xai lai
        {"Authorization", "Bearer my-secret-token"}
    };

    for (int i = 0; i < 105; i++) {
        std::cout << "Dang gui request thu #" << (i + 1) << "...\n";
        auto res = cli.Get("/api/books", hdrs);
        if (!res) {
            std::cout << "[THAT BAI] Request thu #" << i + 1 << " - khong co phan hoi (timeout hay loi ket noi)\n";
            return 1; // tinh la that bai
        }
        last_status = res->status;
        if (res->status == 429) {
            got_429_at = i + 1;
            break; // thoat som khi rate limit bi kich hoat (triggered)
        }
        // Nho cho mot chut de khong flood (spam) qua nhanh (tuỳ muc do chiu dung cua server)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (got_429_at != -1) {
        std::cout << "[PASS] Sau " << got_429_at << " request, server tra ve 429 (chinh xac bi chan boi rate limit)\n";
        return 0;
    }
    else {
        std::cout << "[THAT BAI] Khong thay bao loi 429 sau 105 request, trang thai cuoi cung = " << last_status << "\n";
        return 1;
    }
}

// Kiem tra 1 endpoint (duong dan API), tra ve true neu dung trang thai mong doi, nguoc lai false
bool test_endpoint(httplib::Client& cli, const char* path, int expected_status = 200) {
    auto res = cli.Get(path);
    if (res) {
        if (res->status == expected_status) {
            cout << "[PASS] GET " << path << " (Trang thai " << res->status << ")\n";
            cout << "Du lieu tra ve: " << res->body << "\n\n";
            return true;
        }
        else {
            cout << "[THAT BAI] GET " << path << " (Trang thai " << res->status << ", ky vong phai la " << expected_status << ")\n";
            cout << "Du lieu tra ve: " << res->body << "\n\n";
            return false;
        }
    }
    else {
        cout << "[THAT BAI] GET " << path << " (khong co phan hoi - may chu co dang chay khong?)\n\n";
        return false;
    }
}

// Nhom test: cac API tra ve danh sach (khong can truyen tham so)
int test_api_lists(httplib::Client& cli) {
    int failures = 0;
    cout << "--- Nhom 1: Cac API danh sach ---\n";
    if (!test_endpoint(cli, "/api/books")) ++failures;
    if (!test_endpoint(cli, "/api/readers")) ++failures;
    if (!test_endpoint(cli, "/api/top10")) ++failures;
    if (!test_endpoint(cli, "/api/overdue")) ++failures;
    return failures;
}

// Nhom test: MC1 - tra cuu sach theo ma (co ton tai / khong ton tai / thieu tham so)
int test_api_book_lookup(httplib::Client& cli) {
    int failures = 0;
    cout << "--- Nhom 2: MC1 - Tra cuu sach theo ma ---\n";
    if (!test_endpoint(cli, "/api/book?id=B100", 200)) ++failures;
    if (!test_endpoint(cli, "/api/book?id=KHONG_TON_TAI_999", 404)) ++failures;
    if (!test_endpoint(cli, "/api/book", 404)) ++failures;
    return failures;
}

// Nhom test: lich su muon cua doc gia (thieu id / doc gia khong ton tai)
int test_api_reader_borrows(httplib::Client& cli) {
    int failures = 0;
    cout << "--- Nhom 3: Lich su muon cua doc gia ---\n";
    if (!test_endpoint(cli, "/api/reader-borrows", 400)) ++failures;
    if (!test_endpoint(cli, "/api/reader-borrows?id=KHONG_TON_TAI_999", 404)) ++failures;
    return failures;
}

int main() {
    cout << "=== BAT DAU KIEM THU WEB API (DO AN DSA) ===\n";
    cout << "(Yeu cau web_server.exe dang chay san o dia chi localhost:8080)\n\n";

    httplib::Client cli("127.0.0.1", 8080);

    // Cai dat the Token mac dinh cho tat ca cac request
    cli.set_default_headers({ {"Authorization", "Bearer my-secret-token"} });

    int total_failures = 0;
    total_failures += test_api_lists(cli);
    total_failures += test_api_book_lookup(cli);
    total_failures += test_api_reader_borrows(cli);

    // Goi ham test gioi han request - tra ve 0 neu PASS, 1 neu that bai
    total_failures += test_rate_limit(cli);

    cout << "=== HOAN TAT KIEM THU ===\n";
    if (total_failures == 0) {
        cout << "Tuyet voi! Tat ca test deu PASS (Thanh cong)\n";
        return 0;
    }
    else {
        cout << "Canh bao: Co " << total_failures << " test bi that bai\n";
        return 1;
    }
}