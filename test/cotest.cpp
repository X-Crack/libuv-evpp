#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <inttypes.h>
#include <stdint.h>
#include <vector>
#include <memory>
#include <iostream>
#include <coroutine>
#include <experimental/coroutine>
#include <functional>
#include <chrono>

#include <event_coroutine.h>

void* operator new(size_t sz)
{
    void* m = malloc(sz);
    printf("address:%p len:%d\n", m, sz);
    return m;
}
void operator delete(void* m)
{
    printf("delete:%p\n", m);
    free(m);
}

void myprintf() noexcept
{
    printf("callback\n");
}

int main(int argc, char* argv[])
{
    Evpp::EventCoroutine co;
    auto ptr = new Evpp::EventCoroutineMiddle(std::bind(myprintf));
    auto oldptr = ptr;
    auto p = co.JoinInTask(ptr);
    p.SubmitTask();
    p.CancelTask();
    printf("p:%p\n", &p);
    if (ptr != oldptr)
    {
        delete ptr;
        delete oldptr;
    }
    return 0;
}