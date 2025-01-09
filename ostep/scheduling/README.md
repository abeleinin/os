# Scheduling

- [Scheduling Metrics](#scheduling-metrics)
- [First In, First Out (FIFO)](#first-in-first-out-fifo)
- [Shortest Job First (SJF)](#shortest-job-first-sjf)
- [Shortest Time-to-Completion First (STCF)](#shortest-time-to-completion-first-stcf)
- [Round Robin](#round-robin)
- [Multi-level Feedback Queues](#multi-level-feedback-queues)
- [Proportional Share](#proportional-share)
    - [Lottery Scheduling](#lottery-scheduling)
    - [Stride Scheduling](#stride-scheduling)
    - [Completely Fair Scheduler (CFS)](#completely-fair-scheduler-cfs)

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

### Workload Assumptions 

1. Each job runs for the same amount of time
2. All jobs arrive at the same time
3. Once started, each job runs to completion
4. All jobs only use the CPU (ie they don't perform I/O)
5. The run-time of each job is known

## First In, First Out (FIFO)

Most basic algorithm we can implement is known as First In, First Out (FIFO) scheduling or something First Come, First Served (FCFS). This approach performs poorly when the first job is significantly longer than the rest, causing a high average turnaround time. This is referred to as the **convoy effect**, where a number of relatively-short potential consumers of a resource get queued behind a heavyweight resource consumer.

- [fcfs.c](https://github.com/abeleinin/os/blob/main/ostep/scheduling/fcfs.c)

```
Enter number of processes: 4
Enter arrival time and burst time for process 1: 10 5
Enter arrival time and burst time for process 2: 4 10
Enter arrival time and burst time for process 3: 7 3
Enter arrival time and burst time for process 4: 0 8

Process Arrival Burst   Wait    Turnaround
4       0       8       0       8
2       4       10      4       14
3       7       3       11      14
1       10      5       11      16
```

Simple simulated FCFS algorithm. Example output shows calculated wait time and turnaround time for each process.

## Shortest Job First (SJF)

The Shortest Job First (SFJ) policy runs the shortest job first, then the next shortest, and so on. Optimal for first 2 assumptions.

## Shortest Time-to-Completion First (STCF)

- **Non-preemptive:** Run each job to completion before considering whether to run a new job.
- **Preemptive:** Willing to stop one process from running in order ro run another. The scheduler can perform a context switch, stopping one running process temporarily and resuming (or starting) another.

At any time a new job enter the system, the STCF scheduler determines which of the remaining jobs (including the new job) has the least time left, and schedules that one. Optimal for first 3 assumptions.

## Round Robin

Round Robin runs a job for a **time slice** (sometimes called a **scheduling quantum**) and then switches to the next job in the run queue. Note that the length of a time slice must be a multiple of the timer-interrupt period. The length of the time slice presents a trade-off, making it long enough to **amortize** (incur a cost less often by performing it fewer times) the cost of switching without making it so long that the system is no longer responsive. RR is one of the *worst* policies if turnaround time is our metric, because it stretches out each job as long as it can. More generally, **fair** policies, that evenly distribute CPU among active processes will perform poorly on metrics such as turnaround time. This typ eof trade-off is common in systems; you can't have your cake and eat it too.

Running programs build up a great deal of state: CPU caches, TLDs, branch predictors, and other on-chip hardware

## Multi-level Feedback Queues

1. If Priority(A) > Priority(B), A runs (B doesn't).
2. If Priority(A) = Priority(B), A & B run in round-robin fasion using the time slice (quantum length) of the given queue.
3. When a job enters the system, it's placed at the highest prioirty (the topmost queue).
4. Once a job uses up its time allotment at a given level (regardless of how many times it has given up the CPU), its priority is reduced (ie, it moves down one queue).
5. After some time period S, move all the jobs in the system to the topmost queue.

Multi-level Feedback Queues (MLFQ) tries to learn about processes as they run, and thus uses the history of a job to predict its future behavior. To remember the components it has *multi levels* of queues, and uses *feedback* to determine the prioirty of a given job.

- How to change priority? **Allotment** is the amount of time a job can speand at a given priority level before the scheduler reduces its priority.
- **Starvation** occurs if "too many" interactive jobs consume all CPU time, and thus long-running jobs will *never* receive any CPU time (they **starve**). Rule 5 resolve this issue in MLFQ because thigh-priority jobs share the CPU in a round-robin fashion.
- **Gaming the scheduler** refers to the idea of doing something sneaky to trick the scheduler into giving you more than your fair share of resources.
- How to prevent gaming the scheduler? By performing better **accounting**, the scheduler should keep track of how much allotment a process has used at a given level; once a process has used its allotment, its demoted to the next priority queue. These requirements are reflected in Rule 4.
- [John Ousterhout](https://web.stanford.edu/~ouster/cgi-bin/home.php), called non-trivial parameter values **voo-doo constants** because they seem to require some form of black magic to set correctly.
- How to parameterize a MLFQ scheduler?
    - How many queues?
    - How big should the time slice be per queue? Most MLFQ allow for variance in time-slice length across different queues. High-priority queues short time-slices for interactive jobs (less than ~10ms), low-priority queues contain long-running jobs, longer time slices work well (~100s of ms).
    - Allotment size?
    - How often should prioirty be boosted to avoid starvation?
- Solaris MLFQ uses 60 queues, with slowly increasing time-slice length from 20ms (highest priority) to a few hundred ms (lowest), and prioirty boosted around every 1 second.
- Other MLFQ schedulers adjust priority using methematical formulae (FreeBSD version 4.3)
- SOTA schedulers in the 90s used **decay-usage** algorithms. 
- Computer systems often provide an interface to provide some hints or advice to improve the OS decision making.
    - Scheduler (`nice` utility)
    - Memory manager (`madvise` syscall)
    - File system (informed prefetching and caching)

## Proportional Share

**Proportional-share** scheduler, also referred to as a **fair-share** scheduler is based around the concept of trying to guarantee that each job obtains a certain percentage of the CPUs time. 

- [Lottery and Stride Scheduling: Flexible Proportional-Share Resource Management](https://www.waldspurger.org/carl/papers/phd-mit-tr667.pdf) by Carl A. Waldspurger (1995)

### Lottery scheduling

**Lottery scheduling**, is a probabilistic approach that uses tickets to represent a process's share of the CPU. This approach provides multiple mechanisms to manipulate tickets. One is ticket currency, which allows a user to allocate tickets among their own jobs in whatever currency they could like; the system then automatically converts it into the correct global value.

- **Ticket transfer**: A process can temporarily transfer tickets to another process, which can be useful in a client/server setting.
- **Ticket inflation**: A process can temporarily raise or lower the number of tickets it owns. Only in an environment where a group of processes trust each other. In cloud resource management, ticket inflation could be used to boost starving task, meet deadlines (ie batch jobs for generating daily financial report), or handling resource spikes (ie a database backup process during a disaster recovery scenario)
- Random number generator within a range in C [stackoverflow](https://stackoverflow.com/questions/2509679/how-to-generate-a-random-integer-number-from-within-a-range).
- **Fairness metric** (*F*): th time the first job completes divided by the time that the second job completes. A perfectly fair scheduler would achieve *F = 1*.

### Stride scheduling

**Stride Scheduling** is a deterministic scheduling algorithm that builds on the concept of proportional share scheduling. Each task is assigned a **stride**, which is inversely proportional to its allocated share of the CPU (ie *stride = 1/share*). The scheduler maintains a **pass** value for each task, initially set to zero. 

```c
curr = remove_min(queue);   // pick client with min pass
schedule(curr);             // run for quantum
curr->pass += curr->stride; // update pass using stride
insert(queue, curr);        // return curr to queue
```

When a task is selected to run, its pass value is incremented by its stride, and the scheduler chooses the task with the smallest pass value for the next run. This ensures that tasks receive CPU time in proportion to their allocated shares over time, providing fairness and predictability while avoiding the randomness of lottery scheduling. A nice property of lottery scheduling it that is has no state. In stride scheduling how would you add a new job in the middle of algorithm; what would the pss value be? However, with lottery scheduling, there is no global state per process; we simply add a new process with whatever tickets it has, update the total number of tickets, and continue.

### Completely Fair Scheduler (CFS)

**Completely Fair Scheduler (CFS)**, implements fair-share scheduling, but does so in a highly efficient and scalable manner. 

- **Virtual runtime** (`vruntime`): Accumulation of process run-time at the same rate, in proportion with physical time. During scheduling decisions, CFS picks the process with the *lowest* `vruntime` to run next.
- `sched_latency` (default 48ms): Used to determine how long one process should run before considering a switch (effectively determining its time slice but in a dynamic fashion).
- `min_granularity` (default 6ms): CFS will never set the time slice of a process less than this value, ensuring reduced scheduling overhead.
- Weighting (Niceness)
    - The `nice` parameter can be set from -20 to +19 for a process, with a default of 0
    - Positive nice values imply *lower* priority
    - Negative nice values imply *higher* priority
    - When you're too nice, you just don't get as much attention
    - CFS maps the nice values of each process to a `weight`

Niceness weights allow us to compute the effective time slice of each process, but now accounting for their priority differences:

$$
\text{time_slice}_{k} = \frac{\text{weight}_{k}}{\sum_{i=0}^{n-1}\text{weight}_{i}} \cdot \text{sched_latency}
$$

Updated `vruntime` calculation:

$$
\text{vruntime}_i = \text{vruntime}_i + \frac{\text{weight}_0}{\text{weight}_i} \cdot \text{runtime}_i
$$

The niceness weight table preserves CPU proportionality ratios when the difference in nice values is constant. For example, if process A instead had a nice value of 5 (not -5), and process B had a nice value of 10 (not 0), CFS would schedule them in eactly the same manner.

CFS uses a [red-black tree](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree) data structure to maintain all running (or runnable) processes. Processes are ordered in the tree by `vruntime`. To handle sleeping processes CFS alters the `vruntime` of a job when it wakes up to be the minimum `vruntime` value found in the tree. CFS avoids starvation, but at a cost of having jobs that sleep for short periods of time frequently do not get their fair share of the CPU.

## Additional Reading

- [The Design of the UNIX Operating System](https://abhyass.wordpress.com/wp-content/uploads/2016/09/thedesignofunixoperatingsystem_m_bach.pdf)
- [Informed Prefetching and Caching](https://www.pdl.cmu.edu/ftp/TIP/SOSP15.pdf) 
- [The Battle of the Schedulers: FreeBSD ULE vs. Linux CFS](https://www.usenix.org/system/files/conference/atc18/atc18-bouron.pdf)
- [Symmetric binary B-Trees: Data structure and maintenance algorithms](https://www.rose-hulman.edu/class/cs/csse230/schedule/day16/BayerRBTrees.pdf)
- [Why numbering should start at zero](https://www.cs.utexas.edu/~EWD/transcriptions/EWD08xx/EWD831.html) by Edsger Dijkstra (1982)
- [Memory Resource Management in VMware ESX Server](https://www.waldspurger.org/carl/papers/esx-mem-osdi02.pdf)
