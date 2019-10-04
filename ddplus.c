#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

#define PATH_LEN 1024
#define TEMP_BUFLEN 1024

#define FINDDISK_BASE "/sys/block"
#define FINDDISK_FILE "device/model"

int finddisk_frommodel(char *sDev, char *sCheck) {
	char sPath[PATH_LEN];
	char sData[TEMP_BUFLEN];
	int iRet = -1;

	snprintf(sPath, PATH_LEN, "%s/%s/%s", FINDDISK_BASE, sDev, FINDDISK_FILE);
	int iF = open(sPath, O_RDONLY);
	if (iF == -1) {
		fprintf(stderr, "ERRR:FD_FM:%s: Not found???\n", sDev);
		return -1;
	}
	int iRead = read(iF, sData, TEMP_BUFLEN);
	if (iRead == -1) {
		fprintf(stderr, "ERRR:FD_FM:%s:Read!\n", sDev);
		goto cleanup;
	}
	sData[iRead] = 0;
	if (strncasecmp(sData, sCheck, strlen(sCheck)) >= 0) {
		fprintf(stderr, "INFO:FD_FM:%s:%s:Found\n", sDev, sCheck);
		iRet = 0;
	} else {
		fprintf(stderr, "WARN:FD_FM:%s:%s:NotFound\n", sDev, sCheck);
	}
cleanup:
	close(iF);
	return iRet;
}


int main(int argc, char **argv) {

	return 0;
}
