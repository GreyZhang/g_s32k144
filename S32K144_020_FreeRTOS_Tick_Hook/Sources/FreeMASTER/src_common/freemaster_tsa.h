/*******************************************************************************
*
* Copyright 2004-2013 NXP Semiconductor, Inc.
*
* This software is owned or controlled by NXP Semiconductor.
* Use of this software is governed by the NXP FreeMASTER License
* distributed with this Material.
* See the LICENSE file distributed for more details.
*
****************************************************************************//*!
*
* @brief  FreeMASTER Driver TSA feature
*
*******************************************************************************/

#ifndef __FREEMASTER_TSA_H
#define __FREEMASTER_TSA_H

#include "freemaster_defcfg.h"

/*****************************************************************************
 Target-side Address translation structures and macros
******************************************************************************/

/* current TSA version  */
#define FMSTR_TSA_VERSION 2U

/* TSA flags carried in TSA_ENTRY.info (except the first entry in table) */
#define FMSTR_TSA_INFO_ENTRYTYPE_MASK 0x0003U    /* flags reserved for TSA_ENTRY use */
#define FMSTR_TSA_INFO_STRUCT         0x0000U    /* ENTRYTYPE: structure parent type */
#define FMSTR_TSA_INFO_RO_VAR         0x0001U    /* ENTRYTYPE: read-only variable */
#define FMSTR_TSA_INFO_MEMBER         0x0002U    /* ENTRYTYPE: structure member */
#define FMSTR_TSA_INFO_RW_VAR         0x0003U    /* ENTRYTYPE: read-write variable */
#define FMSTR_TSA_INFO_VAR_FLAG       0x0001U    /* ENTRYTYPE: FLAG: any variable */
#define FMSTR_TSA_INFO_RWV_FLAG       0x0002U    /* ENTRYTYPE: FLAG: R/W access */

/* TSA table index and size (both unsigned, at least 16 bit wide) */
typedef FMSTR_SIZE FMSTR_TSA_TINDEX;
typedef FMSTR_SIZE FMSTR_TSA_TSIZE;

/* pointer types used in TSA tables can be overridden in freemaster.h */
/* (this is why macros are used instead of typedefs) */
#ifndef FMSTR_TSATBL_STRPTR
#define FMSTR_TSATBL_STRPTR  const char*
#endif
#ifndef FMSTR_TSATBL_STRPTR_CAST
#define FMSTR_TSATBL_STRPTR_CAST(x) ((FMSTR_TSATBL_STRPTR)(x))
#endif
#ifndef FMSTR_TSATBL_STRPTR_ENTRY
#define FMSTR_TSATBL_STRPTR_ENTRY(x) {FMSTR_TSATBL_STRPTR_CAST(x)}
#endif
#ifndef FMSTR_TSATBL_VOIDPTR
#define FMSTR_TSATBL_VOIDPTR volatile const void*
#endif
#ifndef FMSTR_TSATBL_VOIDPTR_CAST
#define FMSTR_TSATBL_VOIDPTR_CAST(x) ((FMSTR_TSATBL_VOIDPTR)(x))
#endif
#ifndef FMSTR_TSATBL_VOIDPTR_ENTRY
#define FMSTR_TSATBL_VOIDPTR_ENTRY(x) {FMSTR_TSATBL_VOIDPTR_CAST(x)}
#endif

/* TSA table entry. The unions inside assures variables sized enough to */
/* accommodate both the C-pointer and the user-requested size (FMSTR_ADDR) */
#if defined(__S12Z__)
typedef struct
{
    FMSTR_DUMMY dummy0;
    union { FMSTR_ADDR  p; FMSTR_ADDR n; } name;
    FMSTR_DUMMY dummy1;
    union { FMSTR_ADDR  p; FMSTR_ADDR n; } type;
    FMSTR_DUMMY dummy2;
    union { FMSTR_ADDR p; FMSTR_ADDR n; } addr;
    FMSTR_DUMMY dummy3;
    union { FMSTR_ADDR p; FMSTR_ADDR n; } info;
} FMSTR_TSA_ENTRY;
#else
typedef struct
{
    union { FMSTR_TSATBL_STRPTR  p; FMSTR_ADDR n; } name;
    union { FMSTR_TSATBL_STRPTR  p; FMSTR_ADDR n; } type;
    union { FMSTR_TSATBL_VOIDPTR p; FMSTR_ADDR n; } addr;
    union { FMSTR_TSATBL_VOIDPTR p; FMSTR_ADDR n; } info;
} FMSTR_TSA_ENTRY;
#endif

