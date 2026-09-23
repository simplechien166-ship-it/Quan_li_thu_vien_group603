#ifndef PERSISTENCE_MANAGER_H
#define PERSISTENCE_MANAGER_H

#include "models.h"
#include "CustomHashTable.h"
#include <string>
#include <vector>

class PersistenceManager {
private:
    static std::vector<std::string> parseCSVLine(const std::string& line);

public:
    static bool loadBooks(const std::string& filepath, CustomHashTable<Book>& bookTable);
    static bool saveBooks(const std::string& filepath, CustomHashTable<Book>& bookTable);

    static bool loadReaders(const std::string& filepath, CustomHashTable<Reader>& readerTable);
    static bool saveReaders(const std::string& filepath, CustomHashTable<Reader>& readerTable);

    static bool loadBorrows(const std::string& filepath, std::vector<BorrowRecord>& borrowList);
    static bool saveBorrows(const std::string& filepath, const std::vector<BorrowRecord>& borrowList);
};

#endif // PERSISTENCE_MANAGER_H