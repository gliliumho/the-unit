# Project: The Unit # 
*Last update: 15/10/2015*

**Author: KY Ho & KH Teo**

## Introduction ##

This is the repo for The Unit project. Initial plan/Phase 1 of the project is to make small IoT-like devices to 
be installed on the guard-rail. These devices will include RF transceiver for communication, and 3x LED strips
(Red, Yellow, Green) to indicate the traffic of the road ahead. These devices are called "slaves".All these 
slaves get the traffic info from another device called "master", which will be more powerful and have Internet 
access. Master will control all the slaves through RF communications.

Communication relies on the [node hopping](http://goo.gl/1BpNpe) method. This architecture is designed to reduce 
the number of masters along the highway, mainly due to costs. Basically one slave will receive the packet and 
then retransmit it to the other slaves. If you look at [Great Wall of China](http://goo.gl/WxzdMD), they've 
towers along it. If the enemy attacks, the guards will light up the fire. Then the next tower will see it and 
light up the fire too. Thus, relaying the terrible news of invasion to the city, hundreds or thousands of miles 
away!!.....in ancient speed. Current protocol supports traffic info and heartbeat status from slaves(masters 
gonna know if their slaves are alive or not. Else who would be wiping their shoes?). All these done via the 
relay method.

Phase 2 of the project will include sensors(or cameras, even though unlikely) to collect data along the highway.
At the time of typing this, we're still unclear of what sensor to be installed. 

## Hardware ##

For the actual hardware schematics, you can ask Hakim or Ramesh for hardware design. However, since Recogine is 
mainly a software company and the fact that you're reading this, you're probably a software person. BUT! You'll 
still need some basic knowledge on computer hardware to understand what the registers are doing. 

The hardware that we used in phase 1 is [Nordic Semiconductor NRF9E5](http://goo.gl/kr0hHA) for slave and 
[RIoTboard](http://riotboard.org/) for master. I'd suggest read the [NRF9E5 spec sheet](http://goo.gl/NSFea8), 
especially the NRF905(transceiver) part and also the registers part. For RIoTboard, just refer to the spec 
sheet/user manual for the GPIO pin mapping. 


## Software ##

### Slave Unit ###

Programming of the slaves are done in Keil-C, Cx51. All the codes for slave will need to be compiled using 
uVision4 or uVision5. You may read the sample codes from Nordic Semiconductor or some of the test codes that we 
previously wrote.  

1. **testRange** - written for range test for evaluation kit.
2. **testEEPROM** - test saving groupID & uniqueID in EEPROM. 
				 *Dependencies: libnrf9e5.LIB(/slave_unit/library/uVision)*

3. **testTwoWay** - huge blob of mess. Used to test two-way communication such as traffic info & heartbeat using 
				 delay. Will try using buffer to prevent "echo" problem and packet ID. 
				 *Dependencies: libnrf9e5.LIB(/slave_unit/library/uVision)*

4. **library** - source code for library of functions. **Dependency for testEEPROM & testTwoWay.**


Following codes are archived test codes:
+ _testRFString - Written to test sending multiple bytes through RF._
+ _testMultiHop - PoC for The Unit hopping architecture. At the point when this was written, only 
1 way communication (master to slave) was supported. To learn about two-way communication, look at 
testTwoWay._
+ _testChar & testString - Written to test character/string operation through UART._

### Master Unit ###

_There is still quite a lot more work to do for master._ 

We need to do the following things: 
+ integrate it with traffic server 
+ write APIs so we can integrate it with 3rd party traffic servers
+ integrate with an RF transceiver using UART. This will require coding on both master & slave

Codes for the master are written in Python. I did half-assed code for master to interface with slave using 
RS232. But that code is utterly broken/incomplete. _Available upon request._