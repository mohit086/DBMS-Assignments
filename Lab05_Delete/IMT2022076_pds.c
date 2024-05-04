#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pds.h"

struct PDS_RepoInfo repo_handle;

// Open the data file and index file in "wb" mode
// Initialize index file by storing "0" to indicate there are zero entries in index file
// Close the files
int pds_create(char* repo_name){
	char datafile[35], indexfile[35];
	strcpy(datafile, repo_name);
	strcpy(indexfile, repo_name);
	strcat(datafile, ".dat");
	strcat(indexfile, ".ndx");

	FILE* pds_data_fp = fopen(datafile, "wb");
	FILE* pds_ndx_fp = fopen(indexfile, "wb");
	if (pds_data_fp == NULL || pds_ndx_fp == NULL) return PDS_FILE_ERROR;

	int temp = 0;
	fwrite(&temp, sizeof(int), 1, pds_ndx_fp);
	fclose(pds_data_fp);
	fclose(pds_ndx_fp);
	return PDS_SUCCESS;
}

// pds_load_ndx - Internal function
// Load the index entries into the BST ndx_root by calling bst_add_node repeatedly for each
// index entry. Unlike array, for BST, you need read the index file one by one in a loop
int pds_load_ndx(){
	for (int i = 0; i < repo_handle.rec_count; i++){
		struct PDS_NdxInfo* data = malloc(sizeof(struct PDS_NdxInfo));
		fread(data, sizeof(struct PDS_NdxInfo), 1, repo_handle.pds_ndx_fp);
		bst_add_node(&repo_handle.ndx_root, data->key, data);
	}
	return PDS_SUCCESS;
}

// Open the data file and index file in rb+ mode
// Update the fields of PDS_RepoInfo appropriately
// Read the number of records form the index file
// Load the index into the array and store in ndx_array by reading index entries from the index file
// Close only the index file
int pds_open(char* repo_name, int rec_size){
	if(repo_handle.repo_status == PDS_REPO_OPEN) return PDS_REPO_ALREADY_OPEN;
	char datafile[35], indexfile[35];
	strcpy(datafile, repo_name);
	strcpy(indexfile, repo_name);
	strcat(datafile, ".dat");
	strcat(indexfile, ".ndx");

	repo_handle.pds_data_fp = fopen(datafile, "rb+");
	repo_handle.pds_ndx_fp = fopen(indexfile, "rb+");
	if (repo_handle.pds_data_fp == NULL || repo_handle.pds_ndx_fp == NULL) return PDS_FILE_ERROR;

	repo_handle.repo_status = PDS_REPO_OPEN;
	repo_handle.rec_size = rec_size;
	strcpy(repo_handle.pds_name, repo_name);

	int count;
	fread(&count, sizeof(int), 1, repo_handle.pds_ndx_fp);
	repo_handle.rec_count = count;
	pds_load_ndx();
	fclose(repo_handle.pds_ndx_fp);
	return PDS_SUCCESS;
}

// Seek to the end of the data file
// Create an index entry with the current data file location using ftell
// Add index entry to BST by calling bst_add_node
// Increment record count
// Write the record at the end of the file
// Return failure in case of duplicate key
int put_rec_by_key(int key, void* rec){
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_ADD_FAILED;
	fseek(repo_handle.pds_data_fp, 0, SEEK_END);
	int offset = ftell(repo_handle.pds_data_fp);
	struct PDS_NdxInfo* data = malloc(sizeof(struct PDS_NdxInfo));
	data->key = key;
	data->offset = offset;
	data->is_deleted = 0;
	int status = bst_add_node(&repo_handle.ndx_root, data->key, data);

	struct BST_Node* node = bst_search(repo_handle.ndx_root,key);
	if(!node) return PDS_ADD_FAILED;

	struct PDS_NdxInfo* temp = node->data; 
	if (status == BST_DUP_KEY && !(temp->is_deleted)) return PDS_ADD_FAILED;
	if(temp->is_deleted){
		temp->is_deleted=0;
		temp->offset = offset;
	}
	repo_handle.rec_count++;
	fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp);
	fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
	return PDS_SUCCESS;
}

// Search for index entry in BST by calling bst_search
// Seek to the file location based on offset in index entry
// Read the record from the current location
int get_rec_by_ndx_key(int key, void *rec){
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_FILE_ERROR;
	struct BST_Node* temp = bst_search(repo_handle.ndx_root, key);
	if (temp == NULL) return PDS_REC_NOT_FOUND;
	struct PDS_NdxInfo* data = temp->data;
	if(data->is_deleted) return PDS_REC_NOT_FOUND;
	fseek(repo_handle.pds_data_fp, data->offset, SEEK_SET);
	fread(&key, sizeof(int), 1, repo_handle.pds_data_fp);
	fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
	return PDS_SUCCESS;
}

int get_rec_by_non_ndx_key(void* non_ndx_key, void* rec, int (*matcher)(void* rec, void* non_ndx_key), int* io_count){
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_FILE_ERROR;
	fseek(repo_handle.pds_data_fp,0,SEEK_SET);
	(*io_count)=0;
	int tempkey=0;
	fread(&tempkey,sizeof(int),1,repo_handle.pds_data_fp);
	while (fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp)==1){
		(*io_count)++;
		if(!matcher(rec,non_ndx_key)){
			struct BST_Node* node = bst_search(repo_handle.ndx_root,tempkey);
			if(!node) return PDS_REC_NOT_FOUND;
			struct PDS_NdxInfo* temp = node->data;
			if(temp->is_deleted) return PDS_REC_NOT_FOUND;
			return PDS_SUCCESS;
		}
		fread(&tempkey,sizeof(int),1,repo_handle.pds_data_fp);
	}
	return PDS_REC_NOT_FOUND;
}

int delete_rec_by_ndx_key( int key ){
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_FILE_ERROR;

	struct BST_Node* temp = bst_search(repo_handle.ndx_root, key);
	if(temp==NULL) return PDS_DELETE_FAILED;
	struct PDS_NdxInfo* data = temp->data;
	if(data->is_deleted) return PDS_DELETE_FAILED;
	data->is_deleted = 1;
	return PDS_SUCCESS;
}

void preorder(struct BST_Node* root, FILE* pds_ndx_fp){
	if (root == NULL) return;
	struct PDS_NdxInfo* info = (struct PDS_NdxInfo*)(root->data);
	if(info->is_deleted == 0) fwrite(info, sizeof(struct PDS_NdxInfo), 1, pds_ndx_fp);
	preorder(root->left_child, pds_ndx_fp);
	preorder(root->right_child, pds_ndx_fp);
}

// Open the index file in wb mode (write mode, not append mode)
// Store the number of records
// Unload the ndx_array into the index file by traversing the BST in pre-order mode (overwrite the entire index file)
// Think why should it NOT be in-order?
// Close the index file and data file
int pds_close(){
	char indexfile[35];
	strcpy(indexfile, repo_handle.pds_name);
	strcat(indexfile, ".ndx");
	FILE* pds_ndx_fp = fopen(indexfile, "wb");
	repo_handle.repo_status = PDS_REPO_CLOSED;
	fwrite(&repo_handle.rec_count, sizeof(int), 1, pds_ndx_fp);
	preorder(repo_handle.ndx_root, pds_ndx_fp);
	bst_destroy(repo_handle.ndx_root);
	fclose(repo_handle.pds_data_fp);
	fclose(repo_handle.pds_ndx_fp);
	return PDS_SUCCESS;
}