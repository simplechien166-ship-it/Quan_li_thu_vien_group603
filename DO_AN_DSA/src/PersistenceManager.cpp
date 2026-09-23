#include "../include/PersistenceManager.h"
#include <fstream>
#include <sstream>
#include <ctime>

namespace {
bool parseInteger(const std::string& value, int& result) {
    try {
        size_t position = 0;
        result = std::stoi(value, &position);
        return position == value.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool parseDecimal(const std::string& value, double& result) {
    try {
        size_t position = 0;
        result = std::stod(value, &position);
        return position == value.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool validDate(const std::string& value) {
    if (value.size() != 10 || value[4] != '-' || value[7] != '-') return false;
    int year;
    int month;
    int day;
    if (!parseInteger(value.substr(0, 4), year) || !parseInteger(value.substr(5, 2), month) || !parseInteger(value.substr(8, 2), day)) return false;
    if (year < 1 || month < 1 || month > 12 || day < 1) return false;
    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[month - 1];
    if (month == 2 && ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))) maxDay = 29;
    return day <= maxDay;
}

bool dateNotBefore(const std::string& first, const std::string& second) {
    return first >= second;
}
}

std::vector<std::string> PersistenceManager::parseCSVLine(const std::string& line) {
    std::vector<std::string> result;
    std::string cell;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            result.push_back(cell);
            cell.clear();
        } else {
            cell += c;
        }
    }
    result.push_back(cell);
    return result;
}

bool PersistenceManager::loadBooks(const std::string& filepath, CustomHashTable<Book>& bookTable) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line); // Bỏ qua tiêu đề (Header)

    bool valid = true;
    std::time_t now = std::time(nullptr);
    std::tm* localNow = std::localtime(&now);
    int currentYear = localNow != nullptr ? localNow->tm_year + 1900 : 2026;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = parseCSVLine(line);
        int year;
        int total;
        int available;
        int borrows;
        bool rowValid = tokens.size() == 7 && !tokens[0].empty() && !tokens[1].empty() && !tokens[2].empty()
            && parseInteger(tokens[3], year) && parseInteger(tokens[4], total)
            && parseInteger(tokens[5], available) && parseInteger(tokens[6], borrows)
            && year >= 1700 && year <= currentYear && total >= 0 && available >= 0
            && available <= total && borrows >= 0 && bookTable.search(tokens[0]) == nullptr;
        if (rowValid) {
            Book b;
            b.ma_sach = tokens[0];
            b.ten_sach = tokens[1];
            b.tac_gia = tokens[2];
            b.nam_xuat_ban = year;
            b.so_luong_tong = total;
            b.so_luong_con = available;
            b.luot_muon = borrows;
            bookTable.insert(b.ma_sach, b);
        } else {
            valid = false;
        }
    }
    file.close();
    return valid;
}

bool PersistenceManager::saveBooks(const std::string& filepath, CustomHashTable<Book>& bookTable) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    file << "MaSach,TenSach,TacGia,NamXuatBan,SoLuongTong,SoLuongCon,LuotMuon\n";
    auto books = bookTable.getAllValues();
    for (const auto* b : books) {
        file << b->ma_sach << ",\"" << b->ten_sach << "\",\"" << b->tac_gia << "\","
             << b->nam_xuat_ban << "," << b->so_luong_tong << ","
             << b->so_luong_con << "," << b->luot_muon << "\n";
    }
    file.close();
    return true;
}

bool PersistenceManager::loadReaders(const std::string& filepath, CustomHashTable<Reader>& readerTable) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line); // Bỏ qua tiêu đề

    bool valid = true;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = parseCSVLine(line);
        double fine;
        bool rowValid = tokens.size() == 5 && !tokens[0].empty() && tokens[1].size() >= 2 && tokens[1].size() <= 50
            && (tokens[3] == "ACTIVE" || tokens[3] == "BLOCKED") && parseDecimal(tokens[4], fine)
            && fine >= 0.0 && readerTable.search(tokens[0]) == nullptr;
        if (rowValid) {
            Reader r;
            r.ma_ban_doc = tokens[0];
            r.ho_ten = tokens[1];
            r.lop = tokens[2];
            r.trang_thai_the = stringToStatus(tokens[3]);
            r.tien_no_phat = fine;
            readerTable.insert(r.ma_ban_doc, r);
        } else {
            valid = false;
        }
    }
    file.close();
    return valid;
}

bool PersistenceManager::saveReaders(const std::string& filepath, CustomHashTable<Reader>& readerTable) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    file << "MaBanDoc,HoTen,Lop,TrangThaiThe,TienNoPhat\n";
    auto readers = readerTable.getAllValues();
    for (const auto* r : readers) {
        file << r->ma_ban_doc << ",\"" << r->ho_ten << "\",\"" << r->lop << "\","
             << statusToString(r->trang_thai_the) << "," << r->tien_no_phat << "\n";
    }
    file.close();
    return true;
}

bool PersistenceManager::loadBorrows(const std::string& filepath, std::vector<BorrowRecord>& borrowList) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line); // Bỏ qua tiêu đề

    bool valid = true;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = parseCSVLine(line);
        bool returned = tokens.size() == 7 && (tokens[6] == "1" || tokens[6] == "0" || tokens[6] == "true" || tokens[6] == "false");
        bool rowValid = tokens.size() == 7 && !tokens[0].empty() && !tokens[1].empty() && !tokens[2].empty()
            && validDate(tokens[3]) && validDate(tokens[4]) && dateNotBefore(tokens[4], tokens[3]) && returned
            && ((! (tokens[6] == "1" || tokens[6] == "true")) ? tokens[5].empty() : (validDate(tokens[5]) && dateNotBefore(tokens[5], tokens[3])));
        bool duplicate = false;
        if (!tokens.empty()) {
            for (const auto& existing : borrowList) {
                if (existing.ma_phieu == tokens[0]) {
                    duplicate = true;
                    break;
                }
            }
        }
        rowValid = rowValid && !duplicate;
        if (rowValid) {
            BorrowRecord br;
            br.ma_phieu = tokens[0];
            br.ma_sach = tokens[1];
            br.ma_ban_doc = tokens[2];
            br.ngay_muon = tokens[3];
            br.ngay_hen_tra = tokens[4];
            br.ngay_tra_thuc_te = tokens[5];
            br.da_tra = (tokens[6] == "1" || tokens[6] == "true");
            borrowList.push_back(br);
        } else {
            valid = false;
        }
    }
    file.close();
    return valid;
}

bool PersistenceManager::saveBorrows(const std::string& filepath, const std::vector<BorrowRecord>& borrowList) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    file << "MaPhieu,MaSach,MaBanDoc,NgayMuon,NgayHenTra,NgayTraThucTe,DaTra\n";
    for (const auto& br : borrowList) {
        file << br.ma_phieu << "," << br.ma_sach << "," << br.ma_ban_doc << ","
             << br.ngay_muon << "," << br.ngay_hen_tra << "," << br.ngay_tra_thuc_te << ","
             << (br.da_tra ? "1" : "0") << "\n";
    }
    file.close();
    return true;
}