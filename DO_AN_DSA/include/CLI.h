#ifndef CLI_H
#define CLI_H

#include "LibraryManager.h"

class CLI {
private:
    LibraryManager manager;

    void printHeader() const;
    void printMenu() const;
    
    void handleFindBook();
    void handleFindReader();
    void handleBorrowBook();
    void handleReturnBook();
    void handleOverdueReport();
    void handleBooksByYearRange();
    void handleTop10Books();
    void handleShowAllBooks();
    void handleShowAllReaders();

public:
    CLI();
    void run();
};

#endif // CLI_H