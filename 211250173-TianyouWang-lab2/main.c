//
// Created by 86181 on 2023/4/14.
//
#include<stdio.h>
#include<stdbool.h>
#include<string.h>

extern void white_cout(char* string,int size);
extern void red_cout(char* string,int size);
extern void blue_cout(char* string,int size);
char string[512];
// 扇区大小
#define BPB_BytsPerSec 512
#define BPB_SecPerClus 1
#define BPB_NumFATs 2
#define SectorNoOfRootDirectory	19
#define BPB_RsvdSecCnt 1
#define BPB_FATSz16 9
// 根目录区的起始扇区号
#define SectorNoOfRootDirectory 19
#define RootDirSectors 14	 //根目录占用空间

// 文件夹和文件的属性值
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE   0x20
char add[128]={0};

// 目录项结构体
struct directory_entry {
    unsigned char name[8];         // 文件名
    unsigned char ext[3];          // 扩展名
    unsigned char attr;            // 属性
    unsigned char reserved[10];    // 保留字段
    unsigned short time;           // 创建时间
    unsigned short date;           // 创建日期
    unsigned short cluster;        // 起始簇号
    unsigned int size;             // 文件大小
};

int find_target_folder(FILE *fp,int cluster,int* add_place);
void ls(FILE* fp,int cluster,char *path);
void ls_l(FILE* fp,int cluster,char* path);
void print_sub_file_num(FILE* fp,int cluster);
int have_next_cluster(FILE* fp,int cluster);
void cat(FILE* fp,int cluster);
void print_num(int num);

