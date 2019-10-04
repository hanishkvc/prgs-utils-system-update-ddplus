#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <dirent.h>

#define PATH_LEN 1024
#define TEMP_BUFLEN 1024

#define FINDDISK_BASE "/sys/block"
#define FINDDISK_FILE "device/model"

char *gsSrcModel, *gsDstModel;


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
	//fprintf(stderr, "DBUG:FD_FM:%s: Opened\n", sPath);

	int iRead = read(iF, sData, TEMP_BUFLEN);
	if (iRead == -1) {
		fprintf(stderr, "ERRR:FD_FM:%s:Read!\n", sDev);
		goto cleanup;
	}
	sData[iRead] = 0;
	if (strncasecmp(sData, sCheck, strlen(sCheck)) == 0) {
		fprintf(stderr, "INFO:FD_FM:%s:%s:Found\n", sDev, sCheck);
		iRet = 0;
	} else {
		fprintf(stderr, "WARN:FD_FM:%s:%s:NotFound\n", sDev, sCheck);
	}
cleanup:
	close(iF);
	return iRet;
}


int list_dir(char *sDirPath, char *sPrefix) {
	DIR *rDir = opendir(sDirPath);
	if (rDir == NULL) {
		fprintf(stderr, "ERRR:ld:%s:dirpath?\n", sDirPath);
		return -1;
	}
	while(1) {
		struct dirent *de = readdir(rDir);
		if (de == NULL)
			break;
		fprintf(stderr, "DBUG:ld:%s\n", de->d_name);
	}
	closedir(rDir);
}


int main(int argc, char **argv) {

	if (argc < 3) {
		fprintf(stderr,"INFO:usage: ddplus <SrcModel> <DestModel>\n");
		return 1;
	}
	gsSrcModel = argv[1];
	gsDstModel = argv[2];
	finddisk_frommodel("sda", gsSrcModel);
	finddisk_frommodel("sdb", gsSrcModel);
	finddisk_frommodel("sda", gsDstModel);
	finddisk_frommodel("sdb", gsDstModel);

	list_dir("/sys/block", "sd");

	return 0;
}

