#include    "../include/microRPC.h"
#include    "../include/helpers.h"
#include    <stdio.h> // printf

// *** // Setup functions // *** // 
void initRPC(RPC *rpc){
   for(int i = 0; i < (MAX_INTERFACES); i++){
        rpc->interfaces[i] = 0; // Set interface table to null
   }
    rpc->count = 0; 
}

void initProtocol(Protocol *proto){
    proto->maxCmdLen = 0;
    for(int i = 0; i < MAX_ARGS; i++){
        proto->maxCmdLen += proto->cmdFormat[i].maxLen; // Sum each arg length
    }
    proto->maxCmdLen += MAX_ARGS-1; // Add the number of delimiters
}

void createInterface(Interface *interface, char *id, Protocol *proto, void *data ){
    uCcpy(interface->id, id);
    interface->proto = proto;
    interface->count = 0;
    interface->data = data;
    for(int i = 0; i < MAX_SERVICES; i++){
        interface->services[i] = 0; // Initialize the service table to 0
    }
}

int addInterface(RPC *rpc, Interface *interface){
    // @brief Add an interface to the RPC
    // @param: rpc - pointer to the RPC
    // @param: interface - pointer to the interface to add
    // @return: 0 on success, -1 on failure
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
    int idx = hash(service->key, MAX_SERVICES);
    if(interface->services[idx] != 0){
        return -1; // Service already exists at this index
    }
    interface->services[idx] = service;
    interface->count++;
    return 0;
}

Service *getService(Interface *interface, char *key){
    int idx = hash(key, MAX_SERVICES);
    return interface->services[idx];
}