int main(){
    while(1){
        bool is_ls=false;
        bool is_l=false;
        bool is_cat=false;
        bool is_exi=false;
        bool has_add=false;
        int err_mes_add=0;
        int err_mes_len=0;

        char str[128];
        
        int add_len=0;
        for(int i=0;i<127;++i){
            str[i]=' ';
            add[i]=' ';
        }
        
        for(int i=0;i<512;i++){
            string[i]=0;
        }
        strcpy(string,"input your command:");
        blue_cout(string,19);

        scanf("%[^\n]",str);
        char new_line;
        new_line=getchar();
		
        for(int i=0;i<127;++i){
            if(str[i]=='e'&&(!is_cat)&&(!is_ls)){
                if(i<125&&str[i+1]=='x'&&str[i+2]=='i'&&str[i+3]=='t'&&(str[i+4]==' '||str[i+4]==0||str[i+5]=='\n')){
                    is_exi=true;
                    break;
                }else{
                    err_mes_add=i;
                    for(;i<128&&str[i]!=' '&&str[i]!='\n';++i){}
                        err_mes_len=i-err_mes_add;
                        break;
                }
            }else if(str[i]=='l'){
                ++i;
                if(i<128&&str[i]=='s'){
                    ++i;
                    if(i<128&&(str[i]==' '||str[i]=='\n'||str[i]==0)){
                        is_ls=true;
                        --i;
                    }else{
                        err_mes_add=i-2;
                        err_mes_len=2;
                        for(;str[i]!=' '&&str[i]!='\n'&&str[i]!=0;++i){
                            err_mes_len++;
                        }
                        break;
                    }
                }else{
                    err_mes_add=i-1;
                    if((i==128)||str[i]==' '||str[i]=='\n'||str[i]==0){
                        err_mes_len=1;
                    }
                    err_mes_len=2;
                    break;
                }
            }else if(str[i]=='c'){
                ++i;
                if(i<128&&str[i]=='a'){
                    ++i;
                    if(i<128&&str[i]=='t'){
                        ++i;
                        if(i==128||(str[i]==' '||str[i]=='\n'||str[i]==0)){
                            is_cat=true;
                            --i;
                        }else{
                            //not space
                            err_mes_add=i-3;
                            for(;i<128&&str[i]!=' '&&str[i]!='\n'&&str[i]!=0;++i){}
                            err_mes_len=i-err_mes_add;
                            break;
                        }
                    }else{
                        //not 't'
                        err_mes_add=i-2;
                        if(str[i]==' '||str[i]=='\n'||str[i]==0){
                            err_mes_len=2;
                            break;
                        }else{
                            err_mes_len=3;
                            break;
                        }
                    }
                }else{
                    //not 'a'
                    err_mes_add=i-1;
                    if(str[i]==' '||str[i]=='\n'||str[i]==0){
                        err_mes_len=1;
                        break;
                    }else{
                        err_mes_len=2;
                        break;
                    }
                }
            }else if(i<128&&(str[i]=='\n'||str[i]==0)){
                break;
            }else if(i<128&&str[i]=='-'){
            	if(is_ls){
					++i;
                    bool hasError=false;
                    if(str[i]==' '||str[i]=='\n'||str[i]==0){
                        hasError=true;
                        err_mes_len=1;
                        err_mes_add=i-1;
                    }else{
                    	for(;i<128&&str[i]!=' '&&str[i]!='\n'&&str[i]!=0;++i){
                    	    if(i<128&&str[i]=='l'){
                    	        is_l=true;
                    	    }else{
                    	        err_mes_add=i;
                	            err_mes_len=1;
                	            hasError=true;
                	            break;
                   		    }
                	    }
                    }
                	if(hasError==true){
                	    break;
                	}
            	}else{
            		err_mes_add=i;
            		for(;i<128&&str[i]!=' '&&str[i]!='\n'&&str[i]!=0;++i){}
            		err_mes_len=i-err_mes_add;
            		break;
				}
            }else if(str[i]==' '){
                continue;
            }else{
                if((is_ls||is_cat)&&has_add==false){
                    for(;i<128&&str[i]!=' '&&str[i]!='\n'&&str[i]!=0;++i){
                        if(str[i]<123&&str[i]>96){
                            str[i]=(char)(str[i]-32);
                        }
                        add[add_len++]=str[i];
                    }
                    has_add=true;
                }else{
                    err_mes_add=i;
                    for(;i<128&&str[i]!=' '&&str[i]!='\n'&&str[i]!=0;++i){}
                    err_mes_len=i-err_mes_add;
                    break;
                }
            }
        }

        if(is_exi==true){
            break;
        }else if(err_mes_len!=0){
            //have error
            white_cout("message:",8);
            white_cout(str+err_mes_add,err_mes_len);
            white_cout("\n",1);
        }else{
            //delete the last '/' in the path,to better parse
            if(add[add_len-1]=='/'){
                add[--add_len]=0;
            }

            FILE* fp = fopen("a.img", "rb");
            if (fp == NULL) {
                white_cout("Failed to open file\n",20);
                return 1;
            }else{
                //white_cout("Opened file\n",12);
            }
            
            int cluster_target=0;
            char path[512];
            for(int i=0;i<512;++i){
                path[i]=0;
            }

            // 定位到根目录区的起始位置
            fseek(fp, SectorNoOfRootDirectory * BPB_BytsPerSec, SEEK_SET);
            if(add_len==0){
                cluster_target=2;
                strcpy(path,"/");
            }else{
                int cluster_yet=0;
                cluster_target = find_target_folder(fp,2,&cluster_yet);
            }
            if(cluster_target==-1){
                white_cout("message:file_name_too_long\n",27);
                return 0;
            }else if(cluster_target==-2){
                white_cout("message:file_ext_too_long\n",26);    
            }else if(cluster_target==-3){
                white_cout("no folder: ",11);
                white_cout(add,add_len);
                white_cout("\n",1);
                continue;
            }else{
                //white_cout("has folder ",11);
                //white_cout("\n",1);
                int q=0;
                
                if(add[0]!='/'){
                    path[q++]='/';
                }
                for(int p=0;add[p]!=' '&&add[p]!=0;++p){
                    if(strncmp(add+p,"..",2)==0){
                        if(add[p+2]=='/'||add[p+2]==0||add[p+2]==' '){
                            if(add[p+2]=='/'){
                                p+=2;
                            }else{
                                p+=1;
                            }
                            if(q!=0&&(q>1||path[0]!='/')){
                                q--;
                                path[q]=0;
                                for(;path[q]!='/'&&q>=0;q--){
                                    path[q]=0;
                                }
                                q++;
                            }    
                        }else{
                            path[q++]=add[p];
                        }
                    }else if(strncmp(add+p,".",1)==0){
                        if(add[p+1]=='/'||add[p+1]==0||add[p+1]==' '){
                            if(add[p+1]=='/'){
                                p+=1;
                            }
                        }else{
                            path[q++]=add[p];
                        }
                    }else{
                        path[q++]=add[p];
                    }
                }
                if(path[q-1]!='/'){
                    path[q++]='/';
                }
            }

            if(is_cat){ 
                if(cluster_target==0){
                    white_cout("null\n",5);
                }else{
                    cat(fp,cluster_target);
                }
            }else if(is_ls){
                if(cluster_target==0){
                    white_cout("is a file",9);
                }
                
                if(is_l){
                    ls_l(fp,cluster_target,path);
                }else{
                    ls(fp,cluster_target,path);
                }
            }
            fclose(fp);
        }
    }
}

