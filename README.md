# NachosSolve
my nachos 3.4 solution 


#Lab1 Thread
###Exercise 1  调研
	调研Linux或Windows中进程控制块（PCB）的基本实现方式，理解与Nachos的异同。

###Exercise 2  源代码阅读
	仔细阅读下列源代码，理解Nachos现有的线程机制。
1.	code/threads/main.cc和code/threads/threadtest.cc
2.	code/threads/thread.h和code/threads/thread.cc

###Exercise 3  扩展线程的数据结构
	增加“用户ID、线程ID”两个数据成员，并在Nachos现有的线程管理机制中增加对这两个数据成员的维护机制。

###Exercise 4  增加全局线程管理机制
1. 在Nachos中增加对线程数量的限制，使得Nachos中最多能够同时存在128个线程；
	```shell
	./nachos -q 2
	```
2. 仿照Linux中PS命令，增加一个功能TS(Threads Status)，能够显示当前系统中所有线程的信息和状态。
	```shell
	./nachos -q 3
	```