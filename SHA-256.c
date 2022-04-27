#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Some global contants..
const long long int mod = (1LL) << 32;
#define BYTES_IN_BLOCK 64
#define FINAL_PADDING_LENGTH_IN_BYTES 8
#define BYTES_IN_WORD 4

// We need to initialize the default values for eight buffers to be used in the rounds as follows:

/******** Global Variables *********/
const unsigned int H0_1 = 0x6a09e667;
const unsigned int H0_2 = 0xbb67ae85;
const unsigned int H0_3 = 0x3c6ef372;
const unsigned int H0_4 = 0xa54ff53a;
const unsigned int H0_5 = 0x510e527f;
const unsigned int H0_6 = 0x9b05688c;
const unsigned int H0_7 = 0x1f83d9ab;
const unsigned int H0_8 = 0x5be0cd19;

// You also need to store 64 different keys in an array, ranging from K[0] to K[63]. They are initialized as follows

unsigned int K[] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};


/*********  Some helping functions *********/
unsigned int min(unsigned int a, unsigned int b) { return (a < b) ? a : b; }
unsigned int max(unsigned int a, unsigned int b) { return (a > b) ? a : b; }
unsigned int clock(unsigned int a, unsigned int b) { return ((a << b) + (a >> (32u - b))); }
unsigned int anticlock(unsigned int a, unsigned int b) { return ((a >> b) + (a << (32u - b))); }

// Some important functions we will be using throughout the calculations of the hash function.

unsigned int Ch(unsigned int x, unsigned int y, unsigned int z)
{
    unsigned int ch = (x & y) ^ ((~x) & z);
    return ch;
}

unsigned int Maj(unsigned int x, unsigned int y, unsigned int z)
{
    unsigned int maj = (x & y) ^ (x & z) ^ (y & z);
    return maj;
}

unsigned int CapitalSigma0(unsigned int x)
{
    unsigned int capitalSigma0 = anticlock(x, 2) ^ anticlock(x, 13) ^ anticlock(x, 22);
    return capitalSigma0;
}

unsigned int CapitalSigma1(unsigned int x)
{
    unsigned int capitalSigma1 = anticlock(x, 6) ^ anticlock(x, 11) ^ anticlock(x, 25);
    return capitalSigma1;
}

unsigned int *memory_allocation(char *input_string, unsigned int *sz_of_mess) // to allocate memory for message array
{
    unsigned int *return_value; // used to return memory allocated
    unsigned int n = strlen(input_string);
    unsigned int a = (n / 4) + min(1, n % 4);
    unsigned int b = ((a / 16) + min(1, a % 16)) * 16;
    if (b - a == 1)
        b += 16;         // It is because we are storing the data of input_string in 64 bits less than multiple of 512
                         // So two integers(= 64 bit ) must be left (for storing the length of the string)
                         // If not then add 16 more integers to maintain it the multiple of 512 bits (= 16 integers)
    unsigned int sz = b; // size of memory that is going to be allocated
    *sz_of_mess = sz;
    return_value = (unsigned int *)malloc(sizeof(unsigned int) * sz); // memory allocation
    for (unsigned int i = 0; i < *sz_of_mess; i++)
        return_value[i] = 0;
    return return_value;
}

char *input() // To input the message and store it in the string
{
    char ch;
    char arr[1000000]; //  for storing the message into character array
    unsigned int i = 0;
    ch = getchar();
    while (ch != EOF && ch != '\n')
    {
        arr[i++] = ch;
        ch = getchar();
    }
    arr[i] = '\0';
    char *result = arr; // The string now points to character array
    return result;
}

void extraction(char *input_string, unsigned int *message, unsigned int sz_of_mess) // extracting binary data from input string
{
    long long int n = strlen(input_string);
    n = n * 8;
    unsigned int a = n >> 32;
    unsigned int b = (n << 32) >> 32;
    n /= 8;
    message[sz_of_mess - 1] = b;
    message[sz_of_mess - 2] = a;
    unsigned int j = 0;
    unsigned int i = 0;
    for (; i + 3 < n; i = i + 4)
    {
        unsigned int var = input_string[i + 3];
        var += input_string[i + 2] << 8u;
        var += input_string[i + 1] << 16u;
        var += input_string[i] << 24u;
        message[j++] = var;
    }
    unsigned int var = 0;
    if (i < n)
        var += input_string[i] << 24u;
    else
    {
        var += 1u << 31u;
        message[j++] = var;
        return;
    }
    if (i + 1 < n)
        var += input_string[i + 1] << 16u;
    else
    {
        var += 1 << 23u;
        message[j++] = var;
        return;
    }
    if (i + 2 < n)
    {
        var += input_string[i + 2] << 8u;
        var += 1u << 7u;
    }
    else
    {
        var += 1u << 15u;
        message[j++] = var;
        return;
    }
    message[j++] = var;
}