// 查找target folder
int find_target_folder(FILE *fp,int cluster,int* add_place) {
    int cluster_now=cluster;
    struct directory_entry entry;

    //allocate fp point
    if(cluster_now==2){
        fseek(fp,SectorNoOfRootDirectory*BPB_BytsPerSec,SEEK_SET);
    }else{
        fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec, SEEK_SET);
    }

    //地址解析完毕，返回当前簇号
    if(add[*add_place]==0||add[*add_place]==' '){
        return cluster;
    }
    
    if(add[*add_place]=='/'){
        *add_place+=1;
    }

    //name：要寻文件（夹）的名字
    unsigned char name[8]={' '};
    for(int p=0;p<8;++p){
        name[p]=' ';
    }
    unsigned char ext[3]={' '};
    for(int i=0;i<3;++i){
        ext[i]=' ';
    }
    for(int i=0;add[*add_place]!='/'&&add[*add_place]!=0&&add[*add_place]!=' ';++i){
        if(add[*add_place]=='.'&&add[*add_place+1]!='.'&&add[*add_place+1]!=' '&&add[*add_place+1]!='/'&&add[*add_place+1]!=0){
            *add_place+=1;
            for(int j=0;add[*add_place]!=0&&add[*add_place]!=' ';++j){
                if(j>=3){
                    //ext name too long 
                    return -2;
                }
                if(add[*add_place]=='/'){
                    return -3;
                }
                ext[j]=add[*add_place];
                *add_place+=1;
            }
            break;
        }
        if(i>=8){
            //file name too long
            return -1;
        }
        name[i]=add[*add_place];
        *add_place +=1;
    }

    if(strncmp(name,"..",(size_t)2)==0){
        if(name[3]==' '||name[3]==0){
            if(cluster==2){
                return find_target_folder(fp,2,add_place);
            }
            return 1;
        }
    }else if(strncmp(name,".",(size_t)1)==0){
        if(name[2]==' '||name[2]==0){
            return find_target_folder(fp,cluster,add_place);
        }
    }

    int m=0;
    // 读取每个目录项
    while (fread(&entry, sizeof(entry), 1, fp) == 1) {
        m++;
        // 如果目录项为空，说明已经到达根目录区的结尾
        if (entry.name[0] == 0||m>512/32) {
            cluster_now=have_next_cluster(fp,cluster_now);
            if(cluster_now==0){
                return -3;
            }else if(cluster_now==1){
                return -3;    
            }else{
                fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec, SEEK_SET);
                m=0;
                continue;    
            }
        }

        // 如果目录项不是隐藏文件或卷标，说明是普通文件或文件夹
        if (entry.size>=0&&entry.attr != 0x0F && (entry.attr & ATTR_DIRECTORY || entry.attr & ATTR_ARCHIVE)) {
            // 如果第一个字节是 0xE5，说明该目录项已经被删除，跳过
            if (entry.name[0] == 0xE5) {
                continue;
            }

            if(strncmp(entry.name,name,sizeof(name))==0&&strncmp(entry.ext,ext,sizeof(ext))==0){
                if(strncmp(entry.ext,"   ",3)!=0){
                    return entry.cluster;
                }
                int cluster_yet = find_target_folder(fp,entry.cluster,add_place);
                if(cluster_yet==-3){
                    return -3;
                }else if(cluster_yet==1){
                    return find_target_folder(fp,cluster,add_place);
                }else{
                    return cluster_yet;
                }
            }
        }
    }
}

