#include "include/microRPCTest.h"
#include <stdio.h>


// Color codes for printing
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"


// *** MICRO RPC TESTS *** //

int test_service1(Command *cmd,char *response, void *data){
	uCcpy(response, "TS1OK");
    return 0;
}

int test_service2(Command *cmd,char *response, void *data){
	uCcpy(response, "TS2OK");
    return 0;
}


int main(void){
    // ** // Initialize Gateway // ** //
    Gateway rpc;
    initRPC(&rpc);
    
    Protocol testproto1 = {
        .numArgs = 4,
        .maxCmdLen = 28,
        .delim = ',',
        .cmdFormat = { 
            {.id = "TRGT", .maxSize = 5 },
            {.id = "SRVC", .maxSize = 5 },
            {.id = "PRAM", .maxSize = 5 },
            {.id = "DATA", .maxSize = 5 }
        }
    };

    Interface testInterface1 = {0};
    createInterface(&testInterface1, "IF1",&testproto1,NULL);
    addInterface(&rpc, &testInterface1);
    Service testService1 = {
        .id = "TS1",
        .desc = "Test Service 1",
        .func = &test_service1,
        .response = "\0",
        .ret = 0,
    };
    Service testService2 = {
        .id = "TS2",
        .desc = "Test Service 2",
        .func = &test_service2,
        .response = "\0",
        .ret = 0,
    };
    registerService(&testInterface1, &testService1);
    registerService(&testInterface1, &testService2);

    // ** // Run Tests // ** //
    // ********** // Gateway Test // ********** //
    Command Cmd = {0};
    Message msg = {0};
	const int NUM_TEST = 14;

    char testcmd[NUM_TEST][100] = {
    	"IF1,TS1,0,D", // Test Case 0: Valid Min
        "IF1,TS1,0000,DATA", // Test Case 1: Valid Max
		"IF1,TS1,0000", // Test Case 2: Missing Argument
        "IF1,TS1,", // Test Case 3: Missing Arguments
        "IF1,TS1,,DATA", // Test Case 4: Invalid Min on single argument
        "IF1,TS1,,", // Test Case 5: Invalid Min on all argument
		"IF1,TS1,0000,DATA,Extra", // Test Case 6: Invalid Max with extra argument
		"IF1,TS1,00000,DATA", // Test Case 7: Invalid Max on single argument
        "IF1,TS1,00000,DATAE", // Test Case 8: Invalid Max on all argument
        "IF1,TSx,0000,DATA", // Test Case 9: Invalid Service
        "IFx,TS1,0000,DATA", // Test Case 10: Invalid Interface
        "IF,TS1,0000,DATA", // Test Case 11: Invalid Interface
		"IF1,TS1,0000,DATA,ExtraExtraExtra", // Test Case 12: Invalid MaxCmdLen
		"", // Test Case 13: Invalid Empty

    };

    for(int i =0; i < NUM_TEST; i++){
        msg.buf = testcmd[i];
        msg.len = uCsize(msg.buf);
        // Update the command object
		updateCommand(&Cmd, &msg, &rpc);
		if (Cmd.valid == 0){
			printf(RED "Test Case %d: Invalid Command: %s\n" RESET, i, testcmd[i]);
			clearCommand(&Cmd);
			continue;
		}
		// Execute the command and get the response
		if(execCommand(&Cmd, &rpc) == -1){
			printf(RED "Test Case %d: Invalid Command: %s\n" RESET, i, testcmd[i]);
			clearCommand(&Cmd);
			continue;
		}
		char response[100] = {0};
		getServiceResponse(
			getInterface(&rpc, Cmd.proto->cmdFormat[0].str.buf), 
			Cmd.proto->cmdFormat[1].str.buf,response);

		printf("Response: %s\n",response);
		printf("Return: %d\n",getServiceRet(
			getInterface(&rpc, Cmd.proto->cmdFormat[0].str.buf), 
			Cmd.proto->cmdFormat[1].str.buf));

		printf(GRN "Test Case %d: Valid Command: %s\n" RESET, i, testcmd[i]);
		char target[Cmd.proto->cmdFormat[0].maxSize];
		extractArg(target, Cmd.proto, "TRGT");
		char serviceID[Cmd.proto->cmdFormat[1].maxSize];
		extractArg(serviceID, Cmd.proto, "SRVC");
		char param[Cmd.proto->cmdFormat[2].maxSize];
		extractArg(param, Cmd.proto, "PRAM");
		char data[Cmd.proto->cmdFormat[3].maxSize];
		extractArg(data, Cmd.proto, "DATA");

		printf("| %-10s | %-10s | %-10s | %-10s |\n", "Target", "Service ID", "Param", "Data");
		printf("|------------|------------|------------|------------|\n");
		printf("| %-10s | %-10s | %-10s | %-10s |\n", target, serviceID, param, data);

		clearCommand(&Cmd);
	}
	return 0;
}