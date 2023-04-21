#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
using namespace std;

int main()
{
    // 获取共享内存
    int shmid = shmget(0x5000, 4, 0640);
    if (shmid == -1)
    {
        cout << "无法获取共享内存!" << endl;
        exit(-1);
    }
    void *pshm = shmat(shmid, nullptr, 0);
    /************初始化信号量************/
    // 获取互斥信号量
    int mutexid = semget(0x5001, 1, 0640);
    if (mutexid == -1)
    {
        cout << "互斥信号量获取失败!" << endl;
        exit(-1);
    }
    sembuf mutexP;
    mutexP.sem_num = 0;
    mutexP.sem_op = -1;
    mutexP.sem_flg = 0;
    sembuf mutexV;
    mutexV.sem_num = 0;
    mutexV.sem_op = 1;
    mutexV.sem_flg = 0;
    // 获取 生产者->消费者同步信号量
    int pcSemid = semget(0x5002, 1, 0640);
    if (pcSemid == -1)
    {
        cout << "获取 生产者->消费者同步信号量失败!" << endl;
        exit(-1);
    }
    sembuf pcSemOps;
    pcSemOps.sem_num = 0;
    pcSemOps.sem_op = -1;
    pcSemOps.sem_flg = 0;
    // 获取 消费者->生产者同步信号量
    int cpSemid = semget(0x5003, 1, 0640);
    if (cpSemid == -1)
    {
        cout << "获取 消费者->生产者同步信号量失败!" << endl;
        exit(-1);
    }
    sembuf cpSemOps;
    cpSemOps.sem_num = 0;
    cpSemOps.sem_op = 1;
    cpSemOps.sem_flg = 0;
    // 从共享内存中读取生产者进程生产的数据
    while (true)
    {
        int value = 100;
        if (semop(pcSemid, &pcSemOps, 1) == -1)
        {
            cout << "生产->消费 同步信号量的p操作失败!" << endl;
            exit(-1);
        }
        if (semop(mutexid, &mutexP, 1) == -1)
        {
            cout << "互斥信号量p操作失败!" << endl;
            exit(-1);
        }
        memmove(&value, pshm, 4);
        cout << "从共享内存中读取到的数据是:" << value << endl;
        if (semop(mutexid, &mutexV, 1) == -1)
        {
            cout << "互斥信号量v操作失败!" << endl;
            exit(-1);
        }
        if (semop(cpSemid, &cpSemOps, 1) == -1)
        {
            cout << "消费->生产 同步信号量的v操作失败!" << endl;
            exit(-1);
        }
    }
    return 0;
}