#ifndef UCOMMANDER_H
#define UCOMMANDER_H


// *** // Includes // *** //
#include "../include/helpers.h"


// *** // Static Array Allocation // *** //

const int MAX_CMD_SIZE = 28;
const int MAX_ARGS = 5;
const int MAX_SERVICES = 10;
const int MAX_INTERFACES = 5;
const int MAX_RESPONSE_SIZE = 10;
const int MAX_ID_SIZE = 5;
const int TARGET_ARG_LEN = 3; 


// *** // Data Structures // *** //
typedef struct Message{
    char *buf; 
    int len; // excluding null character
}Message; // A message buffer and its length

typedef struct CmdArg{
    char id[MAX_ID_SIZE]; 
    Message str;
    int maxSize; // Max len including null character
}CmdArg; // Defines the format of an Argument in a command

typedef struct Protocol{
    // @brief Defines the protocol for the TASK interface
    // @REQ: |TargetInterface|Service|...
    int numArgs; 
    char delim;
    int maxCmdLen; // excluding '\0'
    CmdArg cmdFormat[MAX_ARGS];
}Protocol; // Defines the protocol format for a given interface

typedef struct Command{
    Protocol *proto; 
    int valid; 
}Command; // A command that can be executed by an RPC Service

// *** // Service Functions // *** //
typedef int (*rpcFunc)(Command *cmd,char *response, void *data); 

typedef struct Service{
    char id[MAX_ID_SIZE]; 
    char *desc; 
    rpcFunc func; 
    char response[MAX_RESPONSE_SIZE]; // Last response of the service
    int ret; // Last return value of the service
}Service; // An executable function that can be called by a client

typedef struct Interface{
    char id[MAX_ID_SIZE]; 
    Protocol *proto; 
    Service *services[MAX_SERVICES]; // List of services
    int count; 
    void *data; // Pointer to interface data
} Interface; // RPC Services under 

typedef struct Gateway{
    Interface *interfaces[MAX_INTERFACES]; // List of interfaces
    int count; // Number of interfaces
}Gateway; // A list of interfaces that can be called by a client


// *** // Setup functions // *** // 
void initRPC(Gateway *gateway){
    // @brief Initialize the Gateway 
    // @desc: Set all interface pointers to 0
   for(int i = 0; i < (MAX_INTERFACES); i++){
        gateway->interfaces[i] = 0; // Set interface table to null
   }
    gateway->count = 0; 
}

void createInterface(Interface *interface, char *id, Protocol *proto, void *data ){
    // @brief Create an interface object 
    // @desc: Set the interface id, protocol, and data
    uCcpy(interface->id, id);
    interface->proto = proto;
    interface->count = 0;
    interface->data = data;
    for(int i = 0; i < MAX_SERVICES; i++){
        interface->services[i] = 0; // Initialize the service table to 0
    }
}

int addInterface(Gateway *gateway, Interface *interface){
    // @brief Add an interface to the Gateway incremtaly 
    // @desc: Add a pointer to the interface to the Gateway's interface table
    // @note: Collision resolution is not implemented
    if (gateway->count+1 > MAX_INTERFACES){
        return -1; // Interface table is full
    }
    //int idx = hash(interface->id, MAX_INTERFACES); REMOVIG HASHING FOR NOW
    if(gateway->interfaces[gateway->count] != 0){
        return -1; // Interface already exists at this index
    }
    gateway->interfaces[gateway->count] = interface;
    gateway->count++;
    return 0;
}

int registerService(Interface *interface, Service *service){
    // @brief Add a service to an interface by hash id
    //int idx = hash(service->id, MAX_SERVICES); REMOVIG HASHING FOR NOW
    if(interface->services[interface->count] != 0){
        return -1; // Service already exists at this index
    }
    interface->services[interface->count] = service;
    interface->count++;
    return 0;
}


// *** // Internal functions // *** //
Interface *getInterface(Gateway *gateway, char *id){
    // @brief Get an interface from the Gateway by id
    // @return: Pointer to the interface or 0 if not found
    for(int i = 0; i < gateway->count; i++){
        if(uStrcmp(gateway->interfaces[i]->id, id) == 0){
            return gateway->interfaces[i];
        }
    }
    return 0;
}

static Service *getService(Interface *interface, char *id){
    // @brief Get a service from an interface by hash id
    // @return: Pointer to the service or 0 if not found
    for(int i = 0; i < interface->count; i++){
        if(uStrcmp(interface->services[i]->id, id) == 0){
            return interface->services[i];
        }
    }
    return 0;
}

static Protocol *findProtocol(Gateway *gateway, Message *msgCmd){
    // @brief Find the protocol of the target interface
    // @desc: Parse the message and find the target interface 
    // @desc: Return pointer to the protocol of the target interface or 0 if not found

    // Get the target interface id from the message Command
    char targetId[TARGET_ARG_LEN+1];
    int lenTrunk = uCTrunk(targetId, msgCmd->buf, 0,TARGET_ARG_LEN); 
    if(lenTrunk != TARGET_ARG_LEN) return 0; // Target argument is not the correct length
    // Get the target interface's protocol from the Gateway
    Interface *interface = getInterface(gateway, targetId);
    if(interface == 0) return 0; // Target interface does not exist
    // Return the protocol of the target interface
    return interface->proto;
}

