
## Table of Contents
> - [1. Process and Threads](./1.-Process-and-Threads)
> - [2. Memory Management](./2.-Memory-Management/)
> - [3. Deadlock](./3.-Deadlock/)
> - [4. File Systems](./4.-File-Systems/)
> - [5. Input Output](./5.-Input-Output/)


So there is this factory game in Arknight Endfield based on a grid, so everything is rectangular.

It has facilities, powerpoles, waterpipes, and conveyers. They can not overlap.

Rules for conveyers.

1. conveyer takes up a minimum of 1x1 tile.
2. conveyer can split into 3 directions
3. conveyer can converge from 3 directions
4. conveyer can bridge over another converger as long as they are no going aginst easch other.
5. Can not overlap with other facilities

Rules for water pipes
Mostly the same as converyers. exept that it can only go over a converyer if the water pipe is not a pipe splitter, piper converger, pipe bridge.

Each facility can have a input and or output and several converyers can go in or out of it.

So is there an algorithm we can use to optimize this.