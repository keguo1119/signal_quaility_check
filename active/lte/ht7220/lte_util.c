#include "oss.h"
#include "oss_gpio.h"

#ifndef WIN32
#include <sys/wait.h>


int oss_gpio_switch_set(oss_gpio_t *gpio, int value);
void sys_reboot();

int oss_gpio_switch_set(oss_gpio_t *gpio, int value)
{
    static int exist_flag=0;
    unsigned char cmd[128];
    int error_value;
    pid_t status;

    gpio->value = value;

    if (!exist_flag)
    {
        memset(cmd, 0, 128);
        sprintf(cmd, "test -d /sys/class/gpio/gpio%d", gpio->number);
	status = system(cmd);
	

	if (WEXITSTATUS(status))
	{
	        memset(cmd, 0, 128);
       		sprintf(cmd, "echo %d > /sys/class/gpio/export ", gpio->number);

        	status = system(cmd);
		if (-1 == status)
		{
			printf("system error!");
			return (1);		
		}
		else 
		{
			if (WEXITSTATUS(status))
			{
				printf("The gpio is busy");
				return (1);
			}
		}
        	exist_flag++;
	}
//	else
//		printf("gpio%d has created\n", gpio->number);
    }

    if(gpio->flags == OSS_GPIO_OUT)
    {
                memset(cmd, 0, 128);
                sprintf(cmd, "echo %s > /sys/class/gpio/gpio%d/direction", "out", gpio->number);
                

                memset(cmd, 0, 128);
                sprintf(cmd, "echo %d > /sys/class/gpio/gpio%d/value", gpio->value, gpio->number);
                system(cmd);
    }

    else if(gpio->flags == OSS_GPIO_IN)
    {
          memset(cmd, 0, 128);
          sprintf(cmd, "echo %s > /sys/class/gpio%d/direction", "in", gpio->number);
          system(cmd);
    }

    return 0;
}


void sys_reboot()
{
    //system("ifconfig eth0 down ");  //shutdown the eth
    system("ifconfig apcli0 down"); //shutdown the WIFI
    system(" kill $(ps | grep lteapp/./app | grep -v grep | awk '{print $1}') "); //shutdown the app
    system("reboot");

    return ;
}

#endif