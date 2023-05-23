#include "./include/microRPCTest.h"


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
    char configKey[MAX_IDLEN];
    char memberKey[MAX_IDLEN];
    extractArg(configKey, cmd->proto, cmdArg[2]->id);
    extractArg(memberKey, cmd->proto, cmdArg[3]->id);

    // User Code
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

    // *** // MICRO RPC PREPROCESSING // *** //
    // Extract the required arguments from the command message
    CmdArg *cmdArg[MAX_ARGS] = {0};
    for(int i = 0; i < cmd->proto->numArgs; i++){
        cmdArg[i] = &cmd->proto->cmdFormat[i];
    }
    // *** // *** // User Data Requirements // *** // *** //
    char configKey[MAX_IDLEN];
    char memberKey[MAX_IDLEN];
    char memberData[cmdArg[4]->maxLen+1];
    extractArg(configKey, cmd->proto, cmdArg[2]->id);
    extractArg(memberKey, cmd->proto, cmdArg[3]->id);
    extractArg(memberData, cmd->proto, cmdArg[4]->id);
    // **** // *** User Code *** // **** //
    // Get the config from the data table
    LookupTable *dataTable = data;
    Node *configNode = getNode(dataTable, configKey);
    if(configNode == 0) return -1; // Config does not exist
    if (adjMemberData(configNode, memberKey, memberData) != 0) return -1; // Member does not exist
    uCcpy(response, memberData); // update response

    return 0;
};

int SetData(Command *cmd, char *response, void *data){
    //@Brief: Sets the in flash data of a member in a config
    //@Params: cmd - the command object
    //@Params: response - the response string

     // *** // MICRO RPC PREPROCESSING // *** //
    // Extract the required arguments from the command message
    CmdArg *cmdArg[MAX_ARGS] = {0};
    for(int i = 0; i < cmd->proto->numArgs; i++){
        cmdArg[i] = &cmd->proto->cmdFormat[i];
    }
    // *** // *** // User Data Requirements // *** // *** //
    char configKey[MAX_IDLEN];
    char memberKey[MAX_IDLEN];
    char memberData[cmdArg[4]->maxLen+1];
    extractArg(configKey, cmd->proto, cmdArg[2]->id);
    extractArg(memberKey, cmd->proto, cmdArg[3]->id);
    extractArg(memberData, cmd->proto, cmdArg[4]->id);
    // **** // *** User Code *** // **** //
    // Get the config from the data table
    LookupTable *dataTable = data;
    Node *configNode = getNode(dataTable, configKey);
    if(configNode == 0) return -1; // Config does not exist
    if (setMemberData(configNode, memberKey, memberData) != 0) return -1; // Member does not exist
    uCcpy(response, memberData); // update response

    return 0;
};



void printSizes(RPC *rpc, LookupTable *dataTable, Command *cmd){
    printf("\nObject\t\t|Size\t|Address\t|\n");
    printf("-----------------------------------------\n");
    printf("RPC\t\t|%lu\t|%p\t|\n", sizeof(*rpc), rpc);
    printf("LookupTable\t|%lu\t|%p\t|\n", sizeof(*dataTable), dataTable);
    printf("Command\t\t|%lu\t|%p|\t\n", sizeof(*cmd), cmd);

    printf("Number of Data Nodes: %d\n", dataTable->count);

    for(int i = 0; i < NUM_NODES; i++){
        if(dataTable->nodes[i] == 0) continue;
        printf("\nData Node\t|Size\t|Address\t|\n");
        printf("-----------------------------------------\n");
        printf("\n%s\t\t|%lu\t|%p\t|\n",dataTable->nodes[i]->key, sizeof(*dataTable->nodes[i]), dataTable->nodes[i]);
        printf("\n\tMember\t|Value\t\t|Size\t|Address\t\n");
        printf("\t-------------------------------------------------\n");
        for(int j = 0; j < dataTable->nodes[i]->count; j++){
            if (dataTable->nodes[i]->members[j] == 0) continue;
            printf("\n\t%s\t|%s\t|%lu\t|%p\t|\n",
            dataTable->nodes[i]->members[j]->key, 
            dataTable->nodes[i]->members[j]->data,
            sizeof(Member), dataTable->nodes[i]->members[j]);
        }
    }
    printf("Number of Interfaces: %d\n", rpc->count);
    
    for(int i = 0; i < MAX_INTERFACES; i ++){
        if (rpc->interfaces[i] == 0) continue;
        printf("\nInterface\t|Size\tAddress\t|\n");
        printf("-------------------------------------------------\n"); 
        printf("%s\t\t|%lu\t%p\n",rpc->interfaces[i]->id, sizeof(Interface), rpc->interfaces[i]);
        printf("\n\tService\t|Size\tAddress\t|\n");
        printf("\t-------------------------------------------------\n");
        for(int j = 0; j < MAX_SERVICES; j++){
            if (rpc->interfaces[i]->services[j] == 0) continue;
            printf("\t%s\t|%lu\t%p\n",
            rpc->interfaces[i]->services[j]->key, 
            sizeof(rpc->interfaces[i]->services[j]), 
            rpc->interfaces[i]->services[j]);


        }
    }

}

int main(void){

    // ********** // Data Setup // ********** //
    LookupTable dataTable;
    initLookupTable(&dataTable, NUM_NODES);
    Node wifiConfig;
    createNode(&wifiConfig, "WIFI");
    Member ssid = {.key = "SSID", .data = "mySSID"};
    Member pask = {.key = "PASS",.data = "myPASS"};
    Member ip = {.key = "IP",.data = "192.1681.100"};
    addMember(&wifiConfig, &ssid);
    addMember(&wifiConfig, &pask);
    addMember(&wifiConfig, &ip);
    Node camConfig;
    createNode(&camConfig, "CAM");
    Member res = {.key = "RES", .data = "640x480"};
    Member qual = {.key = "QUAL", .data = "100"};
    addMember(&camConfig, &res);
    addMember(&camConfig, &qual);
    Node devConfig;
    createNode(&devConfig, "DEVC");
    Member name = {.key = "NAME", .data = "myDevice"};
    Member id = {.key = "ID",.data = "123456789"};
    addMember(&devConfig, &name);
    addMember(&devConfig, &id);
    addNode(&dataTable, &wifiConfig);
    addNode(&dataTable, &camConfig);
    addNode(&dataTable, &devConfig);

// ********** // RPC Setup // ********** //
    RPC Rpc;
    initRPC(&Rpc);
// Define a Protocol for each Message Type Required
    Protocol taskProto = {
        // Defines the protocol for the TASK interface
        .delim = ',',
        .cmdFormat = {
            { .id = "TRGT", .type = CHAR, .maxLen = TARGET_ARG_LEN},
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
            { .id = "TRGT", .type = CHAR, .maxLen = TARGET_ARG_LEN},
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

    // Memory Size test
    printSizes(&Rpc,&dataTable, &Cmd);
    char buf[MAX_CMDLEN] = {0};
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
