#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "pak/pak.h"

int main() {
  HYUZU_UE_Pak* pak = HYUZU_Pak_Load("amadeus.pak");
  free(pak);

  return 0;
}