
#pragma once

#define AIRMOUSE_CALIBRATION_ADDR   (0x18000)
#define	MPU6050_I2C_ID				(0x68 << 1)

typedef struct {
	s8 btn;
	s8 x;
	s8 y;
	s8 w;
}mouse_data_t;

void airmouse_powerup(void);
void airmouse_powerdown(void);

int airmouse_getxy(mouse_data_t *mouse_data);

