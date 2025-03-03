#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int p_left) __attribute__((noreturn));
// Hàm đệ quy thực hiện sàng số nguyên tố
void primes(int p_left) {
    int prime, num;
    
    // Đọc số nguyên tố đầu tiên từ pipe
    if (read(p_left, &prime, sizeof(int)) == 0) {
        close(p_left);
        exit(0);
    }
    printf("prime %d\n", prime);

    int p_right[2]; // Pipe mới cho tiến trình tiếp theo
    if (pipe(p_right) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    } else if (pid == 0) {  
        // Tiến trình con
        close(p_right[1]);  // Đóng đầu ghi của pipe mới
        close(p_left);       // Đóng pipe cũ
        primes(p_right[0]);  // Gọi đệ quy
    } else {
        // Tiến trình cha
        close(p_right[0]);  // Đóng đầu đọc của pipe mới
        while (read(p_left, &num, sizeof(int)) > 0) {
            if (num % prime != 0) {
                write(p_right[1], &num, sizeof(int));
            }
        }
        // Đóng tất cả pipe sau khi hoàn thành
        close(p_right[1]);
        close(p_left);
        wait(0);  // Chờ tiến trình con kết thúc
        exit(0);
    }
}

// Hàm chính
int main() {
    int p[2];  
    if (pipe(p) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    } else if (pid == 0) {
        close(p[1]); // Đóng đầu ghi của pipe trong tiến trình con
        primes(p[0]); // Bắt đầu sàng số
    } else {
        close(p[0]); // Đóng đầu đọc của pipe trong tiến trình cha
        for (int i = 2; i <= 280; i++) {
            if (write(p[1], &i, sizeof(int)) != sizeof(int)) {
                fprintf(2, "Error writing to pipe\n");
                exit(1);
            }
        }
        close(p[1]); // Đóng đầu ghi sau khi ghi xong
        wait(0);     // Chờ tiến trình con hoàn thành
        exit(0);
    }
}
