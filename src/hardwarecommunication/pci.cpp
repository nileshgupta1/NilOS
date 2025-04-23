#include <hardwarecommunication/pci.h>
using namespace nilos::common;
using namespace nilos::drivers;
using namespace nilos::hardwarecommunication;




PeripheralComponentInterconnectDeviceDescriptor::PeripheralComponentInterconnectDeviceDescriptor()
{
}

PeripheralComponentInterconnectDeviceDescriptor::~PeripheralComponentInterconnectDeviceDescriptor()
{
}


PeripheralComponentInterconnectController::PeripheralComponentInterconnectController()
: dataPort(0xCFC),
  commandPort(0xCF8)
  // 0xCF8 is the CONFIG_ADDRESS port where CONFIG_ADDRESS will be written
  // 0xCFC is the CONFIG_DATA port where data will be read/written from/to after setting command port
  // Accessing the 'Configuration Address Space' which is required to communicate to the PCI devices is done using these two ports
{
}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController()
{
}

/*
    We need the address of the Configuration registers of the specific device present in the Configuraion space for communicating with that PCI devices.
    We will write this address to the command port(0xCF8). After this we can read/write data from/to the data port(0xCFC)
    Format of CONFIG_ADDRESS:
    +--------------------------------------------------------+
    |  31  |  30-24  |  23-16  | 15-11 | 10-8  | 7-2   | 1-0 |
    +--------------------------------------------------------+
    |Enable| Reserved|  Bus #  | Dev # | Func #| Offset|  00 |
    +--------------------------------------------------------+
*/

uint32_t PeripheralComponentInterconnectController::Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset)
{
    uint32_t id =
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC);  // 0xFC = 1111 1100 so last 2 bits are set 0
    commandPort.Write(id);
    uint32_t result = dataPort.Read();
    return result >> (8* (registeroffset % 4));
}

void PeripheralComponentInterconnectController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value)
{
    uint32_t id =
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC);
    commandPort.Write(id);
    dataPort.Write(value); 
}

bool PeripheralComponentInterconnectController::DeviceHasFunctions(uint16_t bus, uint16_t device)
{
    // Just read value at 7th bit of the content at address 0x0E
    // because the offset 0x0E in the configuration space of the device stores the header-type
    // and 7th bit of the header-type indicates if the device is a multi-function device
    return Read(bus, device, 0, 0x0E) & (1<<7);
}

void printf(char* str);
void printfHex(uint8_t);

void PeripheralComponentInterconnectController::SelectDrivers(DriverManager* driverManager)
{
    for(int bus = 0; bus < 8; bus++)
    {
        for(int device = 0; device < 32; device++)
        {
            // Check if the device is a multi-function device
            int numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1; 
            for(int function = 0; function < numFunctions; function++)
            {
                // All key device information gets enumerated by the 'GetDeviceDescriptor' function for all combinations of bus, device and function
                PeripheralComponentInterconnectDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);
                
                // If there is no device then break
                if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
                    break;
                
                printf("PCI BUS ");
                printfHex(bus & 0xFF);
                
                printf(", DEVICE ");
                printfHex(device & 0xFF);

                printf(", FUNCTION ");
                printfHex(function & 0xFF);
                
                printf(" = VENDOR ");
                printfHex((dev.vendor_id & 0xFF00) >> 8);
                printfHex(dev.vendor_id & 0xFF);
                printf(", DEVICE ");
                printfHex((dev.device_id & 0xFF00) >> 8);
                printfHex(dev.device_id & 0xFF);
                printf("\n");
            }
        }
    }
}



/*
Structure of Configuration Register for a device (first few bytes):

Offset  | Size  | Register Name          | Description
--------|-------|------------------------|-------------
0x00    | 2     | Vendor ID              | Unique identifier for the device vendor
0x02    | 2     | Device ID              | Unique identifier for the device
0x04    | 2     | Command                | Device control and status
0x06    | 2     | Status                 | Device status
0x08    | 1     | Revision ID            | Device revision number
0x09    | 1     | Program-If             | Device class (e.g. network, storage, etc.)
0x0A    | 1     | Sub-Class Code         | Device sub-class (e.g. Ethernet, SATA, etc.)
0x0B    | 1     | Class Code             | Device programming interface
0x0C    | 1     | Cache Line Size        | Device cache line size
0x0D    | 1     | Latency Timer          | Device latency timer
0x0E    | 1     | Header Type            | Device header type (e.g. single-function, multi-function, etc.)
0x0F    | 1     | BIST                   | Built-in self-test result
0x3C    | 1     | Interrupt Line         | Which PIC input line the device's interrupt is routed to
*/

PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function)
{
    PeripheralComponentInterconnectDeviceDescriptor result;
    
    result.bus = bus;
    result.device = device;
    result.function = function;
    
    // Details are retrieved from the Configuration Space
    result.vendor_id = Read(bus, device, function, 0x00);
    result.device_id = Read(bus, device, function, 0x02);

    result.class_id = Read(bus, device, function, 0x0b);
    result.subclass_id = Read(bus, device, function, 0x0a);
    result.interface_id = Read(bus, device, function, 0x09);

    result.revision = Read(bus, device, function, 0x08);
    result.interrupt = Read(bus, device, function, 0x3c);
    
    return result;
}
