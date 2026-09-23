#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <iostream>

// Trạng thái thẻ độc giả (YCTP_02)
enum class ReaderStatus {
    ACTIVE,     // Thẻ đang hoạt động
    BLOCKED     // Thẻ bị khóa do nợ tiền phạt >= 100.000 VNĐ
};

inline std::string statusToString(ReaderStatus status) {
    return (status == ReaderStatus::ACTIVE) ? "ACTIVE" : "BLOCKED";
}

inline ReaderStatus stringToStatus(const std::string& str) {
    return (str == "BLOCKED") ? ReaderStatus::BLOCKED : ReaderStatus::ACTIVE;
}

// Cấu trúc Nút cho Hàng đợi Chờ mượn Sách FIFO (YCTP_01)
struct ReservationNode {
    std::string ma_ban_doc;
    ReservationNode* next;

    ReservationNode(const std::string& rId) : ma_ban_doc(rId), next(nullptr) {}
};

// Cấu trúc Sách (Book)
struct Book {
    std::string ma_sach;        // Mã khóa chính (Key - MC1)
    std::string ten_sach;
    std::string tac_gia;
    int nam_xuat_ban;           // Lọc báo cáo (MC2) & Tie-breaker (YCTP_03)
    int so_luong_tong;
    int so_luong_con;           // Kiểm tra == 0 để xếp hàng chờ (YCTP_01)
    int luot_muon;              // Xếp hạng Top 10 Sách Hot (YCTP_03)

    Book() : nam_xuat_ban(0), so_luong_tong(0), so_luong_con(0), luot_muon(0) {}

    Book(std::string id, std::string title, std::string author, int year, int total, int available, int borrows = 0)
        : ma_sach(id), ten_sach(title), tac_gia(author), nam_xuat_ban(year),
          so_luong_tong(total), so_luong_con(available), luot_muon(borrows) {}
};

// Cấu trúc Độc giả (Reader)
struct Reader {
    std::string ma_ban_doc;      // Mã khóa chính (Key - MC1)
    std::string ho_ten;
    std::string lop;
    ReaderStatus trang_thai_the; // ACTIVE hoặc BLOCKED (YCTP_02)
    double tien_no_phat;         // Tiền phạt trễ hạn lũy kế (YCTP_02)

    Reader() : trang_thai_the(ReaderStatus::ACTIVE), tien_no_phat(0.0) {}

    Reader(std::string id, std::string name, std::string cls, ReaderStatus status = ReaderStatus::ACTIVE, double fine = 0.0)
        : ma_ban_doc(id), ho_ten(name), lop(cls), trang_thai_the(status), tien_no_phat(fine) {}
};

// Cấu trúc Phiếu Mượn Trả (BorrowRecord)
struct BorrowRecord {
    std::string ma_phieu;
    std::string ma_sach;
    std::string ma_ban_doc;
    std::string ngay_muon;        // Định dạng YYYY-MM-DD
    std::string ngay_hen_tra;    // Định dạng YYYY-MM-DD
    std::string ngay_tra_thuc_te; // Để rỗng nếu chưa trả
    bool da_tra;

    BorrowRecord() : da_tra(false) {}

    BorrowRecord(std::string id, std::string bId, std::string rId, std::string bDate, std::string dDate, std::string rDate = "", bool returned = false)
        : ma_phieu(id), ma_sach(bId), ma_ban_doc(rId), ngay_muon(bDate), ngay_hen_tra(dDate), ngay_tra_thuc_te(rDate), da_tra(returned) {}
};

#endif // MODELS_H