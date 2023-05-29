# microRPC 
A C framework for enabling Natural Language Remote Procedure Call Interfaces for Embedded Systems

Easily configurable and portable to different platforms with examples for common platforms.

Part of the ***uSystems project***.

Unlike other RPC frameworks, **microRPC** allows the programmer to define message formats directly in the code and doesn't require any additional config files/generated code. 

## Features

**microRPC** :
* Is transport layer agnostic. This means that the programmer can use any transport layer they want. E.g. Serial, Wifi, Bluetooth, etc.
* No dynamic memory allocation, buffer sizes use #define's at compile time. **(WIP Dynamic Memory Allocation Branch)**
* No external dependencies, common functions are re implemented as "helpers.h"
* Fault tolerant, if a message is not parsed correctly, the system will not crash.
* Secure, only the programmer can define the functions/data that can be accessed by the client.


## How does it work ?

High Level Overview:
* The programmer defines: 
  * A **Protocol** : How the messages are expected to be formatted. 
  * An **Interface** : How the messages are handled. Each Interface uses one Protocol to decode the message and can have multiple Services.
  * A **Service** : A function that the server can execute on behalf of the client. Services act as a wrapper around the functionally the programmer wants to expose to the client.
  * A **Command** : A Singleton object that is used to access the Service through an Interface and pass arguments to it.

* The programer can then:
  * Update the Command with a message from the client.
  * Execute the Command on the server.
  * Obtain the services **Response** and the functions **Return Value**. 
  
* **microRPC** will handle the parsing, formatting and validation of the messages.


**Interfaces** are allow a collection of **Services** to use the same **Protocol**. 

If the programmer requires a different kind of message format for a service they wish to expose to the client, they can create a new **Interface** with a different **Protocol**.

## Problem Statement
There exists a gap in open source frameworks that provide a flexible and abstracted way for low power embedded systems to communicate with each other and higher level systems. 

## Terms and Definitions

* **The Programer**: A person who uses the microRPC framework. 
* **Intentionality**: What a speaker(client) intends to a listener(server) to perform.
* **Service**: An action that a server can perform on behalf of a client.


## Proposed Solution

**microRPC** is developed along with the values of the uSystems project. 

By using a static memory allocation at compile time. This allows the user to define the maximum number of nodes, members, data size, etc.

In lingual communication the speaker only has a limited amount of words to express intentionality. Therefore, if follows that the programer should be able to define the maximum number of "intentions" that are needed to be expressed by the system they are developing. This is a common requirement but may be overlooked. 

How does the programer define the maximum number of "intentions" needed by the system?

A simple example is a motor controller connected to a computer:
* The computer needs to Command the motor controller to move
* The motor controller needs to report the sensor data to the computer
* The Computer needs to be able to adjust the PID gains of the motor controller
  
uCommander allows the programmer to expose a Service to the client. The service is defined by the programmer and can be any function that the programmer wants to expose to the client.

In the example above, the motor controller has a function that moves the differential motors, it accepts a linear velocity and angular velocity as arguments. The function runs in a loop until the velocity's are set to 0.

Using the uCommander service api the programmer can define a service that accepts a linear velocity and angular velocity and calls/updates the motor controller function. 

