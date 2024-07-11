Current behavior
Deadlock appears be caused by write data and discovery entity threads using tcp transport on the writer side.

I write a simple test program test-fastdds to reproduce this bug.

I tested test-fastdds with fastdds.Unfortunately,stucked deadlock reappeared! However,with this version, the deadlock is more difficult to trigger.Only starting more subscribers (2000 readers per sub) and one publisher(2000 writers).More topics more likely to reproduce deadlock,so the number of topics from before 200 to now 2000.But following steps more likely trigger deadlock:

1. Start discovery server in one console (fast-discovery-server -i 0 -t 10.8.8.6 -q 30801)
2. Edit DEFAULT_FASTRTPS_PROFILES.xml ,change listen port of tcpv4 to 0 ;
3. Start two subscriber(2000 readers per sub) in two new consoles (./test-fastdds sub 2000);
4. Start one publisher (2000 writers) in new console (./test-fastdds pub 2000);
5. Wait for matching,and then more likely discovery server or writers are sucked and deadlock may occur because of incorrect matching number (if no deadlock, kill and restart publisher once more).

1. Sometimes "Matching unexisting participant from writer" error occured (line 1062 in /workspace/fastdds/src/fastrtps/src/cpp/rtps/builtin/discovery/database/DiscoveryDataBase.cpp ) after killing publisher.
2. Sometimes or When Discovery server error occured ,the server will never drop killed parcipant.
3. After killing publisher, DataReaderListeners almost never be called to notify discovery or match info.

Additionally,other tests produce same stucked dealock:

1. Change Reliability of writers or readers
2. Cmake options to compile fastdds,such is FASTDDS_STATISTICS or STRICT_REALTIME.
3. Less topics and datas lead to a lower probability of deadlock.(such as ./test-fastdds sub 100)
4. Opening multi consoles of subscriber lead to hight probability of deadlock.

Fast DDS version/commit

FastDDS v2.14.2

Platform/Architecture
debian 10 amd64

TCPv4/Reliable/DiscoveryServer
