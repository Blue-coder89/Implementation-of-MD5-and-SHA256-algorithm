#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

//functions to rotate the bits of a number.

#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))
unsigned func0(unsigned a,unsigned b,unsigned c){
		return (a&b)|((~a)&c);
}
unsigned func1(unsigned a,unsigned b,unsigned c){
		return (c&a)|((~c)&b);
}
unsigned func2(unsigned a,unsigned b,unsigned c){
		return a^b^c;
}
unsigned func3(unsigned a,unsigned b,unsigned c){
		return b^(a|(~c));
}

uint32_t h0,h1,h2,h3; // this 32 bit int will contain the final hash value.

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
void md5(char *msg, int mlen)
	{
		//*********** PREPROCESSING STARTS********************//

		unsigned int ho[4]  = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476}; //intial buffer values 
		uint32_t r[64] = 													   // r specifies per round rotation shifts
					{7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
		uint32_t k[] = 
					{0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
					0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
					0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
					0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
					0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
					0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
					0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
					0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
					0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
					0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
					0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
					0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
					0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
					0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
					0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
					0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
		
		int grps = 1 + (mlen+8)/64; // gprs = no of 512 bits blocks in the message	
		h0 =  ho[0];h1 = ho[1];h2 = ho[2];h3 = ho[3];
		
		unsigned char *c_msg;  // contains the copy of message
		c_msg = (unsigned char *)malloc(64*grps);
		memcpy(c_msg,msg,mlen);
		c_msg[mlen] = (unsigned char)0x80; //started paddding with 10000000
		int i = mlen+1;
		for(;i<64*grps;i++)
			c_msg[i]  = 0;     // padding with 0 while there is space
		uint32_t sizeofmsg = 8*mlen;
		i-=8; //  moving pointer i 64 bits to make space for sizeofmessage 
		memcpy(c_msg+i,&sizeofmsg,4);
		//printf("%s \n",c_msg);

		//*********** PREPROCESSING ENDS ********************//

		//*********** HASH_COMPUTATION  STARTS ********************//

		for(i=0;i<grps;i++)
			{
				uint32_t *p_msg; //  it contains the data of msg to be processed
				p_msg =	(uint32_t *) (c_msg + i*64); // copies the data of ith 512 block to  p_msg
				//for(int j =0; j < 64; j++) printf("%x ", ((uint8_t *) c_msg)[j]);
				ho[0] = h0; ho[1] = h1; ho[2]= h2; ho[3] = h3;
				
 				for(int j=0;j<64;j++)
					{
						uint32_t f,g;
						if(j<16)
							{
								f = func0(ho[1],ho[2],ho[3]);
								g =j;
							} 
						else if(j<32)
							{
								f = func1(ho[1],ho[2],ho[3]);
								g= (5*j+1)%16;
							}
						else if(j<48)
							{
								f = func2(ho[1],ho[2],ho[3]);
								g =  (3*j + 5) % 16;
							}
						else 
							{
								f = func3(ho[1],ho[2],ho[3]);
								g = (7*j) % 16;
							}
						
						uint32_t temp = ho[3];
						ho[3] = ho[2];
						ho[2] = ho[1];				
						ho[1] += LEFTROTATE((ho[0]+f+k[j]+p_msg[g]),r[j]);
						ho[0] = temp;
					}
				h0 += ho[0]; // the variable h0,h1,h2,h3 is updated after very 512 block is processed.
				h1 += ho[1];
				h2 += ho[2];
				h3 += ho[3];

			}
			// after the final iteration h0,h1,h2,h3 contains the final output 

	}

		//*********** HASH_COMPUTATION  ENDS ********************//
	


 int main(){
	 
	freopen("input.txt", "r", stdin);
    freopen("output1.txt", "w", stdout);	
	char *msg = input(); // input message
	printf("Input size is %d bytes \n",strlen(msg));
 	md5(msg,strlen(msg));
	uint8_t *p;
    p=(uint8_t *) &h0;	
    printf("%2.2x%2.2x%2.2x%2.2x", p[0], p[1], p[2], p[3], h0);
    p=(uint8_t *)&h1;
    printf("%2.2x%2.2x%2.2x%2.2x", p[0], p[1], p[2], p[3], h1);
    p=(uint8_t *)&h2;
    printf("%2.2x%2.2x%2.2x%2.2x", p[0], p[1], p[2], p[3], h2);
    p=(uint8_t *)&h3;
    printf("%2.2x%2.2x%2.2x%2.2x", p[0], p[1], p[2], p[3], h3);
    puts("");
 }