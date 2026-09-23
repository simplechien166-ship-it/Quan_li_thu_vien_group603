#include "../include/LibraryManager.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <unordered_map>

LibraryManager::LibraryManager(const std::string& dataDirectory) : dataDir(dataDirectory) {}

LibraryManager::~LibraryManager() {
    // Giải phóng bộ nhớ động của các Hàng đợi ReservationQueue
    auto queues = reservationMap.getAllValues();
    for (auto* qPtr : queues) {
        if (qPtr && *qPtr) {
            delete *qPtr;
        }
    }
}

// Chuyển ngày YYYY-MM-DD sang số ngày tuyệt đối để tính toán chênh lệch
bool LibraryManager::isValidDate(const std::string& dateStr) {
    if (dateStr.size() != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
        return false;
    }

    int year;
    int month;
    int day;
    try {
        size_t yearPos = 0;
        size_t monthPos = 0;
        size_t dayPos = 0;
        year = std::stoi(dateStr.substr(0, 4), &yearPos);
        month = std::stoi(dateStr.substr(5, 2), &monthPos);
        day = std::stoi(dateStr.substr(8, 2), &dayPos);
        if (yearPos != 4 || monthPos != 2 || dayPos != 2) return false;
    } catch (const std::exception&) {
        return false;
    }

    if (year < 1 || month < 1 || month > 12 || day < 1) return false;
    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[month - 1];
    bool leapYear = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
    if (month == 2 && leapYear) maxDay = 29;
    return day <= maxDay;
}

int LibraryManager::dateToDays(const std::string& dateStr) {
    if (!isValidDate(dateStr)) return 0;
    int y = std::stoi(dateStr.substr(0, 4));
    int m = std::stoi(dateStr.substr(5, 2));
    int d = std::stoi(dateStr.substr(8, 2));

    // Thuật toán Rata Die tính số ngày
    if (m < 3) {
        y--;
        m += 12;
    }
    return 365 * y + y / 4 - y / 100 + y / 400 + (153 * m - 457) / 5 + d;
}

int LibraryManager::calculateOverdueDays(const std::string& dueDateStr, const std::string& returnDateStr) {
    int dueDays = dateToDays(dueDateStr);
    int returnDays = dateToDays(returnDateStr);
    int diff = returnDays - dueDays;
    return (diff > 0) ? diff : 0; // Nếu trả trước hoặc đúng hạn thì số ngày quá hạn = 0
}

bool LibraryManager::loadAllData() {
    bool b = PersistenceManager::loadBooks(dataDir + "books.csv", bookTable);
    bool r = PersistenceManager::loadReaders(dataDir + "readers.csv", readerTable);
    borrowList.clear();
    borrowIndex.clear();
    bool br = PersistenceManager::loadBorrows(dataDir + "borrows.csv", borrowList);

    std::vector<BorrowRecord> validBorrows;
    validBorrows.reserve(borrowList.size());
    for (const auto& borrow : borrowList) {
        if (bookTable.search(borrow.ma_sach) != nullptr && readerTable.search(borrow.ma_ban_doc) != nullptr) {
            validBorrows.push_back(borrow);
        } else {
            br = false;
        }
    }
    borrowList.swap(validBorrows);

    for (size_t index = 0; index < borrowList.size(); ++index) {
        borrowIndex.insert(borrowList[index].ma_phieu, index);
    }

    synchronizeInventoryWithBorrows();

    // Khởi tạo Hàng đợi cho tất cả các sách
    auto books = bookTable.getAllValues();
    for (auto* book : books) {
        if (reservationMap.search(book->ma_sach) == nullptr) {
            ReservationQueue* q = new ReservationQueue();
            reservationMap.insert(book->ma_sach, q);
        }
    }

    return b && r && br;
}

bool LibraryManager::saveAllData() {
    synchronizeInventoryWithBorrows();
    bool b = PersistenceManager::saveBooks(dataDir + "books.csv", bookTable);
    bool r = PersistenceManager::saveReaders(dataDir + "readers.csv", readerTable);
    bool br = PersistenceManager::saveBorrows(dataDir + "borrows.csv", borrowList);
    return b && r && br;
}

void LibraryManager::synchronizeInventoryWithBorrows() {
    std::unordered_map<std::string, int> activeBorrowCounts;
    for (const auto& borrow : borrowList) {
        if (!borrow.da_tra) {
            activeBorrowCounts[borrow.ma_sach]++;
        }
    }

    for (auto* book : bookTable.getAllValues()) {
        if (book == nullptr) continue;
        int activeBorrows = activeBorrowCounts[book->ma_sach];
        int availableCopies = book->so_luong_tong - activeBorrows;
        book->so_luong_con = availableCopies > 0 ? availableCopies : 0;
    }
}

