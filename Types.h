#ifndef _TYPES_H_
#define _TYPES_H_

#ifndef TRUE
  #define TRUE ((boolean) 1)
#endif

#ifndef FALSE
  #define FALSE ((boolean) 0)
#endif

//typedef unsigned char boolean;  /*    TRUE .. FALSE      */
typedef signed char   sint8;    /*    -128 .. 127        */
typedef unsigned char   uint8;    /*       0 .. 255        */
typedef signed short  sint16;   /*        -32768 .. 32767      */
typedef unsigned short  uint16;   /*       0 .. 65535      */
typedef signed long   sint32;   /*   -2147483648 .. 2147483647 */
typedef unsigned long   uint32;   /*       0 .. 4294967295 */

#endif
