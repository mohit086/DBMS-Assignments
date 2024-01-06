# include <stdio.h> 

struct student {
  int rollnum;
  char name[30];
  int age;
};

int save_num_text( char *filename ) {
  int arr[20];
  for(int i=0;i<20;i++) arr[i] = i+1;
  FILE *fp = fopen(filename,"w");
  if (fp == NULL) return 1;
  for(int i=0;i<20;i++) fprintf(fp,"%d ",arr[i]);
  fprintf(fp,"\n");
  fclose(fp);
  return 0;
}

int read_num_text( char *filename ) {
  FILE *fp = fopen(filename,"r");
  if (fp == NULL) return 1;
  int n;
  for(int i=0;i<20;i++){
    fscanf(fp,"%d",&n);
    printf("%d ",n);
  }
  printf("\n");
  fclose(fp);
  return 0;
}

int save_struct_text( char *filename ) {
  struct student list[5];
  for(int i=0;i<5;i++){
    struct student temp = {.rollnum = 1+i, .name = "xyz", .age = 21+i};
    list[i] = temp;
  }
  FILE *fp = fopen(filename,"w");
  if(fp == NULL) return 1;
  for(int i=0;i<5;i++){
    fprintf(fp,"%d ",list[i].rollnum);
    fprintf(fp,"%s ",list[i].name);
    fprintf(fp,"%d\n",list[i].age);
  }
  fclose(fp);
  return 0;
}

int read_struct_text( char *filename ) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) return 1;
  int a,b;
  char s[30];
  for(int i=0;i<5;i++){
    fscanf(fp,"%d %s %d",&a,s,&b);
    printf("%d %s %d\n",a,s,b);
  }
  fclose(fp);
  return 0;
}

int save_num_binary( char *filename ) {
  int arr[20];
  for(int i=0;i<20;i++) arr[i] = i+1;
  FILE *fp = fopen(filename,"wb");
  if (fp == NULL) return 1;
  fwrite(arr,sizeof(int),20,fp);
  fclose(fp);
  return 0;
}

int read_num_binary( char *filename ) {
  FILE *fp = fopen(filename,"rb");
  if (fp == NULL) return 1;
  int n;
  for(int i=0;i<20;i++){
    fread(&n,sizeof(int),1,fp);
    printf("%d ",n);
  }
  printf("\n");
  fclose(fp);
  return 0;
}

int save_struct_binary( char *filename ) {
  struct student list[5];
  for(int i=0;i<5;i++){
    struct student temp = {.rollnum = 1+i, .name = "xyz", .age = 21+i};
    list[i] = temp;
  }
  FILE *fp = fopen(filename,"wb");
  if(fp == NULL) return 1;
  fwrite(list,sizeof(struct student),5,fp);
  fclose(fp);
  return 0;
}

int read_struct_binary( char *filename ) {
  FILE *fp = fopen(filename,"rb");
  if (fp == NULL) return 1;
  struct student temp;
  for(int i=0;i<5;i++){
    fread(&temp,sizeof(struct student),1,fp);
    printf("%d %s %d\n",temp.rollnum,temp.name,temp.age);
  }
  fclose(fp);
  return 0;
}