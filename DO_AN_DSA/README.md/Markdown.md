# DSALibrary - Hệ thống Quản lý Thư viện Đại học (Nhóm 21)

## MÔ TẢ DỰ ÁN
**DSALibrary** là hệ thống quản lý thư viện và mượn trả sách tập trung dành cho môi trường đại học, được phát triển phục vụ Đồ án môn học **Cấu trúc Dữ liệu và Giải thuật (DASA230179)** dưới sự hướng dẫn của **Thầy Vũ Đình Bảo (T-Bao)**.

Hệ thống áp dụng các Cấu trúc dữ liệu và Giải thuật (DSA) tùy biến tự cài đặt từ đầu (**From Scratch**), kết hợp với Lập trình hướng đối tượng (OOP) và **Kiến trúc 3 Tầng** (Presentation - DSA Core - Persistence). 

Chương trình phục vụ việc tra cứu tức thì tại quầy thủ thư với độ trễ tối thiểu O(1), tự động hóa hàng đợi chờ mượn sách (Waitlist FIFO), xử lý tính tiền phạt trễ hạn, khóa thẻ tự động, và trích xuất báo cáo Top 10 sách mượn nhiều nhất bằng thuật toán Min-Heap tối ưu.

---

## CẤU TRÚC DỰ ÁN

```text
DO_AN_DSA/
│
├── data/                      # Thư mục lưu trữ dữ liệu bền vững (Persistence Layer)
│   ├── books.csv              # Danh mục dữ liệu Sách
│   ├── readers.csv            # Danh mục dữ liệu Độc giả
│   └── borrows.csv            # Lịch sử và thông tin Phiếu mượn
│
├── docx/                      # Báo cáo và tài liệu đồ án (D1 -> D7)
│   ├── D1_DocHieuDe.docx
│   ├── D2_YeuCau_BaiToan.docx
│   ├── D3_BaoCaoThietKe_Q1_Q4.docx
│   ├── D5_NhatKyDebug_Benchmark.docx
│   ├── D6_PeerReview.docx
│   └── D7_NhatKyAI_Reflection.docx
│
├── include/                   # Khai báo các tệp Header (.h)
│   ├── CLI.h                  # Giao diện dòng lệnh Terminal Interface
│   ├── CustomHashTable.h      # CTDL 1: Bảng băm nối xích tự cài đặt
│   ├── CustomMinHeap.h        # CTDL 2: Min-Heap K=10 tự cài đặt
│   ├── httplib.h              # Thư viện Lightweight C++ HTTP Server
│   ├── LibraryManager.h       # Logic nghiệp vụ trung tâm (DSA Core)
│   ├── models.h               # Định nghĩa các Model (Book, Reader, BorrowRecord)
│   ├── PersistenceManager.h   # Tầng xử lý Đọc/Ghi File CSV
│   └── ReservationQueue.h     # CTDL 3: Hàng đợi FIFO danh sách liên kết
│
├── src/                       # Cài đặt chi tiết mã nguồn (.cpp)
│   ├── CLI.cpp
│   ├── CustomHashTable.cpp
│   ├── CustomMinHeap.cpp
│   ├── LibraryManager.cpp
│   ├── models.cpp
│   ├── PersistenceManager.cpp
│   ├── ReservationQueue.cpp
│   └── main.cpp               # Cổng khởi chạy ứng dụng CLI chính
│
├── tests/                     # Thử nghiệm & Kiểm thử hiệu năng
│   ├── benchmark.cpp          # Bài đo đạc hiệu năng với std::chrono (1k -> 100k)
│   └── test_runner.cpp        # Bộ kiểm thử Unit Test tự động với assert()
│
├── Web/                       # Giao diện Web Visualization
│   └── index.html             # Web Frontend (Bootstrap 5 + Async Fetch API)
│
├── src/web_main.cpp           # Cổng khởi chạy Web REST API Server (Port 8080)
├── Makefile                   # Kịch bản biên dịch tự động hóa
└── README.md                  # Tài liệu hướng dẫn dự án (File này)
```

## CÔNG NGHỆ SỬ DỤNG

