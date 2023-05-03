#include <stdio.h>
#include "microRPCTest.h"

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
            { .id = "CNFG", .type = CHAR, .maxLen = 4},
            { .id = "MEBR", .type = INT, .maxLen = 4},
            { .id = "DATA", .type = STR, .maxLen = 15},
        },
        .numArgs = 4,
    };
    initProtocol(&dataProto);
// Define the Interfaces Each Interface has a Protocol
    Interface Tasks;
    createInterface(&Tasks, "TASK", &taskProto, NULL);
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
    char buf[MAX_CMDLEN];
    for(int i =0; i < 10; i++){
        printf("Enter a command: \n");
        //scanf("%s", buf);
        msg.buf = "DATA,GET,WIFI,SSID";
        msg.len = uStrlen(buf);
        // Update the command object
        updateCommand(&Cmd, &msg, &Rpc);

        if(Cmd.valid == 0)
        execCommand(&Cmd, &Rpc);
    }

    return 0;
    
}