/* list of available types for TSA table definition */
#define FMSTR_TSA_STRUCT_CFG(name)  \
    FMSTR_TSATBL_STRPTR_ENTRY(#name), FMSTR_TSATBL_STRPTR_ENTRY(NULL), FMSTR_TSATBL_VOIDPTR_ENTRY(NULL), FMSTR_TSATBL_VOIDPTR_ENTRY(FMSTR_TSA_INFO1(name, FMSTR_TSA_INFO_STRUCT)),

#define FMSTR_TSA_MEMBER_CFG(parenttype,name,type) \
    FMSTR_TSATBL_STRPTR_ENTRY(#name), FMSTR_TSATBL_STRPTR_ENTRY(type), FMSTR_TSATBL_VOIDPTR_ENTRY(&((parenttype*)0)->name), FMSTR_TSATBL_VOIDPTR_ENTRY(FMSTR_TSA_INFO1(((parenttype*)0)->name, FMSTR_TSA_INFO_MEMBER)),

#define FMSTR_TSA_RO_VAR_CFG(name,type) \
    FMSTR_TSATBL_STRPTR_ENTRY(#name), FMSTR_TSATBL_STRPTR_ENTRY(type), FMSTR_TSATBL_VOIDPTR_ENTRY(&(name)), FMSTR_TSATBL_VOIDPTR_ENTRY(FMSTR_TSA_INFO1(name, FMSTR_TSA_INFO_RO_VAR)),

#define FMSTR_TSA_RW_VAR_CFG(name,type) \
    FMSTR_TSATBL_STRPTR_ENTRY(#name), FMSTR_TSATBL_STRPTR_ENTRY(type), FMSTR_TSATBL_VOIDPTR_ENTRY(&(name)), FMSTR_TSATBL_VOIDPTR_ENTRY(FMSTR_TSA_INFO1(name, FMSTR_TSA_INFO_RW_VAR)),

#define FMSTR_TSA_RO_MEM_CFG(name,type,addr,size) \
    FMSTR_TSATBL_STRPTR_ENTRY(#name), FMSTR_TSATBL_STRPTR_ENTRY(type), FMSTR_TSATBL_VOIDPTR_ENTRY(addr), FMSTR_TSATBL_VOIDPTR_ENTRY(FMSTR_TSA_INFO2(size, FMSTR_TSA_INFO_RO_VAR)),

#define FMSTR_TSA_RW_MEM_CFG(name,type,addr,size) \
    FMSTR_TSATBL_STRPTR_ENTRY(#name), FMSTR_TSATBL_STRPTR_ENTRY(type), FMSTR_TSATBL_VOIDPTR_ENTRY(addr), FMSTR_TSATBL_VOIDPTR_ENTRY(FMSTR_TSA_INFO2(size, FMSTR_TSA_INFO_RW_VAR)),

#define FMSTR_TSA_RO_ENTRY(name_string,type_string,addr,size) \
    FMSTR_TSATBL_STRPTR_ENTRY(name_string), FMSTR_TSATBL_STRPTR_ENTRY(type_string), FMSTR_TSATBL_VOIDPTR_ENTRY(addr), FMSTR_TSATBL_VOIDPTR_ENTRY(FMSTR_TSA_INFO2(size, FMSTR_TSA_INFO_RW_VAR)),


/*///////////////////////////////////////////////////////////////////// */
/* TSA "Base Types", all start with one (non-printable) character. */
/* Type is retrieved by PC and parsed according to the binary format of: */
/* "111STTZZ" where TT=type[int,frac,fp,special] S=signed ZZ=size[1,2,4,8] */
/* "11101100" is a special information block (not a real TSA symbol). */

/* S=0 TT=int */
#define FMSTR_TSA_UINT8   "\xE0"
#define FMSTR_TSA_UINT16  "\xE1"
#define FMSTR_TSA_UINT32  "\xE2"
#define FMSTR_TSA_UINT64  "\xE3"
/* S=1 TT=int */
#define FMSTR_TSA_SINT8   "\xF0"
#define FMSTR_TSA_SINT16  "\xF1"
#define FMSTR_TSA_SINT32  "\xF2"
#define FMSTR_TSA_SINT64  "\xF3"
/* S=0 TT=frac */
#define FMSTR_TSA_UFRAC_UQ(m,n) "\xE4:" #m "." #n /* UQm.n fractional m+n=bitsize */
#define FMSTR_TSA_UFRAC16 "\xE5"  /* standard UQ1.15 */
#define FMSTR_TSA_UFRAC32 "\xE6"  /* standard UQ1.31 */
#define FMSTR_TSA_UFRAC64 "\xE7"  /* standard UQ1.63 */
/* S=1 TT=frac */
#define FMSTR_TSA_FRAC_Q(m,n) "\xF4:" #m "." #n /* Qm.n fractional m+n+1=bitsize */
#define FMSTR_TSA_FRAC16  "\xF5"  /* standard Q0.15 */
#define FMSTR_TSA_FRAC32  "\xF6"  /* standard Q0.31 */
#define FMSTR_TSA_FRAC64  "\xF7"  /* standard Q0.63 */
/* S=1 TT=fp */
#define FMSTR_TSA_FLOAT   "\xFA"
#define FMSTR_TSA_DOUBLE  "\xFB"
/* TT=special */
#define FMSTR_TSA_SPECIAL "\xEC"

/* a pointer type is like UINT with proper size set to sizeof(void*)
 * it is platform-specific, so made as external constant variable and
 * implemented in freemaster_tsa.c */
#if FMSTR_USE_TSA
extern FMSTR_TSA_CDECL char FMSTR_TSA_POINTER[];
#else
#define FMSTR_TSA_POINTER ""
#endif

/* macro used to describe "User Type" */
#define FMSTR_TSA_USERTYPE(type) #type

/* macro used to describe pure memory space */
#define FMSTR_TSA_MEMORY NULL

/*****************************/
/* TSA table-building macros */

#if FMSTR_USE_TSA

#define FMSTR_TSA_FUNC(id)  FMSTR_TsaGetTable_##id
#define FMSTR_TSA_FUNC_PROTO(id) const FMSTR_TSA_ENTRY* FMSTR_TSA_FUNC(id) (FMSTR_TSA_TSIZE* pTableSize)

#define FMSTR_TSA_TABLE_BEGIN(id) \
    FMSTR_TSA_FUNC_PROTO(id); \
    FMSTR_TSA_FUNC_PROTO(id) { \
        static FMSTR_TSA_CDECL FMSTR_TSA_ENTRY fmstr_tsatable[] = {

/* entry info  */
#define FMSTR_TSA_INFO1(elem, flags) FMSTR_TSATBL_VOIDPTR_CAST(((sizeof(elem))<<2)|(flags))
#define FMSTR_TSA_INFO2(size, flags) FMSTR_TSATBL_VOIDPTR_CAST(((size)<<2)|(flags))

/* TSA entry describing the structure/union type */
#define FMSTR_TSA_STRUCT(name)  \
    { FMSTR_TSA_STRUCT_CFG(name) },

/* TSA entry describing the structure/union member (must follow the FMSTR_TSA_STRUCT entry) */
#define FMSTR_TSA_MEMBER(parenttype,name,type) \
    { FMSTR_TSA_MEMBER_CFG(parenttype,name,type) },

/* TSA entry describing read-only variable */
#define FMSTR_TSA_RO_VAR(name,type) \
    { FMSTR_TSA_RO_VAR_CFG(name,type) },

/* TSA entry describing read-write variable */
#define FMSTR_TSA_RW_VAR(name,type) \
    { FMSTR_TSA_RW_VAR_CFG(name,type) },

/* TSA entry describing read-only memory area */
#define FMSTR_TSA_RO_MEM(name,type,addr,size) \
    { FMSTR_TSA_RO_MEM_CFG(name,type,addr,size) },

/* TSA entry describing read-write variable */
#define FMSTR_TSA_RW_MEM(name,type,addr,size) \
    { FMSTR_TSA_RW_MEM_CFG(name,type,addr,size) },

/* TSA entry describing virtual directory for the subsequent FILE entries
   The 'dirname' is either root-based if it starts with '/' or is relative to parent folder */
#define FMSTR_TSA_DIRECTORY(dirname) \
    { FMSTR_TSA_RO_ENTRY(dirname,FMSTR_TSA_SPECIAL":DIR",NULL,0) },

/* TSA entry describing virtual file content statically mapped in memory (static content cached by PC)
   The 'filename' may be specified with relative paths using normal slash */
#define FMSTR_TSA_MEMFILE(filename,filemem,filesize) \
    { FMSTR_TSA_RO_ENTRY(filename,FMSTR_TSA_SPECIAL":MEMFILE",&(filemem),(filesize)) },

/* TSA entry describing project to be opened in FreeMASTER
   'project_uri' may be a existing file (one of FILE entries) or a web URI */
#define FMSTR_TSA_PROJECT(projectname,project_uri) \
    { FMSTR_TSA_RO_ENTRY(projectname,FMSTR_TSA_SPECIAL":PRJ",project_uri,sizeof(project_uri)) },

/* TSA entry describing general WEB link. All web links are offered to be navigated in FreeMASTER
   'link_uri' may be a existing file (one of FILE entries) or a web URI */
#define FMSTR_TSA_HREF(linkname,link_uri) \
    { FMSTR_TSA_RO_ENTRY(linkname,FMSTR_TSA_SPECIAL":HREF",link_uri,sizeof(link_uri)) },

#define FMSTR_TSA_TABLE_END() }; \
    if(pTableSize) *pTableSize = sizeof(fmstr_tsatable); \
    return fmstr_tsatable; }

#else /* FMSTR_USE_TSA */

#define FMSTR_TSA_TABLE_BEGIN(id)
#define FMSTR_TSA_INFO1(elem, flags)
#define FMSTR_TSA_INFO2(size, flags)
#define FMSTR_TSA_STRUCT(name)
#define FMSTR_TSA_MEMBER(parenttype,name,type)
#define FMSTR_TSA_RO_VAR(name,type)
#define FMSTR_TSA_RW_VAR(name,type)
#define FMSTR_TSA_RO_MEM(name,type,addr,size)
#define FMSTR_TSA_RW_MEM(name,type,addr,size)
#define FMSTR_TSA_DIRECTORY(dirname)
#define FMSTR_TSA_MEMFILE(filename,filemem,filesize)
#define FMSTR_TSA_PROJECT(projectname,project_uri)
#define FMSTR_TSA_HREF(linkname,link_uri)
#define FMSTR_TSA_TABLE_END()

#endif /* FMSTR_USE_TSA */

/**********************************************/
/* master TSA table-retrieval building macros */

#ifdef __cplusplus
extern "C" {
#endif

#if FMSTR_USE_TSA

#define FMSTR_TSA_TABLE_LIST_BEGIN() \
    const FMSTR_TSA_ENTRY* FMSTR_TsaGetTable(FMSTR_TSA_TINDEX nTableIndex, FMSTR_TSA_TSIZE* pTableSize) {

#define FMSTR_TSA_TABLE(id) \
    if(!nTableIndex--) { \
        FMSTR_TSA_FUNC_PROTO(id); \
        return FMSTR_TSA_FUNC(id)(pTableSize); \
    } else

#if (FMSTR_USE_TSA_DYNAMIC)
#define FMSTR_TSA_TABLE_LIST_END() \
    if(!nTableIndex--) { \
        FMSTR_TSA_FUNC_PROTO(dynamic_tsa); \
        return FMSTR_TSA_FUNC(dynamic_tsa)(pTableSize); \
    } else \
    { return NULL; } }
#else
#define FMSTR_TSA_TABLE_LIST_END() \
    { return NULL; } }
#endif

/*****************************************************************************
 Target-side Address translation functions
******************************************************************************/

/* master TSA table-retrieval function */
const FMSTR_TSA_ENTRY* FMSTR_TsaGetTable(FMSTR_TSA_TINDEX nTableIndex, FMSTR_TSA_TSIZE* pTableSize);
FMSTR_TSA_FUNC_PROTO(dynamic_tsa);

#else /* FMSTR_USE_TSA */

#define FMSTR_TSA_TABLE_LIST_BEGIN()
#define FMSTR_TSA_TABLE(id)
#define FMSTR_TSA_TABLE_LIST_END()

#endif /* FMSTR_USE_TSA */

#ifdef __cplusplus
}
#endif

#endif /* __FREEMASTER_TSA_H */
