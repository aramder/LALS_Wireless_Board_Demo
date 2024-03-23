/******************************************************************************
 * \file    board.c
 * \author  Aram Dergevorkian <aram@aramd.net>
 * \brief   Board configuration parameters and functions
 *
 * \copyright
 * TBD
 *****************************************************************************/

#include "board.h"

/**
 *  @brief Configure GPIO
 *
 *  @return void
 */
void GPIO_config(void) {
  // User Interface GPIO
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);

  // CAN
  pinMode(CAN_TX_PIN, OUTPUT);
  pinMode(CAN_RX_PIN, INPUT);
  pinMode(CAN_ADDR1_PIN, INPUT_PULLUP);
  pinMode(CAN_ADDR2_PIN, INPUT_PULLUP);
  pinMode(CAN_ADDR3_PIN, INPUT_PULLUP);
  pinMode(CAN_ADDR4_PIN, INPUT_PULLUP);

  // Terminal Block GPIO
  pinMode(TERM_PIN_1_PIN, INPUT_PULLUP);
  pinMode(TERM_PIN_2_PIN, INPUT_PULLUP);
  pinMode(TERM_PIN_3_PIN, INPUT_PULLUP);
  pinMode(TERM_PIN_4_PIN, INPUT_PULLUP);
}

/**
 *  @brief Gets CAN address (including DIP configuration)
 *
 *  @return CAN address
 */
uint16_t get_CAN_addr(void) {
  uint16_t selected_can_addr = 0;
  if (digitalRead(CAN_ADDR1_PIN))
    selected_can_addr += 1;
  if (digitalRead(CAN_ADDR2_PIN))
    selected_can_addr += 2;
  if (digitalRead(CAN_ADDR3_PIN))
    selected_can_addr += 4;
  if (digitalRead(CAN_ADDR4_PIN))
    selected_can_addr += 8;
  return selected_can_addr;
}
