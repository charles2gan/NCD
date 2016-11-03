#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include<unistd.h>
#include <math.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define LINE_L 512
#define POOL 24

pthread_mutex_t lock;
pthread_mutex_t lock_printer;
pthread_t tid[POOL];

int sizeFile=0;
char* segmentationList;

int currentRow(int fin){
	static int i = 0;
	static int last;
	int value;
	if(fin==-1)
	{
		pthread_mutex_lock(&lock);
		if(i < last)
		{
			value = i;
			i++;
		}
		else
		{
			value = -1;
		}
		pthread_mutex_unlock(&lock);
	}
	else
	{
		last=fin;
		value=0;
	}
	return value;
}


double ncdDistance(char * file1Name, char * file2Name, int value){
	char line1[512];
	char line2[512];
	char line3[512];
	char line4[512];
	char str[15];
	
	FILE *file1;
	FILE *file2;
	FILE *file3;
	
	long val1;
	long val2;
	long val3;

	
/*	strcpy(line1, "7za a -mx9 test.7z ");
	strcat(line1, file1Name);
	strcat(line1, " > /dev/null; du -k test.7z");

	strcpy(line2, "7za a -mx9 test2.7z ");
	strcat(line2, file2Name);
	strcat(line2, " > /dev/null; du -k test2.7z");
	
	strcpy(line3,"cat ");
	strcat(line3,file1Name);
	strcat(line3," ");
	strcat(line3,file2Name);
	strcat(line3, " > cosa.bin ;7za a -mx9 test3.7z cosa.bin > /dev/null; du -k test3.7z");
*/	

/*
	strcpy(line1, "cat ");
	strcat(line1, file1Name);
	strcat(line1, " | 7za a -mx3 -ms=4g -md=100m -t7z -si test.7z > /dev/null; du -k test.7z");

	strcpy(line2, "cat ");
	strcat(line2, file2Name);
	strcat(line2, " | 7za a -mx3 -ms=4g -md=100m -t7z -si test2.7z > /dev/null; du -k test2.7z");
	
	strcpy(line3,"cat ");
	strcat(line3,file1Name);
	strcat(line3," ");
	strcat(line3,file2Name);
	strcat(line3, " | 7za a -mx3 -ms=4g -md=100m -t7z -si test3.7z > /dev/null; du -k test3.7z");
	system("rm test.7z test2.7z test3.7z ");
	file1= popen(line1,"r");
	file2= popen(line2,"r");
	fscanf(file1,"%ld",&val1);
	fscanf(file2,"%ld",&val2);
	file3= popen(line3,"r");
	fscanf(file3,"%ld",&val3);
	
	pclose(file1);
	pclose(file2);
	pclose(file3);
	//printf("NCD: %lf", ((double) val3 - MIN(val1,val2))/((double) MAX(val1,val2)));
	return  ((double) val3 - MIN(val1,val2))/((double) MAX(val1,val2));
*/	
	sprintf(str, "%d", value);
	strcat(str,".7z");
	

	//This code if for those cases when the .7z has been generated before for all the files
	strcpy(line1, "du -k ");
	strcat(line1, file1Name);
	strcat(line1, ".7z");

	strcpy(line2, "du -k ");
	strcat(line2, file2Name);
	strcat(line2, ".7z");
	
	strcpy(line3,"cat ");
	strcat(line3,file1Name);
	strcat(line3," ");
	strcat(line3,file2Name);
	strcat(line3, " | 7za a -mx9 -ms=4g -md=100m -t7z -si ");
	strcat(line3,str);
	strcat(line3, " > /dev/null; du -k ");
	strcat(line3,str);
	
	strcpy(line4,"rm ");
	strcat(line4,str);
	strcat(line4,"  2> /dev/null");
	system(line4);
	
	file1= popen(line1,"r");
	file2= popen(line2,"r");
	fscanf(file1,"%ld",&val1);
	fscanf(file2,"%ld",&val2);
	file3= popen(line3,"r");
	fscanf(file3,"%ld",&val3);
	
	pclose(file1);
	pclose(file2);
	pclose(file3);
	//printf("NCD: %lf", ((double) val3 - MIN(val1,val2))/((double) MAX(val1,val2)));
	return  ((double) val3 - MIN(val1,val2))/((double) MAX(val1,val2));
}