### CẤU TRÚC DỮ LIỆU (Data Structures - Tự cài đặt From Scratch)
- Custom Hash Table (Separate Chaining): Tra cứu Sách, Độc giả và Phiếu mượn tức thì theo mã định danh, trung bình O(1). Bảng hiện dùng dung lượng cố định và xử lý va chạm bằng nối xích.
- Custom Min-Heap (K=10): Trích xuất Bảng xếp hạng Top 10 đầu sách mượn nhiều nhất với bộ nhớ phụ O(10) cố định và độ phức tạp O(N log K), gần O(N) khi K = 10.
- Reservation Queue (Singly Linked List): Quản lý hàng đợi chờ mượn sách theo nguyên tắc FIFO (Đến trước - Phục vụ trước) với hai con trỏ head và tail (O(1)).
- Array / Vector: Duyệt và lọc danh sách sách theo khoảng năm xuất bản hoặc trích xuất danh sách phiếu mượn quá hạn.

### THUẬT TOÁN (Algorithms)
- Hash Function (djb2): Thuật toán băm chuỗi hash = hash * 33 + c, tương đương (hash << 5) + hash, giúp phân bổ khóa.
- Rata Die Date Algorithm: Quy đổi ngày dạng ISO YYYY-MM-DD sang số ngày tuyệt đối tính từ năm 0001 để tính khoảng cách ngày trễ hạn (O(1)).
- 3-Tier Tie-Breaking Criteria: Quy tắc so sánh 3 tầng: lượt mượn, năm xuất bản và mã sách theo thứ tự ASCII.
- Automated State Transition: Tự động chuyển trạng thái thẻ độc giả sang BLOCKED khi nợ phạt vượt ngưỡng quy định.

### FRAMEWORK & THƯ VIỆN HỖ TRỢ
- C++11 (g++): Ngôn ngữ lập trình hệ thống cốt lõi.
- httplib.h: C++ Lightweight HTTP/RESTful Server cho Web Visualization.
- Bootstrap 5 & FontAwesome: Giao diện Web hiện đại, responsive.
- CSV Flat-Files: Đọc/ghi dữ liệu phẳng lưu trữ bền vững (Persistence Layer).
- std::chrono: Thư viện đo đạc thời gian thực thi chính xác cao (Microsecond / Millisecond).

## CÀI ĐẶT

### YÊU CẦU HỆ THỐNG
- Trình biên dịch C++ hỗ trợ C++11 trở lên (g++, clang++, hoặc MSVC).
- Công cụ make (khuyến nghị cho Linux/macOS/MinGW trên Windows) hoặc g++.

