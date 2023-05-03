#ifndef __HELPERS_H__
#define __HELPERS_H__

#define MSG_ARGS 4
#define DATA_MAX_LEN 15
// ** // *** HELPER FUNCTIONS *** // ** //

// copy a buffer to another buffer
void uCcpy(char *p,char const *q){
    while(*q != '\0'){
        *p++ = *q++;
    }
    *p = '\0';
}

// concantenate buffer q to p return num chars concatnated


// Get the length of a string
unsigned int uStrlen(char *str){
    int len = 0;
    while(*str++ != '\0'){
        len++;
    }
    return len;
}

// Compare two strings
int uStrcmp(char *str1, char *str2){
    while(*str1 != '\0' && *str2 != '\0'){
        if(*str1 != *str2){
            return 1;
        }
        str1++;
        str2++;
    }
    return 0;
}

// Copy a Trunkaed string interval to a buffer
int uCTrunk(char *buf, const char *src, int start, int end){
    //@breif: copy a segment of a string to a buffer
    //@param: buf - buffer to copy to
    //@param: src - string to copy from
    //@param: segment - array of two integers, the start and end of the segment
    //@return: number of characters copied
    int j = 0;
    for(int i = start; i < end;i++){
        buf[j] = src[i];
        j++;
    }
    buf[j] = '\0';
    return j;

    
 

}

// parse a string into token buf return the number of tokens
int uParse(char *src, char *tokens[DATA_MAX_LEN], char delim) {
    int i = 0, j = 0, k = 0;
    while (src[i] != '\0' && k < MSG_ARGS) {
        if (src[i] == delim) {
            tokens[k][j] = '\0';
            k++;
            j = 0;
        } else {
            tokens[k][j] = src[i];
            j++;
        }
        i++;
    }
    tokens[k][j] = '\0';
    return k + 1;
}

// Hash Function for the table
unsigned int hash(char *key, int tableSize){
    int m = 31;
    unsigned int len = uStrlen(key); 
    unsigned int hashValue = 0;
    for(int i = 0; i < len; i++){
        hashValue = (hashValue * m + key[i]);

    }
    return hashValue % tableSize;
}





#endif