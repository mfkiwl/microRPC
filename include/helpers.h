#ifndef __HELPERS_H__
#define __HELPERS_H__

// ** // *** HELPER FUNCTIONS *** // ** //

// Key Terms:
// - CSize: Size of a char array including the null terminator
// - Strlen: Length of a char array not including the null terminator
// - Trunk: Copy a segment of a char array + null terminator to a buffer
// - Split: Copy a of a char array up to a delimiter + null terminator 
// - Cpy: Copy a char array + null terminator to a buffer
// - Cmp: Compare two char arrays



unsigned int uCsize(char *str){
    //@brief: Return the size of a string 
    //@Note:  Includes the null terminator
    int len = 0;
    while(*str++ != '\0'){
        len++;
    }
    return len + 1;
}

void uCcpy(char *p,char const *q){
    //@brief: copy a string from q to p
    //@note: Buffer overflow is not checked
    while(*q != '\0'){
        *p++ = *q++;
    }
    *p = '\0';
}

int uStrcmp(char *str1, char *str2){
    //@brief: compare two strings
    //@return: 0 if equal, 1 if not equal
    while(*str1 != '\0' && *str2 != '\0'){
        if(*str1 != *str2){
            return 1;
        }
        str1++;
        str2++;
    }
    return 0;
}

int uCSplit(char *buf, char *src, char delim, int start){
    //@brief: copy a string to a buffer until a delimiter is found
    //@return: the index of the next character after the delimiter
    int j = 0;
    for(int i = start; src[i] != '\0';i++){
        if (src[i] == delim){
            break;
        }
        buf[j] = src[i];
        j++;
    }
    buf[j] = '\0';
    return j+1;
}

int uCTrunk(char *buf, const char *src, int startIdx, int endIdx){
    //@brief: copy a string segment to a buffer
    //@return: number of characters copied excluding '\0'
    //@note: Null terminatior is inserted
    //@note: buffer overflow is not checked
    
    int j = 0; // buffer index
    for(int i = startIdx; i < endIdx;i++){
        if (src[i] == '\0'){
            break;
        }
        buf[j] = src[i];
        j++;
    }
    buf[j] = '\0'; 
    return j;
}



// Hash Function for the table
unsigned int hash(char *key, int tableSize){
    int m = 31;
    unsigned int len = uCsize(key); 
    unsigned int hashValue = 0;
    for(int i = 0; i < len; i++){
        hashValue = (hashValue * m + key[i]);

    }
    return hashValue % tableSize;
}

#endif