void ls(FILE* fp,int cluster,char* path){
    struct directory_entry entry;
    int cluster_now=cluster;

    //allocate fp point
    if(cluster_now==2){
        fseek(fp,SectorNoOfRootDirectory*BPB_BytsPerSec,SEEK_SET);
    }else{
        fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec, SEEK_SET);
    }

    int path_len=0;
    for(;path[path_len]!=0&&path[path_len]!=' ';++path_len){}
    if(path[0]==0||(path[0]=='/'&&(path[1]==0||path[1]==' '))){
        white_cout("/:\n",3);
    }else{
        white_cout(path,path_len);
        white_cout(":\n",2);
    }
    
    //else if(path[0]!='/'||(path[1]!=0&&path[1]!=' ')){
    //    printf(". .. ");
    //}
    int m=0;
    while (fread(&entry, sizeof(entry), 1, fp) == 1) {
        m++;
        // 如果目录项为空，说明已经到达根目录区的结尾
        if (entry.name[0] == 0||m>512/32) {
            cluster_now = have_next_cluster(fp,cluster_now);
            if(cluster_now==0){
                white_cout("\n",1);
                break;
            }else if(cluster_now==1){
                white_cout("\n",1);
                break;
            }else{
                fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec, SEEK_SET);
                m=0;
                continue;
            }
        }
        
        // 如果目录项不是隐藏文件或卷标，说明是普通文件或文件夹
        if (entry.size>=0&&entry.attr != 0x0F && (entry.attr & ATTR_DIRECTORY || entry.attr & ATTR_ARCHIVE)) {
            // 如果第一个字节是 0xE5，说明该目录项已经被删除，跳过
            if (entry.name[0] == 0xE5) {
                continue;
            }
            //print name
            if(entry.attr&ATTR_DIRECTORY){
                for(int p=0;p<8;++p){
                    if(entry.name[p]!=' '&&entry.name[p]!=0){
                        red_cout(entry.name+p,1);
                    }else{
                        break;
                    }
                }
                white_cout("  ",2);
            }else if(entry.attr&ATTR_ARCHIVE){
                for(int p=0;p<8;++p){
                    if(entry.name[p]!=' '&&entry.name[p]!=0){
                        white_cout(entry.name+p,1);
                    }else{
                        break;
                    }
                }
                white_cout(".",1);
                for(int p=0;p<3;++p){
                    if(entry.ext[p]!=' '&&entry.ext[p]!=0){
                        white_cout(entry.ext+p,1);
                    }else{
                        break;
                    }
                }
                white_cout("  ",2);
            }
        }
    }

    cluster_now=cluster;
    m=0;
    if(cluster_now==2){
        fseek(fp,SectorNoOfRootDirectory*BPB_BytsPerSec,SEEK_SET);
    }else{
        fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec, SEEK_SET);
    }
    char path_next[512];
    for(int i=0;i<512;++i){
        path_next[i]=0;
    }

    while (fread(&entry, sizeof(entry), 1, fp) == 1) {
        m++;
        // 如果目录项为空，说明已经到达根目录区的结尾
        if (entry.name[0] == 0||m>512/32) {
            cluster_now=have_next_cluster(fp,cluster_now);
            if(cluster_now==0){
                return;
            }else if(cluster_now==1){
                return;
            }else{
                m=0;
                fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec, SEEK_SET);
                continue;    
            }
        }
        
        int len=strlen(path);
        // 如果目录项不是隐藏文件或卷标，说明是普通文件或文件夹
        if (entry.size>=0&&entry.attr != 0x0F &&  (entry.attr & ATTR_DIRECTORY) &&(entry.name[0]!=0xE5)&&entry.name[0]!=0x2e) {
            // 如果第一个字节是 0xE5，说明该目录项已经被删除，跳过
            strcpy(path_next,path);
            
            for(int i=0;i<8;++i){
                if(entry.name[i]!=' '&&entry.name[i]!=0){
                    path_next[len++]=entry.name[i];
                }else{
                    break;
                }
            }
            if(path_next[len-1]!='/'){
                path_next[len]='/';
            }
            ls(fp,entry.cluster,path_next);               

            if(cluster_now==2){
                fseek(fp,SectorNoOfRootDirectory*BPB_BytsPerSec+m*sizeof(entry),SEEK_SET);
            }else{
                fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec+m*sizeof(entry), SEEK_SET);
            }
        }
    }
}

