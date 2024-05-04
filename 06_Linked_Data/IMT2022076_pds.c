#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pds.h"
#include "bst.h"

struct PDS_RepoInfo repo_handle;

// Open the data file and index file in "wb" mode
// Initialize index file by storing "0" to indicate there are zero entries in index file
// Close the files
int pds_create(char *repo_name, char* linked_repo_name){
	char datafile[35], indexfile[35], linked_datafile[35], link_file[70];
	strcpy(datafile, repo_name);
	strcpy(indexfile, repo_name);
	if(linked_repo_name!= NULL) strcpy(linked_datafile, linked_repo_name);
	if(linked_repo_name!=NULL){
		strcpy(link_file,datafile);
		strcat(link_file,"_");
		strcat(link_file,linked_datafile);
		strcat(link_file,".dat");
	}
	
	strcat(datafile, ".dat");
	strcat(indexfile, ".ndx");
	if(linked_repo_name!=NULL) strcat(linked_datafile,".dat");

	// giving names to all the 4 files

	FILE* pds_data_fp = fopen(datafile, "wb");
	FILE* pds_ndx_fp = fopen(indexfile, "wb");
	FILE* pds_linked_data_fp = NULL;
	FILE* pds_link_fp = NULL;
	if(linked_repo_name!=NULL) pds_linked_data_fp = fopen(linked_datafile,"wb");
	if(linked_repo_name!=NULL) pds_link_fp = fopen(link_file,"wb");
	if (pds_data_fp == NULL || pds_ndx_fp == NULL) return PDS_FILE_ERROR;

	int temp = 0;
	fwrite(&temp, sizeof(int), 1, pds_ndx_fp); //writing 0 in index file to indicate no index entries
	fclose(pds_data_fp);
	fclose(pds_ndx_fp);
	if(linked_repo_name!=NULL) fclose(pds_linked_data_fp);
	if(linked_repo_name!=NULL) fclose(pds_link_fp);
	return PDS_SUCCESS;
}

// Open the data file and index file in rb+ mode
// Update the fields of PDS_RepoInfo appropriately
// Read the number of records form the index file
// Load the index into the array and store in ndx_array by reading index entries from the index file
// Close only the index file
int pds_open(char *repo_name, char* linked_repo_name, int rec_size, int linked_rec_size){
	char datafile[35], indexfile[35], linked_datafile[35], link_file[70];
	strcpy(datafile, repo_name);
	strcpy(indexfile, repo_name);
	if(linked_repo_name!= NULL) strcpy(linked_datafile, linked_repo_name);

	if(linked_repo_name!=NULL){
		strcpy(link_file,datafile);
		strcat(link_file,"_");
		strcat(link_file,linked_datafile);
		strcat(link_file,".dat");
	}
	
	strcat(datafile, ".dat");
	strcat(indexfile, ".ndx");
	if(linked_repo_name!=NULL) strcat(linked_datafile,".dat");

	FILE* pds_data_fp = fopen(datafile, "rb+");
	FILE* pds_ndx_fp = fopen(indexfile, "rb+");
	FILE* pds_linked_data_fp = NULL;
	FILE* pds_link_fp = NULL;

	if(linked_repo_name!=NULL) pds_linked_data_fp = fopen(linked_datafile,"rb+");
	if(linked_repo_name!=NULL) pds_link_fp = fopen(link_file,"rb+");
	if (pds_data_fp == NULL || pds_ndx_fp == NULL) return PDS_FILE_ERROR;
	
	repo_handle.pds_data_fp = pds_data_fp; 
	repo_handle.pds_ndx_fp = pds_ndx_fp;
	repo_handle.pds_linked_data_fp = pds_linked_data_fp;
	repo_handle.pds_link_fp = pds_link_fp;

	repo_handle.repo_status = PDS_REPO_OPEN; // filling details
	repo_handle.rec_size = rec_size;
	repo_handle.linked_rec_size = linked_rec_size;
	strcpy(repo_handle.pds_name, repo_name);
	
	int count;
	fread(&count, sizeof(int), 1, pds_ndx_fp);
	repo_handle.rec_count = count;

	pds_load_ndx();
	fclose(repo_handle.pds_ndx_fp);
	repo_handle.pds_ndx_fp = NULL;
	return PDS_SUCCESS;
}

