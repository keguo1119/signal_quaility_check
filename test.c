#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char info[]="+CGREG:,4,3";
	char info2[]="\"WCDMA\",60,57,59";
	char info3[]=" 31, 99\r";
	char cmd[128] = "\"%s\",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,,";
	char cmd_buf[128];
	int i, j, k;
	char buf[30];
	char buf2[20];
	
	sscanf(info, "%s,%d,%d", buf, &i, &j);
	printf("info=%s, buf=%s, i=%d,j=%d\n",info,buf, i,j);

	//sscanf(info2, "%[1-9a-zA-Z]", buf);
	sscanf(info2, "\"%s\" ,%d,%d,%d", buf, &i,&j,&k);
	printf("info2=%s, buf=%s, buf2=%s, i=%d, j=%d, k=%d\n",info2,buf,buf2,i,j,k);
	
	j=0;
	for(i=0; i < strlen(info2); i++) {
		if(info2[i] == '"')
			continue;
	     
		
              buf2[j] = info2[i];
		j++;
	}
	buf2[j] = 0;
	printf("buf2=%s\n", buf2);
	sscanf(buf2,"%d %d %d",  &i,&j,&k);
	printf("buf=%s, buf2=%s, i=%d, j=%d, k=%d\n",buf,buf2,i,j,k);
	
	printf("cmd=%s\n", cmd);
	snprintf(cmd_buf, 128, "%s", cmd, "0302");

	printf("cmd_buf=%s", cmd_buf);
	sscanf(info3, "%d, %d", &i, &j);
	printf("i=%d, j=%d\n",  i, j);
	printf("info3=%s, i=%d, j=%d\n", info3, i, j);
}
