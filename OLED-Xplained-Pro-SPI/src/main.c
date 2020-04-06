#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"
#include "constants.h"
#include "functions.h"

volatile char led1;
volatile char led2;
volatile char led3;
volatile char btn1;
volatile char btn2;
volatile char btn3;
volatile char flag_rtt;
volatile char pisca;
volatile char escreve;
volatile uint32_t rtt_time;
volatile char flag_rtc_sec;

void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	pmc_enable_periph_clk(ID_TC);
	
	tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

	NVIC_EnableIRQ((IRQn_Type) ID_TC);
	tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);

	tc_start(TC, TC_CHANNEL);
}

void TC0_Handler(void){
	volatile uint32_t ul_dummy;

	ul_dummy = tc_get_status(TC0, 0);

	UNUSED(ul_dummy);

	led1 = 1;
}void TC1_Handler(void){
	volatile uint32_t ul_dummy;

	ul_dummy = tc_get_status(TC0, 1);

	UNUSED(ul_dummy);

	led2 = 1;
}void TC2_Handler(void){
	volatile uint32_t ul_dummy;

	ul_dummy = tc_get_status(TC0, 2);

	UNUSED(ul_dummy);

	led3 = 1;
	escreve = 1;
}void btn1_callback(){
	btn1 = !btn1;
}void btn2_callback(){
	btn2 = !btn2;
}void btn3_callback(){
	btn3 = !btn3;
}static void RTT_init(uint16_t pllPreScale, uint32_t IrqNPulses){
	uint32_t ul_previous_time;

	/* Configure RTT for a 1 second tick interrupt */
	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);
	
	ul_previous_time = rtt_read_timer_value(RTT);
	while (ul_previous_time == rtt_read_timer_value(RTT));
	
	rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, RTT_MR_ALMIEN);
}

void RTT_Handler(void){
	uint32_t ul_status;

	ul_status = rtt_get_status(RTT);

	// 	/* IRQ due to Time has changed */
 	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) { 
 		rtt_time = ul_status;
 	 }

	/* IRQ due to Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		flag_rtt = 1;
	}
}void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type){
	pmc_enable_periph_clk(ID_RTC);

	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(rtc, 0);

	/* Configura data e hora manualmente */
	rtc_set_date(rtc, t.year, t.month, t.day, t.week);
	rtc_set_time(rtc, t.hour, t.minute, t.seccond);

	/* Configure RTC interrupts */
	NVIC_DisableIRQ(id_rtc);
	NVIC_ClearPendingIRQ(id_rtc);
	NVIC_SetPriority(id_rtc, 0);
	NVIC_EnableIRQ(id_rtc);

	/* Ativa interrupcao via alarme */
	rtc_enable_interrupt(rtc,  irq_type);
}

