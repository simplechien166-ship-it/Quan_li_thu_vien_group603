#include "../include/httplib.h"
#include "../include/LibraryManager.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string bookToJSON(const Book* b) {
    if (!b) return "{}";
    std::ostringstream ss;
    ss << "{"
       << "\"ma_sach\":\"" << b->ma_sach << "\","
       << "\"ten_sach\":\"" << b->ten_sach << "\","
       << "\"tac_gia\":\"" << b->tac_gia << "\","
       << "\"nam_xuat_ban\":" << b->nam_xuat_ban << ","
       << "\"so_luong_tong\":" << b->so_luong_tong << ","
       << "\"so_luong_con\":" << b->so_luong_con << ","
       << "\"luot_muon\":" << b->luot_muon
       << "}";
    return ss.str();
}

int main() {
    LibraryManager manager("data/");
    manager.loadAllData();

    httplib::Server svr;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        std::ifstream file("Web/index.html");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html; charset=utf-8");
        } else {
            res.set_content("<h1>Error 404: Khong tim thay tep Web/index.html</h1>", "text/html");
        }
    });

    svr.Get("/api/books", [&manager](const httplib::Request&, httplib::Response& res) {
        auto books = manager.getAllBooks();
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < books.size(); ++i) {
            ss << bookToJSON(books[i]);
            if (i + 1 < books.size()) ss << ",";
        }
        ss << "]";
        res.set_content(ss.str(), "application/json; charset=utf-8");
    });

    svr.Get("/api/book", [&manager](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("id")) {
            std::string id = req.get_param_value("id");
            Book* b = manager.findBook(id);
            if (b) {
                res.set_content(bookToJSON(b), "application/json; charset=utf-8");
                return;
            }
        }
        res.status = 404;
        res.set_content("{\"error\":\"Not Found\"}", "application/json");
    });

    svr.Get("/api/readers", [&manager](const httplib::Request&, httplib::Response& res) {
        auto readers = manager.getAllReaders();
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < readers.size(); ++i) {
            ss << "{"
               << "\"ma_ban_doc\":\"" << readers[i]->ma_ban_doc << "\","
               << "\"ho_ten\":\"" << readers[i]->ho_ten << "\","
               << "\"lop\":\"" << readers[i]->lop << "\","
               << "\"trang_thai_the\":" << (int)readers[i]->trang_thai_the << ","
               << "\"tien_no_phat\":" << readers[i]->tien_no_phat
               << "}";
            if (i + 1 < readers.size()) ss << ",";
        }
        ss << "]";
        res.set_content(ss.str(), "application/json; charset=utf-8");
    });

    svr.Get("/api/reader-borrows", [&manager](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("id")) {
            res.status = 400;
            res.set_content("{\"error\":\"Missing reader id\"}", "application/json");
            return;
        }

        std::string readerId = req.get_param_value("id");
        if (manager.findReader(readerId) == nullptr) {
            res.status = 404;
            res.set_content("{\"error\":\"Reader not found\"}", "application/json");
            return;
        }

        auto borrows = manager.getBorrowsByReader(readerId);
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < borrows.size(); ++i) {
            Book* book = manager.findBook(borrows[i].ma_sach);
            ss << "{"
               << "\"ma_phieu\":\"" << borrows[i].ma_phieu << "\","
               << "\"ma_sach\":\"" << borrows[i].ma_sach << "\","
               << "\"ten_sach\":\"" << (book ? book->ten_sach : "Khong tim thay") << "\","
               << "\"ngay_muon\":\"" << borrows[i].ngay_muon << "\","
               << "\"ngay_hen_tra\":\"" << borrows[i].ngay_hen_tra << "\","
               << "\"ngay_tra_thuc_te\":\"" << borrows[i].ngay_tra_thuc_te << "\","
               << "\"da_tra\":" << (borrows[i].da_tra ? "true" : "false")
               << "}";
            if (i + 1 < borrows.size()) ss << ",";
        }
        ss << "]";
        res.set_content(ss.str(), "application/json; charset=utf-8");
    });

    svr.Get("/api/top10", [&manager](const httplib::Request&, httplib::Response& res) {
        auto top10 = manager.getTop10PopularBooks();
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < top10.size(); ++i) {
            ss << bookToJSON(&top10[i]);
            if (i + 1 < top10.size()) ss << ",";
        }
        ss << "]";
        res.set_content(ss.str(), "application/json; charset=utf-8");
    });

    svr.Get("/api/overdue", [&manager](const httplib::Request& req, httplib::Response& res) {
        std::string date = req.has_param("date") ? req.get_param_value("date") : "2026-09-16";
        auto list = manager.getOverdueList(date);
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < list.size(); ++i) {
            ss << "{"
               << "\"ma_phieu\":\"" << list[i].ma_phieu << "\","
               << "\"ma_sach\":\"" << list[i].ma_sach << "\","
               << "\"ma_ban_doc\":\"" << list[i].ma_ban_doc << "\","
               << "\"ngay_hen_tra\":\"" << list[i].ngay_hen_tra << "\""
               << "}";
            if (i + 1 < list.size()) ss << ",";
        }
        ss << "]";
        res.set_content(ss.str(), "application/json; charset=utf-8");
    });

    std::cout << "=======================================================\n";
    std::cout << " WEB SERVER DANG CHAY TAI: http://localhost:8080\n";
    std::cout << " Mo Trinh Duyet (Chrome/Edge) va truy cap dia chi tren!\n";
    std::cout << "=======================================================\n";

    svr.listen("0.0.0.0", 8080);
    return 0;
}
