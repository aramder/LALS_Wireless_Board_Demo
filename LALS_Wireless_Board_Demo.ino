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
#include "painlessMesh.h"

unsigned long timeout;
uint16_t selected_can_addr;

CAN_FRAME rxPacket;
CAN_FRAME txPacket;
Scheduler userScheduler;
painlessMesh mesh;

bool calc_delay = false;
SimpleList<uint32_t> nodes;
Task blinkNoNodes;
bool onFlag = false;

void sendMessage();                                                 // Prototype
Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);  // start with a one second interval

void printFrame(CAN_FRAME* message) {
  Serial.print(message->id, HEX);
  if (message->extended)
    Serial.print(" X ");
  else
    Serial.print(" S ");
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

  if (digitalRead(CAN_ADDR3_PIN)) {
    mesh.setDebugMsgTypes(ERROR | DEBUG);  // set before init() so that you can see error messages

    mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
    mesh.onNodeDelayReceived(&delayReceivedCallback);

    userScheduler.addTask(taskSendMessage);
    taskSendMessage.enable();

    blinkNoNodes.set(BLINK_PERIOD, (mesh.getNodeList().size() + 1) * 2, []() {
      // If on, switch off, else switch on
      if (onFlag)
        onFlag = false;
      else
        onFlag = true;
      blinkNoNodes.delay(BLINK_DURATION);

      if (blinkNoNodes.isLastIteration()) {
        // Finished blinking. Reset task for next run
        // blink number of nodes (including this node) times
        blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
        // Calculate delay based on current mesh time and BLINK_PERIOD
        // This results in blinks between nodes being synced
        blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD * 1000)) / 1000);
      }
    });
    userScheduler.addTask(blinkNoNodes);
    blinkNoNodes.enable();

    randomSeed(analogRead(A0));
  }
}

void loop() {
  if (digitalRead(CAN_ADDR4_PIN)) {  // Basic board demo
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, HIGH);
    if (!digitalRead(BTN1_PIN)) {
      delay(200);

    } else if (!digitalRead(BTN2_PIN)) {
      delay(100);

    } else {
      delay(1000);
    }
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
    if (!digitalRead(BTN1_PIN)) {
      delay(200);

    } else if (!digitalRead(BTN2_PIN)) {
      delay(100);

    } else {
      delay(1000);
    }
  } else if (digitalRead(CAN_ADDR3_PIN)) {  // Wireless demo
    mesh.update();
    digitalWrite(LED2_PIN, onFlag);
  } else {  // CAN bus demo
    selected_can_addr = get_CAN_addr();

    if (CAN0.read(rxPacket)) {
      if (rxPacket.id == (CAN_BASE_ADDR + selected_can_addr + (selected_can_addr == 0 ? 3 : 0))) {
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
}

void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  msg += " myFreeMemory: " + String(ESP.getFreeHeap());
  mesh.sendBroadcast(msg);

  if (calc_delay) {
    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
      mesh.startDelayMeas(*node);
      node++;
    }
    calc_delay = false;
  }

  Serial.printf("Sending message: %s\n", msg.c_str());

  taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));  // between 1 and 5 seconds
}


void receivedCallback(uint32_t from, String& msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
  // Reset blink task
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD * 1000)) / 1000);

  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  // Reset blink task
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD * 1000)) / 1000);

  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
