/*
 * CFile1.c
 *
 * Created: 4/6/2020 4:20:57 PM
 *  Author: pedro
 */ 

#include "constants.h"
#include "functions.h"
#include "asf.h"

void pio_init(pio_struct pio[], int n){
	for(int i = 0; i < n; i++){
		pmc_enable_periph_clk(pio[i].id);
		pio_configure(pio[i].pio, pio[i].output ? pio[i].on ? PIO_OUTPUT_0 : PIO_OUTPUT_1 : PIO_INPUT, pio[i].mask, pio[i].output ? PIO_DEFAULT : PIO_PULLUP);
	}
}

void pin_toggle(Pio *pio, uint32_t mask){
	if(pio_get_output_data_status(pio, mask))
	pio_clear(pio, mask);
	else
	pio_set(pio,mask);
}