/******************************************************************************
 * \file    LALS_Wireless_Board_Demo.ino
 * \author  Aram Dergevorkian <aram@aramd.net>
 * \brief   Main
 *
 * \copyright
 * TBD
 *****************************************************************************/

#include <Arduino.h>
#include "board.h"
#include "config.h"
#include "esp32_can.h"

unsigned long timeout;
uint16_t selected_can_addr;

CAN_FRAME rxPacket;
CAN_FRAME txPacket;

void printFrame(CAN_FRAME *message) {
  Serial.print(message->id, HEX);
  if (message->extended) Serial.print(" X ");
  else Serial.print(" S ");
  Serial.print(message->length, DEC);
  Serial.print(" ");
  for (int i = 0; i < message->length; i++) {
    Serial.print(message->data.byte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void setup() {
  // Set up GPIO direction, pullups, etc.
  GPIO_config();

  // USB Serial
  Serial.begin(115200);

  // Setup CAN
  CAN0.setCANPins((gpio_num_t)CAN_RX_PIN, (gpio_num_t)CAN_TX_PIN);
  CAN0.begin(CAN_BAUDRATE);
  CAN0.watchFor();
}

void loop() {
  selected_can_addr = get_CAN_addr();

  if (CAN0.read(rxPacket)) {
    if (rxPacket.id == (CAN_BASE_ADDR + selected_can_addr + 3)) {
      digitalWrite(LED1_PIN, rxPacket.data.uint8[0]);
      digitalWrite(LED2_PIN, rxPacket.data.uint8[1]);
      Serial.println("Received valid packet!");
    } else {
      Serial.println("Received packet with wrong ID");
    }
    printFrame(&rxPacket);
  }

  if (millis() > timeout) {
    txPacket.rtr = 0;
    txPacket.id = CAN_BASE_ADDR + selected_can_addr + 1;
    txPacket.extended = false;
    txPacket.length = 2;
    txPacket.data.uint8[0] = 0xFF && !digitalRead(BTN1_PIN);
    txPacket.data.uint8[1] = 0xFF && !digitalRead(BTN2_PIN);
    CAN0.sendFrame(txPacket);

    timeout = millis() + (10);
  }
}