void RTC_Handler(void){
	uint32_t ul_status = rtc_get_status(RTC);

	/*
	*  Verifica por qual motivo entrou
	*  na interrupcao, se foi por segundo
	*  ou Alarm
	*/
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		flag_rtc_sec = 1;
		rtc_clear_status(RTC, RTC_SCCR_SECCLR);
	}
	
	/* Time or date alarm */
	if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {}
	
	rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
	rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
	rtc_clear_status(RTC, RTC_SCCR_CALCLR);
	rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);
}

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();
	
	pio_struct led1_pio = {LED1_PIO, LED1_PIO_ID, LED1_IDX, LED1_IDX_MASK, 1, 1};
	pio_struct led2_pio = {LED2_PIO, LED2_PIO_ID, LED2_IDX, LED2_IDX_MASK, 1, 0};
	pio_struct led3_pio = {LED3_PIO, LED3_PIO_ID, LED3_IDX, LED3_IDX_MASK, 1, 1};
	pio_struct btn_1_pio = {BTN_1_PIO, BTN_1_PIO_ID, BTN_1_PIO_ID, BTN_1_PIO_IDX_MASK, 0, 0};
	pio_struct btn_2_pio = {BTN_2_PIO, BTN_2_PIO_ID, BTN_2_PIO_IDX, BTN_2_PIO_IDX_MASK, 0, 0};
	pio_struct btn_3_pio = {BTN_3_PIO, BTN_3_PIO_ID, BTN_3_PIO_IDX, BTN_3_PIO_IDX_MASK, 0, 0};
		
	pio_struct pios[] = {led1_pio, led2_pio, led3_pio, btn_1_pio, btn_2_pio, btn_3_pio};
		
	pio_init(pios, 6);
	
	pio_handler_set(BTN_1_PIO, BTN_1_PIO_ID, BTN_1_PIO_IDX_MASK, PIO_IT_RISE_EDGE, btn1_callback);
	pio_enable_interrupt(BTN_1_PIO, BTN_1_PIO_IDX_MASK);
	NVIC_EnableIRQ(BTN_1_PIO_ID);
	NVIC_SetPriority(BTN_1_PIO_ID, 4);
		
	pio_handler_set(BTN_2_PIO, BTN_2_PIO_ID, BTN_2_PIO_IDX_MASK, PIO_IT_FALL_EDGE, btn2_callback);
	pio_enable_interrupt(BTN_2_PIO, BTN_2_PIO_IDX_MASK);
	NVIC_EnableIRQ(BTN_2_PIO_ID);
	NVIC_SetPriority(BTN_2_PIO_ID, 4);
		
	pio_handler_set(BTN_3_PIO, BTN_3_PIO_ID, BTN_3_PIO_IDX_MASK, PIO_IT_RISE_EDGE, btn3_callback);
	pio_enable_interrupt(BTN_3_PIO, BTN_3_PIO_IDX_MASK);
	NVIC_EnableIRQ(BTN_3_PIO_ID);
	NVIC_SetPriority(BTN_3_PIO_ID, 4);
	
	led1 = 0;
	led2 = 0;
	led3 = 0;
	btn1 = 1;
	btn2 = 1;
	btn3 = 1;
	flag_rtt = 1;
	pisca = 0;
	escreve = 0;
	int qtd = 0;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	char timeString[512];
	
	
	TC_init(TC0, ID_TC0, 0, 5);
	TC_init(TC0, ID_TC1, 1, 10);
	TC_init(TC0, ID_TC2, 2, 1);
	
	calendar rtc_initial = {2018, 3, 19, 12, 15, 45 ,1};
	RTC_init(RTC, ID_RTC, rtc_initial, RTC_SR_SEC);

  // Init OLED
	gfx_mono_ssd1306_init();
  
  // Escreve na tela um circulo e um texto
	gfx_mono_draw_string("5.10.1", 0,0, &sysfont);

  /* Insert application code here, after the board has been initialized. */
	while(1) {
		if(led1 && btn1 && pisca){
			pin_toggle(LED1_PIO, LED1_IDX_MASK);
			led1 = 0;
		}
		if(led2 && btn2 && pisca){
			pin_toggle(LED2_PIO, LED2_IDX_MASK);
			led2 = 0;
		}
		if(led3 && btn3 && pisca){
			pin_toggle(LED3_PIO, LED3_IDX_MASK);
			led3 = 0;
		}
		if (flag_rtt){
			uint16_t pllPreScale = (int) (((float) 32768) / 4.0);
			uint32_t irqRTTvalue = 20;

			RTT_init(pllPreScale, irqRTTvalue);
			pisca = !pisca;
			flag_rtt = 0;
		}
		if(escreve){
			if(qtd == 5){
				qtd = 0;
				gfx_mono_draw_string("", 0, 16, &sysfont);
			}
			qtd += 1;
			char palavra[5] = "     ";
			for(int i = 0; i < qtd; i++){
				palavra[i] = '1';
			}
			char contador[40];
			sprintf(contador, "%d", rtt_time);
			gfx_mono_draw_string(palavra, 0, 16, &sysfont);
			escreve = 0;
		}
		if(flag_rtc_sec){
			rtc_get_time(RTC, &hour, &minute, &second);
			sprintf(timeString, "%2d:%2d:%2d", hour, minute, second);
			gfx_mono_draw_string(timeString, 50, 0, &sysfont);
			flag_rtc_sec = 0;
		}
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}
