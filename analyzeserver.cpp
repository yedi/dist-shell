#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <string.h>

#include <sys/time.h>
#include <math.h>

//timeval_subtract and timeval_print was taken from a stackoverflow problem
/* Return 1 if the difference is negative, otherwise 0.  */
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}

void timeval_print(struct timeval *tv)
{
    char buffer[30];
    time_t curtime;

    printf("%ld.%06ld", tv->tv_sec, tv->tv_usec);
    curtime = tv->tv_sec;
    strftime(buffer, 30, "%m-%d-%Y  %T", localtime(&curtime));
    printf(" = %s.%06ld\n", buffer, tv->tv_usec);
}

double calcMean(double *list) {
	int i;
	double sum = 0;
	for (i = 0; i < 100; i++) {
		sum += list[i];
	}
	return sum/100;
}

double calcStd(double *list, double mean) {
	int i;
	double sum = 0;
	for (i = 0; i < 100; i++) {
		sum += pow(mean-list[i], 2);
	}
	return sum/99;
}
#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff /* should be in <netinet/in.h> */
#endif

int main(int argc, char *argv[]) {
	int c;
	int tflag = 0;
   	int fflag = 0;
   	int rflag = 0;
   	int cflag = 0;
   	char *ffile;

   	while ((c = getopt (argc, argv, "ctrf:")) != EOF) {
	switch (c) {
		case 't':
		 	tflag++;
		 	break;
		case 'r':
			rflag++;
			break;
		case 'f':
			ffile = optarg;
			fflag++;
			break;
		case 'c':
			cflag++;
			break;
      }
   	}

   	if (tflag) {
		int i;
		struct timeval tvBegin, tvEnd, tvDiff;
		gettimeofday(&tvBegin, NULL);
		for (i = 0; i < 100; i++) {
			system("./dsh -s cccwork2.wpi.edu -p 4444 -c \"test\"");
		}
		//system("./dsh -s cccwork2.wpi.edu -c \"close\"");
		gettimeofday(&tvEnd, NULL);
		timeval_subtract(&tvDiff, &tvEnd, &tvBegin);

		FILE * pFile;
		pFile = fopen ("setupData.txt","a");
		if (pFile!=NULL)
		{
			fprintf(pFile, "%ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);
			fclose (pFile);
		}
	    printf("%ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);
	}
	else if (rflag) {
		int i;
		struct timeval tvBegin, tvEnd, tvDiff;
		FILE * pFile;
		pFile = fopen ("thruData.txt","a");
		for (i = 0; i < 100; i++) {
			gettimeofday(&tvBegin, NULL);
			system("./dsh -s cccwork2.wpi.edu -p 4444 -c \"cat testdata.dat\"");
			gettimeofday(&tvEnd, NULL);
			timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
			fprintf(pFile, "%ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);
		}
		//system("./dsh -s cccwork2.wpi.edu -c \"close\"");
		fclose (pFile);

	   	//printf("%ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);
	}
	else if (fflag) {
		char *filename, *pfilename;
		int setup = 0;
		fstream inStream;
		printf("here is %s\n", ffile);
		if (strcmp(ffile, "setup") == 0) {
			filename = "setupData.txt";
			pfilename = "fsetupData.txt";
			setup++;
		}
		else {
			filename = "thruData.txt";
			pfilename = "fthruData.txt";
		}

		inStream.open(filename, ios :: in);
		if(inStream.fail())
		{
			//return false;
			cout << "couldn't open\n";
			return 0;
		}

		double inNum;
		
		FILE *pFile;
		pFile = fopen(pfilename, "a");
		if ( pFile != NULL)
		{
			double number;
			char numtxt[80];
			printf("here");
			int i;
			for (i = 0; i < 100; i++)
			{
				inStream >> inNum;
				if (setup) inNum = inNum/100;
				else inNum = inNum*100;
				sprintf(numtxt,"%.8f",inNum);
				puts(numtxt);
				fputs ( numtxt, pFile ); /* write the line */
			}
			fclose(pFile);
		}
		inStream.close();
	}
	else if (cflag) {
		char *thrufile, *setupfile;
		double thruList[100];
		double setupList[100];
		double thruMean, thruStd, setupMean, setupStd, thruReal;
		int setup;
		fstream inStream;

		thrufile = "thruData.txt";
		setupfile = "fsetupData.txt";
		
		inStream.open(thrufile, ios :: in);
		if(inStream.fail())
		{
			//return false;
			cout << "couldn't open\n";
			return 0;
		}
		int i;
		for (i = 0; i < 100; i++) {
			inStream >> thruList[i];
		}
		inStream.close();

		inStream.open(setupfile, ios :: in);
		if(inStream.fail())
		{
			//return false;
			cout << "couldn't open\n";
			return 0;
		}

		for (i = 0; i < 100; i++) {
			inStream >> setupList[i];
		}
		inStream.close();

		setupMean = calcMean(setupList);
		thruMean = calcMean(thruList);
		setupStd = calcStd(setupList, setupMean);
		thruStd = calcStd(thruList, thruMean);
		thruReal = thruMean - setupMean;

		FILE *pFile;
		pFile = fopen("finalData.txt", "a");
		if ( pFile == NULL)
		{
			printf("couldn't open file");
		}
		
		fprintf(pFile, "Setup Average: %.8f\nSetup Standard Deviation: %.8f\nThroughput Average: %.8f\nThroughput Standard Deviation: %.8f\nActual Throughput Average: %.8f (Throughput - Setup)\n\n",
		setupMean, setupStd, thruMean, thruStd, thruReal);
		fprintf(pFile, "Data:\nSetup     | Throughput\n");
		for (i = 0; i < 100; i++) {
			fprintf(pFile, "%.8f|%.8f\n",setupList[i],thruList[i]);
		}
		fclose(pFile);
	}
	return 0;
}