Protocol *findProtocol(RPC *rpc, String *msgCmd){

    // Extract Target Interface id from message 
    char targetId[TARGET_LEN+1]; // Target interface id
    int lenTrunk = uCTrunk(targetId, msgCmd->buf, 0,TARGET_LEN ); 
    if(lenTrunk != TARGET_LEN){
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
    // Validate Argument lengths against the protocol
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
    cmd->proto = findProtocol(rpc, msgCmd);
    // Check if the command is of the correct length
    if(msgCmd->len > cmd->proto->maxCmdLen){
        return -1; // msg is too long
    }
    cmd->len = msgCmd->len;
   
    // Validate message against the target interfaces's protocol
    cmd->valid = !updateArguments(cmd->proto->cmdFormat, msgCmd, cmd->proto);
    return 0; 
}

int execCommand(Command *cmd, RPC *rpc){
    const int TARGET_IDX = 0;  
    const int SERVICE_IDX =  1;

    Protocol *proto = cmd->proto; 
    // Extract the target interface and service if from the command
    char targetId[TARGET_LEN+1];
    char serviceId[proto->cmdFormat[SERVICE_IDX].maxLen+1];  
    uCTrunk(targetId, cmd->proto->cmdFormat[TARGET_IDX].str.buf, 0,TARGET_LEN ); 
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
    Service *service = getService(interface, key);
    if(service == 0){
        return -1; // Service does not exist at this index 
    }
    uCcpy(response, service->response);
    return 0;
}

int getServiceRet(Interface *interface, char *key){
    Service *service = getService(interface, key);
    if(service == 0){
        return -1; // Service does not exist at this index
    }
    return service->ret;
}


#include <stdio.h>
#include "../tests/microRPCTest.h"


// ********** // RPC Exposed Services // ********** //
int RunLed(Command *cmd, char *response, void *data){
    // Turn on or off the LED
    char arg[cmd->proto->cmdFormat[1].maxLen+1];
    extractArg(arg, cmd->proto, "CMD");
    printf("LED: %s\n", arg);
    uCcpy(response, "LED ON");
    return 0;
}

int CamShoot(Command *cmd, char *response, void *data){
    // Take a picture
    char args[cmd->proto->cmdFormat[3].maxLen+1];
    extractArg(args, cmd->proto, "DATA");
    printf("CAM: %s\n", args);
    uCcpy(response, "CAM SHOOT");
    return 0;
}

int GetData(Command *cmd, char *response, void *data){
    //@Brief: Gets the data of a member in a config
    //@Params: cmd - the command object
    //@Params: response - the response string

    // Extract the required arguments from the command message
    CmdArg *cmdArg[MAX_ARGS] = {0};
    for(int i = 0; i < cmd->proto->numArgs; i++){
        cmdArg[i] = &cmd->proto->cmdFormat[i];
    }

    char configKey[5];
    char memberKey[5];
    extractArg(configKey, cmd->proto, cmdArg[2]->id);
    extractArg(memberKey, cmd->proto, cmdArg[3]->id);

    // Get the config from the data table
    LookupTable *dataTable = data;
    Node *configNode = getNode(dataTable, configKey);
    if(configNode == 0) return -1; // Config does not exist
    response  = getMemberData(configNode, memberKey); 
    printf("GET DATA: %s\n", response);
    return 0;
};

int AdjData(Command *cmd, char *response, void *data){
    //@Brief: Adjusts the in memory data of a member in a config
    //@Params: cmd - the command object
    //@Params: response - the response string


    return 0;
};

int SetData(Command *cmd, char *response, void *data){
    //@Brief: Sets the in flash data of a member in a config
    //@Params: cmd - the command object
    //@Params: response - the response string



    return 0;
};

int main(void){

    // ********** // Data Setup // ********** //
    LookupTable dataTable;
    initLookupTable(&dataTable, NUM_NODES);
    Node wifiConfig;
    createNode(&wifiConfig, "WIFI");
    Member ssid = {
        .key = "SSID",
        .data = "mySSID",
    };
    Member pask = {
        .key = "PASS",
        .data = "myPASS",
    };
    Member ip = {
        .key = "IP",
        .data = "192.1681.100",
    };
    addMember(&wifiConfig, &ssid);
    addMember(&wifiConfig, &pask);
    addMember(&wifiConfig, &ip);
    addNode(&dataTable, &wifiConfig);
    Node camConfig;
    createNode(&camConfig, "CAM");
    Member res = {
        .key = "RES",
        .data = "640x480",
    };
    Member qual = {
        .key = "QUAL",
        .data = "100",
    };
    addMember(&camConfig, &res);
    addMember(&camConfig, &qual);
    addNode(&dataTable, &camConfig);

    Node deviceConfig;
    createNode(&deviceConfig, "DEVC");
    Member name = {
        .key = "NAME",
        .data = "myDevice",
    };
    Member id = {
        .key = "UDI",
        .data = "1234567890",
    };
    Member swVersion = {
        .key = "SWV",
        .data = "1.0.0",
    };
    addMember(&deviceConfig, &name);
    addMember(&deviceConfig, &id);
    addMember(&deviceConfig, &swVersion);
    addNode(&dataTable, &deviceConfig);
    

// ********** // RPC Setup // ********** //
    RPC Rpc;
    initRPC(&Rpc);
// Define a Protocol for each Message Type Required
    Protocol taskProto = {
        // Defines the protocol for the TASK interfac
        .delim = ',',
        .cmdFormat = {
            { .id = "TRGT", .type = CHAR, .maxLen = TARGET_LEN},
            { .id = "CMD", .type = CHAR, .maxLen = 4},
            { .id = "TIME", .type = INT, .maxLen = 2},
            { .id = "DATA", .type = STR, .maxLen = 15},
        },
        .numArgs = 4,
    };
    initProtocol(&taskProto);
    Protocol dataProto = {
        // Defines the protocol for the DATA interface
        .delim = ',',
        .cmdFormat = {
            { .id = "TRGT", .type = CHAR, .maxLen = TARGET_LEN},
            { .id = "CMD", .type = CHAR, .maxLen = 4},
            { .id = "CNFG", .type = CHAR, .maxLen = 4},
            { .id = "MEBR", .type = INT, .maxLen = 4},
            { .id = "DATA", .type = STR, .maxLen = 15}
        },
        .numArgs = 5,
    };
    initProtocol(&dataProto);
// Define the Interfaces Each Interface has a Protocol
    Interface Tasks;
    createInterface(&Tasks, "TASK", &taskProto, &dataTable);
    addInterface(&Rpc, &Tasks);
    Interface Data;
    createInterface(&Data, "DATA", &dataProto, &dataTable);
    addInterface(&Rpc, &Data);
// Define the Services Each Service has a Function
    Service RunLedService = {
        .key = "LED",
        .desc = "Turns on or off the LED",
        .func = &RunLed,
        .ret = 0
    };
    Service CamShootService = {
        .key = "CAM",
        .desc = "Takes a picture",
        .func = &CamShoot,
        .ret = 0
    };
    registerService(&Tasks, &RunLedService);
    registerService(&Tasks, &CamShootService);
    Service GetDataService = {
        .key = "GET",
        .desc = "Gets member data from a config",
        .func = &GetData,
        .ret = 0
    };
    Service AdjDataService = {
        .key = "ADJ",
        .desc = "Adjusts the data of a member in a config",
        .func = &AdjData,
        .ret = 0
    };
    Service SetDataService = {
        .key = "SET",
        .desc = "Sets the data of a member in a config",
        .func = &SetData,
        .ret = 0
    };
    registerService(&Data, &GetDataService);
    registerService(&Data, &AdjDataService);
    registerService(&Data, &SetDataService);

// ********** // RPC Test // ********** //
    Command Cmd = {0};
    String msg = {0};
    char buf[MAX_CMDLEN] = {0};
    printf("Addr dataTable: %p\n", &dataTable);
    for(int i =0; i < 10; i++){
        printf("Enter a command: \n");
        scanf("%s", buf);
    
        msg.buf = buf;
        msg.len = uStrlen(msg.buf);
        // Update the command object
        updateCommand(&Cmd, &msg, &Rpc);

        if(Cmd.valid == 0) {
            printf("Invalid Command\n");
            continue;
        }
        execCommand(&Cmd, &Rpc);
    }


    return 0;
    
}





