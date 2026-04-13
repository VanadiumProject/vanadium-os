## Vanadium Console Architecture Overview
### **High level Diagram:**
>**TODO** [[PCB]]
### **Components Responsibilities:**
- **[Main CPU](Main_CPU):** Game logic, scheduler coordination, OS kernel interactions.
- **[[PGPU]]:** Graphics rendering, command queue processing.
- **[[APU]]:** Sound generation, procedural audio for cartridges.
### **Communication & Memory Model**
- [Shared memory map overview](Memory_Map).
- [Command queues and interrupt priorities](Interuptions).
- [Synchronization rules](Scheduler).
### **Design Decisions**
- [Modular architecture rationale](PCB), [see also](Expansion_Connector).
- [Three CPU design rationale](Execution_Model).
- [RISC-V choice justification](SoC_Choice).
### **Early Contributor Guidelines**
- [What to focus on at this stage](_QA_Plan).
- [Reference](Test_Cases) flows to simulate/test.
- Notes on future expansion.
