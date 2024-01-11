#ifndef GERAL_H
#define GERAL_H

void must_init(bool teste, const char *erro);
bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2);
int between(int lo, int hi);
float between_f(float lo, float hi);

#endif // GERAL_H
