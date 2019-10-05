/*
 * Update Systems Simbly, Linux based
 * v20191005IST1119, HanishKVC
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

#define PRG_VERSION "v20191005IST1149"

#define PATH_LEN 1024
#define TEMP_BUFLEN 1024

#define FINDDISK_BASE "/sys/block"
#define FINDDISK_FILE "device/model"

char *gsDevPath;
char *gsSrcModel, *gsDstModel;
long long int giSrcOffset, giDstOffset, giTransSize;

#define MAX_STRINGS 128
#define STRING_LEN 512
//typedef char TMYSTRING[MAX_STRINGS][STRING_LEN] AOfSTR;
typedef char AOFSTR[MAX_STRINGS][STRING_LEN];


#define dprintf dummyi
int dummyi(int dbgLvl, ...) {
	return 0;
}


int _readfile(char *sFilePath, char *sData, int iDataLen, char *sFile) {

	int iF = open(sFilePath, O_RDONLY);
	if (iF == -1) {
		fprintf(stderr, "ERRR:readf:%s: Not found???\n", sFile);
		return -1;
	}
	dprintf(50, stderr, "DBUG:readf:%s: Opened\n", sFile);

	int iRead = read(iF, sData, iDataLen);
	if (iRead == -1) {
		fprintf(stderr, "ERRR:readf:%s:Read!\n", sFile);
	} else {
		sData[iRead] = 0;
	}
	close(iF);
	return iRead;
}

int readfile(char *sFile, char *sData, int iDataLen) {
	return _readfile(sFile, sData, iDataLen, sFile);
}

int finddisk_frommodel(char *sDev, char *sCheck) {
	char sPath[PATH_LEN];
	char sData[TEMP_BUFLEN];

	snprintf(sPath, PATH_LEN, "%s/%s/%s", FINDDISK_BASE, sDev, FINDDISK_FILE);

	if (readfile(sPath, sData, TEMP_BUFLEN) <= 0) {
		fprintf(stderr, "ERRR:FD_FM:%s:Read failed\n", sDev);
		return -1;
	}

	if (strncasecmp(sData, sCheck, strlen(sCheck)) == 0) {
		dprintf(50, stderr, "DBUG_INFO:FD_FM:%s:%s:Found\n", sDev, sCheck);
		return 0;
	} else {
		dprintf(50, stderr, "DBUG_WARN:FD_FM:%s:%s:NotFound\n", sDev, sCheck);
	}
	return -2;
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
		dprintf(50, stderr, "DBUG_INFO:ld:%s\n", saFiles[iCur]);
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


void gudbud1(char *sSrc, char *sDst) {

	int iLen = strlen(sSrc);

	int iStart = iLen - 8;
	if (iStart < 0) {
		fprintf(stderr, "ERRR:gudbud1: insufficient data, quiting...\n");
		exit(100);
	}
	strncpy(sDst, &sSrc[iStart], 8);

	int iRounds = iLen/8;
	for(int i = 0; i < iRounds; i++) {
		for(int j = 0; j < 8; j++) {
			sDst[j] = sDst[j] ^ sSrc[i*8+j];
		}
	}
}


void procd1(char *sData, int iLen) {

	int iLoops = iLen/8;
	int iRemain = iLen%8;
	uint64_t *piData;
	uint64_t iData;

	if (iRemain != 0) {
		dprintf(10, stderr, "ERRR:procd1: data not 64bit mult, quiting...\n");
		exit(101);
	}
	for(int i = 0; i < iLoops; i++) {
		piData = (uint64_t*)(&sData[i*8]);
		iData = *piData;
		int iOp = i % 2;
		switch (iOp) {
			case 0:
				iData = iData ^ ((0x5a78a58735c9ca36+i)*i);
				break;
			case 1:
				iData = iData ^ (0x5a78a58735c9ca36*i);
				break;
			case 2:
				iData = iData ^ (0x5a78a58735c9ca36-i);
				break;
			case 3:
				iData = iData ^ (0x5a78a58735c9ca36<<1);
				break;

		}
		//iData = iData ^ i;
		*piData = iData;
	}
}


int readex(int iF, char *sData, int iLen) {
	int iRead, iRemaining;

	iRemaining = iLen;
	while(iRemaining > 0) {
		iRead = read(iF, &sData[iLen-iRemaining], iRemaining);
		if (iRead == -1) { // May handle EINTR, later if it occurs beyond once in a blue moon
			fprintf(stderr, "WARN:theRead:%s\n", strerror(errno));
			return (iLen-iRemaining);
		}
		if (iRead == 0) {
			fprintf(stderr, "WARN:theRead:EOF reached\n");
			return (iLen-iRemaining);
		}
		iRemaining -= iRead;
	}
	return iLen;
}


int writeex(int iF, char *sData, int iLen) {
	int iWrote, iRemaining;

	iRemaining = iLen;
	while(iRemaining > 0) {
		iWrote = write(iF, &sData[iLen-iRemaining], iRemaining);
		if (iWrote == -1) { // May handle EINTR, later if it occurs beyond once in a blue moon
			fprintf(stderr, "WARN:theWrite:%s\n", strerror(errno));
			return (iLen-iRemaining);
		}
		iRemaining -= iWrote;
	}
	return iLen;
}


int dd_s2d(char *sDevPath, char *sSrcDisk, char *sDstDisk, long long int iSrcOffset, long long int iDstOffset, long long int iTransferSize) {
	char sSrcPath[PATH_LEN], sDstPath[PATH_LEN];

	snprintf(sSrcPath, PATH_LEN, "%s/%s", sDevPath, sSrcDisk);
	snprintf(sDstPath, PATH_LEN, "%s/%s", sDevPath, sDstDisk);

	int iFSrc = open(sSrcPath, O_RDONLY);
	if (iFSrc == -1) {
		fprintf(stderr, "ERRR:du: Failed to open Src [%s]\n", sSrcPath);
		return -1;
	}
	int iFDst = open(sDstPath, O_RDWR);
	if (iFDst == -1) {
		fprintf(stderr, "ERRR:du: Failed to open Dst [%s]\n", sDstPath);
		return -2;
	}

	if (lseek(iFSrc, iSrcOffset, SEEK_SET) == -1) {
		fprintf(stderr, "ERRR:du: Failed Src [%s] seekto [%lld];[%s]\n", sSrcPath, iSrcOffset, strerror(errno));
		return -11;
	}
	fprintf(stderr, "INFO:du: Src [%s] seekdto [%lld]\n", sSrcPath, iSrcOffset);
	if (lseek(iFDst, iDstOffset, SEEK_SET) == -1) {
		fprintf(stderr, "ERRR:du: Failed Dst [%s] seekto [%lld]\n", sDstPath, iDstOffset);
		return -12;
	}
	fprintf(stderr, "INFO:du: Dst [%s] seekdto [%lld]\n", sDstPath, iDstOffset);

	int iLen = 0x100000;
	char sData[iLen];
	int iProgress = 0;
	while (iTransferSize > 0) {
		if ((iProgress % 1024) == 0) {
			fprintf(stderr, "INFO:du: Remaining [%lld]...\n", iTransferSize);
		}
		if (iLen > iTransferSize)
			iLen = iTransferSize;
		int iRead = readex(iFSrc, sData, iLen);
		if (iRead != iLen) {
			fprintf(stderr, "ERRR:du: Failed with read\n");
			return -21;
		}
		procd1(sData, iRead);
		int iWrite = writeex(iFDst, sData, iRead);
		if (iWrite != iRead) {
			fprintf(stderr, "ERRR:du: Failed with write\n");
			return -22;
		}
		iTransferSize -= iLen;
		iProgress += 1;
	}

	return 0;
}


int main(int argc, char **argv) {

	char sSrcDisk[STRING_LEN], sDstDisk[STRING_LEN];

	fprintf(stderr, "INFO:ddplus %s\n", PRG_VERSION);
	if (argc < 7) {
		fprintf(stderr,"INFO:usage: ddplus <DevPath> <SrcModel> <DestModel> <SrcOffset> <DstOffset> <TransferSize>\n");
		return 1;
	}
	gsDevPath = argv[1];
	gsSrcModel = argv[2];
	gsDstModel = argv[3];
	giSrcOffset = strtoll(argv[4], NULL, 0);
	giDstOffset = strtoll(argv[5], NULL, 0);
	giTransSize = strtoll(argv[6], NULL, 0);


	if (find_srcdst(sSrcDisk, gsSrcModel, sDstDisk, gsDstModel) != 0) {
		fprintf(stderr,"ERRR:ddplus: Failed to find Source or Dest Disk, quiting...\n");
		return 2;
	}

	if (dd_s2d(gsDevPath, sSrcDisk, sDstDisk, giSrcOffset, giDstOffset, giTransSize) != 0) {
		fprintf(stderr,"ERRR:ddplus: Failed to update, quiting...\n");
		return 3;
	} else {
		fprintf(stderr,"INFO:ddplus: Done with update, quiting...\n");
	}

	return 0;
}

