 
#ifndef __NILOS__MULTITASKING_H
#define __NILOS__MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

namespace nilos
{


    /*
    What happens in most x86 systems is that, when an interrupt is triggered, initially the user SS and user ESP of current running task (currently stored in CPU's SS and ESP registers) are saved to the location pointed by the ESP0 (kernel stack location).
    Then CPU then loads its SS and ESP registers with the kernel stack pointer (SS0 and ESP0) from the TSS (present in GDT) to switch to the kernel stack.
    CPU then pushes the current EFLAGS, CS, EIP, and SS onto the newly active kernel stack.
    After this CPU jumps to the ISR (interrupt service routine) code and begins execution on the kernel's stack.
    The ISR code explicitly pushes the registers EAX, EBX, ECX, EDX, ESI, EDI, and EBP onto the kernel stack.
    At the end the ESP (surrently pointing to top of these pushed registers) is stored in the Task/Process Control Block to be used when the task is scheduled again.
    */

    /*
    In our case, we will have an instance of the TaskManager class in the kernel which will be responsible for managing the tasks.
    The TaskManager class will have an array of Tasks (stack + state), which will be similar to the array of Process Control Blocks (PCBs) in an OS kernel.
    We can call the Scheduler function of the TaskManager class to switch between tasks using the array of PCBs and the CPUState contained in thhe PCBs.
    */

    // The struct represents the state of a CPU at a given point in time. Contains both CPU pushed and User pushed data.
    struct CPUState
    {
        // The following registers are pushed by the ISR (software) when an interrupt occurs (see interruptstubs.s file)
        common::uint32_t eax; // Accumulator register
        common::uint32_t ebx; // Base register
        common::uint32_t ecx; // Counter register
        common::uint32_t edx; // Data register

        common::uint32_t esi; // Stack index register
        common::uint32_t edi; // Data index register
        common::uint32_t ebp; // Stack Base Pointer

        /*
        common::uint32_t gs;
        common::uint32_t fs;
        common::uint32_t es;
        common::uint32_t ds;
        */
        common::uint32_t error;  // Error code (if any)

        // The following registers are pushed by the CPU (hardware) automatically when an interrupt occurs
        common::uint32_t eip;    // Instruction Pointer
        common::uint32_t cs;     // Code Segment
        common::uint32_t eflags; // Flags register
        common::uint32_t esp;    // Stack Pointer
        common::uint32_t ss;     // Stack Segment  
    } __attribute__((packed));
    // __attribute__((packed)) attribute ensures no padding is added between its members, preserving its exact memory layout.
    
    
    // The Task class represents a single task in the system. It contains a stack and a CPU state stored in the stack itself.
    // These tasks get stored as array in TaskManager class which is stored in the kernel. So that array acts somewhat like the array of Process Control Blocks (PCBs) in an OS.
    class Task
    {
    // Friend class is a class that is granted access to the private and protected members of another class
    friend class TaskManager;
    private:
        common::uint8_t stack[4096]; // Allocating 4 KiB for each task's stack
        CPUState* cpustate; // Current state of the CPU, including registers. Will be storedin this task's stack
    public:
        // The task will have to talk to the GDT and it needs a function pointer to the function that will be executed when the task is scheduled 
        Task(GlobalDescriptorTable *gdt, void entrypoint());
        ~Task();
    };
    
    
    // The TaskManager class is responsible for managing and scheduling multiple tasks. 
    // It maintains a list of tasks, tracks the current task, and provides methods for adding tasks and scheduling them based on the CPU state.
    class TaskManager
    {
    private:
        // The array stores information about the tasks, including their CPU state and stack.
        // This acts like the array of Process Control Blocks (PCBs) in an OS.
        Task* tasks[256];
        int numTasks;
        int currentTask;
    public:
        TaskManager();
        ~TaskManager();
        bool AddTask(Task* task);
        // Takes current running task's CPUState and Returns the CPUState of the task which is to be scheduled next from the array of the PCBs
        CPUState* Schedule(CPUState* cpustate);
    };
    
    
    
}


#endif