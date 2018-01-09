#ifndef __OSS_GPIO_H__
#define __OSS_GPIO_H__

#define  OSS_GPIO_IN   0
#define  OSS_GPIO_OUT  1

#define  OSS_GPIO_LOW   0
#define  OSS_GPIO_HIGH  1


typedef	struct TagOssGpio {
        unsigned int	number;
        unsigned int	flags;
        unsigned int    value;
}oss_gpio_t;

/*
//int oss_gpio_init(oss_gpio_t *gpio, int num);

//int oss_gpio_set(oss_gpio_t *gpio, int value);

*/

int oss_gpio_init(int io);
int oss_gpio_output_mode(int io);
int oss_gpio_input_mode(int io);
int oss_gpio_pin_set(int io);
int oss_gpio_pin_clear(int io);

#endif

