/******************************************************************************
 * \file    config.h
 * \author  Aram Dergevorkian <aram@aramd.net>
 * \brief   System configuration information
 *
 * \copyright
 * TBD
 *****************************************************************************/

// CAN
#define CAN_BAUDRATE 500E3
#define CAN_BASE_ADDR 0x100

// Mesh
#define MESH_SSID "LALS_Control"
#define MESH_PASSWORD "choochoobuckaroo"
#define MESH_PORT 5555
#define BLINK_PERIOD 1000   // milliseconds until cycle repeat
#define BLINK_DURATION 100  // milliseconds LED is on for
