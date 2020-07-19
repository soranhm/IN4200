#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  double t=0.0;
  while (t < 0.2) {
    t += 0.05;
    for(int it = 500; it <= 3000; it+=500){
      printf("%f, %d\n", t,it);
    }

  }
  return 0;
}
