#include <stdio.h>
#include "../include/helpers.h"

// Color codes for printing
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"


// *** HELPER FUNCTION TESTS *** //

void test_uCcpy() {
    // Test case 1: copy non-empty string
    char src1[] = "hello";
    char dest1[10];
    uCcpy(dest1, src1);
    if (uStrcmp(dest1, src1) == 0) {
        printf(GRN "uCcpy: Test case 1 passed\n" RESET);
    } else {
        printf(RED "uCcpy: Test case 1 failed\n" RESET);
    }

    // Test case 2: copy empty string
    char src2[] = "";
    char dest2[10];
    uCcpy(dest2, src2);
    if (uStrcmp(dest2, src2) == 0) {
        printf(GRN "uCcpy: Test case 2 passed\n" RESET);
    } else {
        printf(RED "uCcpy: Test case 2 failed\n" RESET);
    }

    // Test case 3: copy string to itself
    char src3[] = "hello";
    uCcpy(src3, src3);
    if (uStrcmp(src3, "hello") == 0) {
        printf(GRN "uCcpy: Test case 3 passed\n" RESET);
    } else {
        printf(RED "uCcpy: Test case 3 failed\n" RESET);
    }

    // Test case 4: copy string with null character
    char src4[] = "hello\0world";
    char dest4[15];
    uCcpy(dest4, src4);
    if (uStrcmp(dest4, src4) == 0) {
        printf(GRN "uCcpy: Test case 4 passed\n" RESET);
    } else {
        printf(RED "uCcpy: Test case 4 failed\n" RESET);
    }

    // Test case 5: copy string with insufficient buffer size
    char src5[] = "hello";
    char dest5[3];
    uCcpy(dest5, src5);
    if (uStrcmp(dest5, "he") == 0) {
        printf(GRN "uCcpy: Test case 5 passed\n" RESET);
    } else {
        printf(RED "uCcpy: Test case 5 failed\n" RESET);
    }
}

void test_uStrlen() {
    // Test case 1: empty string
    char str1[] = "";
    if (uCsize(str1) == 1) {
        printf(GRN "uCsize: Test case 1 passed\n" RESET);
    } else {
        printf(RED "uCsize: Test case 1 failed\n" RESET);
    }

    // Test case 2: non-empty string
    char str2[] = "hello";
    if (uCsize(str2) == 6) {
        printf(GRN "uCsize: Test case 2 passed\n" RESET);
    } else {
        printf(RED "uCsize: Test case 2 failed\n" RESET);
    }

    // Test case 3: string with null character
    char str3[] = "hello\0world";
    if (uCsize(str3) == 6) {
        printf(GRN "uCsize: Test case 3 passed\n" RESET);
    } else {
        printf(RED "uCsize: Test case 3 failed\n" RESET);
    }
}

void test_uStrcmp() {
    // Test case 1: strings are equal
    char str1[] = "hello";
    char str2[] = "hello";
    if (uStrcmp(str1, str2) == 0) {
        printf(GRN "uStrcmp: Test case 1 passed\n" RESET);
    } else {
        printf(RED "uStrcmp: Test case 1 failed\n" RESET);
    }

    // Test case 2: strings are not equal
    char str3[] = "hello";
    char str4[] = "worl";
    if (uStrcmp(str3, str4) == 1) {
        printf(GRN "uStrcmp: Test case 2 passed\n" RESET);
    } else {
        printf(RED "uStrcmp: Test case 2 failed\n" RESET);
    }

    // Test case 3: strings are equal up to null character
    char str5[] = "hello\0world";
    char str6[] = "hello\0world";
    if (uStrcmp(str5, str6) == 0) {
        printf(GRN "uStrcmp: Test case 3 passed\n" RESET);
    } else {
        printf(RED "uStrcmp: Test case 3 failed\n" RESET);
    }
}

