# microRPC 
A C framework enabling Natural Language Remote Procedure Call Interfaces for Embedded Systems

Easily configurable and portable to different platforms with examples for common platforms.

Part of the ***uSystems project***.

Unlike other RPC frameworks, **microRPC** allows the programmer to define message formats directly in the code and doesn't require any addition config files/generated code. 

## Features

**microRPC** :
* Is transport layer agnostic. This means that the programmer can use any transport layer they want. E.g. Serial, Wifi, Bluetooth, etc.
* No dynamic memory allocation, buffer sizes use #define's at compile time.
* No external dependencies, common functions are re implemented as "helpers.h"
* Fault tolerant, if a message is not parsed correctly, the system will not crash.
* Secure, only the programmer can define the functions/data that can be accessed by the client.


## How does it work ?

High Level Overview:
* The programmer defines: 
  * A **Protocol** : How the messages are formatted and parsed.
  * An **Interface** : How the messages are handled. Each Interface uses one Protocol and can have multiple Services.
  * A **Service** : A function that the server can execute on behalf of the client.
  * A **Command** : A Singleton object that is used to access the Service and pass arguments to it.

* The programer can then:
  * Update the Command with a message from the client.
  * Execute the Command on the server.
  * Obtain the services **Response** and the functions *Return Value*. 
  
* **microRPC** will handle the parsing, formatting and validation of the messages.


**Interfaces** are allow a collection of **Services** to use the same **Protocol**. 

If the programmer requires a different kind of message format for a service they wish to expose to the client, they can create a new **Interface** with a different **Protocol**.

## Problem Statement


* A Comparrison of existing RPC and RPC-*like* frameworks 

| Framework | Description | Dependacies | Pros | Cons |
| --- | --- | --- | --- | --- |
protocolBuffers | |     |
Cap'n Proto | |     |
eRPC |
jsonRPC | |     |
xmlRPC | |     |
nanoRPC | |     |
mqttRPC | |     |




There exists a gap in open source frameworks that provide a consistent low level framework for building remote procedure call interfaces for embedded systems.
library1 is small and flexible but does not extend well to different platforms and is not able to handle large amounts of data needed for many applications.
Library 2 is a large library that can be employed on higher level nodes but is not suitable for microcontrollers due to the large memory footprint.





## Abstract
In Linguistics 'it is commonly assumed that communication involves speakers expressing their intentions thorough verbal and nonverbal means, and the recipients recognizing or attributing those attentions to the speakers' [Michael Hugh](https://www.tandfonline.com/doi/citedby/10.1080/07268600802516301?scroll=top&needAccess=true&role=tab) 

Therefore it follows that the way distributed systems communicate with each other and higher level systems should be as intentional as possible.

Intentionality is analogous to asking a question and getting an answer. In code, this is achieved by defining a function, calling it with the appropriate arguments and getting a return value.

This is trivial in a single threaded environment. However, in a distributed system, each node needs to be able to communicate with each other and the higher level system. This is where the problem arises. 

This problem is not new, and proprietary solutions exist. However, there is a lack of open source solutions that are flexible and portable to high level platforms.

In this paper we propose a low level flexible framework, **uCommander** aims to improve the way distributed systems communicate with each other. By  standardizing the way messages, in a data communications context, can express Intententionaliy. 


## Terms and Definitions

* **the programer**: A person who uses the uCommander framework. 
* **intentionality**: What a speaker(client) intends to a listener(server) to perform.
* **service**: An action that a server can perform on behalf of a client.


## Proposed Solution

**microRPC** is developed along with the values of the uSystems project. 

By using a static memory allocation at compile time. This allows the user to define the maximum number of nodes, members, data size, etc.

In lingusical communication the speaker only has a limited ammout of words to express intentionality. Therefore, if follows that the programer should be able to define the maximum numbeer of "intentions" that are needed to be expressed by the system they are developing. This is a common requierment but may be overlooked. 

How does the programer define the maximum number of "intentions" needed by the system?

A simple example is a motor controller connected to a computer:
* The computer needs to Command the motor controller to move
* The motor controller needs to report the sensor data to the computer
* The Computer needs to be able to adjust the PID gains of the motor controller
  
uCommander allows the programmer to expose a Service to the client. The service is defined by the programmer and can be any function that the programmer wants to expose to the client.

In the example above, the motor controller has a function that moves the diffeential motors, it accepts a liniear velocity and angular velocity as arguments. The function runs in a loop unitll the velocitys are set to 0.

Using the uCommander service api the programmer can define a service that accepts a linear velocity and angular velocity and calls/updates the motor controller function. 


## Design

**Seriveces** define the functionality the programmer wants to expose,with
* Protocol defenitions : number of arguments, max size of arguments, etc
* A function pointer to the function to be called on the server
* A void pointer to the data strucutre to be accessed by the service.

**ServiceInterface** is the high level interface that the programer interacts with. It is a wrapper around **Services** that standardaizes updating, executing and responing to services Commands Intentions . 

## Key Features
* Static memory allocation at compile time
* 100% table usage
* Flexibal strucutres for data and functions
* Error handling and timeout for function execution
* Easlily portable to different platforms
* Message Transport Layer agnostic
* Examples that take advantage of the features of the platform,threading rtos tasks, etc
  
## Why the need for uCommander ?
In many systems, designers want to be able to adjust certian parameters dynamically. E.g. the PID gains of a motor controller, calibration settings for a sensor, etc.

The initial reason for creating uCommander was to allow an ESP32 to connect to different Wifi Networks as I moved betweeen University and Home. 

That particualr probelm ***can*** be solved by hardcoding the SSID and Password into the code and then reflashing the code when i move between locations. Or storing a list of SSIDs and Passwords in the ESP32 and iterating through them until a connection is made. Both of these solutions are not ideal.

The main adantage of using uCommander over the alternative approches is the consistent API used to access and modify the data through human readable messages, without the need to reflash the code.
This allows easy exensitablisy of functionalhy in during development, countless projects have had to re immplmenent their ad hock serial communication protocols when they need a different kind of respoine message or need to pass differnt arguemtns to the function. 
In uCommander adding additional functiony is as simple as writing a new function and defining how it is accessed through a service call. 

### The bad way of doing it:
```c
char recv;
seitch (recv) {
case 'a':
  // do something
  break;
case 'b':
  // do something else
  break;
default:
  // do nothing/error handling
  break;
};
```
Why is this bad? Well as soon as your program requires more than a simple do this do that pattern, you will need to call different functions with multiple/different arguemtns and the client will expect message responses in differetn formats and with different data. This is where uCommander comes in and gives the developer a very low level RPC framework to build their application specific communication protocol on top of.

If the benefits are not imidiatly obvious, consider this; you have a system with 100 config parameters, wifi ssids, pid gains ect. You could write 100 functions and 100 case statements to handle them, or you could define hash table or linked list of config parameters and write a single function to get/set them all. The developer can then expose this function by definign a service, and the client can now call the function through a single message and get a single response.

Now also notice that in the above example the exposed functions accept differnty arguemtns and reutn different messages, this is all handled by the interface. The interface's update,execute and respond functions are the only thing that needs to be changed if the message format changes. The developer can then focus on the funcionaly of the system and not how to parse and format each message in a way that is compatible with the client.






  




