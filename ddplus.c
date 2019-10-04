#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

#define PATH_LEN 1024
#define TEMP_BUFLEN 1024

#define FINDDISK_BASE "/sys/block"
#define FINDDISK_FILE "device/model"

char *gsSrcModel, *gsDstModel;

#define MAX_STRINGS 128
#define STRING_LEN 512
//typedef char TMYSTRING[MAX_STRINGS][STRING_LEN] AOfSTR;
typedef char AOFSTR[MAX_STRINGS][STRING_LEN];


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


int list_dir(char *sDirPath, char *sPrefix, AOFSTR saFiles) {
	DIR *rDir = opendir(sDirPath);
	int iCur = 0;
	if (rDir == NULL) {
		fprintf(stderr, "ERRR:ld:%s:dirpath?\n", sDirPath);
		return -1;
	}
	while(1) {
		struct dirent *de = readdir(rDir);
		if (de == NULL)
			break;
		if (strncmp(de->d_name, sPrefix, strlen(sPrefix)) != 0)
			continue;
		strncpy(saFiles[iCur], de->d_name, STRING_LEN);
		fprintf(stderr, "DBUG:ld:%s\n", saFiles[iCur]);
		iCur++;
	}
	closedir(rDir);
	return iCur;
}


int find_srcdst(char *sSrcDisk, char *sSrcModel, char *sDstDisk, char *sDstModel) {

	AOFSTR saFiles;
	int iFiles = list_dir("/sys/block", "sd", saFiles);
	if (iFiles <= 0) {
		fprintf(stderr, "ERRR:ddplus:find_sd: No Disks\n");
		return -1;
	}

	// Find Source Disk
	bool bSrcDisk = false;
	for(int i = 0; i < iFiles; i++) {
		if (finddisk_frommodel(saFiles[i], sSrcModel) < 0) {
			continue;
		}
		strncpy(sSrcDisk, saFiles[i], STRING_LEN);
		bSrcDisk = true;
		break;
	}
	if (bSrcDisk) {
		fprintf(stderr, "INFO:ddplus:find_sd:Source[%s]\n", sSrcDisk);
	} else {
		fprintf(stderr, "ERRR:ddplus:find_sd:NO Source\n");
		return -1;
	}

	// Find Dest Disk
	bool bDstDisk = false;
	for(int i = 0; i < iFiles; i++) {
		if (finddisk_frommodel(saFiles[i], sDstModel) < 0) {
			continue;
		}
		strncpy(sDstDisk, saFiles[i], STRING_LEN);
		bDstDisk = true;
		break;
	}
	if (bDstDisk) {
		fprintf(stderr, "INFO:ddplus:find_sd:Dest[%s]\n", sDstDisk);
	} else {
		fprintf(stderr, "ERRR:ddplus:find_sd:NO Dest\n");
		return -2;
	}

	return 0;
}


int main(int argc, char **argv) {

	char sSrcDisk[STRING_LEN], sDstDisk[STRING_LEN];

	if (argc < 3) {
		fprintf(stderr,"INFO:usage: ddplus <SrcModel> <DestModel>\n");
		return 1;
	}
	gsSrcModel = argv[1];
	gsDstModel = argv[2];

	find_srcdst(sSrcDisk, gsSrcModel, sDstDisk, gsDstModel);

	return 0;
}

