#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
        int i = 0,j = 19,n=10;
        while(i<n ||j<20){
          if(i<j){
            printf("i = %d, j = %d\n",i,j);
            i++;
          }
          j++;

        }
        printf("i = %d, j = %d\n",i,j);
        return 0;
}
