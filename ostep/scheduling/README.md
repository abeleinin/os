# Scheduling

- [Scheduling Metrics](#scheduling-metrics)
- [Scheduling Algorithms](#scheduling-algorithms)

## Scheduling Metrics

- **Turnaround time:** The time at which the job completes minus the time at which the job arrived in the system:

$$
T_{turnaround} = T_{completion} - T_{arrival}
$$

- **Response time:** The time from when the job arrives in a system to the first time it's scheduled.

$$
T_{response} = T_{first \ run} - T_{arrival}
$$

- **Fairness:** Determines whether users or applications are receiving a fair share of system resources. There are several mathematical and conceptual definitions of fairness.

## Scheduling Algorithms

### Workload Assumptions 

1. Each job runs for the same amount of time
2. All jobs arrive at the same time
3. Once started, each job runs to completion
4. All jobs only use the CPU (ie they don't perform I/O)
5. The run-time of each job is known

### First In, First Out (FIFO)

Most basic algorithm we can implement is known as First In, First Out (FIFO) schdeuling or something First Come, First Served (FCFS). This approach performs poorly when the first job is significantly longer than the rest, causing a high average turnaround time. This is referred to as the **convoy effect**, where a number of relatively-short potential consumers of a resource get queued behind a heavyweight resource consumer.

### Shortest Job First (SJF)

The Shortest Job First (SFJ) policy runs the shortest job first, then the next shortest, and so on. Optimal for first 2 assumptions.

### Shortest Time-to-Completion First (STCF)

- **Non-preemptive:** Run each job to completion before considering whether to run a new job.
- **Preemptive:** Willing to stop one process from running in order ro run another. The scheduler can perform a context switch, stopping one running process temporarily and resuming (or starting) another.

At any time a new job enter the system, the STCF scheduler determines which of the remaining jobs (including the new job) has the least time left, and schedules that one. Optimal for first 3 assumptions.

### Round Robin

Round Robin runs a job for a **time slice** (sometimes called a **scheduling quantum**) and then switches to the next job in the run queue. Note that the length of a time slice must be a multiple of the timer-interrupt period.
