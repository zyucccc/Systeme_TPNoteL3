#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define Size_Buf 4096

//etape2   obtenir et appliquer la permission
void appliquepermission(int infd,int outfd){
struct stat st;
if(fstat(infd,&st)==-1)
{printf("error permission\n");exit(1);}
 if(fchmod(outfd,st.st_mode)==-1)
 {printf("error application\n");exit(1);}
 printf("Permission bien appliqué\n");
}

//etape 1  fonction copie -- files
void copy(int infd,int outfd){
     char buf[Size_Buf];
     //etape 2 appliqué
    appliquepermission(infd,outfd);
    int size = read(infd,buf,sizeof(buf));
  while(size > 0)
  {
      write(outfd,buf,size);
      size = read(infd,buf,sizeof(buf));
  }
  printf("Copie réussie\n");
  close(infd);
  close(outfd);
}

//etape 4  Distinguer les répertoires des fichiers
int distinguer(char* infd,char* outfd){
    struct stat inst;
    // struct stat outst; 
    if( stat(infd,&inst)==-1 )
    {printf("error permission--distinguer\n");exit(1);}
    // stat (outfd,&outst);
      if (S_ISREG (inst.st_mode)/*  && S_ISREG (outst.st_mode) */) { //si entree file,return 0
      printf("entree file\n");
        return 0;
    }
    else if (S_ISDIR (inst.st_mode) /* && S_ISDIR (outst.st_mode) */) { //si entree directory,return 1
    printf("entree directory\n");
        return 1;
    }
    return 2;
}


//etape 4 fonction copie -- repertoires
void copy2(char* in,char * out){
struct stat sta;
    DIR * indir;
    struct dirent * inptr;
    indir = opendir(in);
    if(indir == NULL /*|| outdir == NULL*/){printf("error open repertoire\n");exit(0);}
    while ((inptr = readdir(indir)) != NULL) {
            //etape 3  path = directory + / + filename
            if(strcmp(inptr->d_name,".") != 0 && strcmp(inptr->d_name,"..") != 0){
                char* infilename = inptr->d_name;
                printf("NAME: %s\n",infilename);
                char* inpath = (char*)malloc(sizeof(char)*(strlen(infilename)+strlen(in)+1));
                char* outpath = (char*)malloc(sizeof(char)*(strlen(infilename)+strlen(out)+1));
                strcpy(inpath,in);
                strcat(inpath,"/");
                strcat(inpath,infilename);
                strcpy(outpath,out);
                strcat(outpath,"/");
                strcat(outpath,infilename);
                
                //si l'entree est directory,on appel recursive la fonction
             if(distinguer(inpath,outpath)==1){
                //creer les sous-repertoires dans repertoires destinees
                if(stat(outpath,&sta)==-1)
             {  
                stat(inpath,&sta);
                mkdir(outpath,sta.st_mode);
            }
                // appel recursives
               copy2(inpath,outpath);
             }
             //si l'entree est file,on copie directement
             else if((distinguer(inpath,outpath)==0)){
            int infd = open(inpath,O_RDONLY);
            int outfd = open(outpath,O_CREAT|O_WRONLY,0777);
             if (infd == -1 || outfd == -1){
             printf("error open fichier\n");exit(0);
             }
             copy(infd,outfd);
             }
             free(inpath);
             free(outpath);
            }
        }

    closedir(indir);

}


int main(int argc,char *argv[]){
    if(distinguer(argv[1],argv[2])==0){
    //si l'entree est file,on execute ci-dessous
        int infd = open(argv[1],O_RDONLY);
        int outfd = open(argv[2],O_CREAT|O_WRONLY,0777);
    if(infd == -1 || outfd == -1){
    printf("error\n");return -1;
}
    copy(infd,outfd);
    return 0;
    }
    //si l'entree est directory,on execute ci-dessous
    else if (distinguer(argv[1],argv[2])==1){
    copy2(argv[1],argv[2]);
}

else {printf("error entree\n");return -1;}
    
    printf("Fin\n");
    return 0;
}
