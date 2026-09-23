#include "../include/CLI.h"
#include <iostream>
#include <iomanip>
#include <limits>

CLI::CLI() : manager("data/") {}

void CLI::printHeader() const {
    std::cout << "===============================================================\n";
    std::cout << "        HE THONG QUAN LY THU VIEN (LIBRARY MANAGEMENT)        \n";
    std::cout << "===============================================================\n";
}

void CLI::printMenu() const {
    std::cout << "\n--- CHON CHUC NANG NGUOI DUNG ---\n";
    std::cout << "1. Tra cuu Thong tin Sach (MC1 - O(1))\n";
    std::cout << "2. Tra cuu Thong tin Doc gia (MC1 - O(1))\n";
    std::cout << "3. Muon Sach (Auto Queue FIFO neu het sach - YCTP_01)\n";
    std::cout << "4. Tra Sach (Auto Fine 5k/ngay & Auto-Assign Queue - YCTP_01,02)\n";
    std::cout << "5. Bao cao Danh sach Muon Qua han (MC2)\n";
    std::cout << "6. Thong ke Top 10 Sach Hot nhat (Min-Heap K=10 - YCTP_03)\n";
    std::cout << "7. Hien thi Tat ca Sach trong Thu vien\n";
    std::cout << "8. Hien thi Tat ca Doc gia\n";
    std::cout << "9. Loc Sach theo Khoang Nam Xuat Ban (MC2)\n";
    std::cout << "0. Luu du lieu va Thoat chuong trinh\n";
    std::cout << "---------------------------------------------------------------\n";
    std::cout << "Nhap lua chon [0-9]: ";
}

void CLI::handleFindBook() {
    std::string id;
    std::cout << "Nhap Ma Sach can tim: ";
    std::cin >> id;

    Book* b = manager.findBook(id);
    if (b) {
        std::cout << "\n[THONG TIN SACH TIM THAY]\n";
        std::cout << "Ma Sach     : " << b->ma_sach << "\n";
        std::cout << "Ten Sach    : " << b->ten_sach << "\n";
        std::cout << "Tac Gia     : " << b->tac_gia << "\n";
        std::cout << "Nam Xuat Ban: " << b->nam_xuat_ban << "\n";
        std::cout << "So Luong    : " << b->so_luong_con << " / " << b->so_luong_tong << "\n";
        std::cout << "Luot Muon   : " << b->luot_muon << "\n";

        auto* queue = manager.getReservationQueue(id);
        if (queue && !queue->isEmpty()) {
            std::cout << "Hang doi cho : " << queue->getSize() << " nguoi dang cho mượn.\n";
        }
    } else {
        std::cout << "\n[!] KHONG TIM THAY SACH VOI MA: " << id << "\n";
    }
}

void CLI::handleFindReader() {
    std::string id;
    std::cout << "Nhap Ma Doc Gia can tim: ";
    std::cin >> id;

    Reader* r = manager.findReader(id);
    if (r) {
        std::cout << "\n[THONG TIN DOC GIA TIM THAY]\n";
        std::cout << "Ma Doc Gia  : " << r->ma_ban_doc << "\n";
        std::cout << "Ho Ten      : " << r->ho_ten << "\n";
        std::cout << "Lop         : " << r->lop << "\n";
        std::cout << "Trang Thai  : " << statusToString(r->trang_thai_the) << "\n";
        std::cout << "Tien No Phat: " << (long long)r->tien_no_phat << " VNĐ\n";
    } else {
        std::cout << "\n[!] KHONG TIM THAY DOC GIA VOI MA: " << id << "\n";
    }
}

void CLI::handleBorrowBook() {
    std::string rId, bId, bDate, dDate;
    std::cout << "Nhap Ma Doc Gia: "; std::cin >> rId;
    std::cout << "Nhap Ma Sach   : "; std::cin >> bId;
    std::cout << "Nhap Ngay Muon (YYYY-MM-DD): "; std::cin >> bDate;
    std::cout << "Nhap Ngay Hen Tra (YYYY-MM-DD): "; std::cin >> dDate;

    std::string res = manager.borrowBook(rId, bId, bDate, dDate);
    std::cout << "\n>>> " << res << "\n";
}

void CLI::handleReturnBook() {
    std::string pId, rDate;
    std::cout << "Nhap Ma Phieu Muon: "; std::cin >> pId;
    std::cout << "Nhap Ngay Tra Thuc Te (YYYY-MM-DD): "; std::cin >> rDate;

    std::string res = manager.returnBook(pId, rDate);
    std::cout << "\n>>> " << res << "\n";
}

void CLI::handleOverdueReport() {
    std::string curDate;
    std::cout << "Nhap Ngay Hien Tai de Kiem Tra (YYYY-MM-DD): ";
    std::cin >> curDate;

    auto list = manager.getOverdueList(curDate);
    std::cout << "\n================ BAN TIN MƯON SACH QUA HAN ================\n";
    if (list.empty()) {
        std::cout << "Khong co phieu muon nao qua han tinh den ngay " << curDate << ".\n";
    } else {
        std::cout << std::left << std::setw(12) << "Ma Phieu"
                  << std::setw(12) << "Ma Sach"
                  << std::setw(12) << "Ma Doc Gia"
                  << std::setw(15) << "Ngay Hen Tra" << "\n";
        std::cout << "-----------------------------------------------------------\n";
        for (const auto& br : list) {
            std::cout << std::left << std::setw(12) << br.ma_phieu
                      << std::setw(12) << br.ma_sach
                      << std::setw(12) << br.ma_ban_doc
                      << std::setw(15) << br.ngay_hen_tra << "\n";
        }
    }
}

