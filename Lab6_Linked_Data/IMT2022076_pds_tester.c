#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pds.h"
#include "structs.h"

#define SUCCESS 0
#define FAILURE 1

#define TREPORT(a1,a2) printf("Status: %s - %s\n\n",a1,a2); fflush(stdout);

void process_line(char *test_case);

int comparator (const void * a, const void * b){
   return (*(int*)a - *(int*)b);
}

int main(int argc, char *argv[]){
	FILE *cfptr;
	char test_case[50];

	if( argc != 2 ){
		fprintf(stderr, "Usage: %s testcasefile\n", argv[0]);
		exit(1);
	}

	cfptr = (FILE *) fopen(argv[1], "r");
	while(fgets(test_case, sizeof(test_case)-1, cfptr)){
		if(!strcmp(test_case,"\n") || !strcmp(test_case,"")) continue;
		process_line(test_case);
	}
	return 0;
}

void process_line(char *test_case){
	char repo_name[30], linked_repo_name[30];
	char command[15], info[1024], param1[15], param2[15], param3[15];
	int p_id, c_id, act_status, rec_size, ex_status, linked_rec_size;
	struct Shop test_shop;
	struct Product test_product;
	int arr[100];

	rec_size = sizeof(struct Shop);
	linked_rec_size = sizeof(struct Product);

	sscanf(test_case, "%s%s%s%s", command, param1, param2, param3); //splits testcase into 4 arguments
	if(!strcmp(command,"GET_LINK")){ // if it is get link, take additional inputs
		char temp[30];
		strcpy(temp,test_case);
		strcpy(command,strtok(temp," "));
		strcpy(param1,strtok(NULL," "));
		strcpy(param2,strtok(NULL," "));
		if(!strcmp(param2,"0")){
			strcpy(param3,strtok(NULL," "));
			int n = atoi(param3);
			for(int i =0;i<n;i++){
				char st[30];
				strcpy(st,strtok(NULL," "));
				arr[i] = atoi(st);
			}
		}
	}
	printf("Test case: %s", test_case);
	fflush(stdout);


	if(!strcmp(command,"CREATE")){ // if create command
		strcpy(repo_name, param1);
		strcpy(linked_repo_name,param2);

		if(!strcmp(param3,"0")) ex_status = SUCCESS;
		else ex_status = FAILURE;

		if(!strcmp(linked_repo_name,"NULL")) act_status = pds_create(repo_name,NULL);
		else act_status = pds_create(repo_name,linked_repo_name);

		if(act_status == PDS_SUCCESS) act_status = SUCCESS;
		else act_status = FAILURE;
		if(act_status == ex_status) {TREPORT("PASS","");}
		else{
			sprintf(info,"pds_create returned status %d",act_status);
			TREPORT("FAIL", info);
		}
	}


	else if(!strcmp(command,"OPEN")){ // call pds_open
		strcpy(repo_name, param1);
		strcpy(linked_repo_name, param2);

		if(!strcmp(param3,"0")) ex_status = SUCCESS;
		else ex_status = FAILURE;

		if(!strcmp(linked_repo_name,"NULL")) act_status = pds_open(repo_name, NULL, rec_size,linked_rec_size);
		else act_status = pds_open(repo_name, linked_repo_name, rec_size,linked_rec_size);

		if(act_status == PDS_SUCCESS) act_status = SUCCESS;
		else act_status = FAILURE;
		if(act_status == ex_status) {TREPORT("PASS", "");}
		else{
			sprintf(info,"pds_open returned status %d",act_status);
			TREPORT("FAIL", info);
		}
	}



	else if(!strcmp(command,"STORE_PARENT")){ // used put_rec_by_key
		if(!strcmp(param2,"0")) ex_status = SUCCESS;
		else ex_status = FAILURE;
		sscanf(param1, "%d", &p_id);

		sprintf(test_shop.id, "%d", p_id);
		sprintf(test_shop.name, "shop_%d", p_id);
		act_status = put_rec_by_key(p_id, &test_shop );
		if(act_status == PDS_SUCCESS) act_status = SUCCESS;
		else act_status = FAILURE;
		if( act_status == ex_status ) {TREPORT("PASS","");}
		else{
			sprintf(info,"put_rec_by_key returned status %d",act_status);
			TREPORT("FAIL", info);
		}
	}



	else if( !strcmp(command,"STORE_CHILD") ){ // same, just replace put_rec with put_linked_rec
		if(!strcmp(param2,"0")) ex_status = SUCCESS;
		else ex_status = FAILURE;
		sscanf(param1, "%d", &c_id);

		sprintf(test_product.id, "%d", c_id);
		sprintf(test_product.name, "product_%d", c_id);
		act_status = put_linked_rec_by_key(c_id, &test_product);
		if(act_status == PDS_SUCCESS) act_status = SUCCESS;
		else act_status = FAILURE;
		if( act_status == ex_status ) {TREPORT("PASS", "");}
		else{
			sprintf(info,"put_linked_rec_by_key returned status %d",act_status);
			TREPORT("FAIL", info);
		}
	}



	else if(!strcmp(command,"ADD_LINK")){ // used pds_link_rec
		if(!strcmp(param3,"0")) ex_status = SUCCESS;
		else ex_status = FAILURE;

		sscanf(param1,"%d",&p_id);
		sscanf(param2,"%d",&c_id);

		act_status = pds_link_rec(p_id,c_id);
		if(act_status == PDS_SUCCESS) act_status = SUCCESS;
		else act_status = FAILURE;

		if(act_status == ex_status) {TREPORT("PASS","");}
		else{
			sprintf(info,"link_rec returned status %d",act_status);
			TREPORT("FAIL", info);
		}
	}



	else if(!strcmp(command,"GET_LINK")){
		int count = 0;
		sscanf(param1,"%d",&p_id);
		sscanf(param3,"%d",&count);

		if(!strcmp(param2,"0")) ex_status = SUCCESS;
		else ex_status = FAILURE;

		int linked_keys_result[15];
		int result_set_size = 0;
		act_status = pds_get_linked_rec(p_id,linked_keys_result,&result_set_size);

		if(act_status!=0) act_status = FAILURE;
		if(act_status!=ex_status && act_status == FAILURE){
			sprintf(info,"pds_get_linked_rec failed");
			TREPORT("FAIL", info);
		}
		else if(act_status == ex_status && act_status == FAILURE) {TREPORT("PASS","");}
		else{
			if(result_set_size!=count){
				if(ex_status == FAILURE) {TREPORT("PASS","");}
				else{
					sprintf(info,"Result size not matching, got %d expected %d",result_set_size,count);
					TREPORT("FAIL", info);
				}
			}
			else{
				int flag = 1;

				qsort(linked_keys_result,result_set_size,sizeof(int),comparator);
				qsort(arr,result_set_size,sizeof(int),comparator);

				int temp = 1;
				for(int i=0;i<result_set_size;i++){
					if(arr[i]!=linked_keys_result[i]){
						temp=0;
						break;
					}
				}
				if(!temp){
					sprintf(info, "Links not matching");
					TREPORT("FAIL", info);
				}
				else{
					for(int i=0;i<count;i++){
						sprintf(test_product.id, "%d", linked_keys_result[i]);
						sprintf(test_product.name, "product_%d", linked_keys_result[i]);
						
						struct Product * temp = (struct Product *)malloc(sizeof(struct Product));
						act_status = get_linked_rec_by_key(linked_keys_result[i],temp);

						if(strcmp(temp->id,test_product.id) || strcmp(temp->name,test_product.name)){
							sprintf(info,"ID/Name not matching with expected value %s %s %s %s",temp->id,test_product.id,temp->name,test_product.name);
							TREPORT("FAIL", info);
							flag = 0;
							break;
						}
					}
					if(flag){
						if(ex_status == SUCCESS) {TREPORT("PASS","")}
						else TREPORT("FAIL","");
					}
				}
			}
		}
	}



	else if( !strcmp(command,"NDX_SEARCH") ){ // used get_rec_by_ndx_key
		if(!strcmp(param2,"0")) ex_status = SUCCESS;
		else ex_status = FAILURE;

		sscanf(param1, "%d", &p_id);
		act_status = get_rec_by_ndx_key( p_id, &test_shop );
		if(act_status == PDS_SUCCESS) act_status = SUCCESS;
		else act_status = FAILURE;
		if(act_status != ex_status){
			sprintf(info,"search key: %d; Got status %d",p_id, act_status);
			TREPORT("FAIL", info);
		}
		else{
			char ex_name[30];
    		sprintf(ex_name, "shop_%d", p_id);
			if(ex_status == 0){
				if (strcmp(test_shop.name,ex_name) == 0){TREPORT("PASS","");}
				else{
					sprintf(info,"Shop data not matching... Expected:{%d,%s} Got:{%s,%s}\n",p_id, ex_name, test_shop.id, test_shop.name);
					TREPORT("FAIL", info);
				}
			}
			else {TREPORT("PASS", "");}
		}
	}

	else if( !strcmp(command,"SEARCH_CHILD") ){ // used get_linked_rec_by_key
		if(!strcmp(param2,"0")) ex_status = SUCCESS;
		else ex_status = FAILURE;

		sscanf(param1, "%d", &c_id);
		act_status = get_linked_rec_by_key( c_id, &test_product );
		if(act_status == PDS_SUCCESS) act_status = SUCCESS;
		else act_status = FAILURE;
		if(act_status != ex_status){
			sprintf(info,"search key: %d; Got status %d",c_id, act_status);
			TREPORT("FAIL", info);
		}
		else{
			char ex_name[30];
    		sprintf(ex_name, "product_%d", c_id);
			if(ex_status == 0){
				if (strcmp(test_product.name,ex_name) == 0){TREPORT("PASS","");}
				else{
					sprintf(info,"Product data not matching... Expected:{%d,%s} Got:{%s,%s}\n",c_id, ex_name, test_product.id, test_product.name);
					TREPORT("FAIL", info);
				}
			}
			else {TREPORT("PASS", "");}
		}
	}


	
	else if(!strcmp(command,"NON_NDX_SEARCH")){
		char name[50], ex_name[50];
		int ex_io, act_io;
		
		if(strcmp(param2,"-1") == 0) ex_status = FAILURE;
		else ex_status = SUCCESS;

		sscanf(param1, "%s", name);
		sscanf(param2, "%d", &ex_io);
		
		strcpy(test_shop.id,"0");
		act_io = 0;
		act_status = get_rec_by_non_ndx_key(name, &test_shop, matcher, &act_io);
		if(act_status != PDS_SUCCESS) act_status = FAILURE;
		else act_status = SUCCESS;
		if(act_status != ex_status){
			sprintf(info,"expected status: %d; Got status %d",ex_status, act_status);
			TREPORT("FAIL", info);
		}
		else{
			sscanf(name + sizeof("shop"), "%d", &p_id);
			sprintf(ex_name,"shop_%d",p_id);
			if( ex_status == 0 ){
				if (strcmp(test_shop.name, ex_name) == 0){
						if(ex_io == act_io){TREPORT("PASS", "");}
						else{
							sprintf(info,"Num I/O not matching for shop %d... Expected: %d Got:%d\n",p_id, ex_io, act_io);
							TREPORT("FAIL", info);
						}
				}
				else{
					sprintf(info,"Shop data not matching... Expected:{%d,%s} Got:{%s,%s}\n",
						p_id, ex_name, 
						test_shop.id, test_shop.name
					);
					TREPORT("FAIL", info);
				}
			}
			else {TREPORT("PASS","");}
		}
	}



	else if( !strcmp(command,"NDX_DELETE") ){ // call delete_rec_by_ndx_key
		if(strcmp(param2,"0") == 0) ex_status = SUCCESS;
		else ex_status = FAILURE;

		sscanf(param1, "%d", &p_id);
		act_status = delete_rec_by_ndx_key( p_id );
		if(act_status) act_status=FAILURE;
		if( act_status != ex_status ){
			sprintf(info,"delete key: %d; Got status %d",p_id, act_status);
			TREPORT("FAIL", info);
		}
		else{TREPORT("PASS", "");}
	}



	else if(!strcmp(command,"CLOSE")){ // just call pds_close
		if(!strcmp(param1,"0")) ex_status = SUCCESS;
		else ex_status = FAILURE;

		act_status = pds_close();
		if(act_status == PDS_SUCCESS) act_status = SUCCESS;
		else act_status = FAILURE;
		if(act_status == ex_status) {TREPORT("PASS","");}
		else{
			sprintf(info,"pds_close returned status %d",act_status);
			TREPORT("FAIL", info);
		}
	}
}