void ls_l(FILE* fp,int cluster,char* path){
    struct directory_entry entry;
    int cluster_now=cluster;

    int path_len=0;
    for(;path[path_len]!=0&&path[path_len]!=' ';++path_len){}
    white_cout(path,path_len);

    if(path[0]==0){
        white_cout("/ ",2);
    }
    white_cout(" ",1);

    print_sub_file_num(fp,cluster);
    white_cout(":\n",2);
    if(path[0]!=0&&(path[0]!='/'||(path[1]!=' '&&path[1]!=0))){
        red_cout(".\n..\n",5);
    }

    int m=0;
    if(cluster==2){
        fseek(fp,SectorNoOfRootDirectory*BPB_BytsPerSec,SEEK_SET);
    }else{
        fseek(fp, ((cluster - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec, SEEK_SET);
    }
    while (fread(&entry, sizeof(entry), 1, fp) == 1) {
        m++;
        // 如果目录项为空，说明已经到达根目录区的结尾
        if (entry.name[0] == 0||m>512/32) {
            cluster_now = have_next_cluster(fp,cluster_now);
            if(cluster_now==0){
                white_cout("\n",1);
                break;
            }else if(cluster_now==1){
                white_cout("\n",1);
                break;
            }else{
                m=0;
                fseek(fp,((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec,SEEK_SET);
                continue;
            }
        }
        
        // 如果目录项不是隐藏文件或卷标，说明是普通文件或文件夹
        if (entry.size>=0&&entry.attr != 0x0F && (entry.attr & ATTR_DIRECTORY || entry.attr & ATTR_ARCHIVE)) {
            // 如果第一个字节是 0xE5，说明该目录项已经被删除，跳过
            if (entry.name[0] == 0xE5) {
                continue;
            }
            if(entry.name[0]==0x2E){
                continue;
            }
            //print name
            if(entry.attr&ATTR_DIRECTORY){
                for(int p=0;p<8;++p){
                    if(entry.name[p]!=' '&&entry.name[p]!=0){
                        red_cout(entry.name+p,1);
                    }else{
                        break;
                    }
                }
                white_cout("  ",2);
                print_sub_file_num(fp,entry.cluster);
                if(cluster==2){
                    fseek(fp,SectorNoOfRootDirectory*BPB_BytsPerSec+m*sizeof(entry),SEEK_SET);
                }else{
                    fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec+m*sizeof(entry), SEEK_SET);
                }
            }else if(entry.attr&ATTR_ARCHIVE){
                for(int p=0;p<8;++p){
                    if(entry.name[p]!=' '&&entry.name[p]!=0){
                        white_cout(entry.name+p,1);
                    }else{
                        break;
                    }
                }
                white_cout(".",1);
                for(int p=0;p<3;++p){
                    if(entry.ext[p]!=' '&&entry.ext[p]!=0){
                        white_cout(entry.ext+p,1);
                    }else{
                        break;
                    }
                }
                white_cout("  ",2);
                print_num(entry.size);
            }
            white_cout("\n",1);
        }
    }
    
    cluster_now=cluster;
    m=0;
    char path_next[512];
    for(int i=0;i<512;++i){
        path_next[i]=0;
    }

    if(cluster==2){
        fseek(fp,SectorNoOfRootDirectory*BPB_BytsPerSec,SEEK_SET);
    }else{
        fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec, SEEK_SET);
    }
    while (fread(&entry, sizeof(entry), 1, fp) == 1) {
        m++;
        // 如果目录项为空，说明已经到达根目录区的结尾
        if (entry.name[0] == 0) {
            return;
        }
        if(entry.name[0]=='.'){
            continue;
        }
        int len=strlen(path);
        // 如果目录项不是隐藏文件或卷标，说明是普通文件或文件夹
        if (entry.size>=0&&entry.attr != 0x0F &&  (entry.attr & ATTR_DIRECTORY) &&(entry.name[0]!=0xE5)) {
            // 如果第一个字节是 0xE5，说明该目录项已经被删除，跳过
            strcpy(path_next,path);
            for(int i=0;i<8;++i){
                if(entry.name[i]!=' '&&entry.name[i]!=0){
                    path_next[len++]=entry.name[i];
                }else{
                    break;
                }
            }
            if(path_next[len-1]!='/'){
                path_next[len]='/';
            }

            ls_l(fp,entry.cluster,path_next);
            if(cluster==2){
                fseek(fp,SectorNoOfRootDirectory*BPB_BytsPerSec+m*sizeof(entry),SEEK_SET);
            }else{
                fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec+m*sizeof(entry), SEEK_SET);
            }
        }
    }
}

void print_sub_file_num(FILE* fp,int cluster){
    struct directory_entry entry;
    int folder_num=0;
    int file_num=0;
    int cluster_now=cluster;
    int m=0;

    //allocate fp point
    if(cluster_now==2){
        fseek(fp,SectorNoOfRootDirectory*BPB_BytsPerSec,SEEK_SET);
    }else{
        fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec, SEEK_SET);
    }
    while (fread(&entry, sizeof(entry), 1, fp) == 1) {
        m++;
        // 如果目录项为空，说明已经到达根目录区的结尾
        if (entry.name[0] == 0||m>512/32) {
            cluster_now=have_next_cluster(fp,cluster_now);
            if(cluster_now==0){
                break;
            }else if(cluster_now==1){
                break;
            }else{
                m=0;
                fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec, SEEK_SET);
                continue;
            }
        }
        
        // 如果目录项不是隐藏文件或卷标，说明是普通文件或文件夹
        if (entry.size>=0&&entry.attr != 0x0F && (entry.attr & ATTR_DIRECTORY || entry.attr & ATTR_ARCHIVE)) {
            // 如果第一个字节是 0xE5，说明该目录项已经被删除，跳过
            if (entry.name[0] == 0xE5) {
                continue;
            }
            if(entry.name[0]=='.'){
                continue;
            }
            if(entry.attr&ATTR_DIRECTORY){
                folder_num++;
            }else if(entry.attr&ATTR_ARCHIVE){
                file_num++;
            }
        }
    }
    print_num(folder_num);
    white_cout(" ",1);
    print_num(file_num);
}

