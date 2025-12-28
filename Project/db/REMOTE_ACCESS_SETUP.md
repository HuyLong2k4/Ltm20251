# Hướng dẫn cấu hình MySQL cho phép kết nối từ xa

## 1. Cấu hình MySQL Server

### Bước 1: Chỉnh sửa file cấu hình MySQL
```bash
sudo nano /etc/mysql/mysql.conf.d/mysqld.cnf
```

Tìm dòng:
```
bind-address = 127.0.0.1
```

Thay đổi thành một trong hai cách:
- **Cách 1** (Cho phép tất cả IP): `bind-address = 0.0.0.0`
- **Cách 2** (Cho phép IP cụ thể): `bind-address = 192.168.1.100` (IP của server)

Lưu file và thoát (Ctrl+X, Y, Enter)

### Bước 2: Khởi động lại MySQL
```bash
sudo systemctl restart mysql
# hoặc
sudo service mysql restart
```

### Bước 3: Kiểm tra MySQL đã lắng nghe trên tất cả interface
```bash
# Dùng ss (công cụ hiện đại, thường có sẵn)
sudo ss -tuln | grep 3306

# Hoặc dùng netstat (nếu đã cài net-tools)
sudo netstat -tuln | grep 3306

# Hoặc dùng lsof
sudo lsof -i :3306
```

Kết quả mong muốn: `0.0.0.0:3306` (thay vì `127.0.0.1:3306`)

**Nếu gặp lỗi "netstat: command not found", cài đặt:**
```bash
sudo apt install net-tools
```

## 2. Cấp quyền truy cập cho User MySQL

### Đăng nhập vào MySQL
```bash
mysql -u root -p
```

### Cấp quyền cho user từ một IP cụ thể
```sql
-- MySQL 8.0+: Tạo user trước, sau đó cấp quyền

-- Cách 1: Cho phép root từ IP cụ thể (ví dụ: 192.168.1.50)
CREATE USER 'root'@'192.168.1.50' IDENTIFIED BY '123456';
GRANT ALL PRIVILEGES ON booking.* TO 'root'@'192.168.1.50';

-- Cách 2: Cho phép root từ bất kỳ IP nào (không an toàn, chỉ dùng cho dev)
CREATE USER 'root'@'%' IDENTIFIED BY '123456';
GRANT ALL PRIVILEGES ON booking.* TO 'root'@'%';

-- Nếu user đã tồn tại, chỉ cần GRANT:
GRANT ALL PRIVILEGES ON booking.* TO 'root'@'%';

-- Cập nhật quyền
FLUSH PRIVILEGES;

-- Kiểm tra quyền đã cấp
SELECT user, host FROM mysql.user WHERE user='root';
```

### Tạo user mới cho remote access (khuyến nghị)
```sql
-- Tạo user mới thay vì dùng root
CREATE USER 'booking_user'@'%' IDENTIFIED BY 'password_manh';

-- Cấp quyền cho database booking
GRANT ALL PRIVILEGES ON booking.* TO 'booking_user'@'%';

-- Cập nhật quyền
FLUSH PRIVILEGES;
```

## 3. Cấu hình Firewall (nếu có)

### Ubuntu/Debian (ufw)
```bash
sudo ufw allow 3306/tcp
sudo ufw reload
```

### CentOS/RHEL (firewalld)
```bash
sudo firewall-cmd --permanent --add-port=3306/tcp
sudo firewall-cmd --reload
```

## 4. Cập nhật code kết nối

### Lấy IP của MySQL Server

**Trường hợp 1: Client và Server trên cùng 1 máy (Local)**
```c
#define SERVER "localhost"  // hoặc "127.0.0.1"
```

**Trường hợp 2: Client và Server trên cùng mạng LAN**

Trên máy có MySQL Server, chạy lệnh để lấy IP:
```bash
# Cách 1: Xem tất cả IP
ip addr show

# Cách 2: Lấy IP của interface chính
hostname -I

# Cách 3: Lọc IP cụ thể
ip addr show | grep "inet " | grep -v 127.0.0.1
```

Ví dụ kết quả: `192.168.1.100` hoặc `10.0.0.5`

**Trường hợp 3: Client và Server qua Internet (Public IP)**

Trên máy có MySQL Server:
```bash
# Lấy IP public
curl ifconfig.me
# hoặc
curl ipinfo.io/ip
# hoặc
wget -qO- ifconfig.me
```

⚠️ **Lưu ý**: Khi dùng IP public, cần cấu hình router/firewall forward port 3306.

### Cập nhật code

Trong file `db/connect.h`, thay đổi:
```c
#define SERVER "localhost"  // Thay bằng IP của MySQL server
```

Ví dụ:
```c
// Kết nối local
#define SERVER "localhost"

// Kết nối qua LAN
#define SERVER "192.168.1.100"

// Kết nối qua Internet (IP public)
#define SERVER "203.162.10.50"
```

## 5. Kiểm tra kết nối từ client

```bash
mysql -h <IP_SERVER> -u root -p booking
```

Ví dụ:
```bash
mysql -h 192.168.1.100 -u root -p booking
```

## ⚠️ Lưu ý bảo mật

1. **Không nên dùng user root từ xa** - Tạo user riêng với quyền hạn chế
2. **Đặt mật khẩu mạnh** - Không dùng mật khẩu đơn giản như "123456"
3. **Giới hạn IP** - Chỉ cho phép kết nối từ IP cần thiết, không dùng '%' (tất cả IP)
4. **Sử dụng SSL/TLS** - Mã hóa kết nối MySQL để bảo mật
5. **Đặt mật khẩu trong file config** - Không nên hardcode, dùng file .env
6. **Firewall** - Chỉ mở port 3306 cho các IP tin cậy

## Troubleshooting

### Lỗi: "Can't connect to MySQL server"
- Kiểm tra MySQL service đang chạy: `sudo systemctl status mysql`
- Kiểm tra firewall có block port 3306 không
- Kiểm tra bind-address trong config

### Lỗi: "Access denied"
- Kiểm tra user có quyền truy cập từ host đó chưa
- Chạy lại `FLUSH PRIVILEGES;`

### Lỗi: "Too many connections"
- Tăng giới hạn connection trong MySQL config: `max_connections = 200`
