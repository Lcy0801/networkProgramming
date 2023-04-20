#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ctime>
#include <signal.h>
#include <string.h>
using namespace std;

union Semun
{
    short val;
    struct semid_ds *buf;
    unsigned short *array;
};

void EXIT(int sig = 0)
{
    // 释放共享内存
    int shmid = shmget(0x5000, 4, 0640);
    if (shmid != -1)
    {
        shmctl(shmid, IPC_RMID, nullptr);
    }
    // 释放信号量
    int mutexid = semget(0x5001, 1, 0640);
    if (mutexid != -1)
    {
        semctl(mutexid, 0, IPC_RMID, nullptr);
    }
    int pcSemid = semget(0x5002, 1, 0640);
    if (pcSemid != -1)
    {
        semctl(pcSemid, 0, IPC_RMID, nullptr);
    }
    int cpSemid = semget(0x5003, 1, 0640);
    if (cpSemid != -1)
    {
        semctl(cpSemid, 0, IPC_RMID, nullptr);
    }
    exit(0);
}

int main()
{
    signal(SIGINT, EXIT);
    signal(SIGTERM, EXIT);
    // 创建共享内存
    int shmid = shmget(0x5000, 4, 0640 | IPC_CREAT);
    if (shmid == -1)
    {
        cout << "共享内存创建失败!" << endl;
        EXIT();
        exit(-1);
    }
    void *pshm = shmat(shmid, nullptr, 0);
    // 创建互斥信号量
    int mutextid = semget(0x5001, 1, 0640 | IPC_CREAT);
    if (mutextid == -1)
    {
        cout << "互斥信号量创建失败!" << endl;
        EXIT();
        exit(-1);
    }
    // 初始化互斥信号量
    Semun mutextValue;
    mutextValue.val = 1;
    if (semctl(mutextid, 0, SETVAL, mutextValue) == -1)
    {
        cout << "互斥信号量初始化失败!" << endl;
        EXIT();
        exit(-1);
    }
    // 互斥信号量p操作
    sembuf mutextP;
    mutextP.sem_flg = 0;
    mutextP.sem_op = -1;
    mutextP.sem_num = 0;
    // 互斥信号量v操作
    sembuf mutextV;
    mutextV.sem_flg = 0;
    mutextV.sem_op = 1;
    mutextV.sem_num = 0;

    // 创建 生产->消费 同步信号量
    int pcSemid = semget(0x5002, 1, 0640 | IPC_CREAT);
    if (pcSemid == -1)
    {
        cout << "生产->消费 同步信号量创建失败!" << endl;
        EXIT();
        exit(-1);
    }
    Semun pcSemValue;
    pcSemValue.val = 0;
    if (semctl(pcSemid, 0, SETVAL, pcSemValue) == -1)
    {
        cout << "生产->消费 同步信号量初始化失败!" << endl;
        EXIT();
        exit(-1);
    }
    // 对 生产->消费 同步信号量的v操作
    sembuf pcSemOps;
    pcSemOps.sem_num = 0;
    pcSemOps.sem_op = 1;
    pcSemOps.sem_flg = 0;

    // 创建 消费->生产 同步信号量
    int cpSemid = semget(0x5003, 1, 0640 | IPC_CREAT);
    if (cpSemid == -1)
    {
        cout << "消费->生产 同步信号量创建失败!" << endl;
        EXIT();
        exit(-1);
    }
    Semun cpSemValue;
    cpSemValue.val = 1;
    if (semctl(cpSemid, 0, SETVAL, cpSemValue) == -1)
    {
        cout << "消费->生产 同步信号量初始化失败!" << endl;
        EXIT();
        exit(-1);
    }
    // 对 消费->生产 同步信号量的v操作
    sembuf cpSemOps;
    cpSemOps.sem_num = 0;
    cpSemOps.sem_op = -1;
    cpSemOps.sem_flg = 0;
    /************信号量初始化结束*************/

    srand((unsigned int)time(nullptr));
    while (true)
    {
        int value = rand();
        if (semop(cpSemid, &cpSemOps, 1) == -1)
        {
            cout << "消费->生产 同步信号量的p操作失败!" << endl;
            EXIT();
            exit(-1);
        }
        // 临界区间上锁
        if (semop(mutextid, &mutextP, 1) == -1)
        {
            cout << "互斥信号量p操作失败!" << endl;
            EXIT();
            exit(-1);
        }
        // 向共享内存中写入数据
        memset(pshm, value, 4);
        // 临界区间解锁
        if (semop(mutextid, &mutextV, 1) == -1)
        {
            cout << "互斥信号量v操作失败!" << endl;
            EXIT();
            exit(-1);
        }
        if (semop(pcSemid, &pcSemOps, 1) == -1)
        {
            cout << "生产->消费 同步信号量的v操作失败!" << endl;
            EXIT();
            exit(-1);
        }
    }
    return 0;
}