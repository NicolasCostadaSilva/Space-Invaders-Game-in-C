#ifndef TECLADO_H
#define TECLADO_H

#define KEY_SEEN     1
#define KEY_RELEASED 2

void keyboard_init();
void keyboard_update(ALLEGRO_EVENT* event);

#endif // TECLADO_H
