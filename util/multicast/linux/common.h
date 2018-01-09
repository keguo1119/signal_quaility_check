#ifndef  __COMMMON_H__
#define  __COMMMON_H__

#define MAGIC_FLAG	0xcdcddcdc
#define MAGIC_STR   "xyz_get_board_info"
#define MAX_BUF_SIZE	2048
#define MULTICAST_ADDR	"234.0.0.1" 
#define MULTICAST_PORT	5454

typedef struct TagBoardInfo 
{
	unsigned int	flag;
	unsigned char	name[32];			/* 单板名称 */
	unsigned char	ip[32];				/* 单板IP */
	unsigned char	mac[32];			/* 单板MAC */
	unsigned char	id[32];	 		    /* 单板ID */
	unsigned char	hw_ver[32];		    /* 硬件版本 */	
	unsigned char	soft_ver[32];		/* 软件版本 */
	unsigned char	type[32];			/* 单板型号 */
	unsigned char   dev_ip[16];         /* 单板所在设备地址 */
	unsigned char   rsv[32];			/* 预留位 */
}board_info_t;

#define BOARD_INFO_FILE	"/usr/local/board/cfg/board-info.dat"


#endif