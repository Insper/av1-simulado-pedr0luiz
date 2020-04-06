#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void pio_init(pio_struct pio[], int n);

void pin_toggle(Pio *pio, uint32_t mask);

#endif /* FUNCTIONS_H_ */