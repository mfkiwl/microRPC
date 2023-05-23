#ifndef UCOMMANDER_H
#define UCOMMANDER_H


// *** // Includes // *** //
#include "../include/helpers.h"


// *** // Static Array Allocation // *** //

const int MAX_CMDLEN = 28;
const int MAX_ARGS = 5;
const int MAX_SERVICES = 10;
const int MAX_INTERFACES = 10;
const int MAX_RESPONSE_LEN = 10;
const int MAX_IDLEN = 5;
const int TARGET_ARG_LEN = 4;


// *** // Data Structures // *** //
typedef struct String{
    char *buf; // Pointer to the string buffer
    int len;
}String;

typedef struct CmdArg{
    char id[MAX_IDLEN]; // ID of the argument
    enum {CHAR,INT,STR} type; // Type of the argument
    String str;
    int maxLen; // Max len excluding '/0'
}CmdArg; // Defines the format of an Argument in a command

typedef struct Protocol{
    // @brief Defines the protocol for the TASK interface
    // @REQ: |TargetInterface|Service|CommandMessage|
    int numArgs; 
    int maxCmdLen;
    CmdArg cmdFormat[MAX_ARGS];
    char delim;
}Protocol; 

typedef struct Command{
    Protocol *proto; // The protocol of the command
    int len; // Length of the command
    int valid; 
}Command; 

// *** // Service Functions // *** //
typedef int (*ServiceFunc)(Command *cmd,char *response, void *data); 

typedef struct Service{
    char key[MAX_IDLEN]; // Key of the service
    char *desc; // Description of the service
    ServiceFunc func; // pointer to the service function
    char response[MAX_RESPONSE_LEN]; // Last response of the service
    int ret; // Last return value of the service
}Service; // An executable function that can be called by a client

typedef struct Interface{
    char id[MAX_IDLEN]; // Name of the interface
    Protocol *proto; // Protocol of the interface
    Service *services[MAX_SERVICES]; // List of services
    int count; // Number of services
    void *data; // Pointer to interface data
} Interface; // A list of services that can be called by a client

typedef struct RPC{
    Interface *interfaces[MAX_INTERFACES]; // List of interfaces
    int count; // Number of interfaces
}RPC; // A list of interfaces that can be called by a client


// *** // Setup functions // *** // 
void initRPC(RPC *rpc){
    // @brief Initialize the RPC 
    // @desc: Set all interface pointers to 0
   for(int i = 0; i < (MAX_INTERFACES); i++){
        rpc->interfaces[i] = 0; // Set interface table to null
   }
    rpc->count = 0; 
}

