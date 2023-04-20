#include <iostream>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
using namespace std;

int main()
{
    // 父进程忽略子进程结束的信号 防止僵尸进程的产生
    signal(SIGCHLD, SIG_IGN);
    cout << "程序开始执行" << endl;
    sleep(1);
    int x;
    int pid = fork();
    if (pid)
    {
        cout << "这里是父进程" << endl;
        x = 1;
        cout << x << endl;
        sleep(3);
    }
    else
    {
        cout << "这里是子进程" << endl;
        x = 10;
        cout << x << endl;
        sleep(3);
    }
    cout << "程序执行结束" << endl;
    return 0;
}