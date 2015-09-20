// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"

// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------


// 每个thread上CPU之后，调度算法没有让他下CPU的策略，必须由thread自己交出CPU Yield
void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}


//----------------------------------------------------------------------
// 测试最多启动128个线程
//----------------------------------------------------------------------
void OneThread(int num)
{
	int loop = 2;
	while(loop--){
		printf("I am the thread whose tid=%d\n", currentThread->GetThreadId());
		currentThread->Yield();	
	}
}

void RunManyThreadsTest()
{
	DEBUG('t', "Entering RunManyThreadsTest");
	
	for(int i=1;i<=200;i++)
	{
		char name[20] = "forked thread";
		Thread *t = new Thread(name);
		//printf("分配了一个线程,tid=%d\n",i,t->GetThreadId());
		t->Fork(OneThread, i);
	}
	currentThread->Yield();
}

//----------------------------------------------------------------------
// 测试TS:列出全部线程信息功能
//----------------------------------------------------------------------

void ListThreadsStatus()
{
	for(int i=1;i<=10;i++){
		ListAllThreads();	
		Thread *t = new Thread("thread test~");
		t->Fork(OneThread, i);
	}
	currentThread->Yield();
}


//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
	// testnum 表示的是测试的号码
    switch (testnum) {
    case 1:
		ThreadTest1();
		break;
	case 2:
		RunManyThreadsTest();
		break;
	case 3:
		ListThreadsStatus();
    	break;
	default:
		printf("No test specified.\n");
		break;
    }
}