void* calculationThread(void *arg)
{
	int value;
	int j;
	double * list;
	
	value = currentRow(-1);
	while(value != -1)
	{
		list = malloc(sizeof(double)*(sizeFile-value+1));
	
		for(j=value+1;j < sizeFile; j++){
			list[j-value+1]=1.0-ncdDistance(segmentationList+value*LINE_L,segmentationList+j*LINE_L,value);
		}
		
		pthread_mutex_lock(&lock_printer);

		for(j=value+1;j < sizeFile; j++){
			fprintf(stdout,"%lf",list[j-value+1]);
			if(j < sizeFile - 1){
				fprintf(stdout,";");
			}
		}
		fprintf(stdout,"\t%d\n",value);

	//	fprintf(stdout,"\n");
		
		pthread_mutex_unlock(&lock_printer);
		value = currentRow(-1);
		free(list);
	}
	return NULL;
}

int commandDirectory(char * directory){
	char lsCommand[LINE_L];
	int err;
//	char line[LINE_L];
	FILE * listFiles;
	char ch;
//	distmatrix_t distMatrix;
//	double lDistance;
//	double costMax;
	int i;
	strcpy(lsCommand,"ls ");
	strcat(lsCommand,directory);
	strcat(lsCommand,"*.bin > file.ini");
	//strcat(lsCommand,"* > file.ini");
	system(lsCommand);
	
	listFiles = fopen("file.ini","r");
	
	while(!feof(listFiles))
	{
	  ch = fgetc(listFiles);
	  if(ch == '\n')
	  {
		sizeFile++;
	  }
	}
	if(fseek(listFiles, 0L, SEEK_SET))
		perror("Could not reach the beginning of the file");
	segmentationList = malloc(sizeof(char)*LINE_L*sizeFile);
	for(i=0;i<sizeFile;i++){
		fgets(segmentationList+i*LINE_L,LINE_L,listFiles);
		segmentationList[i*LINE_L+strlen(segmentationList+i*LINE_L)-1]='\0';
		//printSegmentation(segmentation);
	}
	currentRow(sizeFile);
	//distMatrix = newDistMatrix();
	//iniDistMatrix(sizeFile,sizeFile,distMatrix);
	
/*	for(i=0;i<sizeFile;i++){
		for(j=i+1;j < sizeFile; j++){
			fprintf(stdout,"%lf",1.0-ncdDistance(segmentationList+i*LINE_L,segmentationList+j*LINE_L));
			if(j < sizeFile - 1){
				fprintf(stdout,";");
			}
		}
		fprintf(stdout,"\n");
	}
*/
	for(i=0;i < POOL;i++)
    {
        err = pthread_create(&(tid[i]), NULL, &calculationThread, NULL);
/*        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            printf("\n Thread created successfully\n");
*/
    }
	return 0;
}

int main(int argc, char **argv)
{
	int i;
	if (pthread_mutex_init(&lock, NULL) != 0)
    {
  //      printf("\n mutex init failed\n");
        return 1;
    }
	if (pthread_mutex_init(&lock_printer, NULL) != 0)
    {
    //    printf("\n mutex init failed\n");
        return 1;
    }
	
	commandDirectory(argv[1]);
	
	
	for(i=0;i < POOL;i++)
    {
		pthread_join(tid[i], NULL);
	}
	pthread_mutex_destroy(&lock);
	pthread_mutex_destroy(&lock_printer);

	//printf("NCD: %lf\n", ncdDistance(argv[1],argv[2]));
}
