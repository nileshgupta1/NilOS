#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H
#include "types.h"
#include "port.h"
#include "gdt.h"


class InterruptManager;

class InterruptHandler{ // This class will handle each interrupts in detail
protected:
    uint8_t interruptNumber;
    InterruptManager* interruptManager;
    InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager);
    ~InterruptHandler();
public:
    virtual uint32_t HandleInterrupt(uint32_t esp); // A derived class can have its own implementation of a virtual function
};

class InterruptManager{
    friend class InterruptHandler;
protected:

    static InterruptManager* ActiveInterruptManager; // Used to make sure that there is only one active interrupt manager at a time
    InterruptHandler* handlers[256]; // Store handlers for all the interrupts

    //blueprint of the IDT
    struct GateDescriptor{
        uint16_t handlerAddressLowBits;
        uint16_t gdt_codeSegmentSelector;
        uint8_t reserved;
        uint8_t access;
        uint16_t handlerAddressHighBits;
    }__attribute__((packed)); //Prevent Compiler from moving objects for optimization as it needs to be byte perfect

    static GateDescriptor interruptDescriptorTable[256];

    struct InterruptDescriptorTablePointer{
        uint16_t size;
        uint32_t base; //Adress of the table

    }__attribute__((packed)); //Prevent Compiler from moving objects for optimization as it needs to be byte perfect

    uint16_t hardwareInterruptOffset;
    //static InterruptManager* ActiveInterruptManager;

    static void SetInterruptDescriptorTableEntry(uint8_t interrupt,
        uint16_t codeSegmentSelectorOffset, void (*handler)(),
        uint8_t DescriptorPrivilegeLevel, uint8_t DescriptorType);

    static void InterruptIgnore();

    static void HandleInterruptRequest0x00(); //Timer or Hardware Clock interrupt
    static void HandleInterruptRequest0x01(); //Keyboard interrupt
    static void HandleInterruptRequest0x02();
    static void HandleInterruptRequest0x03();
    static void HandleInterruptRequest0x04();
    static void HandleInterruptRequest0x05();
    static void HandleInterruptRequest0x06();
    static void HandleInterruptRequest0x07();
    static void HandleInterruptRequest0x08();
    static void HandleInterruptRequest0x09();
    static void HandleInterruptRequest0x0A();
    static void HandleInterruptRequest0x0B();
    static void HandleInterruptRequest0x0C();
    static void HandleInterruptRequest0x0D();
    static void HandleInterruptRequest0x0E();
    static void HandleInterruptRequest0x0F();
    static void HandleInterruptRequest0x31();



    static void HandleException0x00();
    static void HandleException0x01();
    static void HandleException0x02();
    static void HandleException0x03();
    static void HandleException0x04();
    static void HandleException0x05();
    static void HandleException0x06();
    static void HandleException0x07();
    static void HandleException0x08();
    static void HandleException0x09();
    static void HandleException0x0A();
    static void HandleException0x0B();
    static void HandleException0x0C();
    static void HandleException0x0D();
    static void HandleException0x0E();
    static void HandleException0x0F();
    static void HandleException0x10();
    static void HandleException0x11();
    static void HandleException0x12();
    static void HandleException0x13();

    static uint32_t HandleInterrupt(uint8_t interrupt, uint32_t esp); // the assembler code is going to give the interrupt number and esp, the current stack pointer
    uint32_t DoHandleInterrupt(uint8_t interrupt, uint32_t esp);



    //there are 2 PICs: master and slave
    // Master PIC handles IRQs 0-7, Slave handles IRQs 8-15. The keyboard controller sends IRQ1 line signal 
    Port8BitSlow programmableInterruptControllerMasterCommandPort;
    Port8BitSlow programmableInterruptControllerMasterDataPort;
    Port8BitSlow programmableInterruptControllerSlaveCommandPort;
    Port8BitSlow programmableInterruptControllerSlaveDataPort;

public:
    InterruptManager(uint16_t hardwareInterruptOffset, GlobalDescriptorTable* globalDescriptorTable);
    ~InterruptManager();

    uint16_t HardwareInterruptOffset();
    void Activate();
    void Deactivate();

};

#endif