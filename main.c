/**
 * @file
 * @ingroup ??
 * @author Diego Badillo <diego.badillo-san-juan@inria.fr>
 * @brief This is a short example of how to get measurements from the rotary encoder AS5048B.
 *
 * @copyright Inria, 2024
 *
 */
#include <nrf.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "board_config.h"
#include "i2c.h"
#include "timer_hf.h"
#include "gpio.h"

//=========================== defines ==========================================

#define SLAVE_ADDRESS_REG       (0x15)
#define AS5048B_SLAVE_ADDRESS   (0x40)

#define AS5048B_ANGLE_0         (0xFE)
#define AS5048B_ANGLE_1         (0xFF)
// 14 bits in total, 6-13(MSB) and 0-5(LSB) respectively

// define SCL and SDA gpio pins
static const gpio_t SCL = { .port = 0, .pin = 26 };  // P0.09
static const gpio_t SDA = { .port = 0, .pin = 27 }; // P0.10

// define pi
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

double convertToRadians(uint16_t angle) {
    // convert the angle to radians [0, 2pi)
    // magic number is to exclude 2pi
    double radians = ( (double)angle / (double)0x3FFF ) * ( 2 * M_PI ) * (0.999938961118232313984);
    return radians;
}

double convertToDegrees(uint16_t angle) {
    // convert the angle to radians [0, 2pi)
    // magic number is to exclude 2pi
    int degrees = (int)(( (double)angle / (double)0x3FFF ) * 360. * (0.999938961118232313984));
    return degrees;
}

//=========================== main =============================================

/**
 *  @brief The program starts executing here.
 */
int main(void) {
    db_board_init();
    db_timer_hf_init();
    db_i2c_init(&SCL, &SDA);

    // db_i2c_begin();
    // uint8_t who_am_i;
    // db_i2c_read_regs(AS5048B_SLAVE_ADDRESS, SLAVE_ADDRESS_REG, &who_am_i, 1);
    // printf("SLAVE_ADDRESS_REG value: 0x%x\n", who_am_i);
    // db_i2c_end();

    uint8_t data[2];
    uint16_t angle_int;
    // double angle_radians;
    int angle_degrees;

    // read encoder data in a loop
    while (1) {
        db_i2c_begin();
        db_i2c_read_regs(AS5048B_SLAVE_ADDRESS, AS5048B_ANGLE_0, &data[0], 1);
        db_i2c_read_regs(AS5048B_SLAVE_ADDRESS, AS5048B_ANGLE_1, &data[1], 1);
        db_i2c_end();

        // combine MSB and LSB to get the angle value
        angle_int = (data[0] << 6) | data[1];
        // angle_radians = convertToRadians(angle_int); 
        angle_degrees = convertToDegrees(angle_int); 

        // SEGGER doesn't support float prints
        printf("angle_int: %i, angle_degrees: %i\n", angle_int, angle_degrees);
        db_timer_hf_delay_ms(200);
    }

    // one last instruction, doesn't do anything, it's just to have a place to put a breakpoint.
    __NOP();
}
