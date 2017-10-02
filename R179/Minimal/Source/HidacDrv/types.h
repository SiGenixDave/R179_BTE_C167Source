
/******************************************************************************
*
*   C167 compiler specific
*
******************************************************************************/
#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef _DEBUG
typedef unsigned char       UINT_8;
typedef unsigned short int  UINT_16;
typedef unsigned int        UINT_32;

typedef char                INT_8;
typedef short int           INT_16;
typedef int                 INT_32;

/* Only used for unit testing in Visual Studio. Allows for a DLL
   to be created and calls from an application into the DLL */
#define EXPORT __declspec(dllexport)

#else
typedef unsigned char       UINT_8;
typedef unsigned int        UINT_16;
typedef unsigned long       UINT_32;

typedef char                INT_8;
typedef int                 INT_16;
typedef long                INT_32;

typedef struct{
  UINT_16 T_0;
  UINT_16 T_8;
  INT_16 x;
}ANALOG_IN_16;

typedef struct{
  UINT_16 T_0;
  UINT_16 T_8;
  INT_32 x;
}ANALOG_IN_32;

typedef struct{
  ANALOG_IN_16 ab;
  ANALOG_IN_16 bc;
  ANALOG_IN_16 ca;
}ANALOG_IN_ABC_16;

typedef struct{
  ANALOG_IN_32 ab;
  ANALOG_IN_32 bc;
  ANALOG_IN_32 ca;
}ANALOG_IN_ABC_32;

typedef struct{
  ANALOG_IN_16 a;
  ANALOG_IN_16 b;
  ANALOG_IN_16 c;
}ANALOG_IN_ABCPH_16;

typedef struct{
  ANALOG_IN_32 a;
  ANALOG_IN_32 b;
  ANALOG_IN_32 c;
}ANALOG_IN_ABCPH_32;

typedef struct{
  INT_16 a;
  INT_16 b;
  INT_16 c;
}INT_ABC_16;

typedef struct{
  INT_32 a;
  INT_32 b;
  INT_32 c;
}INT_ABC_32;

#define EXPORT
#endif

typedef INT_8               CHAR;

typedef enum
{
    FALSE,
    TRUE
} BOOLEAN;


/******************************************************************************
*
*   C167 specific (DAS need to move to another file)
*
******************************************************************************/
#ifdef _DEBUG
    #define DISABLE_ALL_INTERRUPTS()
    #define ENABLE_ALL_INTERRUPTS()
#else
    #define DISABLE_ALL_INTERRUPTS()  IEN = 0; _nop()
    #define ENABLE_ALL_INTERRUPTS()   IEN = 1; _nop()
#endif

#endif /*_TYPES_H_*/