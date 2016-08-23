#include <inttypes.h>
#include <rocket/strutils.h>

int wrstring(char *ptr,const char *s) {
  int n=0;
  while(*s!=0) {
    *ptr++=*s++;
    n++;
  }
  *ptr=0;
  return n;
}

int wruint64(char *ptr,uint64_t v) {
  int i,n=0;
  {
    if (v==0) n=1;
    else {
      uint64_t w=v;
      while(w>0) { n++; w/=10; }
    }
  }

  for(i=0;i<n;i++) {
    ptr[n-i-1]='0'+(v%10);
    v/=10;
  }
  ptr[n]=0;
  return n;
}


void wruint64_hex(char *p, uint64_t v) {
  static const char alp[]="0123456789abcdef";
  int i;
  for(i=60;i>=0;i-=4) *(p++)=alp[(v>>i)&0xf];
}

