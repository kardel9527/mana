#ifndef __K_MACROS_H_
#define __K_MACROS_H_

#define NMS_BEGIN(NAME) namespace NAME {
#define NMS_END }

#define sfree(p) do { if (p) free(p); p = 0; } while(0)
#define sdelete(p) do { if (p) { delete p; p = 0; } } while(0)
#define sclose(fd) do { if (fd > 0) { ::close(fd); fd = -1; } } while (0)
#define bit_max_num(BIT) ((-1L) ^ (-1L << (BIT)))

#endif //__K_MACROS_H_
