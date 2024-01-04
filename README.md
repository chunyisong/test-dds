Deadlock appears be caused by write data and discovery entity threads using tcp transport on the writer side.

To reproduce this bug, open two different consoles:
In the first one for publisher: ./test-dds pub
In the second one for subscriber: ./test-dds sub

Then the deadlock will most likely occur.If not writers not stucked,restart second console.
Additionally,other tests produce same stucked dealock:
1. Change Reliability of writers or readers
2. Using discovery server (This is actual plan.Here using init peers for simplicity.)
3. Cmake options to compile fastdds,such is FASTDDS_STATISTICS or STRICT_REALTIME.
4. Less data leads to a lower probability of deadlock.(such as ./test-dds pub 1 100)

