#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
using namespace std;

int main()
{
    // 获取共享内存
    int shmid = shmget(0x5003, 4, 0640);
    if(shmid ==-1)
    {
        cout << "无法获取共享内存!" << endl;
        return 0;
    }
    
}