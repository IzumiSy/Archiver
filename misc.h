#ifndef _MISC_H_
#define _MISC_H_

int get32(const UCHAR *p)
{
    return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}

void put32(UCHAR *p, int i)
{
    p[0] =  i        & 0xff;
    p[1] = (i >>  8) & 0xff;
    p[2] = (i >> 16) & 0xff;
    p[3] = (i >> 24) & 0xff;
}

#endif
