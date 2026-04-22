## Vanadium Console Architecture Overview
### **High level Diagram:**
>**TODO** [[PCB]]
### **Components Responsibilities:**
- [CPU](../02_Architecture/Main_CPU): With 4 cores, and an integrated GPU it's the main processing unit
### **Communication & Memory Model**
- [Shared memory map overview](../02_Architecture/Memory_Map).
- [Command queues and interrupt priorities](../04_OS_VanadiumOS/Interuptions).
- [Synchronization rules](../04_OS_VanadiumOS/Scheduler).
### **Design Decisions**
- [Modular architecture rationale](../03_Hardware/PCB), [see also](../03_Hardware/Expansion_Connector).
- [Multi-core CPU rationale](../02_Architecture/Execution_Model).
- [RISC-V choice justification](../03_Hardware/SoC_Choice).
### **Early Contributor Guidelines**
- [What to focus on at this stage](../07_Quality_Tests/_QA_Plan).
- [Reference](../07_Quality_Tests/Test_Cases) flows to simulate/test.
- Notes on future expansion.
