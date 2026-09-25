#ifndef LIBRARY_MANAGER_H
#define LIBRARY_MANAGER_H

#include "models.h"
#include "CustomHashTable.h"
#include "CustomMinHeap.h"
#include "ReservationQueue.h"
#include "PersistenceManager.h"

#include <string>
#include <vector>
#include <shared_mutex>

class LibraryManager {
private:
    CustomHashTable<Book> bookTable;                // Tra cứu Sách O(1)
    CustomHashTable<Reader> readerTable;            // Tra cứu Độc giả O(1)
    std::vector<BorrowRecord> borrowList;           // Danh sách Phiếu mượn
    CustomHashTable<size_t> borrowIndex;            // MaPhieu -> vị trí trong borrowList O(1) trung bình
    
    // Quản lý Hàng đợi Chờ mượn theo từng Mã Sách (Key: ma_sach, Value: ReservationQueue*)
    CustomHashTable<ReservationQueue*> reservationMap;

    std::string dataDir;
    mutable std::shared_mutex mtx; // bảo vệ trạng thái nội bộ cho thread-safety

    // Các hàm bổ trợ xử lý ngày tháng (YYYY-MM-DD)
    static bool isValidDate(const std::string& dateStr);
    static int dateToDays(const std::string& dateStr);
    static int calculateOverdueDays(const std::string& dueDateStr, const std::string& returnDateStr);
    void synchronizeInventoryWithBorrows();

public:
    LibraryManager(const std::string& dataDirectory = "data/");
    ~LibraryManager();

    // Nạp & Lưu dữ liệu CSV
    bool loadAllData();
    bool saveAllData();

    // Quản lý Sách & Tra cứu (MC1)
    bool addBook(const Book& book);
    Book* findBook(const std::string& ma_sach);
    std::vector<Book*> getAllBooks();

    // Quản lý Độc giả & Tra cứu (MC1)
    bool addReader(const Reader& reader);
    Reader* findReader(const std::string& ma_ban_doc);
    std::vector<Reader*> getAllReaders();

    // Nghiệp vụ Mượn sách (YCTP_01 & YCTP_02)
    // Trả về kết quả chuỗi thông báo kết quả giao dịch
    std::string borrowBook(const std::string& ma_ban_doc, const std::string& ma_sach, const std::string& ngay_muon, const std::string& ngay_hen_tra);

    // Nghiệp vụ Trả sách (YCTP_01 & YCTP_02)
    std::string returnBook(const std::string& ma_phieu, const std::string& ngay_tra_thuc_te);

    // Báo cáo & Thống kê (MC2 & YCTP_03)
    std::vector<BorrowRecord> getOverdueList(const std::string& currentDateStr); // MC2
    std::vector<BorrowRecord> getBorrowsByReader(const std::string& ma_ban_doc);
    std::vector<Book> getBooksByYearRange(int minYear, int maxYear);             // MC2
    std::vector<Book> getTop10PopularBooks();                                      // YCTP_03

    // Truy vấn hàng đợi chờ mượn sách
    ReservationQueue* getReservationQueue(const std::string& ma_sach);
};

#endif // LIBRARY_MANAGER_H