// While the value of K[i] in all those rounds is pre-initialized, W[i] is another input that is calculated individually
//  for each block, depending on the number of iterations being processed at the moment.
void ExpandedMessageBlocks(unsigned int *B, unsigned int *W)
{

    for (int j = 0; j < 16; j++)
        W[j] = B[j];
    for (int k = 16; k <= 63; k++)
    {
        unsigned int s0 = anticlock(W[k - 15], 7) ^ anticlock(W[k - 15], 18) ^ (W[k - 15] >> 3);
        unsigned int s1 = anticlock(W[k - 2], 17) ^ anticlock(W[k - 2], 19) ^ (W[k - 2] >> 10);
        W[k] = W[k - 16] + s0 + s1 + W[k - 7];
    }
}

// Our main function which will step by step calculate and update Hi_j's.

void Compute(char *input_string, const int hexstringlen, unsigned long *hash)
{
    // vars for the main loop after preprocessing step given below.
    unsigned long a, b, c, d, e, f, g, h;
    unsigned long Hi_1, Hi_2, Hi_3, Hi_4, Hi_5, Hi_6, Hi_7, Hi_8;
    unsigned int W[64];
    unsigned int *B;
    unsigned int T1, T2;
    unsigned int sz_of_B;

    /** Prep for Main Loop ***/
    Hi_1 = H0_1;
    Hi_2 = H0_2;
    Hi_3 = H0_3;
    Hi_4 = H0_4;
    Hi_5 = H0_5;
    Hi_6 = H0_6;
    Hi_7 = H0_7;
    Hi_8 = H0_8;
   
    for (int i = 0; i < 64; i++)
    {
        W[i] = 0;   // initialising W with 0;
    }

    B = memory_allocation(input_string, &sz_of_B);
    extraction(input_string, B, sz_of_B);

    /** Main Loop ***/
    // process each block one by one
    for (int i = 0; i < sz_of_B / 16; i++, B += 16)
    {
        a = Hi_1;
        b = Hi_2;
        c = Hi_3;
        d = Hi_4;
        e = Hi_5;
        f = Hi_6;
        g = Hi_7;
        h = Hi_8;

        ExpandedMessageBlocks(B, W);

        for (int j = 0; j < 64; j++)
        {   
            // 
            T1 = ((((h % mod + CapitalSigma1(e) % mod) % mod + Ch(e, f, g)) % mod + K[j]) % mod + W[j]) % mod;
            T2 = (CapitalSigma0(a) % mod + Maj(a, b, c) % mod) % mod;
            h = g;
            g = f;
            f = e;
            e = (d % mod + T1 % mod) % mod;
            d = c;
            c = b;
            b = a;
            a = (T1 % mod + T2 % mod) % mod;
        }

    // For each iteration updating the Hi_j's by adding a,b,c... to Hi_1,Hi_2..... 
        Hi_1 = (Hi_1 % mod + a % mod) % mod;
        Hi_2 = (Hi_2 % mod + b % mod) % mod;
        Hi_3 = (Hi_3 % mod + c % mod) % mod;
        Hi_4 = (Hi_4 % mod + d % mod) % mod;
        Hi_5 = (Hi_5 % mod + e % mod) % mod;
        Hi_6 = (Hi_6 % mod + f % mod) % mod;
        Hi_7 = (Hi_7 % mod + g % mod) % mod;
        Hi_8 = (Hi_8 % mod + h % mod) % mod;
    }

    // copying the address of Hi_j's in hash array which will form our final answer.
    memcpy(hash, &Hi_1, sizeof(unsigned long));
    hash++;
    memcpy(hash, &Hi_2, sizeof(unsigned long));
    hash++;
    memcpy(hash, &Hi_3, sizeof(unsigned long));
    hash++;
    memcpy(hash, &Hi_4, sizeof(unsigned long));
    hash++;
    memcpy(hash, &Hi_5, sizeof(unsigned long));
    hash++;
    memcpy(hash, &Hi_6, sizeof(unsigned long));
    hash++;
    memcpy(hash, &Hi_7, sizeof(unsigned long));
    hash++;
    memcpy(hash, &Hi_8, sizeof(unsigned long));
}

/******** MAIN ********/
int main()
{
       freopen("C:\\Users\\HP\\Desktop\\CS201 PROJECT\\input.txt", "r", stdin);
       freopen("C:\\Users\\HP\\Desktop\\CS201 PROJECT\\output.txt", "w", stdout);
 
    unsigned long hash[8];
    int i;
    // printf("Enter the string to hash: ");
    char *str = input();
    Compute(str, strlen(str), hash);
    // printf("The SHA 256 hash is: ");

    // Printing the final hash value.
    for (i = 0; i < 8; i++)
    {
        printf("%08x", hash[i]);
    }
    return 0;
}