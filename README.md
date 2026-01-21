# MovieGo - Cinema Booking System

Hệ thống đặt vé xem phim trực tuyến với kiến trúc Client-Server, sử dụng Socket TCP và MySQL.

---

## Yêu Cầu Hệ Thống

- **OS**: Linux (Ubuntu/Debian khuyên dùng) hoặc WSL2 trên Windows
- **Compiler**: GCC (hỗ trợ C99 trở lên)
- **Database**: MySQL Server 5.7+
- **Libraries**:
  - `libmysqlclient-dev` (MySQL C API)
  - `pthread` (POSIX threads)
  - `math` library

---

### Hướng Dẫn Cài Đặt

### 1. Cài Đặt Dependencies

#### Trên Ubuntu/Debian:
```bash
# Cập nhật package manager
sudo apt update

# Cài đặt GCC compiler
sudo apt install build-essential

# Cài đặt MySQL Server
sudo apt install mysql-server

# Cài đặt MySQL development libraries
sudo apt install libmysqlclient-dev

# Khởi động MySQL service
sudo systemctl start mysql
sudo systemctl enable mysql
```

#### Trên CentOS/RHEL:
```bash
sudo yum install gcc make
sudo yum install mysql-server mysql-devel
sudo systemctl start mysqld
```

### 2. Cấu Hình MySQL

```bash
# Đăng nhập vào MySQL
sudo mysql -u root -p

# Trong MySQL shell, chạy:
# (Nếu cần đặt password cho root)
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '123456';
FLUSH PRIVILEGES;
EXIT;
```

### 3. Tạo Database

```bash
# Di chuyển vào thư mục project
cd Ltm20251/Project

# Import database schema
mysql -u root -p < db/booking.sql

# Nhập password: 123456
```

**Lưu ý**: Nếu password MySQL của bạn khác, cập nhật trong file [db/connect.h](Project/db/connect.h):
```c
#define SERVER "localhost"
#define USER "root"
#define PASSWORD "123456"
#define DATABASE "booking"
```

### 4. Compile Chương Trình

```bash
# Trong thư mục Project/
make clean  # Xóa các file binary cũ (nếu có)
make all    # Compile cả server và client
```

Sau khi compile thành công, bạn sẽ có:
- `server_out` - Server executable
- `client_out` - Client executable

---

##  Hướng Dẫn Chạy

### Bước 1: Chạy Server

```bash
# Trong thư mục Project/
./server_out <port>

# Ví dụ:
./server_out 8080
```

**Output mong đợi:**
```
Connected to database successfully
Server is listening on port 8080...
```

### Bước 2: Chạy Client

Mở terminal mới (hoặc tab mới):

```bash
# Trong thư mục Project/
./client_out <server_ip> <port>

# Ví dụ - kết nối đến localhost:
./client_out 127.0.0.1 8080

# Hoặc kết nối đến server từ xa:
./client_out 192.168.1.100 8080
```

**Output mong đợi:**
```
Connected to server successfully!
