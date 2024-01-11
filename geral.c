#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include "geral.h"

int between(int lo, int hi)
{
    return lo + (rand() % (hi - lo));
}

float between_f(float lo, float hi)
{
    return lo + ((float)rand() / (float)RAND_MAX) * (hi - lo);
}

void must_init(bool teste, const char *erro) {
    if(teste) 
        return;

    printf("nn foi possivel inicializar %s\n", erro);
    exit(1);
}

bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2) {
    
    if(ax1 > bx2) return false;
    if(ax2 < bx1) return false;
    if(ay1 > by2) return false;
    if(ay2 < by1) return false;

    return true;
}