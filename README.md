Current behavior
Deadlock appears be caused by write data and discovery entity threads using tcp transport on the writer side.

I write a simple test program test-dds to reproduce this bug.

To reproduce this bug, open two different consoles:
In the first one for publisher: ./test-dds pub
Then edit DEFAULT_FASTRTPS_PROFILES.xml ,change listen port of tcpv4 to 0 or other different port number.
In the second one for subscriber: ./test-dds sub

Then the deadlock will most likely occur.If no stuckting,restart second console.
From console of publisher,_currentMatchedPubs and _totalPubOkDatas log are not changed.
From console of subscriber,_currentMatchedSubs and _totalSubValidDatas logs are alse not changed.

Additionally,other tests produce same stucked dealock:

Change Reliability of writers or readers
Using discovery server (This is actual deployment.Here using init peers for simplicity.)
Cmake options to compile fastdds,such is FASTDDS_STATISTICS or STRICT_REALTIME.
Less topics and datas lead to a lower probability of deadlock.(such as ./test-dds sub 100)
Open multi consoles of sub lead to hight probability of deadlock.
Fast DDS version/commit
FastDDS v2.13.0

Platform/Architecture
Ubuntu Focal 20.04 amd64

Transport layer
TCPv4