bool LibraryManager::addBook(const Book& book) {
    bookTable.insert(book.ma_sach, book);
    if (reservationMap.search(book.ma_sach) == nullptr) {
        ReservationQueue* q = new ReservationQueue();
        reservationMap.insert(book.ma_sach, q);
    }
    return true;
}

Book* LibraryManager::findBook(const std::string& ma_sach) {
    return bookTable.search(ma_sach);
}

std::vector<Book*> LibraryManager::getAllBooks() {
    return bookTable.getAllValues();
}

bool LibraryManager::addReader(const Reader& reader) {
    readerTable.insert(reader.ma_ban_doc, reader);
    return true;
}

Reader* LibraryManager::findReader(const std::string& ma_ban_doc) {
    return readerTable.search(ma_ban_doc);
}

std::vector<Reader*> LibraryManager::getAllReaders() {
    return readerTable.getAllValues();
}

ReservationQueue* LibraryManager::getReservationQueue(const std::string& ma_sach) {
    ReservationQueue** qPtr = reservationMap.search(ma_sach);
    return (qPtr != nullptr) ? *qPtr : nullptr;
}

// Xử lý Mượn Sách (YCTP_01 & YCTP_02)
std::string LibraryManager::borrowBook(const std::string& ma_ban_doc, const std::string& ma_sach, const std::string& ngay_muon, const std::string& ngay_hen_tra) {
    if (!isValidDate(ngay_muon) || !isValidDate(ngay_hen_tra) || dateToDays(ngay_hen_tra) < dateToDays(ngay_muon)) {
        return "LỖI: Ngày mượn hoặc ngày hẹn trả không hợp lệ.";
    }

    Reader* reader = findReader(ma_ban_doc);
    if (!reader) return "LỖI: Không tìm thấy Mã độc giả " + ma_ban_doc;

    // Kiểm tra Trạng thái thẻ (YCTP_02)
    if (reader->trang_thai_the == ReaderStatus::BLOCKED) {
        return "TỪ CHỐI GIAO DỊCH: Thẻ của độc giả [" + reader->ho_ten + "] đang bị KHÓA do nợ phạt " + std::to_string((int)reader->tien_no_phat) + " VNĐ!";
    }

    Book* book = findBook(ma_sach);
    if (!book) return "LỖI: Không tìm thấy Mã sách " + ma_sach;

    // Xử lý Hết sách (YCTP_01) -> Xếp vào Hàng đợi chờ FIFO
    if (book->so_luong_con <= 0) {
        ReservationQueue* queue = getReservationQueue(ma_sach);
        if (queue) {
            if (queue->existsInQueue(ma_ban_doc)) {
                return "THÔNG BÁO: Độc giả [" + reader->ho_ten + "] đã có trong hàng đợi của sách [" + book->ten_sach + "].";
            }
            if (!queue->push(ma_ban_doc)) {
                return "TỪ CHỐI: Hàng đợi của sách [" + book->ten_sach + "] đã đầy (tối đa 100 người).";
            }
            return "THÔNG BÁO: Sách [" + book->ten_sach + "] hiện đã HẾT. Đã tự động xếp độc giả [" + reader->ho_ten + "] vào Hàng đợi chờ FIFO (Vị trí #" + std::to_string(queue->getSize()) + ").";
        }
    }

    // Thực hiện tạo phiếu mượn mới
    book->so_luong_con--;
    book->luot_muon++;

    std::string ma_phieu = "PM" + std::to_string(borrowList.size() + 1001);
    BorrowRecord record(ma_phieu, ma_sach, ma_ban_doc, ngay_muon, ngay_hen_tra, "", false);
    borrowList.push_back(record);
    borrowIndex.insert(ma_phieu, borrowList.size() - 1);

    return "THÀNH CÔNG: Đã tạo Phiếu mượn [" + ma_phieu + "] cho độc giả [" + reader->ho_ten + "] mượn sách [" + book->ten_sach + "].";
}

