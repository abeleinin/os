#include <stdio.h>
#include <stdlib.h>

// Process structure
typedef struct {
    int pid;
    int arrivalTime;
    int burstTime;
    int waitTime;
    int turnaroundTime;
} proc;

// Compares the arrival time of two processess
int compare_by_arrival_time(const void* a, const void* b) {
  const proc* procA = (const proc*)a;
  const proc* procB = (const proc*)b;

  return procA->arrivalTime - procB->arrivalTime;
}

// Calculates wait time and turnaround time for each process
void first_come_first_serve(proc* processes, int n) {
    int currentTime = 0;

    for (int i = 0; i < n; i++) {
        if (currentTime < processes[i].arrivalTime) {
            currentTime = processes[i].arrivalTime;
        }

        processes[i].waitTime = currentTime - processes[i].arrivalTime;
        processes[i].turnaroundTime = processes[i].waitTime + processes[i].burstTime;
        currentTime += processes[i].burstTime;
    }
}

int main(int argc, char* argv[]) {
    int n;
    printf("Enter number of processes: ");
    scanf("%d", &n);

    proc processes[n];

    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        printf("Enter arrival time and burst time for process %d: ", i + 1);
        scanf("%d %d", &processes[i].arrivalTime, &processes[i].burstTime);
    }

    // Sort the input processes by arrival time
    int N = sizeof(processes) / sizeof(processes[0]);
    qsort(processes, N, sizeof(proc), compare_by_arrival_time);

    // Schedule the processes
    first_come_first_serve(processes, n);

    printf("\nProcess\tArrival\tBurst\tWait\tTurnaround\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n",
               processes[i].pid,
               processes[i].arrivalTime,
               processes[i].burstTime,
               processes[i].waitTime,
               processes[i].turnaroundTime);
    }

    return 0;
}
