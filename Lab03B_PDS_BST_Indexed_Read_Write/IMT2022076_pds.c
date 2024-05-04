#include <stdio.h>
#include<string.h>
#include<stdlib.h>
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
	fwrite(&z,sizeof(int),1,fp2);
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
int pds_open( char *repo_name, int rec_size ){
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
	fread(&repo_handle.rec_count,sizeof(int),1,repo_handle.pds_ndx_fp);
	pds_load_ndx();
	fclose(repo_handle.pds_ndx_fp);
	return PDS_SUCCESS;
}

// pds_load_ndx - Internal function
// Load the index entries into the BST ndx_root by calling bst_add_node repeatedly for each 
// index entry. Unlike array, for BST, you need read the index file one by one in a loop
int pds_load_ndx(){
	for(int i=0;i<repo_handle.rec_count;i++){
		struct PDS_NdxInfo *temp = malloc(sizeof(struct PDS_NdxInfo));
		fread(temp,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
		bst_add_node(&repo_handle.ndx_root,temp->key,temp);
	}
	return PDS_SUCCESS;
}

// put_rec_by_key
// Seek to the end of the data file
// Create an index entry with the current data file location using ftell
// Add index entry to BST by calling bst_add_node
// Increment record count
// Write the record at the end of the file
// Return failure in case of duplicate key
int put_rec_by_key( int key, void *rec ){
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_ADD_FAILED;
	struct PDS_NdxInfo *entry = malloc(sizeof(struct PDS_NdxInfo));
	entry->key = key;
	if(fseek(repo_handle.pds_data_fp,0,SEEK_END)) return PDS_FILE_ERROR;
	entry->offset = ftell(repo_handle.pds_data_fp);
	if(bst_add_node(&repo_handle.ndx_root,key,entry)) return PDS_FILE_ERROR;
	repo_handle.rec_count++;
	fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
	return PDS_SUCCESS;
}

// get_rec_by_key
// Search for index entry in BST by calling bst_search
// Seek to the file location based on offset in index entry
// Read the record from the current location
int get_rec_by_key( int key, void *rec ){
	struct BST_Node *temp = bst_search(repo_handle.ndx_root,key);
	if(!temp) return PDS_REC_NOT_FOUND;
	struct PDS_NdxInfo *info=(struct PDS_NdxInfo*)temp->data;
	fseek(repo_handle.pds_data_fp,info->offset,SEEK_SET);
	fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
	return PDS_SUCCESS;
}

void bst_preorder(struct BST_Node *root){
	if (!root) return;
	fwrite(root->data,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
	bst_preorder(root->left_child);
	bst_preorder(root->right_child);
}
 
// pds_close
// Open the index file in wb mode (write mode, not append mode)
// Store the number of records
// Unload the ndx_array into the index file by traversing the BST in pre-order mode (overwrite the entire index file)
// Think why should it NOT be in-order?
// Close the index file and data file
int pds_close(){
	char indexfile[35];
	strcpy(indexfile,repo_handle.pds_name);
	strcat(indexfile,".ndx");
	FILE* temp = fopen(indexfile,"wb");
	fwrite(&repo_handle.rec_count,sizeof(int),1,temp);
	bst_preorder(repo_handle.ndx_root);
	fclose(temp);
	fclose(repo_handle.pds_data_fp);
	repo_handle.repo_status = PDS_REPO_CLOSED;
	bst_destroy(repo_handle.ndx_root);
	return PDS_SUCCESS;
}
