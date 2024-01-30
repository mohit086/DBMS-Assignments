#include <stdio.h>
#include<string.h>
#include "pds.h"

struct PDS_RepoInfo repo_handle;

// pds_create
// Open the data file and index file in "wb" mode
// Initialize index file by storing "0" to indicate there are zero entries in index file
// close the files
int pds_create(char *repo_name){
	char filename[35],indexname[35];
	strcpy(filename,repo_name);
	strcpy(indexname,repo_name);
	strcat(filename,".dat");
	strcat(indexname,".ndx");
	FILE *fp1 = fopen(filename,"wb");
	FILE *fp2 = fopen(indexname,"wb");
	if(fp1==NULL || fp2==NULL) return PDS_FILE_ERROR;
	int z = 0;
	fwrite(&z,sizeof(z),1,fp2);
	fclose(fp1);
	fclose(fp2);
	return PDS_SUCCESS;
}

// pds_open
// Open the data file and index file in rb+ mode
// Update the fields of PDS_RepoInfo appropriately
// Read the number of records form the index file
// Load the index into the array and store in ndx_array by reading index entries from the index file
// Close only the index file
int pds_open(char *repo_name, int rec_size){
	char filename[35],indexname[35];
	strcpy(filename,repo_name);
	strcpy(indexname,repo_name);
	strcat(filename,".dat");
	strcat(indexname,".ndx");
	repo_handle.pds_data_fp = fopen(filename,"rb+");
	repo_handle.pds_ndx_fp = fopen(indexname,"rb+");
	if(repo_handle.pds_data_fp==NULL || repo_handle.pds_ndx_fp==NULL) return PDS_FILE_ERROR;
	repo_handle.repo_status = PDS_REPO_OPEN;
	repo_handle.rec_size = rec_size;
	strcpy(repo_handle.pds_name,repo_name);
	int count;
	fread(&count,sizeof(count),1,repo_handle.pds_ndx_fp);
	pds_load_ndx();
	return PDS_SUCCESS;
}

// pds_load_ndx
// Internal function used by pds_open to read index entries into ndx_array
int pds_load_ndx(){
	struct PDS_NdxInfo temp;
	for(int i=0;i<repo_handle.rec_count;i++){
		fread(&temp,sizeof(temp),1,repo_handle.pds_ndx_fp);
		repo_handle.ndx_array[i] = temp;
	}
	return PDS_SUCCESS;
}

// put_rec_by_key
// Seek to the end of the data file
// Create an index entry with the current data file location using ftell
// Add index entry to ndx_array using offset returned by ftell
// Increment record count
// Write the record at the end of the file
// Return failure in case of duplicate key
int put_rec_by_key(int key, void *rec){
    if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_ADD_FAILED;
	struct PDS_NdxInfo entry;
	entry.key = key;
	fseek(repo_handle.pds_data_fp,0,SEEK_END);
	entry.offset = ftell(repo_handle.pds_data_fp);
	repo_handle.rec_count++;
	repo_handle.ndx_array[repo_handle.rec_count] = entry;
	fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
	return PDS_SUCCESS;
}

// get_rec_by_key
// Search for index entry in ndx_array
// Seek to the file location based on offset in index entry
// Read the key at the current location 
// Read the record from the current location
int get_rec_by_key(int key, void *rec){
	int temp;
	for(int i=0;i<=repo_handle.rec_count;i++){
		if(repo_handle.ndx_array[i].key==key){
			fseek(repo_handle.pds_data_fp,repo_handle.ndx_array[i].offset,SEEK_SET);
			fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
			return PDS_SUCCESS;
		}
	}
	return PDS_REC_NOT_FOUND;
}

// pds_close
// Open the index file in wb mode (write mode, not append mode)
// Store the number of records
// Unload the ndx_array into the index file by traversing it (overwrite the entire index file)
// Close the index file and data file
int pds_close(){
	char indexfile[35];
	strcpy(indexfile,repo_handle.pds_name);
	strcat(indexfile,".ndx");
	FILE* temp = fopen(indexfile,"wb");
	fwrite(&repo_handle.rec_count,sizeof(int),1,temp);
	fwrite(repo_handle.ndx_array,repo_handle.rec_size,1,temp);
	fclose(temp);
	fclose(repo_handle.pds_data_fp);
	return PDS_SUCCESS;
}