void print_num(int num){
    char num_str[64];
    for(int i=0;i<64;++i){
        num_str[i]=0;
    }
    if(num<10){
        num_str[0]=num+'0';
        white_cout(num_str,1);
    }else{
        int i=0;
        int below=1;
        for(;num>=below;++i){
            num_str[i]='0'+(num%(below*10))/below;
            below*=10;
        }
        i--;
        for(;i>=0;--i){
            white_cout(num_str+i,1);
        }
    }
}

int have_next_cluster(FILE* fp,int cluster){
    
    unsigned short cluster_next = 0;
    fseek(fp, BPB_RsvdSecCnt * BPB_BytsPerSec + cluster*3/2, SEEK_SET);
    fread(&cluster_next,sizeof(cluster_next),1,fp);
    if(cluster&1){
        //后12个数
        cluster_next=cluster_next&0xfff0;
        cluster_next=cluster_next/16;
    }else{
        //前12个数
        cluster_next=cluster_next&0xfff;
    }
    if(cluster_next==0000){
        return 0;
    }else if(cluster_next==0xff7){
        return 0;
    }else if(cluster_next==0xfff){
        return 1;
    }else{
        return cluster_next;
    }
    
}

void cat(FILE* fp,int cluster){
    int cluster_now=cluster;

    if(cluster_now==2){
        fseek(fp,SectorNoOfRootDirectory*BPB_BytsPerSec,SEEK_SET);
    }else{
        fseek(fp, ((cluster_now - 2) * BPB_SecPerClus+BPB_RsvdSecCnt+BPB_NumFATs*BPB_FATSz16+RootDirSectors) * BPB_BytsPerSec, SEEK_SET);
    }
    
    char a[1];
    while (fread(&a, sizeof(a), 1, fp) == 1) {
        if(a[0]!=0){
            white_cout(a,1);
        }else{
            break;
        }
    }

    white_cout("\n",1);
}
