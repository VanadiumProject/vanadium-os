## Vanadium Console Architecture Overview
### **High level Diagram:**
>**TODO** [[PCB]]
### **Components Responsibilities:**
- [CPU](Main_CPU): With 4 cores, and an integrated GPU it's the main processing unit
### **Communication & Memory Model**
- [Shared memory map overview](Memory_Map).
- [Command queues and interrupt priorities](Interuptions).
- [Synchronization rules](Scheduler).
### **Design Decisions**
- [Modular architecture rationale](PCB), [see also](Expansion_Connector).
- [Multi-core CPU rationale](Execution_Model).
- [RISC-V choice justification](SoC_Choice).
### **Early Contributor Guidelines**
- [What to focus on at this stage](_QA_Plan).
- [Reference](Test_Cases) flows to simulate/test.
- Notes on future expansion.
