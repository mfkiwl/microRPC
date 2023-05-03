#ifndef UCOMMANDER_H
#define UCOMMANDER_H
/*
MAX SIZES Defentions - Thsi is needed for static allocation
*/

#define MAX_CMDLEN 28
#define MAX_ARGS 5
#define MAX_SERVICES 10
#define MAX_INTERFACES 10
#define MAX_PROTOCOLS 2
#define MAX_RESPONSE 10
#define MAX_ID 5

#define TARGET_ARG 1
#define TARGET_LEN 4

typedef struct String{
    char *buf; // Pointer to the string buffer
    int len;
}String;

typedef struct CmdArg{
    char id[MAX_ID]; // ID of the argument
    enum {CHAR,INT,STR} type; // Type of the argument
    String str;
    int maxLen; // Max len excluding '/0'
}CmdArg; // Defines the format of an Arguemtn in a command

typedef struct Protocol{
    /// @brief Defines the protocol for the TASK interface
    /// REQ: |INTERFACE|ServiceKey|CommandMessage|
    int numArgs; 
    int maxCmdLen;
    CmdArg cmdFormat[MAX_ARGS];
    char delim;
}Protocol; 

typedef struct Command{
    Protocol *proto; // The protocol of the command
    char *msg; // The message of the command
    int len; // Length of the command
    int valid; // Flag to determine Validate of last command received
}Command; // Services Respond to Commands

typedef int (*ServiceFunc)(Command *cmd,char *response, void *data); // A function that can be called by a client

typedef struct Service{
    char key[MAX_ID]; // Key of the service
    char *desc; // Description of the service
    ServiceFunc func; // pointer to the service function
    char response[MAX_RESPONSE]; // Last response of the service
    int ret; // Last return value of the service
}Service; // An executable function that can be called by a client

typedef struct Interface{
    char id[MAX_ID]; // Name of the interface
    Protocol *proto; // Protocol of the interface
    Service *services[MAX_SERVICES]; // List of services
    int count; // Number of services
    void *data; // Pointer to interface data
} Interface; // A list of services that can be called by a client

typedef struct RPC{
    Interface *interfaces[MAX_INTERFACES]; // List of interfaces
    char trgtServiceKey[MAX_ID]; // target service key
    int count; // Number of interfaces
}RPC; // A list of interfaces that can be called by a client


// *** // Set up functions // *** //

/* @brief Initialize the Remote Procedure Call Object
*/
void initRPC(RPC *rpc); 

/* @brief Initialize the Protocol
*/
void initProtocol(Protocol *proto);

/* @brief: Create an interface with a protocol and id
    @param: interface - pointer to the interface to initialize
    @param: id - id of the interface
    @param: proto - pointer to the protocol of the interface
    @Note: Each interface has a unique id
    @Note: Multiple interfaces can have the same protocol
*/
void createInterface(Interface *interface, char *id, Protocol *proto, void *data);

/* @brief: Add an interface to the RPC
    @param: rpc - pointer to the RPC
    @param: interface - pointer to the interface to add
    @Note: Each interface has a unique id
    @Return: 0 if successful, -1 if failed

*/
int addInterface(RPC *rpc, Interface *interface);   

// ***  // Internal Functions // *** //

// @brief Get an interface from the RPC
// @param: rpc - pointer to the RPC
// @param: id - id of the interface to get
// @return: pointer to the interface on success, 0 on failure
Interface *getInterface(RPC *rpc, char *key); 

//@brief: Add a service to an interface
//@param: interface - pointer to the interface
//@param: service - pointer to the service to add
//@return: 0 on success, -1 on failure
int registerService(Interface *interface, Service *Service); 

// @brief Get a service from an interface
// @param: interface - pointer to the interface
// @param: key - key of the service to get
// @return: pointer to the service on success, 0 on failure
Service *getService(Interface *interface, char *key);

//@brief: Searches for the Protocol of the Target Interface in the message
//@Note: Requirers Message to be in microRPC format:
// <targetId> <ServiceId> <arg1> .... <argN>
// @Return: Pointer to the protocol on success, 0 on failure
Protocol *findProtocol(RPC *rpc, String *msgCmd); 

//@Brief: Validate the arguments of a message against a Commands protocol
//@param: cmdFormat - pointer to the command format array of the Commands protocol
//@param: msgCmd - pointer to the message
//@param: proto - pointer to the protocol
//@return: 0 on success, -1 on failure
int updateArguments(CmdArg *cmdFormat, String *msgCmd, Protocol *proto); // Check is a message Matches a services Protocol Format

// *** // *** Exposed functions *** // *** //


// @brief Update the command Object with a new message link 
// @param: cmd - pointer to the command object
// @param: msgCmd - pointer to the message
// @param: rpc - pointer to the RPC
//@Note: Message is required to have the following format:
//<targetId> <serviceId> <arg1> <arg2> ... <argN>
int updateCommand(Command *cmd, String *msgCmd, RPC *rpc);

//@brief: Execute a command on the RPC
//@param: cmd - pointer to the command object
//@param: rpc - pointer to the RPC 
//@Note: Blocking unless target service uses async
int execCommand(Command *cmd, RPC *rpc);

//@brief: Copies extract of the argument string from the command by id
//@Note: Command Args Strings are not null terminated and employ zero copy as such 
//@Note: This is needed as a work around for Dynamic allocation of arguemtent strings
int extractArg(char *arg, Protocol *proto, char *argId);

//@brief: Get the last response of a service
//@param: interface - pointer to the interface
//@param: key - key of the service
//@param: response - pointer to the response buffer
//@return: 0 on success, -1 on failure
int getServiceResponse(Interface *interface, char *key, char *response); // Gets the last response of a service

//@brief: Get the last return value of a service
//@param: interface - pointer to the interface
//@param: key - key of the service
//@return: return value of the service
int getServiceRet(Interface *interface, char *key); // Gets the last return value of a service

int serializeProtocol(Protocol *proto, String *msg); // Serializes a protocol into a message for sending to a client

int publishInterface(Interface *interface, String *msg); // Serializes an interface into a message for sending to a client


#endif 


