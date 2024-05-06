Current behavior
Deadlock appears be caused by write data and discovery entity threads using tcp transport on the writer side.

I write a simple test program test-dds to reproduce this bug.

I tested test-dds with fastdds.Unfortunately,stucked deadlock reappeared! However,with this version, the deadlock is more difficult to trigger.Only starting more subscribers (200 readers per sub) and one publisher(200 writers) and not killing writers can not reproduce the issue (after about 30 trials of simple test,may be lucky).But following steps more likely trigger deadlock:

1. Start discovery server in one console (fast-discovery-server -i 0 -t 10.8.8.6 -q 17480)
2. Edit DEFAULT_FASTRTPS_PROFILES.xml ,change listen port of tcpv4 to 0 ;
3. Start two subscriber(200 readers per sub) in two new consoles (./test-dds sub);
4. Start one publisher (200 writers) in new console (./test-dds pub);
5. Wait 30 seconds and kill publisher process and restart publisher,then more likely deadlock may occur (if no deadlock, kill and restart publisher once more),**and write operation are stucked as follows:
![image](https://github.com/eProsima/Fast-DDS/assets/7147583/345b6d3e-04e7-420a-b55c-634380bee5f5)
Actually,the publisher process is killed and restarted once,so only 200 writer topics,but the subscribers still had 400 writers.
Additionally,other issues as follows through tests:

1. Sometimes "Matching unexisting participant from writer" error occured (line 1062 in /workspace/fastdds/src/fastrtps/src/cpp/rtps/builtin/discovery/database/DiscoveryDataBase.cpp ) after killing publisher.
2. Sometimes or When Discovery server error occured ,the server will never drop killed parcipant.
3. After killing publisher, DataReaderListeners almost never be called to notify discovery or match info.

Additionally,other tests produce same stucked dealock:

1. Change Reliability of writers or readers
2. Cmake options to compile fastdds,such is FASTDDS_STATISTICS or STRICT_REALTIME.
3. Less topics and datas lead to a lower probability of deadlock.(such as ./test-dds sub 100)
4. Opening multi consoles of subscriber lead to hight probability of deadlock.

Fast DDS version/commit

FastDDS v2.14.1(current master)[https://github.com/eProsima/Fast-DDS/commit/9928d1df72f9bd5e3a85ed9eb25702ea0c5a4ec7]

Platform/Architecture
debian 10 amd64

TCPv4/Reliable/DiscoveryServer
