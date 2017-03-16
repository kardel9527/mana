#ifndef __K_MACROS_H_
#define __K_MACROS_H_

#define NMS_BEGIN(NAME) namespace NAME {
#define NMS_END }

#define safe_free(p) do { if (p) free(p); p = 0; } while(0)
#define safe_delete(p) do { if (p) delete(p); p = 0; } while(0)

#endif //__K_MACROS_H_