// Xử lý Trả Sách & Gán Sách Tự động (YCTP_01 & YCTP_02)
std::string LibraryManager::returnBook(const std::string& ma_phieu, const std::string& ngay_tra_thuc_te) {
    if (!isValidDate(ngay_tra_thuc_te)) {
        return "LỖI: Ngày trả thực tế không hợp lệ.";
    }

    BorrowRecord* record = nullptr;
    size_t* recordIndex = borrowIndex.search(ma_phieu);
    if (recordIndex != nullptr && *recordIndex < borrowList.size() && !borrowList[*recordIndex].da_tra) {
        record = &borrowList[*recordIndex];
    }

    if (!record) return "LỖI: Không tìm thấy Phiếu mượn hợp lệ hoặc phiếu đã được trả!";

    if (dateToDays(ngay_tra_thuc_te) < dateToDays(record->ngay_muon)) {
        return "LỖI: Ngày trả không được trước ngày mượn.";
    }

    record->da_tra = true;
    record->ngay_tra_thuc_te = ngay_tra_thuc_te;

    Book* book = findBook(record->ma_sach);
    Reader* reader = findReader(record->ma_ban_doc);

    std::string resultMsg = "Trả sách thành công.";

    // 1. Tính tiền phạt quá hạn (YCTP_02)
    int overdueDays = calculateOverdueDays(record->ngay_hen_tra, ngay_tra_thuc_te);
    if (overdueDays > 0 && reader != nullptr) {
        double fine = overdueDays * 5000.0; // 5.000 VNĐ / ngày
        reader->tien_no_phat += fine;
        resultMsg += "\n[CẢNH BÁO]: Quá hạn " + std::to_string(overdueDays) + " ngày! Tiền phạt phát sinh: " + std::to_string((int)fine) + " VNĐ.";

        // Khóa thẻ tự động nếu tổng tiền nợ >= 100.000 VNĐ
        if (reader->tien_no_phat >= 100000.0) {
            reader->trang_thai_the = ReaderStatus::BLOCKED;
            resultMsg += "\n[KHOÁ THẺ]: Tổng tiền nợ (" + std::to_string((int)reader->tien_no_phat) + " VNĐ) >= 100.000 VNĐ. Thẻ độc giả đã bị KHÓA!";
        }
    }

    // 2. Gán sách tự động cho người chờ đầu hàng đợi (YCTP_01)
    ReservationQueue* queue = getReservationQueue(record->ma_sach);
    std::string nextReaderId;
    bool assigned = false;
    int skippedReaders = 0;
    while (queue && queue->pop(nextReaderId)) {
        Reader* nextReader = findReader(nextReaderId);
        if (nextReader != nullptr && nextReader->trang_thai_the == ReaderStatus::ACTIVE && book != nullptr) {
            book->luot_muon++;
            std::string newPhieuId = "PM" + std::to_string(borrowList.size() + 1001);
            BorrowRecord autoRecord(newPhieuId, book->ma_sach, nextReaderId, ngay_tra_thuc_te, ngay_tra_thuc_te, "", false);
            borrowList.push_back(autoRecord);
            borrowIndex.insert(newPhieuId, borrowList.size() - 1);
            resultMsg += "\n[TỰ ĐỘNG GÁN SÁCH]: Sách vừa trả đã được tự động cấp cho độc giả trong hàng đợi chờ: [" + nextReader->ho_ten + "] (Phiếu mượn mới: " + newPhieuId + ").";
            assigned = true;
            break;
        }
        skippedReaders++;
    }

    if (!assigned && book != nullptr) {
        book->so_luong_con++;
    }
    if (skippedReaders > 0) {
        resultMsg += "\n[HÀNG ĐỢI]: Đã bỏ qua " + std::to_string(skippedReaders) + " người không đủ điều kiện nhận sách.";
    }

    return resultMsg;
}

// Báo cáo danh sách quá hạn (MC2)
std::vector<BorrowRecord> LibraryManager::getOverdueList(const std::string& currentDateStr) {
    std::vector<BorrowRecord> overdueList;
    for (const auto& br : borrowList) {
        if (!br.da_tra) {
            int overdueDays = calculateOverdueDays(br.ngay_hen_tra, currentDateStr);
            if (overdueDays > 0) {
                overdueList.push_back(br);
            }
        }
    }
    return overdueList;
}

std::vector<BorrowRecord> LibraryManager::getBorrowsByReader(const std::string& ma_ban_doc) {
    std::vector<BorrowRecord> readerBorrows;
    for (const auto& borrow : borrowList) {
        if (borrow.ma_ban_doc == ma_ban_doc) {
            readerBorrows.push_back(borrow);
        }
    }
    return readerBorrows;
}

std::vector<Book> LibraryManager::getBooksByYearRange(int minYear, int maxYear) {
    std::vector<Book> filteredBooks;
    if (minYear > maxYear) {
        return filteredBooks;
    }

    auto books = bookTable.getAllValues();
    for (const auto* book : books) {
        if (book != nullptr && book->nam_xuat_ban >= minYear && book->nam_xuat_ban <= maxYear) {
            filteredBooks.push_back(*book);
        }
    }

    return filteredBooks;
}

// Trích xuất Top 10 Sách Mượn Nhiều Nhất với Min-Heap (YCTP_03)
std::vector<Book> LibraryManager::getTop10PopularBooks() {
    CustomMinHeap minHeap;
    auto books = bookTable.getAllValues();

    for (const auto* bookPtr : books) {
        if (bookPtr != nullptr) {
            minHeap.insertOrUpdate(*bookPtr);
        }
    }

    return minHeap.getSortedTop10();
}