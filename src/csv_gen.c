#include <stdio.h>
#include <string.h>
#include "../inc/csv_gen.h"

FILE *fp;
void create_csv(char *d_h, double ti, double te, double tr, double res, double fan){
    if(ti==0 && te==0 && tr==0 && res==0 && fan==0){
        char *filename, temp[23];
        int i=0;
        while(d_h[i+9]!='\0'){
            temp[i]=d_h[i+4];
            i++;
        }
        temp[i]='2';
        temp[i+1]='1';
        temp[i+2]='\0';
        temp[i+3]='.';
        temp[i+4]='c';
        temp[i+5]='s';
        temp[i+6]='v';
        filename=strcat(temp,".csv");
        i=0;
        while(filename[i]!='\0'){
            if(filename[i]==' '){
                filename[i]='_';
            }
            i++;
        }
        char f_path[30];
        snprintf(f_path, 30, "../log/%s", filename);
        fp=fopen(f_path,"w");
    }
    else{
        fprintf(fp, "%s,%lf,%lf,%lf,%lf,%lf\n", d_h, ti, te, tr, res, fan);
    }
}
void close_csv(){
    fprintf(fp, "%c", '\0');
    fclose(fp);
}