### HƯỚNG DẪN CÀI ĐẶT
1.Clone hoặc tải dự án về máy:
git clone [https://github.com/your-username/DO_AN_DSA.git](https://github.com/your-username/DO_AN_DSA.git)
cd DO_AN_DSA

2.Kiểm tra trình biên dịch g++:
g++ --version

3.Chạy ứng dụng
Cách 1: Sử dụng Makefile:
  make
  ./library_app.exe
  make test
  make benchmark
  make clean

Cách 2: Biên dịch thủ công bằng g++:
  g++ -std=c++11 -Wall -Iinclude src/ReservationQueue.cpp src/CustomMinHeap.cpp src/PersistenceManager.cpp src/LibraryManager.cpp src/CLI.cpp src/main.cpp -o main.exe
  ./main.exe

  Biên dịch Web Server:
  g++ -std=c++11 -Wall -Iinclude src/ReservationQueue.cpp src/CustomMinHeap.cpp src/PersistenceManager.cpp src/LibraryManager.cpp src/web_main.cpp -lws2_32 -o web_server.exe
  ./web_server.exe
  Sau khi chạy, mở trình duyệt tại: http://localhost:8080
        
## HƯỚNG DẪN SỬ DỤNG
1. Menu Quản lý Sách 📚
- Tra cứu sách theo ID: Nhập mã sách (ví dụ: B01) để lấy chi tiết thông tin tức thì (O(1)).
- Lọc sách theo khoảng năm xuất bản: Nhập Y_{min} và Y_{max} để xuất danh sách các đầu sách được xuất bản trong khoảng thời gian đó.
- Xem danh sách toàn bộ sách: Hiển thị danh mục kho sách hiện có kèm lượt mượn tích lũy.
2. Menu Quản lý Độc giả 🧍
- Tra cứu độc giả theo ID: Nhập mã sinh viên/độc giả (ví dụ: R001) để kiểm tra thông tin, trạng thái thẻ và khoản nợ phạt.
- Xem danh sách độc giả: Hiển thị trạng thái thẻ (ACTIVE hoặc BLOCKED) và tiền nợ phạt tích lũy.
3. Menu Quản lý Mượn / Trả Sách 🔄
- Lập phiếu mượn mới:
            + Kiểm tra trạng thái thẻ độc giả (Từ chối ngay nếu thẻ bị BLOCKED).
            + Nếu sách còn trong kho (so_luong_con > 0): Lập phiếu mượn thành công, giảm so_luong_con và tăng luot_muon.
            + Nếu sách đã hết kho (so_luong_con == 0): Tự động đưa độc giả vào Hàng đợi chờ mượn FIFO (ReservationQueue).
- Hoàn trả sách:
            + Tính số ngày quá hạn và tiền phạt phát sinh (5.000 VNĐ/ngày).
            + Tự động khóa thẻ (ACTIVE -> BLOCKED) nếu tổng tiền nợ tích lũy >= 100.000 VNĐ.
            + Tự động chuyển giao sách: Nếu hàng đợi chờ mượn có người, Pop độc giả đứng đầu hàng đợi và tự động khởi tạo phiếu mượn mới. Nếu hàng đợi trống, tăng so_luong_con thêm 1.
4. Menu Thống kê & Báo cáo 📊
- Báo cáo phiếu mượn quá hạn: Nhập ngày kiểm tra (dạng YYYY-MM-DD) để lọc toàn bộ các phiếu mượn chưa trả quá hạn.
- Top 10 sách mượn nhiều nhất: Trích xuất Bảng xếp hạng Top 10 sử dụng thuật toán Custom Min-Heap K=10.

Các tính năng nổi bật
1. Hàng đợi đặt trước tự động (Auto Waitlist Assignment)
- Hệ thống gắn một Hàng đợi FIFO (ReservationQueue) cho mỗi đầu sách khi so_luong_con == 0:
+ Đảm bảo tuyệt đối nguyên tắc "Đến trước - Phục vụ trước".
+ Tự động Pop và chuyển giao sách cho độc giả tiếp theo trong hàng chờ ngay khi có sự kiện trả sách.
+ Thiết lập ngưỡng MAX_QUEUE_SIZE = 100 chống tràn bộ nhớ RAM.
2. Tính tiền phạt & Tự động khóa thẻ
- Công thức phạt: TienPhat = max(0, SoNgayTre) * 5.000 VNĐ/ngày.
- Cơ chế khóa thẻ:
  + Ngay khi tổng tiền nợ phạt tích lũy >= 100.000 VNĐ, trạng thái thẻ tự động đổi từ ACTIVE -> BLOCKED.
  + Ngăn chặn tuyệt đối mọi giao dịch mượn mới từ các độc giả bị khóa thẻ.

3. Thuật toán Min-Heap Top 10 Tối ưu
- Thay vì tốn chi phí QuickSort toàn bộ N phần tử (O(N log N)), hệ thống duy trì một Min-Heap kích thước cố định K=10:
  + Độ phức tạp thời gian giảm xuống O(N log K), gần O(N) với K = 10.
  + Tiết kiệm dung lượng bộ nhớ RAM, chỉ tốn O(10) con trỏ cố định.

4. Lưu trữ dữ liệu bền vững (Persistence Layer)
- CSV Flat-Files:
  + Dữ liệu tự động nạp từ thư mục `data/` vào bộ nhớ RAM khi ứng dụng khởi động.
  + Tự động đồng bộ ghi ngược lại các tệp CSV khi đóng/thoát chương trình.

---

## DỮ LIỆU MẪU

Khi khởi chạy lần đầu, nếu chưa có dữ liệu, hệ thống sẽ tự động khởi tạo tập dữ liệu mẫu:

### SÁCH (`data/books.csv`)
Dữ liệu hiện có 151 đầu sách thuộc các nhóm công nghệ thông tin, điện - điện tử, hóa học thực phẩm, khoa học ứng dụng, thời trang, du lịch, kinh tế, cơ khí động lực học và nhiều lĩnh vực khác.

Một số mã sách mẫu: `B01` (C++ Primer Plus), `B12` (Điện Tử Cơ Bản), `B27` (Hóa Học Thực Phẩm), `B42` (Kinh Tế Vĩ Mô), `B50` (Cơ Khí Động Lực).

### ĐỘC GIẢ (`data/readers.csv`)
Dữ liệu hiện có 300 sinh viên với mã từ `R001` đến `R300`. `R003` được giữ ở trạng thái `BLOCKED` để kiểm thử chức năng khóa thẻ.

---

## VÍ DỤ SỬ DỤNG

### VÍ DỤ 1: TRA CỨU SÁCH TỨC THÌ O(1)
1. Chọn menu Tra cứu sách theo mã.
2. Nhập mã sách: `B01`.
3. Hệ thống trả về tức thì: Tên sách, Tác giả, Năm xuất bản, Số lượng còn lại, Lượt mượn tích lũy.

### VÍ DỤ 2: ĐẶT MƯỢN SÁCH KHI BẢN SAO ĐÃ HẾT
1. Độc giả `R001` lập phiếu mượn một sách đang có `so_luong_con = 0`.
2. Hệ thống đưa độc giả vào hàng đợi FIFO, tối đa 100 người cho mỗi đầu sách.
3. Khi có người trả sách, hệ thống tự động gán sách cho người đứng đầu hàng đợi nếu thẻ còn `ACTIVE`.

### VÍ DỤ 3: BÁO CÁO TOP 10 SÁCH HOT
1. Chọn menu Top 10 Sách mượn nhiều nhất.
2. Hệ thống duyệt qua Custom Min-Heap K=10 và in ra Bảng xếp hạng từ #1 đến #10 được căn chỉnh cột ngăn nắp.

---

## CẤU TRÚC DỮ LIỆU FILE CSV

### `data/books.csv`
```csv
MaSach,TenSach,TacGia,NamXuatBan,SoLuongTong,SoLuongCon,LuotMuon
B01,"C++ Primer Plus","Bjarne Stroustrup",2021,5,2,45
B02,"Introduction to Algorithms","Thomas H. Cormen",2020,3,0,120
```

### `data/readers.csv`
```csv
MaBanDoc,HoTen,Lop,TrangThaiThe,TienNoPhat
R001,"Nguyen Van An",CNTT-K65,ACTIVE,0
R003,"Le Van Cuong",HTTT-K64,BLOCKED,120000
```

### `data/borrows.csv`
```csv
MaPhieu,MaSach,MaBanDoc,NgayMuon,NgayHenTra,NgayTraThucTe,DaTra
PM1001,B01,R001,2026-08-01,2026-08-15,2026-08-14,1
PM1002,B02,R002,2026-08-10,2026-08-24,,0
```

## XỬ LÝ LỖI THƯỜNG GẶP

1. Lỗi `"Command 'g++' not found"`
   + **Nguyên nhân**: Máy tính chưa được cài đặt trình biên dịch GCC/G++ hoặc chưa cấu hình biến môi trường PATH.
  + **Khắc phục**:
     - Trên Windows: Cài đặt `MinGW-w64` và thêm đường dẫn `bin` vào biến môi trường Environment Variables.
     - Trên Linux/Ubuntu: Chạy lệnh `sudo apt update && sudo apt install build-essential`.

2. Lỗi `"Port 8080 is already in use"` khi khởi chạy Web Server
   + **Nguyên nhân**: Cổng 8080 đang bị một ứng dụng khác (như Tomcat, Docker, IIS hoặc tiến trình chạy ngầm) chiếm dụng.
    + **Khắc phục**: Tắt ứng dụng đang dùng cổng 8080 hoặc mở tệp `src/web_main.cpp` và đổi cổng sang `8081` hoặc `8082`.

3. Lỗi PowerShell `Remove-Item` khi chạy `make clean`
   + **Nguyên nhân**: Cú pháp lệnh dọn dẹp mặc định của `make` bị xung đột tham số trên môi trường Windows PowerShell.
   + **Khắc phục**: Thực thi thủ công lệnh PowerShell chuẩn:
     ```powershell
     Remove-Item -Force -ErrorAction SilentlyContinue *.exe, *.o
     ```

4. Lỗi không tìm thấy file dữ liệu (`[ERROR]: Cannot open file data/books.csv`)
   + **Nguyên nhân**: Thư mục `data/` chưa được khởi tạo hoặc đường dẫn tương đối bị sai khi chạy file thực thi từ thư mục khác.
   + **Khắc phục**: Đảm bảo chạy chương trình tại thư mục gốc của dự án (`DO_AN_DSA/`) hoặc tạo sẵn thư mục `data/`.

---

## ĐỘ PHỨC TẠP THUẬT TOÁN

| Thao tác / Nghiệp vụ | Cấu trúc dữ liệu sử dụng | Độ phức tạp thời gian | Độ phức tạp không gian | Ghi chú & Đánh giá |
| :--- | :--- | :---: | :---: | :--- |
| **Tra cứu Sách / Độc giả theo ID (MC1)** | Custom Hash Table (Separate Chaining) | O(1) | O(N) | Trung bình O(1), Tối đa O(N) khi xảy ra đụng độ hàng loạt |
| **Chèn Sách / Độc giả mới** | Custom Hash Table | O(1) trung bình | O(1) | Xử lý va chạm bằng Separate Chaining |
| **Thêm độc giả vào Waitlist (YCTP_01)** | ReservationQueue (Singly Linked List) | O(1) | O(1) | Thêm vào cuối hàng chờ nhờ duy trì con trỏ `tail` |
| **Pop độc giả khỏi Waitlist (YCTP_01)** | ReservationQueue | O(1) | O(1) | Lấy phần tử ở đầu hàng chờ nhờ con trỏ `head` |
| **Tính ngày trễ hạn & Tiền phạt (YCTP_02)** | Rata Die Date Algorithm | O(1) | O(1) | Quy đổi chuỗi ISO YYYY-MM-DD ra số ngày tuyệt đối |
| **Trích xuất Top 10 Sách Hot (YCTP_03)** | Custom Min-Heap (K=10) | O(N log K), K=10 | O(K) | Tiết kiệm bộ nhớ so với sắp xếp toàn bộ O(N log N) |
| **Lọc sách theo khoảng năm XB (MC2)** | Linear Scan / Vector Iteration | O(N) | O(M) | Duyệt tuyến tính phù hợp với workload tìm kiếm tần suất 10\% |
| **Tra cứu phiếu mượn theo mã** | Borrow Index + Hash Table | O(1) trung bình | O(N) | Ánh xạ MaPhieu tới vị trí trong danh sách phiếu |
| **Kiểm tra dữ liệu đầu vào** | PersistenceManager / Validation | O(N) | O(N) | Bỏ qua dòng sai định dạng và báo trạng thái nạp dữ liệu |
| **Nạp / Đồng bộ dữ liệu CSV** | PersistenceManager | O(N) | O(N) | Đọc/ghi tệp phẳng khi bắt đầu và kết thúc chương trình |

---

## BẢNG KẾT QUẢ THỰC NGHIỆM HIỆU NĂNG (BENCHMARK) & ĐÁNH GIÁ

### 1. BẢNG SỐ LIỆU ĐO ĐẠC THỰC TẾ (`tests/benchmark.cpp`)

Kết quả thực nghiệm đo bằng đồng hồ độ phân giải cao (`std::chrono::high_resolution_clock`) trên các quy mô dữ liệu N bản ghi giả lập:

| Quy mô (N) | Linear MC1 (ms) | Hash MC1 (ms) | Range MC2 (ms) | Top 10 (ms) | Queue (ms) |
| :---: | ---: | ---: | ---: | ---: | ---: |
| **1.000** | 1.222 | 0.042 | 0.001 | 0.123 | 0.031 |
| **10.000** | 129.895 | 0.685 | 0.026 | 0.367 | 0.032 |
| **100.000** | 18,610.339 | 20.208 | 0.602 | 3.139 | 0.034 |

### 2. ĐÁNH GIÁ CHI TIẾT KẾT QUẢ THỰC NGHIỆM

- **HIỆU NĂNG HASH TABLE**:
  + Với 100.000 bản ghi, tra cứu Hash Table mất 20.208 ms, trong khi Linear Search mất 18.610 giây.
  + Hash Table có độ phức tạp trung bình O(1) cho mỗi lần tra cứu; Linear Search dùng làm baseline có tổng chi phí O(N²) khi thực hiện N lần tìm kiếm.

- **Tối ưu hóa Top 10 Min-Heap**:
  + Với N = 100.000 đầu sách, thuật toán Min-Heap K=10 hoàn thành trong **3.139 ms**.
  + Độ phức tạp là O(N log K); vì K cố định bằng 10 nên gần tương đương O(N).

- **Hàng đợi Waitlist & Quản lý bộ nhớ**:
  + Thao tác Enqueue/Dequeue trên `ReservationQueue` mất khoảng **0.034 ms** ở mốc 100.000 bản ghi benchmark.
  + Ngưỡng `MAX_QUEUE_SIZE = 100` giúp giới hạn bộ nhớ và từ chối người đăng ký vượt quá sức chứa.
  + Cột `Memory est.` trong benchmark là bộ nhớ ước lượng cho các đối tượng Book, không phải số đo RSS toàn bộ tiến trình.