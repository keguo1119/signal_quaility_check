#ifndef __SCAN_FILE_H__
#define __SCAN_FILE_H__



void scan_file_info_save( char *info_buf, TModemLocal *pm, int num);
void scan_file_save_file_close();
void scan_file_error_info_save(char *err_buf);
#endif