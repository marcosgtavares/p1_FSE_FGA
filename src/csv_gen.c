#include <stdio.h>
#include <string.h>
#include "csv_gen.h"

FILE *fp;
void create_csv(char *d_h, double ti, double te, double tr, double res, double fan){
    if(ti==0 && te==0 && tr==0 && res==0 && fan==0){
        char *filename;
        filename=strcat(d_h,".csv");
        int i=0;
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
    fclose(fp);
}