// pds_load_ndx - Internal function
// Load the index entries into the BST ndx_root by calling bst_add_node repeatedly for each
// index entry. Unlike array, for BST, you need read the index file one by one in a loop
int pds_load_ndx(){
	for (int i = 0; i < repo_handle.rec_count; i++){
		struct PDS_NdxInfo *data = malloc(sizeof(struct PDS_NdxInfo));
		fread(data, sizeof(struct PDS_NdxInfo), 1, repo_handle.pds_ndx_fp);
		bst_add_node(&repo_handle.ndx_root, data->key, data);
	}
	return PDS_SUCCESS;
}

// Seek to the end of the data file
// Create an index entry with the current data file location using ftell
// Add index entry to BST by calling bst_add_node
// Increment record count
// Write the record at the end of the file
// Return failure in case of duplicate key
int put_rec_by_key(int key, void *rec){
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_FILE_ERROR;
	fseek(repo_handle.pds_data_fp, 0, SEEK_END);
	int offset = ftell(repo_handle.pds_data_fp);
	struct PDS_NdxInfo *data = malloc(sizeof(struct PDS_NdxInfo));
	data->key = key;
	data->offset = offset;
	data->is_deleted = 0;
	int status = bst_add_node(&repo_handle.ndx_root, data->key, data);
	struct BST_Node * temp1 = bst_search(repo_handle.ndx_root,key);

	if(!temp1) return PDS_ADD_FAILED;
	struct PDS_NdxInfo * temp = temp1->data; 
	if (status == BST_DUP_KEY && !(temp->is_deleted)) return PDS_ADD_FAILED;
	if(temp->is_deleted){
		temp->is_deleted=0;
		temp->offset = offset;
	}

	repo_handle.rec_count++;
	fwrite(&key,sizeof(int),1,repo_handle.pds_data_fp);
	fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
	return PDS_SUCCESS;
}


// seek to the end of the link file
// put the key and the record at the end
// there is no indexfile for child
int put_linked_rec_by_key( int key, void *rec ){
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_FILE_ERROR;
	fseek(repo_handle.pds_linked_data_fp, 0, SEEK_END);
	fwrite(&key,sizeof(int),1,repo_handle.pds_linked_data_fp);
	fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_linked_data_fp);
	return PDS_SUCCESS;
}

// find the node in the indexfile using bst_search
// using the offset value from that node, seek to the correct location
// read the key and data if the record is not deleted
int get_rec_by_ndx_key(int key, void *rec){
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_FILE_ERROR;
	struct BST_Node *temp = bst_search(repo_handle.ndx_root, key);
	if (temp == NULL) return PDS_REC_NOT_FOUND;
	struct PDS_NdxInfo *data = temp->data;
	if(data->is_deleted) return PDS_REC_NOT_FOUND;
	fseek(repo_handle.pds_data_fp, data->offset, SEEK_SET);
	fread(&key,sizeof(int),1,repo_handle.pds_data_fp);
	fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
	return PDS_SUCCESS;
}

// linear search through file to look for non-ndx-key, if record is not deleted store it
int get_rec_by_non_ndx_key(void *non_ndx_key, void *rec, int (*matcher)(void *rec, void *non_ndx_key), int *io_count){
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_FILE_ERROR;
	fseek(repo_handle.pds_data_fp,0,SEEK_SET);
	(*io_count)=0;
	int temp_key=0;
	fread(&temp_key,sizeof(int),1,repo_handle.pds_data_fp);
	while (fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp)==1){
		(*io_count)++;
		if(!matcher(rec,non_ndx_key)){
			struct BST_Node * temp1 = bst_search(repo_handle.ndx_root,temp_key);
			if(!temp1) return PDS_ADD_FAILED;
			struct PDS_NdxInfo * temp = (struct PDS_NdxInfo*)(temp1->data);
			if(temp->is_deleted) return PDS_REC_NOT_FOUND;
			if(!(temp->is_deleted)) return PDS_SUCCESS;
		}
		fread(&temp_key,sizeof(int),1,repo_handle.pds_data_fp);
	}
	return PDS_REC_NOT_FOUND;
}

// here there is no bst, so just do a linear search for the key in the linked_datafile
// read the key and data of the record once you find it
int get_linked_rec_by_key(int key, void *rec){
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_FILE_ERROR;
	fseek(repo_handle.pds_linked_data_fp,0,SEEK_SET);
	while(!feof(repo_handle.pds_linked_data_fp)){
        int temp;
        fread(&temp,sizeof(key),1,repo_handle.pds_linked_data_fp);
		if(feof(repo_handle.pds_linked_data_fp)) break;
        if(temp==key){
        	fread(rec,repo_handle.linked_rec_size,1,repo_handle.pds_linked_data_fp);
			return PDS_SUCCESS;
		}
		else fseek(repo_handle.pds_linked_data_fp,repo_handle.linked_rec_size,SEEK_CUR);
		
    }
    return PDS_REC_NOT_FOUND;
}

