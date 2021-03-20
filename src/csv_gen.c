#include <stdio.h>
#include <string.h>
#include "../inc/csv_gen.h"

FILE *fp;
void create_csv(char *d_h, double ti, double te, double tr, double res, double fan){
    if(ti==0 && te==0 && tr==0 && res==0 && fan==0){
        char temp[23];
        int i=0;
        while(d_h[i+9]!='\0'){
            temp[i]=d_h[i+4];
            i++;
        }
        temp[i]='2';
        temp[i+1]='1';
        temp[i+2]='.';
        temp[i+3]='c';
        temp[i+4]='s';
        temp[i+5]='v';
        temp[i+6]='\0';
        //filename=strcat(temp,"scsv");
        i=0;
        while(temp[i]!='\0'){
            if(temp[i]==' '){
                temp[i]='_';
            }
            i++;
        }
        char f_path[30];
        snprintf(f_path, 30, "../log/%s", temp);
        fp=fopen(f_path,"w");
        fprintf(fp, "Data e hora,TI,TE,TR,Resistor,Ventoinha\n");
    }
    else{
        fprintf(fp, "%s,%lf,%lf,%lf,%lf,%lf\n", d_h, ti, te, tr, res, fan);
    }
}
void close_csv(){
    fprintf(fp, "%c", '\0');
    fclose(fp);
}
