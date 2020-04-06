#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "asf.h"

typedef struct {
	Pio* pio;
	int id;
	int idx;
	int mask;
	int output;
	int on;
} pio_struct;

typedef struct  {
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t week;
	uint32_t hour;
	uint32_t minute;
	uint32_t seccond;
} calendar;

// Configs do botao 1 -> PD28
#define BTN_1_PIO PIOD
#define BTN_1_PIO_ID ID_PIOD
#define BTN_1_PIO_IDX 28
#define BTN_1_PIO_IDX_MASK (1 << BTN_1_PIO_IDX)

// Configs do botao 2 -> PC31
#define BTN_2_PIO PIOC
#define BTN_2_PIO_ID ID_PIOC
#define BTN_2_PIO_IDX 31
#define BTN_2_PIO_IDX_MASK (1 << BTN_2_PIO_IDX)

// Configs do botao 3 -> PA19
#define BTN_3_PIO PIOA
#define BTN_3_PIO_ID ID_PIOA
#define BTN_3_PIO_IDX 19
#define BTN_3_PIO_IDX_MASK (1 << BTN_3_PIO_IDX)

// LED1 PA0
#define LED1_PIO      PIOA
#define LED1_PIO_ID   ID_PIOA
#define LED1_IDX      0
#define LED1_IDX_MASK (1 << LED1_IDX)

// LED2 PC30
#define LED2_PIO      PIOC
#define LED2_PIO_ID   ID_PIOC
#define LED2_IDX      30
#define LED2_IDX_MASK (1 << LED2_IDX)

// LED3 PB2
#define LED3_PIO      PIOB
#define LED3_PIO_ID   ID_PIOB
#define LED3_IDX      2
#define LED3_IDX_MASK (1 << LED3_IDX)

#endif // CONSTANTS_H