static int updateArguments(Message *msgCmd, Protocol *proto){
    // @brief Update the arguments of the protocol with the message
    // @desc: Parse the message and update the protocol arguments zero copy
    // @desc: Validate Argument lengths against the protocol
    // @return: 0 if successful, -1 if error

    if(msgCmd == 0 || proto == 0) return -1; // Null pointer

    int argLen = 0; // Length of the current argument
    int argIdx = 0; // Index of argument in message
    int delimIdx[proto->numArgs]; // Index of delimiter found
    for(int i = 0; i < msgCmd->len; i++){
        // Check for delimiter or end of message
        if(msgCmd->buf[i] == proto->delim || msgCmd->buf[i] == '\0'){
            if (argIdx == proto->numArgs ) return -1; // Too many arguments
            if( argLen >= proto->cmdFormat[argIdx].maxSize) return -1; // Argument is too long
    
            delimIdx[argIdx] = i; // Store the index of the delimiter   
            // Assign the argument to the protocol 
            // TODO: THIS SEEMS IS HACKY.
            proto->cmdFormat[argIdx].str.buf = &msgCmd->buf[i-argLen]; // Point to start of arg
            proto->cmdFormat[argIdx].str.len = argLen; // Set the length of the arg
            argLen = 0; 
            argIdx++; 
        }
        else{
            argLen++;
        }
    }
    return 0;
}



// *** // **** User Exposed Functions **** // *** //
void clearCommand(Command *cmd){
    // @brief Clear the command 
    cmd->valid = 0;
    if(cmd->proto != 0){
        for(int i =0; i < MAX_ARGS; i++){
            cmd->proto->cmdFormat[i].str.buf = 0; // Set the buffer to null
            cmd->proto->cmdFormat[i].str.len = 0; // Set the length to 0
        }
        cmd->proto = 0; // Unassign the protocol
    }
    
}

void clearServiceResponse(Service *service){
    // @brief Clear the service response
    // @desc: Clear the service response by setting the length to 0
    for(int i =0; i< MAX_RESPONSE_SIZE; i++){
        service->response[i] = 0;
    }
    service->ret = 0;
}

int updateCommand(Command *cmd, Message *msgCmd, Gateway *gateway){
    // @brief Update the command with the message
    // @desc: Parse the message and update the command
    // @return: 0 if successful, -1 if error
    cmd->proto = findProtocol(gateway, msgCmd);
    if(cmd->proto == 0){
        cmd->valid = 0;
        return -1; // Target interface does not exist
    }
    // Check if the command is of the correct length
    if(msgCmd->len > cmd->proto->maxCmdLen){
        cmd->valid = 0;
        return -1; // msg is too long
    }
    // Validate message against the target interfaces's protocol
    cmd->valid = !updateArguments(msgCmd, cmd->proto);
    return 0; 
}

int execCommand(Command *cmd, Gateway *gateway){
    // @brief Execute the command 
    //  @desc: Execute the command by calling the target service 
    //  @return: 0 if successful, -1 if error

    const int TARGET_IDX = 0;  
    const int SERVICE_IDX =  1;

    if(cmd->valid == 0) return -1; // Command is not valid
    Protocol *proto = cmd->proto; 
    // Extract the target interface and service if from the command
    char targetId[TARGET_ARG_LEN+1];
    uCTrunk(targetId, cmd->proto->cmdFormat[TARGET_IDX].str.buf, 0,TARGET_ARG_LEN ); 
    char serviceId[proto->cmdFormat[SERVICE_IDX].maxSize+1];  
    uCTrunk(serviceId, cmd->proto->cmdFormat[SERVICE_IDX].str.buf, 0,proto->cmdFormat[SERVICE_IDX].str.len);

    // Get the target interface from Gateway
    Interface *interface = getInterface(gateway, targetId);
    if(interface == 0){
        return -1; // Interface does not exist
    }
    // Get the target service from the interface
    Service *service = getService(interface, serviceId);
    if(service == 0){
        return -1; // Service does not exist
    }
    // Execute the service function
    service->ret = service->func(cmd,service->response,interface->data);

    return 0;
}

int extractArg(char *arg, Protocol *proto, char *argId){
    // @brief Extract an argument from the protocol
    // @desc: Copy an argument from the protocol pointer by argument id
    // Find the argument index
    int argIdx = -1;
    for(int i = 0; i < MAX_ARGS; i++){
        if(uStrcmp(proto->cmdFormat[i].id, argId) == 0){
            argIdx = i;
            break;
        }
    }
    if(argIdx == -1){
        return -1; // Argument does not exist
    }
    // Copy the argument string by len
    uCTrunk(arg,proto->cmdFormat[argIdx].str.buf, 0, proto->cmdFormat[argIdx].str.len);

    return argIdx;
}

int getServiceResponse(Interface *interface, char *id, char *response){
    // @brief Get the response of a service
    // @desc: Copy the latests response of a service into the response buffer
    Service *service = getService(interface, id);
    if(service == 0){
        return -1; // Service does not exist at this index 
    }
    uCcpy(response, service->response);
    
    return 0;
}

int getServiceRet(Interface *interface, char *id){
    // @brief Get the return value of a service
    Service *service = getService(interface, id);
    if(service == 0){
        return -1; // Service does not exist at this index
    }
    return service->ret;
}



#endif 


