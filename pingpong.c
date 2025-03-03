#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int p1[2], p2[2];  // Tạo hai pipe: p1 (cha -> con), p2 (con -> cha)
    pipe(p1);
    pipe(p2);

    int pid = fork();
    if (pid < 0) {
        printf("Fork failed\n");
        exit(1);
    }

    if (pid == 0) {  // Tiến trình con
        char buf;
        read(p1[0], &buf, 1);  // Nhận "ping" từ cha
        printf("%d: received ping\n", getpid());

        write(p2[1], "X", 1);  // Gửi "pong" về cho cha
        exit(0);
    } else {  // Tiến trình cha
        write(p1[1], "X", 1);  // Gửi "ping" cho con

        char buf;
        read(p2[0], &buf, 1);  // Nhận "pong" từ con
        printf("%d: received pong\n", getpid());

        wait(0);  // Chờ tiến trình con kết thúc

    }
}
