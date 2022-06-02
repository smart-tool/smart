 /*
     This is the implementation of SSM algorithm by Abdulrakeeb Al-Ssulami
     SSM V1.0 - January 10, 2015
     Citaion:
     A. M. Al-Ssulami, Hybrid string matching algorithm with a pivot, J. Inf. Sci. 41 (1) (2015) 82-88.
*/
#include "include/define.h"
#include "include/main.h"

void Horspool_Distance(int Horspool[], int Dist[], unsigned char *x, int m, int *dMax)
{
    int i;
    Dist[m]=0;
    for (i = 0; i < SIGMA; i++) Horspool[i] = m;
    for (i = 0; i < m-1; i++)
    {
        if (Horspool[x[i]] == m)
            Dist[i] = i + 1;
        else
            Dist[i] = i - (m - Horspool[x[i]] - 1);
        Horspool[x[i]] = m - i - 1;
        if (Dist[i]>Dist[m])
        {
            Dist[m]=Dist[i];
            *dMax=i;
        }
    }
}

//************************************************************************//
void PreProc(int Dist[], int Shift[], unsigned char *x, int m)
{
    /*----compute the safe shift from left to right*/
    int i, rept = 1;
    Shift[0] = 1;
    for (i = 1; i < m; i++)
    {
        if (x[i] != x[i - 1])
        {
            Shift[i] = 1;
            rept = 1;
        }
        else
        {
            rept++;
            Shift[i] = rept;
        }
    }

    /*----compute the safe shift from right to left*/
    rept = 1;
    for (i = m - 1; i > 0; i--)
    {
        if (x[i] != x[i - 1])
        {
            if (rept >= Shift[i - 1])
                Shift[i - 1] = rept + 1;
            rept = 1;
        }
        else
            rept++;
    }

    /*----Compute the max shift of all shifts----*/

    for (i = 0; i < m; i++)
        if (Shift[i] < Dist[m])
            Shift[i] = Dist[m];
}

/*-------------------------------------------------------------*/
int search(unsigned char *x, int m, unsigned char *y, int n)
{

    int count, q, j, Max, jMax, Pos;
    int Hors[SIGMA], Sht[XSIZE], Dis[XSIZE];
    unsigned char xMax;

    /*Preprocessing phase-----------*/
    BEGIN_PREPROCESSING
    Horspool_Distance(Hors, Dis, x, m, &Max);
    PreProc(Dis, Sht, x, m);
    END_PREPROCESSING

    /*Searching Phase--------------*/
    BEGIN_SEARCHING
    count = 0;
    j = m-1;
    jMax=Max-m+1;
    xMax=x[Max];
    memcpy(y+n, x, m);
    while (j < n)
    {
        while (y[j+jMax] != xMax)  j = j + Hors[y[j]];

        q=m-1;
        Pos= 1 - m + j;
        while ((q >= 0) && (x[q] == y[q + Pos]))
            q--;

        if ((q < 0) && (j<n))
         {
            count++;
            q++;
         }

        j = j + Sht[q];
    }
    END_SEARCHING
    return count;
}

