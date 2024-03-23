/******************************************************************************
 * \file    board.h
 * \author  Aram Dergevorkian <aram@aramd.net>
 * \brief   Board configuration parameters and functions
 *
 * \copyright
 * TBD
 *****************************************************************************/

#include <Arduino.h>

#define BOARD_NAME "LALS Wireless Board PWA revision A"

// User Interface
#define LED1_PIN 9
#define LED2_PIN 48
#define BTN1_PIN 8
#define BTN2_PIN 47

// CAN
#define CAN_TX_PIN 21
#define CAN_RX_PIN 14
#define CAN_ADDR1_PIN 13
#define CAN_ADDR2_PIN 12
#define CAN_ADDR3_PIN 11
#define CAN_ADDR4_PIN 10

// Terminal Block GPIO
#define TERM_PIN_1_PIN 39
#define TERM_PIN_2_PIN 40
#define TERM_PIN_3_PIN 41
#define TERM_PIN_4_PIN 42

void GPIO_config(void);
uint16_t get_CAN_addr(void);
