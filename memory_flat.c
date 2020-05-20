
#include <stdint.h>
#include "memory_flat.h"



typedef unsigned int uint;
typedef struct MemBlock{
	char *start;
	char *end;
} MemBlock;

static MemBlock *pMemTop;
static struct init_param InitParam;


static int GetBlockSize(MemBlock *block){
	return block->end-block->start;
}


static void *impl_memmove(void* dst, const void* src, int n)
{
    char* s_dst;
    char* s_src;
    s_dst = (char*)dst;
    s_src = (char*)src;
    if(s_dst>s_src) {
        s_dst = s_dst+n-1;
        s_src = s_src+n-1;
        while(n--) {
            *s_dst-- = *s_src--;
        }
    }else {
        while(n--) {
            *s_dst++ = *s_src++;
        }
    }
    return dst;
}

static void *impl_malloc(uint size){
	MemBlock *pMemLastBlock;
	MemBlock *space;
	uint blockSize;
	char *pMem;
	pMem=NULL;

	size+=sizeof(uint);
	pMemLastBlock=((MemBlock *)InitParam.mem_end)-1;

	for(space=pMemTop;space<(MemBlock *)InitParam.mem_end;space++){
		blockSize=GetBlockSize(space);
		if(blockSize>size){
			pMem=space->start;
			space->start+=size;
			break;
		}else if(blockSize==size){
			pMem=space->start;
			impl_memmove(pMemTop+1,pMemTop,(char *)space-(char *)pMemTop);
			pMemTop+=1;
			break;
		}
	}

	pMemLastBlock->end=(char *)pMemTop;
	if(!pMem){
		return NULL;
	}
	*((uint *)pMem)=size;
	return pMem+sizeof(uint);
}

static int impl_realloc0(void *pMem,uint size){
	int oldSize;
	char *newStart;
	MemBlock *space;
	oldSize=*(((uint *)pMem)-1);
	newStart=(char *)pMem-sizeof(uint)+oldSize;
	for(space=pMemTop;space<(MemBlock *)InitParam.mem_end;space++){
		if(space->start==newStart){
			newStart=newStart-oldSize+size;
			if(newStart>space->end){
				return 0;
			}else if(newStart==space->end){
				impl_memmove(pMemTop+1,pMemTop,(char *)space-(char *)pMemTop);
				pMemTop+=1;
			}else{
				space->start=newStart;
			}
			*(((uint *)pMem)-1)=size;
			return 1;
		}
	}
	return 0;
}

static int FreeMemBlock(void *pMem,uint size){
	int mergeCnt;
	MemBlock *itpMB;
	MemBlock *pMemLastBlock;
	
	mergeCnt=-1;
	pMemLastBlock=((MemBlock *)InitParam.mem_end)-1;
	for(itpMB=pMemTop;itpMB<=(MemBlock *)InitParam.mem_end;itpMB++){

		if(itpMB->start>(char *)pMem){
			if(itpMB->start-size==pMem){
				mergeCnt++;
				itpMB->start=pMem;
			}
			itpMB--;
			if(itpMB>=pMemTop&&itpMB->end==pMem){
				mergeCnt++;
				itpMB->end=((char *)pMem+size);
			}
			if(mergeCnt>0){
				(itpMB+1)->start=itpMB->start;
				impl_memmove(pMemTop+mergeCnt,pMemTop,(char *)itpMB-(char *)pMemTop);
			}else if(mergeCnt==-1){
				if(GetBlockSize(pMemLastBlock)<sizeof(MemBlock)){
					return 0;
				}
				impl_memmove(pMemTop+mergeCnt,pMemTop,(char *)(itpMB+1)-(char *)pMemTop);
				itpMB->start=pMem;
				itpMB->end=itpMB->start+size;
			}
			pMemTop+=mergeCnt;
			pMemLastBlock->end=(char *)pMemTop;
			break;
		}
	}
	return 1;
}
static int impl_free(void *pMem){
	uint size;
	pMem=(uint *)pMem-1;
	size=*((uint *)pMem);
	return FreeMemBlock(pMem,size);
}

static void *impl_memcpy(void *dst,void *src,unsigned int n){
	int i;
	for(i=0;i<n;i++){
		((char *)dst)[i]=((char *)src)[i];
	}
	return dst;
}



static void *impl_memset(void *s,int ch,uint n){
	int i;
	for(i=0;i<n;i++){
		((char *)s)[i]=ch;
	}
	return s;
}

static void *impl_realloc(void *s,int newSize){
	void *s2;
	uint size;
	size=*(((uint *)s)-1);
	if(impl_realloc0(s,newSize)){
		return s;
	}else{
		s2=impl_malloc(newSize);
		impl_memmove(s2,s,size);
		impl_free(s);
		return s2;
	}
}



int mod_memory_flat_init(struct init_param *param){
	pMemTop=param->mem_end-sizeof(MemBlock);
	pMemTop->start=param->mem_start;
	pMemTop->end=(char *)pMemTop;
	InitParam.mem_start=param->mem_start;
	InitParam.mem_end=param->mem_end;
	return 1;
}

#if _DEBUG == 1
#include <stdio.h>
void print_mem_stat(){
	MemBlock *block=pMemTop;
	for(;(void *)block<InitParam.mem_end;block++){
		printf("start:%X,end:%X\n",block->start,block->end);
	}
}
#endif