

#include <stdlib.h>
#include <stdio.h>
#include "memory_flat.h"
#include "memory_flat_csym.h"

void *area;

void init_mod(){
    area=malloc(32*1024*1024);
    struct init_param p;
    p.mem_start=area;
    p.mem_end=(char *)area+32*1024*1024;
    mod_memory_flat_init(&p);
}

int assert(int t){
    if(!t){
        printf("failed.\n");
        return 0;
    }
    return 1;
}
int main(int argc,char *argv[]){
    init_mod();
    
    void *p1=c_malloc(200);
    void *p2=c_malloc(300);
    //0[p1]200[p2]500
    c_free(p1);
    //0_200[p2]500
    void *p3=c_malloc(100);
    //0[p3]]100_200[p2]500
    if(!assert(p1==p3)){
        return 1;
    }
    p1=c_malloc(80);
    //0[p3,p1]180_200[p2]500
    if(!assert(p1<p2&&p1>p3)){
        return 1;
    }
    c_free(p3);
    //0_100[p1]180_200[p2]500
    p3=c_malloc(200);
    //0_100[p1]180_200[p2]500[p3]700
    if(!assert(p3>p2)){
        return 1;
    }
    c_free(p2);
    p2=c_malloc(300);
    //0_100[p1]180[p2]480_500[p3]700
    if(!assert(p2>p1&&p3>p2)){
        return 1;
    }
    c_free(p2);
    c_free(p1);
    p2=c_malloc(300);
    //0[p2]300_500[p3]700
    if(!assert(p2<p1)){
        return 1;
    }
    p1=c_realloc(p2,350);
    //0[p1]350_500[p3]700
    if(!assert(p2==p1)){
        return 1;
    }
    
    p2=c_malloc(100);
    p1=c_realloc(p1,400);

    //0_350[p2]450_500[p3]700[p1];
    if(!assert(p1>p2&&p1>p3)){
        return 1;
    }
    p1=c_realloc(p1,100);
    //0_350[p2]450_500[p3]700[p1];
    if(!assert(p1>p2&&p1>p3)){
        return 1;
    }
    p3=c_realloc(p3,300);
    //0[p3]300_350[p2]450_700[p1];
    
    if(!assert(p3<p2&&p3<p1)){
        return 1;
    }
    c_free(p1);
    c_free(p2);
    c_free(p3);
    printf("pass all test.");
    return 0;
}
