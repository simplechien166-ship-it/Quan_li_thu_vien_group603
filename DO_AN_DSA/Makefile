CXX = g++
CXXFLAGS = -std=c++11 -Wall -Iinclude

SRC = src/ReservationQueue.cpp src/CustomMinHeap.cpp src/PersistenceManager.cpp src/LibraryManager.cpp src/CLI.cpp
MAIN_SRC = src/main.cpp
TEST_SRC = tests/test_runner.cpp
BENCH_SRC = tests/benchmark.cpp

TARGET = library_app.exe
TEST_TARGET = run_tests.exe
BENCH_TARGET = run_benchmark.exe
WEB_TARGET = web_server.exe
WEB_SRC = src/web_main.cpp

all: $(TARGET)

$(TARGET): $(MAIN_SRC) $(SRC)
    $(CXX) $(CXXFLAGS) $^ -o $@

test: $(TEST_TARGET)
    ./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) $(SRC)
    $(CXX) $(CXXFLAGS) $^ -o $@

benchmark: $(BENCH_TARGET)
    ./$(BENCH_TARGET)

web: $(WEB_TARGET)
    ./$(WEB_TARGET)

$(BENCH_TARGET): $(BENCH_SRC) $(SRC)
    $(CXX) $(CXXFLAGS) $^ -o $@

$(WEB_TARGET): $(WEB_SRC) $(SRC)
    $(CXX) $(CXXFLAGS) $^ -lws2_32 -o $@

clean:
    rm -f $(TARGET) $(TEST_TARGET) $(BENCH_TARGET) $(WEB_TARGET) *.exe
