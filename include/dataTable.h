#ifndef __DATATABLE_H__
#define __DATATABLE_H__

#include "helpers.h"
/* Example implementation of a hash table storig config data keys and values */

// Lookup Table Definitions
#define NUM_NODES  10 //  Number of nodes in the Lookup Table
#define MAX_MEMBERS  4 // Maximum number of members per node
#define KEY_LEN  5 // Maximum length of a key





typedef struct Member{
    char key[KEY_LEN]; 
    char data[DATA_MAX_LEN]; 
} Member; // Maps Data to a key

typedef struct Node{
    char key[KEY_LEN]; 
    Member *members[MAX_MEMBERS]; 
    int count; 
} Node; // Holds Homogeneous data key links

typedef struct LookupTable{
    Node *nodes[NUM_NODES]; 
    int count; 
} LookupTable; // Holds all Data Nodes 

void initLookupTable(LookupTable *table, int numNodes){
    // Initialise the table to 0
    for(int i = 0; i < numNodes; i++){
        table->nodes[i] = 0; // Init all nodes to 0
    }
    table->count = 0;
}

void createNode(Node *node, char *key){
    uCcpy(node->key,key); // Copy to the key as we need to keep the key constant. 
    node->count = 0; // Number of members in the node
    }

int addMember(Node *node, Member *Member){
    int idx = node->count; // Next Index of the member Available
    if(idx < MAX_MEMBERS){ // Check if the node is full 
        node->members[idx] = Member; // add pointer to the member to the node
    node->count++; // Increment the count
    } else{
        return -1; // Node is full
    }
    return 0;
}

int addNode(LookupTable *table, Node *node){
    // Add a node to the table 
    if (table->count >= NUM_NODES){
        return -1; // Table is full
    }
    int index = hash(node->key,NUM_NODES); // Get the index of the node
    if (table->nodes[index] != 0){
        return -1; // Node already exists at the index
    }
    table->nodes[index] = node; // Add the node to the table
    table->count++; 
    return 0;
}

char *getMemberData(Node *node, char *key){
    // Return the pointer to the member value
    for(int i = 0; i < node->count; i++){
        if (uStrcmp(node->members[i]->key,key) == 0){
            return node->members[i]->data; // Return the data
        }
    }
    return 0; // Member does not exist
}

int setMemberData(Node *node, char *key, char *data){
    // Set the value of a member
    for(int i = 0; i < node->count; i++){
        if (uStrcmp(node->members[i]->key,key) == 0){
            uCcpy(node->members[i]->data,data); // Copy the data to the member
            return 0;
        }
    }
    return -1; // Member does not exist
}

int adjMemberData(Node *node, char *key, char *data){
    // Adjust the value of a member
    //placeholder
    if(setMemberData(node,key,data) !=0){
        return -1;// Member does not exist
    }
    return 0; 
}

Node *getNode(LookupTable *table, char *key){
    // Return the pointer to the node
    int index = hash(key,NUM_NODES);
    if (index >= NUM_NODES){
        return 0; // Node index is out of range
    }
    if (table->nodes[index] == 0){
        return 0; // Node does not exist at the index
    }
    return table->nodes[index]; 
}


#endif