#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include"pds.h"

// Define the global variable
struct PDS_RepoInfo repo_handle;

int pds_create(char *repo_name){
	char filename[35];
	strcpy(filename, repo_name);
	strcat(filename,".dat");
	FILE *fp = fopen(filename,"wb+");
	if(fp==NULL) return PDS_FILE_ERROR;
	else{
		fclose(fp);
		return PDS_SUCCESS;
	}
}

int pds_open(char *repo_name, int rec_size){
	char filename[35];
	strcpy(filename,repo_name);
	strcat(filename,".dat");
	repo_handle.pds_data_fp = fopen(filename,"rb+");
	repo_handle.repo_status = PDS_REPO_OPEN;
	repo_handle.rec_size = rec_size;
	return PDS_SUCCESS;
}

int put_rec_by_key(int key, void *rec){
        if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_ADD_FAILED;
	fseek(repo_handle.pds_data_fp,0,SEEK_END);
	fwrite(&key,sizeof(key),1,repo_handle.pds_data_fp);
	fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
	return PDS_SUCCESS;
}

int get_rec_by_key(int key, void *rec){
	int temp;
	fseek(repo_handle.pds_data_fp,0,SEEK_SET);
	while(!feof(repo_handle.pds_data_fp)){
		fread(&temp,sizeof(int),1,repo_handle.pds_data_fp); 
		fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
		if(temp==key) return PDS_SUCCESS;
	}
	return PDS_REC_NOT_FOUND;
}

int pds_close(){
	fclose(repo_handle.pds_data_fp);
	strcpy(repo_handle.pds_name,"");
	repo_handle.repo_status = PDS_REPO_CLOSED;
}

