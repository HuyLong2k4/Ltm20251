# Server Response Codes

## Success Codes (1xxx)

### Authentication & User Management
- `1010` - LOGIN_SUCCESS_USER - User đăng nhập thành công
- `1011` - LOGIN_SUCCESS_MANAGER - Manager đăng nhập thành công  
- `1012` - LOGIN_SUCCESS_ADMIN - Admin đăng nhập thành công
- `1020` - REGISTER_SUCCESS - Đăng ký thành công
- `1030` - LOGOUT_SUCCESS - Đăng xuất thành công
- `1110` - CHANGE_PASSWORD_SUCCESS - Đổi mật khẩu thành công

### Film Management
- `1040` - ADD_FILM_SUCCESS - Thêm phim thành công
- `1050` - POST_FILM_SUCCESS - Đăng phim thành công
- `1060` - EDIT_FILM_SUCCESS / SHOW_CATEGORY_SUCCESS - Chỉnh sửa phim / Hiển thị danh mục thành công
- `1070` - BROWSE_CATEGORY_SUCCESS - Duyệt theo danh mục thành công
- `1071` - BROWSE_THEATER_SUCCESS - Duyệt theo rạp thành công
- `1072` - BROWSE_TIME_SUCCESS - Duyệt theo khung giờ thành công
- `1073` - FIND_FILM_SUCCESS - Tìm phim thành công

### Ticket & Seat Management
- `1080` - VIEW_CHAIR_SUCCESS - Xem ghế thành công
- `1090` - CHOOSE_CHAIR_SUCCESS / ADD_SHOWTIME_SUCCESS - Chọn ghế / Thêm suất chiếu thành công
- `1200` - BOOK_TICKET_SUCCESS - Đặt vé thành công

### Showtime Management
- `1090` - ADD_SHOWTIME_SUCCESS - Thêm suất chiếu thành công
- `1091` - SHOW_SHOWTIME_SUCCESS - Hiển thị suất chiếu thành công
- `1100` - (Reserved)
- `1102` - (Reserved - Duplicate of 1091)

### Admin Functions
- `1300` - SHOW_ALL_USERS_SUCCESS - Hiển thị tất cả users thành công
- `1301` - DELETE_USER_SUCCESS - Xóa user thành công
- `1302` - UPDATE_USER_ROLE_SUCCESS - Cập nhật role user thành công

## Error Codes (2xxx)

### Authentication & User Errors
- `2011` - LOGIN_FAIL - Đăng nhập thất bại (sai username/password)
- `2012` - LOGIN_ALREADY - User đã đăng nhập
- `2021` - REGISTER_FAIL - Đăng ký thất bại
- `2110` - CHANGE_PASSWORD_FAIL - Đổi mật khẩu thất bại

### Film Management Errors
- `2041` - ADD_FILM_FAIL - Thêm phim thất bại
- `2051` - POST_FILM_FAIL - Đăng phim thất bại
- `2060` - SHOW_CATEGORY_FAIL - Hiển thị danh mục thất bại
- `2061` - EDIT_FILM_FAIL - Chỉnh sửa phim thất bại
- `2062` - NO_EDIT_FILM - Không có chỉnh sửa
- `2071` - BROWSE_FAIL - Duyệt thất bại
- `2072` - FIND_FILM_FAIL - Tìm phim thất bại

### Ticket & Seat Errors
- `2081` - VIEW_CHAIR_FAIL - Xem ghế thất bại
- `2091` - CHOOSE_CHAIR_FAIL - Chọn ghế thất bại
- `2200` - BOOK_TICKET_FAIL - Đặt vé thất bại

### Showtime Management Errors
- `2090` - ADD_SHOWTIME_FAIL - Thêm suất chiếu thất bại
- `2092` - SHOW_TIME_FAIL - Hiển thị suất chiếu thất bại
- `2100` - (Reserved)

### Admin Function Errors
- `2300` - SHOW_ALL_USERS_FAIL - Hiển thị users thất bại
- `2301` - DELETE_USER_FAIL - Xóa user thất bại
- `2302` - UPDATE_USER_ROLE_FAIL - Cập nhật role thất bại

## Data Not Found Codes (3xxx)

### Load Data Errors
- `3001` - CANT_LOAD_CATEGORIES - Không thể load danh mục
- `3002` - NO_CATEGORIES - Không có danh mục
- `3003` - CANT_LOAD_CINEMAS - Không thể load rạp
- `3004` - NO_CINEMAS - Không có rạp
- `3005` - CANT_LOAD_SHOWTIMES - Không thể load suất chiếu
- `3006` - NO_SHOWTIMES - Không có suất chiếu
- `3007` - CANT_LOAD_FILMS - Không thể load phim
- `3008` - NO_FILMS - Không có phim
- `3009` - CANT_LOAD_SEATS - Không thể load ghế
- `3010` - NO_SEATS - Không có ghế
- `3011` - CANT_LOAD_ROOMS - Không thể load phòng
- `3012` - NO_ROOMS - Không có phòng

### Ticket Errors
- `3013` - NO_TICKETS - Không có vé
- `3014` - FAILED_RETRIEVE - Lấy thông tin thất bại
- `3015` - TICKET_NOT_FOUND - Không tìm thấy vé

### Validation Errors
- `3016` - INVALID_CINEMA - Cinema ID không hợp lệ
- `3017` - FAILED_ROOMS - Lỗi lấy danh sách phòng
- `3018` - INVALID_REQUEST - Request không hợp lệ
- `3019` - INVALID_SHOWTIME - Showtime không hợp lệ
- `3020` - NO_SEATS_AVAILABLE - Không còn ghế trống
- `3021` - INVALID_FILM_ID - Film ID không hợp lệ
- `3022` - DB_QUERY_ERROR - Lỗi query database
- `3023` - INVALID_CINEMA_ID - Cinema ID không hợp lệ
- `3024` - INVALID_ROOM_ID - Room ID không hợp lệ hoặc phòng không active
- `3025` - ROOM_NOT_BELONG_CINEMA - Phòng không thuộc rạp đã chọn
- `3026` - INVALID_DATETIME_FORMAT - Format datetime không hợp lệ
- `3027` - SCHEDULING_CONFLICT - Xung đột lịch chiếu

## Usage

Khi nhận response từ server:
1. Server sẽ gửi response code đầu tiên (dạng số nguyên)
2. Client kiểm tra code:
   - 1xxx = Success - Có thể có data tiếp theo
   - 2xxx = Error - Không có data
   - 3xxx = Not Found / Validation Error - Không có data hoặc data rỗng
3. Sau code, nếu có data sẽ được gửi kèm theo
4. Kết thúc bằng "END"

## Example

```
Success với data:
1073\r\n
ID: 1 | Title: Avengers | Category: Action | Show_time: 120 minutes\r\n
ID: 2 | Title: Avatar | Category: Sci-Fi | Show_time: 180 minutes\r\n
END\r\n

Error:
2011\r\n
END\r\n

No data:
3008\r\n
END\r\n
```
