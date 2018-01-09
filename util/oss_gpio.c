#include "oss.h"
#include "oss_gpio.h"

#if 0
///////////////////////////////////////////////////////////////
int oss_gpio_init(oss_gpio_t *gpio, int num)
{
	int loop;
	unsigned char cmd[128];
	
	for(loop = 0; loop < num ; loop++)
	{
		// export gpio
		memset(cmd, 0, 128);
		sprintf(cmd, "echo %d > /sys/class/gpio/export", gpio[loop].number);
		system(cmd);

		//exist ?
		
		// setup out/in
		if(gpio->flags == OSS_GPIO_OUT)
		{
			memset(cmd, 0, 128);
			sprintf(cmd, "echo %s > /sys/class/gpio%d/direction", "out", gpio[loop].number);
			system(cmd);		

			memset(cmd, 0, 128);
			sprintf(cmd, "echo %d > /sys/class/gpio%d/value", gpio[loop].value, gpio[loop].number);
			system(cmd);					
		}
		else if(gpio->flags == OSS_GPIO_IN)
		{
			memset(cmd, 0, 128);
			sprintf(cmd, "echo %s > /sys/class/gpio%d/direction", "in", gpio[loop].number);
			system(cmd);		
		}
	}
	
	return 1;
}

/////////////////////////////////////////////////////////////
int oss_gpio_set(oss_gpio_t *gpio, int value)
{
	unsigned char cmd[128];

	if(gpio->flags == OSS_GPIO_OUT)
	{
		memset(cmd, 0, 128);
		sprintf(cmd, "echo %s > /sys/class/gpio%d/direction", "out", gpio->number);
		system(cmd);		

		memset(cmd, 0, 128);
		sprintf(cmd, "echo %d > /sys/class/gpio%d/value", gpio->value, gpio->number);
		system(cmd);					
	}

	return 1;
}
#endif

int oss_gpio_init(int io)
{
#ifdef WIN32    
    return (1); 
#else 
    unsigned char cmd[128];
    int error_value;
    int status;

    memset(cmd, 0, 128);
    sprintf(cmd, "test -d /sys/class/gpio/gpio%d", io);
    status = system(cmd);

    if (WEXITSTATUS(status))
    {
        memset(cmd, 0, 128);
        sprintf(cmd, "echo %d > /sys/class/gpio/export ", io);
        status = system(cmd);

        if (-1 == status)
        {
            printf("system error!");
            exit(1);
        }
        else
        {
            if (WEXITSTATUS(status))
            {
                printf("The gpio is busy");
                return (1);
            }
        }

#ifdef DEBUG
    printf("gpio%d create\n", io);
#endif

    }
#endif
}

int oss_gpio_output_mode(int io)
{
#ifdef WIN32
    return (1);
#else
    unsigned char cmd[128];
    int error_value;
    int status;

    memset(cmd, 0, 128);
    sprintf(cmd, "echo %s > /sys/class/gpio/gpio%d/direction", "out", io);
    status = system(cmd);

    if (-1 == status)
    {
        printf("system error!");
        exit(1);
    }
    else
    {
        if (WEXITSTATUS(status))
        {
            printf("The gpio%d non-existence\n", io);
            return (1);
        }
    }

#ifdef DEBUG
    printf("gpio%d  set output \n", io);
#endif
#endif
}

int oss_gpio_input_mode(int io) 
{
#ifdef WIN32
    return (1);
#else
    unsigned char cmd[128];
    int error_value;
    int status;

    memset(cmd, 0, 128);
    sprintf(cmd, "echo %s > /sys/class/gpio/gpio%d/direction", "in", io);
    status = system(cmd);

    if (-1 == status)
    {
        printf("system error!");
        exit(1);
    }
    else
    {
        if (WEXITSTATUS(status))
        {
            printf("The gpio%d non-existence\n", io);
            return (1);
        }
    }

#ifdef DEBUG
    printf("gpio%d  set intput \n", io);
#endif
#endif
}

int oss_gpio_pin_set(int io)
{
#ifdef WIN32
    return (1);
#else
    unsigned char cmd[128];
    int error_value;
    int status;

    memset(cmd, 0, 128);
    sprintf(cmd, "echo 1 > /sys/class/gpio/gpio%d/value\n", io);
    status = system(cmd);

    if (-1 == status)
    {
        printf("system error!");
        exit(1);
    }
    else
    {
        if (WEXITSTATUS(status))
        {
            printf("The gpio%d non-existence\n", io);
            return (1);
        }
    }

#ifdef DEBUG
    printf("pin gpio%d set\n", io);
#endif
#endif
}

int oss_gpio_pin_clear(int io)
{
#ifdef WIN32
    return (1);
#else
    unsigned char cmd[128];
    int error_value;
    int status;

    memset(cmd, 0, 128);
    sprintf(cmd, "echo 0 > /sys/class/gpio/gpio%d/value\n", io);
    status = system(cmd);

    if (-1 == status)
    {
        printf("system error!");
        exit(1);
    }
    else
    {
        if (WEXITSTATUS(status))
        {
            printf("The gpio%d non-existence\n", io);
            return (1);
        }
    }

#ifdef DEBUG
    printf("pin gpio%d clear\n", io);
#endif
#endif
}

/*
int main(int argc, char *argv[])
{
    int gpio[] = {0, 1, 2 , 3 , 4, 6, 11, 37};
    int i;

    for (i=0; i<8; i++) {
        oss_gpio_init(gpio[i]);
        oss_gpio_output_mode(gpio[i]);
    }

    for (i=0; i<8; i++) {
         oss_gpio_init(gpio[i]);
        oss_gpio_pin_set(gpio[i]);
        printf("%d\n", gpio[i]);
        sleep(1);
    }

    for (i=0; i<8; i++) {
        oss_gpio_pin_clear(gpio[7-i]);
        sleep(1);
    }
}

*/


