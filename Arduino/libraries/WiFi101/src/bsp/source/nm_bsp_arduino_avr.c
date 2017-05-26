/**
 *
 * \file
 *
 * \brief This module contains SAMD21 BSP APIs implementation.
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifdef ARDUINO_ARCH_AVR

#include "bsp/include/nm_bsp.h"
#include "bsp/include/nm_bsp_arduino.h"
#include "common/include/nm_common.h"

#define IS_MEGA (defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560))

static tpfNmBspIsr gpfIsr;

volatile uint8_t *_receivePortRegister;
volatile uint8_t *_pcint_maskreg;
uint8_t _receiveBitMask;
volatile uint8_t prev_pin_read = 1;

uint8_t rx_pin_read()
{
  return *_receivePortRegister & _receiveBitMask;
}

#if !IS_MEGA

#if defined(PCINT0_vect)
ISR(PCINT0_vect)
{
	if (!rx_pin_read() && gpfIsr)
	{
		gpfIsr();
	}
}
#endif

#if defined(PCINT1_vect)
ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
#endif

#if defined(PCINT2_vect)
ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));
#endif

#if defined(PCINT3_vect)
ISR(PCINT3_vect, ISR_ALIASOF(PCINT0_vect));
#endif

#endif // !IS_MEGA

#if defined(TIMER4_OVF_vect)
ISR(TIMER4_OVF_vect) {
	uint8_t curr_pin_read = rx_pin_read();
	if ((curr_pin_read != prev_pin_read) && !curr_pin_read && gpfIsr)
	{
		gpfIsr();
	}
	prev_pin_read = curr_pin_read;
}

// stategy 3 - start a timer and perform a sort of polling
void attachFakeInterruptToTimer(void) {
	TCCR4B = (1<<CS41);
	TIMSK4 = (1<<TOIE4);
    OCR4C = 0xFF;
}
#else
void attachFakeInterruptToTimer(void) {
}
#endif

// strategy 1 - attach external interrupt to change pin (works on 328)
void attachInterruptToChangePin(int pin) {
	pinMode(pin, INPUT_PULLUP);
	_receiveBitMask = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	_receivePortRegister = portInputRegister(port);

	if (!digitalPinToPCICR(pin)) {
		//need to fallback to strategy 2
		attachFakeInterruptToTimer();
		return;
	}

	*digitalPinToPCICR(pin) |= _BV(digitalPinToPCICRbit(pin));
	_pcint_maskreg = digitalPinToPCMSK(pin);
	*_pcint_maskreg |= _BV(digitalPinToPCMSKbit(pin));
}

void detachInterruptToChangePin(int pin) {
    *_pcint_maskreg &= ~(_BV(digitalPinToPCMSKbit(pin)));
}

void attachInterruptMultiArch(uint32_t pin, void *chip_isr, uint32_t mode)
{
	int pin_irq;
	gpfIsr = chip_isr;

	// stategy 0 - attach external interrupt to pin (works on 32u4)
	pin_irq = digitalPinToInterrupt((int)pin);
	if (pin_irq == (int)NOT_AN_INTERRUPT) {
		attachInterruptToChangePin(pin);
		return;
	}

	attachInterrupt(pin_irq, chip_isr, mode);
	return;
}

void detachInterruptMultiArch(uint32_t pin)
{
	int pin_irq;

	pin_irq = digitalPinToInterrupt((int)pin);
	if (pin_irq == (int)NOT_AN_INTERRUPT) {
		detachInterruptToChangePin(pin);
		return;
	}

	detachInterrupt(pin_irq);
}

#endif