void initProtocol(Protocol *proto){
    // @brief Initialize the protocol
    // @desc: Count the arg length and set the max command length
    proto->maxCmdLen = 0;
    for(int i = 0; i < MAX_ARGS; i++){
        proto->maxCmdLen += proto->cmdFormat[i].maxLen; // Sum each arg length
    }
    proto->maxCmdLen += MAX_ARGS-1; // Add the number of delimiters
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

int addInterface(RPC *rpc, Interface *interface){
    // @brief Add an interface to the RPC by hash key
    // @desc: Add a pointer to the interface to the RPC's interface table
    // @note: Collision resolution is not implemented
    if (rpc->count+1 >= MAX_INTERFACES){
        return -1; // Interface table is full
    }
    int idx = hash(interface->id, MAX_INTERFACES);
    if(rpc->interfaces[idx] != 0){
        return -1; // Interface already exists at this index
    }
    rpc->interfaces[idx] = interface;
    rpc->count++;
    return 0;
}


// *** // Internal functions // *** //
Interface *getInterface(RPC *rpc, char *id){
    // @brief Get an interface from the RPC by hash key
    int idx = hash(id, MAX_INTERFACES);
    if (idx >= MAX_INTERFACES){
        return 0; // Index out of range
    }
    if(rpc->interfaces[idx] == 0){
        return 0; // Interface does not exist at this index
    }

    return rpc->interfaces[idx];
}

int registerService(Interface *interface, Service *service){
    // @brief Add a service to an interface by hash key
    int idx = hash(service->key, MAX_SERVICES);
    if(interface->services[idx] != 0){
        return -1; // Service already exists at this index
    }
    interface->services[idx] = service;
    interface->count++;
    return 0;
}

Service *getService(Interface *interface, char *key){
    // @brief Get a service from an interface by hash key
    int idx = hash(key, MAX_SERVICES);
    return interface->services[idx];
}

Protocol *findProtocol(RPC *rpc, String *msgCmd){
    // @brief Find the protocol of the target interface
    // @desc: Get the target interface id from the message
    char targetId[TARGET_ARG_LEN+1]; // Target interface id
    int lenTrunk = uCTrunk(targetId, msgCmd->buf, 0,TARGET_ARG_LEN ); 
    if(lenTrunk != TARGET_ARG_LEN){
        return 0; // Invalid target interface key
    }
    // Get the target interface's protocol from the RPC
    Interface *interface; 
    interface = getInterface(rpc, targetId);
    if(interface == 0){
        return 0; // Target Interface does not exist
    }
    // Return the protocol of the target interface
    return interface->proto;
}

int updateArguments(CmdArg *cmdFormat, String *msgCmd, Protocol *proto){
    // @brief Update the arguments of the protocol with the message
    // @desc: Parse the message and update the protocol arguments
    // @note: Validate Argument lengths against the protocol
    int argLen = 0; // Length of the current argument
    int argIdx = 0; // Index of argument in message
    int delimIdx[proto->numArgs]; // Index of delimiter found
    for(int i = 0; i < msgCmd->len+1; i++){
        // Check for delimiter or end of message
        if(msgCmd->buf[i] == proto->delim || msgCmd->buf[i] == '\0'){
            if (argIdx == proto->numArgs ){
                return -1; // Too many arguments
            }
            // Check if the argument is of the correct length
            if(argLen > proto->cmdFormat[argIdx].maxLen){
                return -1; // Argument is too long
            }
            delimIdx[argIdx] = i; // Store the index of the delimiter
            // Assign the argument to the protocol 
            proto->cmdFormat[argIdx].str.buf = &msgCmd->buf[i-argLen]; // Point to start of arg
            proto->cmdFormat[argIdx].str.len = argLen;
            argLen = 0; 
            argIdx++; // find next argument 
        }
        else{
            argLen++;
        }
    }
    return 0; // Success
}


// *** // **** User Exposed Functions **** // *** //
int updateCommand(Command *cmd, String *msgCmd, RPC *rpc){
    // @brief Update the command with the message
    // @desc: Parse the message and update the command
    cmd->proto = findProtocol(rpc, msgCmd);
    if(cmd->proto == 0){
        return -1; // Target interface does not exist
    }
    // Check if the command is of the correct length
    if(msgCmd->len > cmd->proto->maxCmdLen){
        return -1; // msg is too long
    }
    // Copy the message into the command
    cmd->len = msgCmd->len;
   
    // Validate message against the target interfaces's protocol
    cmd->valid = !updateArguments(cmd->proto->cmdFormat, msgCmd, cmd->proto);
    return 0; 
}

int execCommand(Command *cmd, RPC *rpc){
    // @brief Execute the command 
    //  @desc: Execute the command by calling the target service 
    //  @note: The command must be valid
    const int TARGET_IDX = 0;  
    const int SERVICE_IDX =  1;

    Protocol *proto = cmd->proto; 
    // Extract the target interface and service if from the command
    char targetId[TARGET_ARG_LEN+1];
    char serviceId[proto->cmdFormat[SERVICE_IDX].maxLen+1];  
    uCTrunk(targetId, cmd->proto->cmdFormat[TARGET_IDX].str.buf, 0,TARGET_ARG_LEN ); 
    uCTrunk(serviceId, cmd->proto->cmdFormat[SERVICE_IDX].str.buf, 0,proto->cmdFormat[SERVICE_IDX].str.len);

    // Get the target interface from RPC
    Interface *interface = getInterface(rpc, targetId);
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

int getServiceResponse(Interface *interface, char *key, char *response){
    // @brief Get the response of a service
    // @desc: Copy the latests response of a service into the response buffer
    Service *service = getService(interface, key);
    if(service == 0){
        return -1; // Service does not exist at this index 
    }
    uCcpy(response, service->response);
    
    return 0;
}

int getServiceRet(Interface *interface, char *key){
    // @brief Get the return value of a service
    Service *service = getService(interface, key);
    if(service == 0){
        return -1; // Service does not exist at this index
    }
    return service->ret;
}

#endif 


