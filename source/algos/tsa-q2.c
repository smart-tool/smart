#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include<string.h>
#include<inttypes.h>
#include "include/define.h"
#include "include/main.h"


#define Q 2
#define HS(x,i) (x[i]<<1)+x[i+1]

//searching
int search(unsigned char *P, int m, unsigned char *T, int n) {
	unsigned char PopCount[65536];
	int j,i;
	uint64_t D;
	uint64_t mask=0;
	uint64_t B[512],B1[512];

  if(m>64) return -1;
  if(m<2) return -1;

  BEGIN_PREPROCESSING
  memset(B,0,256*4);
  for (j=0; j<m-Q+1; ++j) B[HS(P,j)]|=((uint64_t)1<<(j));
  //for (j=0; j<256; ++j) B1[j] = B[j]+1;
  for (PopCount[i=0]=0; ++i<=65535;PopCount[i]=PopCount[i&(i-1)]+1);
  int count = 0;
  END_PREPROCESSING
 

  BEGIN_SEARCHING
  T[n+1]=255; //sentinel
 for (i=m-Q; i<=n-1; i+=m)
	{	
		D=B[HS(T,i)]; 
		j=1;
		while ((j<m-Q+1) && (D&=(((B[HS(T,i-j)]+1)<<j)-1)&((B[HS(T,i+j)]>>j)|((~(uint64_t)0)<<(m-j)))))
		j++; 

		count += PopCount[D&0xffff];
		if ( sizeof(D) > 2 ) {
		    count += PopCount[(D>>16)&0xffff];
		    if ( sizeof(D) > 4 ) {
                count += PopCount[(D>>32)&0xffff]; 
				if ( sizeof(D) > 6 )
					count += PopCount[(D>>48)&0xffff]; 
			}
		}

	} 
  END_SEARCHING  
  return(count); 
}


