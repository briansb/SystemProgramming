//#define _GNU_SOURCE

#include <iostream>
#include <thread>
#include <sched.h>

// #include <stdio.h>
// #include <sys/sysinfo.h>

// https://stackoverflow.com/questions/280909/how-to-set-cpu-affinity-for-a-process-from-c-or-c-in-linux
// https://www.dynamsoft.com/blog/insights/set-cpu-affinity-process/



// https://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
const auto processor_count = std::thread::hardware_concurrency();
//

int main()
{

    printf("Number of processors = %d\n", processor_count);

    cpu_set_t  mask;
    CPU_ZERO(&mask);
    int chosen = 2;
    //CPU_SET(0, &mask);  // CPU 1
    //CPU_SET(1, &mask);  // CPU 2
    //CPU_SET(2, &mask);  // CPU 3
    //CPU_SET(3, &mask);  // CPU 4
    CPU_SET(chosen, &mask);

  
    int result = sched_setaffinity(0, sizeof(mask), &mask);
    printf("Result = %d\n", result);


    double sum;
    while (1) {
        for (long i = 0; i < 10000000; ++i) {
            sum += i;
        }
        printf("Sum = %f, on CPU number %d\n", sum, chosen);
        sum = 0;
    }
    
    return 0;
}