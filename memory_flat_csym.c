

#include "memory_flat.c"


void *c_malloc(uint size){
    impl_malloc(size);
}
void *c_realloc(void *s,int newSize){
    impl_realloc(s,newSize);
}
void c_free(void *pMem){
    impl_free(pMem);
}
void *c_memmove(void* dst, void* src, int n){
    return impl_memmove(dst,src,n);
}
void *c_memcpy(void* dst, void* src, int n){
    return impl_memcpy(dst,src,n);
}
void *c_memset(void *s,int ch,uint n){
    return impl_memset;
}