
#include <multitasking.h>

using namespace nilos;
using namespace nilos::common;


Task::Task(GlobalDescriptorTable *gdt, void entrypoint())
{
    // cpustate of a task is supposed to point to the top of the task's stack
    /*
        Task's Stack:
        +----------------------------------------------+
        |        (empty)               |   (CPUState)  |
        +----------------------------------------------+
        ^stack                         ^cpustate       ^stack+4096
    */
    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));
    
    cpustate -> eax = 0;
    cpustate -> ebx = 0;
    cpustate -> ecx = 0;
    cpustate -> edx = 0;

    cpustate -> esi = 0;
    cpustate -> edi = 0;
    cpustate -> ebp = 0;
    
    /*
    cpustate -> gs = 0;
    cpustate -> fs = 0;
    cpustate -> es = 0;
    cpustate -> ds = 0;
    */
    
    // cpustate -> error = 0;    
   
    // cpustate -> esp = ;
    cpustate -> eip = (uint32_t)entrypoint;
    cpustate -> cs = gdt->CodeSegmentSelector();
    // cpustate -> ss = ;
    cpustate -> eflags = 0x202;
    
}

Task::~Task()
{
}

        
TaskManager::TaskManager()
{
    numTasks = 0;
    currentTask = -1;
}

TaskManager::~TaskManager()
{
}

bool TaskManager::AddTask(Task* task)
{
    if(numTasks >= 256)
        return false;
    tasks[numTasks++] = task;
    return true;
}

// Takes current running task's CPUState and Returns the CPUState of the task which is to be scheduled next in Round Robin manner
CPUState* TaskManager::Schedule(CPUState* cpustate)
{
    if(numTasks <= 0)
        return cpustate;
    
    if(currentTask >= 0)
        tasks[currentTask]->cpustate = cpustate; // storing the old task's CPUState
    
    if(++currentTask >= numTasks) // currentTask is now set to the task at the next index (in cyclic manner)
        currentTask %= numTasks;
    return tasks[currentTask]->cpustate;
}

    