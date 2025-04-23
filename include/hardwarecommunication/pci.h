#ifndef __NILOS__HARDWARECOMMUNICATION__PCI_H
#define __NILOS__HARDWARECOMMUNICATION__PCI_H


#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>

namespace nilos
{
    namespace hardwarecommunication
    {

        // This class will store information about a PCI device which will be enumerated by the 'PeripheralComponentInterconnectController' class in SelectDrivers function
        class PeripheralComponentInterconnectDeviceDescriptor
        {
        public:
            nilos::common::uint32_t portBase;
            nilos::common::uint32_t interrupt;
            
            nilos::common::uint16_t bus;
            nilos::common::uint16_t device;
            nilos::common::uint16_t function;
        
            nilos::common::uint16_t vendor_id;
            nilos::common::uint16_t device_id;
            
            nilos::common::uint8_t class_id;
            nilos::common::uint8_t subclass_id;
            nilos::common::uint8_t interface_id;
        
            nilos::common::uint8_t revision;
            
            PeripheralComponentInterconnectDeviceDescriptor();
            ~PeripheralComponentInterconnectDeviceDescriptor();
            
        };

        class PeripheralComponentInterconnectController
        {
            // PCI devices only respond if the Configuration Address Space (a 256-byte region pre device) is present
            // that contains 'Standardized Registers' with information about each device such as vendor id, device id, etc.
            // Suppose the Configuration Space is already enumerated (done during the boot or when new device is detected), now to start communication:
        
            // So first we need to write the CONFIG_ADDRESS (that points to required standardized register in conguration space of needed device) to a specific command port(0xCF8) which specifies the device and function we want to access 
            // After this we can read data of our specified device and function in previous step from a specific data port(0xCFC)
        
            // So, to communicate with the peripheral device we need two steps:
            // 1. Write CONFIG_ADDRESS to the command port specifying the device
            // 2. Read data from the data port from the specified device
        
            // stores CONFIG_ADDRESS port = 0xCF8. To this port, OS writes address of the 'Configuration Register' called CONFIG_ADDRESS it want to access here
            // the CONFIG_ADDRESS is then written to this port using outl assembly command
            Port32Bit commandPort; 
            // stores CONFIG_DATA port = 0xCFC. Using this OS reads/writes data from/to the selected 'Configuration Register'
            // data is read and written to this port using inl and outl assembly commands
            Port32Bit dataPort; 
        
        public:
            PeripheralComponentInterconnectController();
            ~PeripheralComponentInterconnectController();
        
        
            // Using this function we can read data from a certain function of a device present on a certain bus (using the Configuration space and inl assembly command)
            // the offset is required to specify which register we want to read
            nilos::common::uint32_t Read(nilos::common::uint16_t bus, nilos::common::uint16_t device, nilos::common::uint16_t function, nilos::common::uint32_t registeroffset);
            void Write(nilos::common::uint16_t bus, nilos::common::uint16_t device, nilos::common::uint16_t function, nilos::common::uint32_t registeroffset, nilos::common::uint32_t value);
            // Asks if the device has functions or not otherwise selection of drivers will be slow
            bool DeviceHasFunctions(nilos::common::uint16_t bus, nilos::common::uint16_t device);
        
        
            // In the end, we want to connect the driver manager (see drivers/driver.h) with the PCI controller
            // and insert the drivers for the devices connected to the PCI controller in the 'drivers' array of the driver manager.
            // This method which will enumerate the description of a device and select drivers based on fields such as class id, subclass id, vendor id, device id.
            void SelectDrivers(nilos::drivers::DriverManager* driverManager);
            // This method will return a Descriptor for a specific device so that the correct driver can be chosen in the SelectDrivers function.
            // The Descriptor is obtained from the Configuration space.
            PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(nilos::common::uint16_t bus, nilos::common::uint16_t device, nilos::common::uint16_t function);
        };
        
    }
}



#endif