void test_uCSplit() {
    // Test case 1: non-empty string
    char src1[] = "hello,world";
    char buf1[10];
    int start1 = 0;
    int next1 = uCSplit(buf1, src1, ',', start1);
    if (uStrcmp(buf1, "hello") == 0 && next1 == 6) {
        printf(GRN "uCSplit: Test case 1 passed\n" RESET);
    } else {
        printf(RED "uCSplit: Test case 1 failed\n" RESET);
    }

    // Test case 2: empty string
    char src2[] = "";
    char buf2[10];
    int start2 = 0;
    int next2 = uCSplit(buf2, src2, ',', start2);
    if (uStrcmp(buf2, "") == 0 && next2 == 1) {
        printf(GRN "uCSplit: Test case 2 passed\n" RESET);
    } else {
        printf(RED "uCSplit: Test case 2 failed\n" RESET);
    }

    // Test case 3: string with no delimiter
    char src3[] = "hello";
    char buf3[10];
    int start3 = 0;
    int next3 = uCSplit(buf3, src3, ',', start3);
    if (uStrcmp(buf3, "hello") == 0 && next3 == 6) {
        printf(GRN "uCSplit: Test case 3 passed\n" RESET);
    } else {
        printf(RED "uCSplit: Test case 3 failed\n" RESET);
    }

    // Test case 4: string with multiple delimiters
    char src4[] = "hello,world,how,are,you";
    char buf4[10];
    int start4 = 0;
    int next4 = uCSplit(buf4, src4, ',', start4);
    if (uStrcmp(buf4, "hello") == 0 && next4 == 6) {
        printf(GRN "uCSplit: Test case 4.1 passed\n" RESET);
    } else {
        printf(RED "uCSplit: Test case 4.1 failed\n" RESET);
    }
    start4 = next4;
    next4 += uCSplit(buf4, src4, ',', start4);
    if (uStrcmp(buf4, "world") == 0 && next4 == 12) {
        printf(GRN "uCSplit: Test case 4.2 passed\n" RESET);
    } else {
        printf(RED "uCSplit: Test case 4.2 failed\n" RESET);
    }
    start4 = next4;
    next4 += uCSplit(buf4, src4, ',', start4);
    if (uStrcmp(buf4, "how") == 0 && next4 == 16) {
        printf(GRN "uCSplit: Test case 4.3 passed\n" RESET);
    } else {
        printf(RED "uCSplit: Test case 4.3 failed\n" RESET);
    }
    start4 = next4;
    next4 += uCSplit(buf4, src4, ',', start4);
    if (uStrcmp(buf4, "are") == 0 && next4 == 20) {
        printf(GRN "uCSplit: Test case 4.4 passed\n" RESET);
    } else {
        printf(RED "uCSplit: Test case 4.4 failed\n" RESET);
    }
    start4 = next4;
    next4 += uCSplit(buf4, src4, ',', start4);
    if (uStrcmp(buf4, "you") == 0 && next4 == 24) {
        printf(GRN "uCSplit: Test case 4.5 passed\n" RESET);
    } else {
        printf(RED "uCSplit: Test case 4.5 failed\n" RESET);
    }
}

void test_uCTrunk(){
       // Test case 1: copy non-empty segment
    char src1[] = "hello world";
    char dest1[10];
    int len1 = uCTrunk(dest1, src1, 0, 5);
    if (uStrcmp(dest1, "hello") == 0 && len1 == 5) {
        printf(GRN "uCTrunk: Test case 1 passed\n" RESET);
    } else {
        printf(RED "uCTrunk: Test case 1 failed\n" RESET);
    }

    // Test case 2: copy empty segment
    char src2[] = "hello world";
    char dest2[10];
    int len2 = uCTrunk(dest2, src2, 5, 5);
    if (uStrcmp(dest2, "") == 0 && len2 == 0) {
        printf(GRN "uCTrunk: Test case 2 passed\n" RESET);
    } else {
        printf(RED "uCTrunk: Test case 2 failed\n" RESET);
    }

    // Test case 3: copy segment with null character
    char src3[] = "hello\0world";
    char dest3[10];
    int len3 = uCTrunk(dest3, src3, 0, 5);
    if (uStrcmp(dest3, "hello") == 0 && len3 == 5) {
        printf(GRN "uCTrunk: Test case 3 passed\n" RESET);
    } else {
        printf(RED "uCTrunk: Test case 3 failed\n" RESET);
    }

    // Test case 4: copy segment with insufficient buffer size
    char src4[] = "hello world";
    char dest4[3];
    int len4 = uCTrunk(dest4, src4, 0, 5);
    if (uStrcmp(dest4, "he") == 0 && len4 == 3) {
        printf(GRN "uCTrunk: Test case 4 passed\n" RESET);
    } else {
        printf(RED "uCTrunk: Test case 4 failed\n" RESET);
    } 

    // Test case 5: copy last segment
    char src5[] = "hello world";
    char dest5[10];
    int len5 = uCTrunk(dest5, src5, 6, 11);
    if (uStrcmp(dest5, "world") == 0 && len5 == 5) {
        printf(GRN "uCTrunk: Test case 5 passed\n" RESET);
    } else {
        printf(RED "uCTrunk: Test case 5 failed\n" RESET);
    }
}


int main() {
    test_uStrlen();
    test_uCcpy();
    test_uStrcmp();
    test_uCSplit();
    test_uCTrunk();
    return 0;
}