// search the key, and change the is_deleted flag to 1
int delete_rec_by_ndx_key(int key){
	struct BST_Node *temp = bst_search(repo_handle.ndx_root, key);
	if(temp==NULL) return PDS_DELETE_FAILED;
	struct PDS_NdxInfo *data = temp->data;
	if(data->is_deleted) return PDS_DELETE_FAILED;
	data->is_deleted = 1;
	return PDS_SUCCESS;
}

int pds_link_rec (int key1, int key2){ // check if both parent and child exist and are not deleted, then link
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_FILE_ERROR;
	struct PDS_link_info * temp = malloc(sizeof(struct PDS_link_info));
	struct BST_Node* tmp = bst_search(repo_handle.ndx_root,key1);
	if(tmp ==NULL) return PDS_ADD_FAILED;
	struct PDS_NdxInfo *data = tmp->data;
	if(data->is_deleted) return PDS_ADD_FAILED;
	int is_child = 0;
	fseek(repo_handle.pds_linked_data_fp,0,SEEK_SET);

	while(!feof(repo_handle.pds_linked_data_fp)){
        int t;
        fread(&t,sizeof(int),1,repo_handle.pds_linked_data_fp);
		if(feof(repo_handle.pds_linked_data_fp)) break;
        if(t == key2){
			is_child =1;
			break;
		}
		fseek(repo_handle.pds_linked_data_fp,repo_handle.linked_rec_size,SEEK_CUR);

    }
	if(!is_child) return PDS_ADD_FAILED;
	temp->parent_key = key1;
	temp->child_key = key2;

	fseek(repo_handle.pds_link_fp,0,SEEK_END);
	fwrite(temp,sizeof(struct PDS_link_info),1,repo_handle.pds_link_fp);
	return PDS_SUCCESS;
}

int pds_get_linked_rec(int parent_key, int linked_keys_result[], int* result_set_size){
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_FILE_ERROR;
	fseek(repo_handle.pds_link_fp,0,SEEK_SET);
	struct BST_Node* temp = bst_search(repo_handle.ndx_root,parent_key);
	if(temp==NULL) return PDS_REC_NOT_FOUND;
	struct PDS_NdxInfo *data = temp->data;
	if(data->is_deleted) return PDS_REC_NOT_FOUND;
	(*result_set_size) = 0;

	while(!feof(repo_handle.pds_link_fp)){
		struct PDS_link_info *temp1 = malloc(sizeof(struct PDS_link_info));
		fread(temp1,sizeof(struct PDS_link_info),1,repo_handle.pds_link_fp);
		if(temp1->parent_key == parent_key){
			linked_keys_result[*result_set_size] = temp1->child_key;
			(*result_set_size)++;
		}
	}
	return PDS_SUCCESS;
}

void preorder(struct BST_Node *root, FILE* pds_ndx_fp){
	if (root == NULL) return;
	struct PDS_NdxInfo * temp = root->data;
	if(!temp->is_deleted) fwrite(root->data, sizeof(struct PDS_NdxInfo), 1, pds_ndx_fp);
	preorder(root->left_child, pds_ndx_fp);
	preorder(root->right_child, pds_ndx_fp);
}

int pds_close(){
	if(repo_handle.repo_status == PDS_REPO_CLOSED) return PDS_FILE_ERROR;
	char indexfile[35];
	strcpy(indexfile, repo_handle.pds_name);
	strcat(indexfile, ".ndx");
	FILE* pds_ndx_fp = fopen(indexfile, "wb");
	fwrite(&repo_handle.rec_count, sizeof(int), 1, pds_ndx_fp);
	preorder(repo_handle.ndx_root, pds_ndx_fp);
	bst_destroy(repo_handle.ndx_root);

	fclose(repo_handle.pds_data_fp);
	fclose(pds_ndx_fp);
	if(repo_handle.pds_link_fp!=NULL) fclose(repo_handle.pds_link_fp);
	if(repo_handle.pds_link_fp!=NULL) fclose(repo_handle.pds_linked_data_fp);

	repo_handle.repo_status = PDS_REPO_CLOSED;
	return PDS_SUCCESS;
}
