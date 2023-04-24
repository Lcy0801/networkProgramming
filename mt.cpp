#include <iostream>
#include <signal.h>
#include <ctime>
#include <pthread.h>
using namespace std;

int value = 0;

void *threadFunc(void *arg)
{
    int type = *(int *)arg;
    if (type == 0)
    {
        while (true)
        {
            sleep(3);
            value = rand() % 10;
            cout << "生成随机数:" << value << endl;
        }
    }
    else
    {
        while (true)
        {
            sleep(3);
            cout << "打印随机数:" << value << endl;
        }
    }
}

int main()
{
    srand((unsigned int)time(nullptr));
    pthread_t tid1, tid2;
    pthread_attr_t x;
    int type1 = 0;
    int type2 = 1;
    pthread_create(&tid1, nullptr, threadFunc, &type1);
    pthread_create(&tid2, nullptr, threadFunc, &type2);
    sleep(3);
    cout << "主线程执行结束!" << endl;
    return 0;
}