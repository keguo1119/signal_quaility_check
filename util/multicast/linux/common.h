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
	unsigned char	name[32];			/* �������� */
	unsigned char	ip[32];				/* ����IP */
	unsigned char	mac[32];			/* ����MAC */
	unsigned char	id[32];	 		    /* ����ID */
	unsigned char	hw_ver[32];		    /* Ӳ���汾 */	
	unsigned char	soft_ver[32];		/* ����汾 */
	unsigned char	type[32];			/* �����ͺ� */
	unsigned char   dev_ip[16];         /* ���������豸��ַ */
	unsigned char   rsv[32];			/* Ԥ��λ */
}board_info_t;

#define BOARD_INFO_FILE	"/usr/local/board/cfg/board-info.dat"


#endif