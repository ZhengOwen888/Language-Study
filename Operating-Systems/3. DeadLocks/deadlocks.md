# Deadlocks

## Table of Contents

## Resources

Resources are objects that can be granted and used by the processes that requested them.

| Resource Type | Description |
|---|---|
| Preemptable | Resource that can be taken away from the process owning it with no side effects. |
| Nonpreemptable | Resource that can not be taken away without causing potential failure. |

### Sequence to use a Resource
1. Request the resource.
2. Use the resource.
3. Release the resource.


## What is Deadlock?
Deadlock happens when a set of processes is waiting for another event to happen, but that event can only be triggered by another process in the same set. In simple words, every processes are stuck in a cycle of waiting for resource. This specific deadlock where resources are involved are called resource deadlocks.

### Conditions for Resource Deadlock
1. Mutual exclusion, resources can not be shared simultaneously between processes.
2. Condition where a process is hogging one resource while waiting for another.
3. Condition where you can't focibly take away a resource from a process.
4. Condition where each process in a set is waiting for the other process in a cycle to release their resource. (e.g. Process A  waits on B, B waits on C, C waits on A -> a cycle with no exit)

## Deadlock Detection and Recovery

### Deadlock Detection with One Resource of Each Type

We can use a graph to represent the resources and processes in a system. Essentially we will perform a depth first search (DFS) traverse each edge once and see if we land back to a visited node.

| Element | Shape | Edge Direction |
|---|---|---|
| Resource | Square | Points **to** a process (acquired) |
| Process | Circle | Points **to** a resource (requesting) |

### Cycle Detection Algorithm

**Setup (Run once for every node):**
- Pick a node N as the starting node.
- Initialize an empty list **L**.
- Mark all edges as **unvisited**.

**Steps:**

1. Add the current node to the end of **L**.
   - If the node appears in **L more than once** -> a cycle exists, **terminate**.

2. Check if the current node has any unvisited outgoing edges.
   - **Yes** -> go to step 3.
   - **No** -> go to step 4.

3. Pick an unvisited outgoing edge at random, mark it as visited, follow it to the next node, and **go back to step 1**.

4. If the current node is the **start node** -> no cycle exists, **terminate**.
   Otherwise -> dead end reached. Remove the current node from **L**, backtrack to the previous node, and **go back to step 1**.


<!-- ### Deadlock Detectoin with Multiple Resources of Each Type

### Deadlock Recovery

## Deadlock Avoidance

## Deadlock Prevention -->