#include "manometer.h"


int manometer_init(void) {
	/* init pins as analog inputs */
	ANSELBbits.ANSB0 = 1;
	ANSELBbits.ANSB1 = 1;
	ANSELBbits.ANSB3 = 1;
	ANSELBbits.ANSB4 = 1;
	TRISBbits.TRISB0 = 1;
	TRISBbits.TRISB1 = 1;
	TRISBbits.TRISB3 = 1;
	TRISBbits.TRISB4 = 1;

	/* init ADC calibration settings */
	ADC0CFG = DEVADC0;
	ADC1CFG = DEVADC1;
	ADC2CFG = DEVADC2;
	ADC3CFG = DEVADC3;
	ADC4CFG = DEVADC4;
	ADC7CFG = DEVADC7;

	/* configure ADCCON1, will turn on later, no features enabled */
	ADCCON1 = 0;

	/* configure ADCCON2, no settings required since we use class 1 inputs */
	ADCCON2 = 0;

	/* init warm up register, wake-up exponent = 2^5 * TADx */
	ADCANCON = 0;
	ADCANCONbits.WKUPCLKCNT = 5;

	/*
	 * clock settings:
	 * input clock source is PBCLK3
	 * control clock freq is half the input clock
	 * select AVDD and AVSS as reference source
	 */
	ADCCON3 = 0;
	ADCCON3bits.ADCSEL = 0;
	ADCCON3bits.CONCLKDIV = 1;
	ADCCON3bits.VREFSEL = 0;

	/*
	 * for all 4 ADCs, configure conversion clock to half of control
	 * clock, sampling time to 5 * TADx and resolution to 12 bits
	 */
	ADC0TIMEbits.ADCDIV = 1;
	ADC0TIMEbits.SAMC = 5;
	ADC0TIMEbits.SELRES = 3;
	ADC1TIMEbits.ADCDIV = 1;
	ADC1TIMEbits.SAMC = 5;
	ADC1TIMEbits.SELRES = 3;
	ADC3TIMEbits.ADCDIV = 1;
	ADC3TIMEbits.SAMC = 5;
	ADC3TIMEbits.SELRES = 3;
	ADC4TIMEbits.ADCDIV = 1;
	ADC4TIMEbits.SAMC = 5;
	ADC4TIMEbits.SELRES = 3;

	/* input selection for ADCs (ADCx = ANx) */
	ADCTRGMODEbits.SH0ALT = 0;
	ADCTRGMODEbits.SH1ALT = 0;
	ADCTRGMODEbits.SH3ALT = 0;
	ADCTRGMODEbits.SH4ALT = 0;

	/* ADC input mode is unsigned and single ended */
	ADCIMCON1bits.SIGN0 = 0;
	ADCIMCON1bits.DIFF0 = 0;
	ADCIMCON1bits.SIGN1 = 0;
	ADCIMCON1bits.DIFF1 = 0;
	ADCIMCON1bits.SIGN3 = 0;
	ADCIMCON1bits.DIFF3 = 0;
	ADCIMCON1bits.SIGN4 = 0;
	ADCIMCON1bits.DIFF4 = 0;

	/* configure interrupts (unused) */
	ADCGIRQEN1 = 0;
	ADCGIRQEN2 = 0;

	/* configure scanning (unused) */
	ADCCSS1 = 0;
	ADCCSS2 = 0;

	/* configure digital comparators (unused) */
	ADCCMPCON1 = 0;
	ADCCMPCON2 = 0;
	ADCCMPCON3 = 0;
	ADCCMPCON4 = 0;
	ADCCMPCON5 = 0;
	ADCCMPCON6 = 0;

	/* configure oversampling (unused) */
	ADCFLTR1 = 0;
	ADCFLTR2 = 0;
	ADCFLTR3 = 0;
	ADCFLTR4 = 0;
	ADCFLTR5 = 0;
	ADCFLTR6 = 0;

	/* setup trigger sources (edge trigger, trigger from software) */
	ADCTRGSNSbits.LVL0 = 0;
	ADCTRGSNSbits.LVL1 = 0;
	ADCTRGSNSbits.LVL3 = 0;
	ADCTRGSNSbits.LVL4 = 0;
	ADCTRG1bits.TRGSRC0 = 1;
	ADCTRG1bits.TRGSRC1 = 1;
	ADCTRG1bits.TRGSRC3 = 1;
	ADCTRG2bits.TRGSRC4 = 1;

	/* configure early interrupts (unused) */
	ADCEIEN1 = 0;
	ADCEIEN2 = 0;

	/* turn the ADC module on */
	ADCCON1bits.ON = 1;

	/* wait for voltage ref to be stable and without fault */
	while (!ADCCON2bits.BGVRRDY);
	while (ADCCON2bits.REFFLT);

	/* enable clock to analog circuit */
	ADCANCONbits.ANEN0 = 1;
	ADCANCONbits.ANEN1 = 1;
	ADCANCONbits.ANEN3 = 1;
	ADCANCONbits.ANEN4 = 1;

	/* wait for ADC to be ready */
	while (!ADCANCONbits.WKRDY0);
	while (!ADCANCONbits.WKRDY1);
	while (!ADCANCONbits.WKRDY3);
	while (!ADCANCONbits.WKRDY4);

	/* enable the ADC modules */
	ADCCON3bits.DIGEN0 = 1;
	ADCCON3bits.DIGEN1 = 1;
	ADCCON3bits.DIGEN3 = 1;
	ADCCON3bits.DIGEN4 = 1;

	return 0;
}


int manometer_read(int res[4]) {
	/* trigger conversion */
	ADCCON3bits.GSWTRG = 1;

	/* wait for conversion to complete and fetch results */
	while (ADCDSTAT1bits.ARDY0 == 0);
	res[0] = ADCDATA0;
	while (ADCDSTAT1bits.ARDY1 == 0);
	res[1] = ADCDATA1;
	while (ADCDSTAT1bits.ARDY3 == 0);
	res[2] = ADCDATA3;
	while (ADCDSTAT1bits.ARDY4 == 0);
	res[3] = ADCDATA4;

	/* TODO: process results before returning (?) */
	/* TODO: maybe trigger a conversion at the end so the function is faster? */
	/*		(if the exact microsecond the data was sampled is not important) */

	return 0;
}



