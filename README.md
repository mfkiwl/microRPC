# microRPC 
A C framework for enabling natural Language Remote Procedure Call Interfaces for Embedded Systems.

A small single header library, **microRPC** does not employ dynamic memory allocation, with a minimum memory footprint

Unlike other RPC frameworks, **microRPC** allows the programmer to define message formats directly in the code and doesn't require any additional config files/generated code. 

- [microRPC](#microrpc)
  - [Features](#features)
  - [How does it work ?](#how-does-it-work-)
  - [Problem Statement](#problem-statement)
  - [Usage](#usage)
  - [WIP](#wip)


## Features

**microRPC** :
* No dynamic memory allocation, buffer sizes use #define's at compile time. **(WIP Dynamic Memory Allocation Branch)**
* Is transport layer agnostic. This means that the programmer can use any transport layer they want. E.g. Serial, Wifi, Bluetooth, etc.
* No external dependencies, common string functions are re-implemented in "helpers.h"
* Fault tolerant, if a message is not formatted correctly, the Command will not be executed.
* Secure, the programer explicitly defines the message format and which functions are exposed to the client as RPC's.


## How does it work ?

High Level Overview:
* The programmer defines: 
  * A **Protocol** : How the messages are expected to be formatted. 
  * An **Interface** : How the messages are handled. Each Interface uses one Protocol to decode the message and can have multiple Services.
  * A **Service** : A Remote Procedure that the server can execute on behalf of the client. Services act as a wrapper around the functionally the programmer wants to expose to the client.
  * A **Command** : A Singleton object that is used to access the Service through the Gateway and pass arguments to it.
  * **Gateway** : Acts as destination router, sending commands to their target Interfaces

* The programer can then:
  * Update the Command with a message from the client.
  * Execute the Command on the server.
  * Obtain the services **Response** and the functions **Return Value**. 
  
* **microRPC** will handle the parsing, formatting and validation of the messages.

**Interfaces**  allow a collection of **Services** to use the same **Protocol**. 

If the programmer requires a different kind of message format for a service, a new **Interface** with a different **Protocol** can be defined.

## Problem Statement
There exists a gap in open source frameworks that provide a flexible and abstracted way for low power embedded systems to communicate with each other and higher level systems. This is escpecialy true for systems that cannot use dynamic memory allocation, or have limited memory, thus limiting the use of existing C++ based RPC frameworks like gRPC.

Note that the helper functions in "helpers.h" reimplement some common string handleing functions with a few changes as shown below:
| String.h Function | Description | helpers.h Function | Differences |
| --- | --- | --- | --- |
| strlen | Returns num of chars in buffer | uCsize | Returns len + '\0' |
| strcpy | Copies src to dest | uCcpy | Copies src to dest and adds '\0' |
| strcmp | Compares two strings | uCcmp | Compares two buffers; 0 if equal |
| strtok | Splits string into tokens | uCSplit | Copies buffer unitll delim; returns index of next char after delim|

## Usage

Re-define the static char buffer sizes, in either the header file or in your project,
to suit your needs.
```c
// Re define the static char buffer sizes
#define MAX_CMD_SIZE 28
#define MAX_ARG_SIZE 5
#define MAX_ARGS 5
#define MAX_SERVICES 5
#define MAX_INTERFACES 5
#define MAX_ID_SIZE 5
#define TARGET_ARG_LEN MAX_ID_SIZE - 1

#include "microRPC.h"
```
```c
// Define a Protocol
Protocol testproto1 = {
  .numArgs = 4,
  .maxCmdLen = 28,
  .maxArgLen = 5,
  .delim = ',',
  .cmdFormat = { 
    {.id = "TRGT", .maxSize = MAX_ID_SIZE },
    {.id = "SRVC", .maxSize = MAX_ID_SIZE },
    {.id = "PRAM", .maxSize = 5 },
    {.id = "DATA", .maxSize = 5 }
  }
};
```
```c
// Define an Interface
Interface testInterface1 = {0};
createInterface(&testInterface1,"IF1", &testproto1, NULL );
```
```c
// Define a RPC Function
int testService1(Command *cmd, char *response, void *data){
  // *** // MICRO RPC WRAPPER // *** //
  //Extract the arguments from the command into a char array
  char cmdArgs[cmd->proto.numArgs][cmd->proto.maxArgLen];
  for(int i = 0; i < cmd->proto.numArgs; i++){
    extractArg(cmdArgs[i], cmd->proto,cmd->proto.cmdFormat[i].id);
  }
  // *** // USER CODE // *** //
  printf("| %-10s | %-10s | %-10s | %-10s |\n", "Target", "Service ID", "Param", "Data");
  printf("|------------|------------|------------|------------|\n");
  printf("| %-10s | %-10s | %-10s | %-10s |\n", cmdArgs[0], cmdArgs[1], cmdArgs[2], cmdArgs[3]);

  return 0;
}
```
```c
// Define a Service
Service testService = {
  .id = "TEST",
  .func = &testService1,
  .data = NULL
};
```
```c
// Register the Service to the Interface
registerService(&testInterface1, &testService);
```
```c
// Update the Command with a message from the client
Command testCmd = {0};
char msg[] = "IF1,TEST,1,2";
updateCommand(&testCmd, msg, &testInterface1);
```

```c
// Execute the Command on the server
executeCommand(&testCmd);

clearCommand(&testCmd); // Clear the Command
```

## WIP
* Dynamic Memory Allocation Branch
  