void CLI::handleTop10Books() {
    auto top10 = manager.getTop10PopularBooks();
    std::cout << "\n================ TOP 10 SACH MUON NHIEU NHAT ================\n";
    std::cout << std::left << std::setw(6) << "Hang"
              << std::setw(10) << "Ma Sach"
              << std::setw(32) << "Ten Sach"
              << std::setw(12) << "Nam XB"
              << std::setw(12) << "Luot Muon" << "\n";
    std::cout << "-------------------------------------------------------------\n";
    int rank = 1;
    for (const auto& b : top10) {
        std::cout << std::left << std::setw(6) << rank++
                  << std::setw(10) << b.ma_sach
                  << std::setw(32) << (b.ten_sach.length() > 30 ? b.ten_sach.substr(0, 27) + "..." : b.ten_sach)
                  << std::setw(12) << b.nam_xuat_ban
                  << std::setw(12) << b.luot_muon << "\n";
    }
}

void CLI::handleBooksByYearRange() {
    int minYear;
    int maxYear;
    std::cout << "Nhap Nam Xuat Ban Tu: ";
    std::cin >> minYear;
    std::cout << "Nhap Nam Xuat Ban Den: ";
    std::cin >> maxYear;

    auto books = manager.getBooksByYearRange(minYear, maxYear);
    std::cout << "\n================ SACH THEO KHOANG NAM ================\n";
    if (minYear > maxYear) {
        std::cout << "Khoang nam khong hop le: nam bat dau phai nho hon hoac bang nam ket thuc.\n";
        return;
    }

    if (books.empty()) {
        std::cout << "Khong tim thay sach trong khoang " << minYear << " - " << maxYear << ".\n";
        return;
    }

    std::cout << std::left << std::setw(10) << "Ma Sach"
              << std::setw(34) << "Ten Sach"
              << std::setw(24) << "Tac Gia"
              << std::setw(12) << "Nam XB" << "\n";
    std::cout << "---------------------------------------------------------------\n";
    for (const auto& book : books) {
        std::cout << std::left << std::setw(10) << book.ma_sach
                  << std::setw(34) << (book.ten_sach.length() > 32 ? book.ten_sach.substr(0, 29) + "..." : book.ten_sach)
                  << std::setw(24) << (book.tac_gia.length() > 22 ? book.tac_gia.substr(0, 19) + "..." : book.tac_gia)
                  << std::setw(12) << book.nam_xuat_ban << "\n";
    }
}

void CLI::handleShowAllBooks() {
    auto books = manager.getAllBooks();
    std::cout << "\n================ DANH SACH TAAT CA SACH ================\n";
    std::cout << std::left << std::setw(10) << "Ma Sach"
              << std::setw(30) << "Ten Sach"
              << std::setw(10) << "Con Lai"
              << std::setw(12) << "Luot Muon" << "\n";
    std::cout << "--------------------------------------------------------\n";
    for (const auto* b : books) {
        std::cout << std::left << std::setw(10) << b->ma_sach
                  << std::setw(30) << (b->ten_sach.length() > 28 ? b->ten_sach.substr(0, 25) + "..." : b->ten_sach)
                  << std::setw(10) << (std::to_string(b->so_luong_con) + "/" + std::to_string(b->so_luong_tong))
                  << std::setw(12) << b->luot_muon << "\n";
    }
}

void CLI::handleShowAllReaders() {
    auto readers = manager.getAllReaders();
    std::cout << "\n================ DANH SACH DOC GIA ================\n";
    std::cout << std::left << std::setw(12) << "Ma Doc Gia"
              << std::setw(25) << "Ho Ten"
              << std::setw(12) << "Trang Thai"
              << std::setw(15) << "No Phat (VND)" << "\n";
    std::cout << "---------------------------------------------------\n";
    for (const auto* r : readers) {
        std::cout << std::left << std::setw(12) << r->ma_ban_doc
                  << std::setw(25) << r->ho_ten
                  << std::setw(12) << statusToString(r->trang_thai_the)
                  << std::setw(15) << (long long)r->tien_no_phat << "\n";
    }
}

void CLI::run() {
    printHeader();
    std::cout << "Dang nap du lieu tu thu muc data/...\n";
    if (manager.loadAllData()) {
        std::cout << "Nap du lieu thanh cong!\n";
    } else {
        std::cout << "[CANH BAO] Tap tin du lieu chua co hoac bi loi, he thong se tao moi khi luu.\n";
    }

    int choice = -1;
    while (choice != 0) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Loi nhap lieu! Vui long nhap so tu 0 den 8.\n";
            continue;
        }

        switch (choice) {
            case 1: handleFindBook(); break;
            case 2: handleFindReader(); break;
            case 3: handleBorrowBook(); break;
            case 4: handleReturnBook(); break;
            case 5: handleOverdueReport(); break;
            case 6: handleTop10Books(); break;
            case 7: handleShowAllBooks(); break;
            case 8: handleShowAllReaders(); break;
            case 9: handleBooksByYearRange(); break;
            case 0:
                std::cout << "\nDang luu du lieu xuong dia...\n";
                if (manager.saveAllData()) {
                    std::cout << "Luu du lieu thanh cong. Tam biet!\n";
                } else {
                    std::cout << "Loi khi luu du lieu!\n";
                }
                break;
            default:
                std::cout << "Lua chon khong hop le!\n";
        }
    }
}