/*
 -------------------------------------------------------------------------
 Copyright (c) 2003, Copera, Inc., Mountain View, CA, USA.
 All rights reserved.

 LICENSE TERMS

 The free distribution and use of this software in both source and binary 
 form is allowed (with or without changes) provided that:

   1. distributions of this source code include the above copyright 
      notice, this list of conditions and the following disclaimer;

   2. distributions in binary form include the above copyright
      notice, this list of conditions and the following disclaimer
      in the documentation and/or other associated materials;

   3. the copyright holder's name is not used to endorse products 
      built using this software without specific written permission. 

 DISCLAIMER

 This software is provided 'as is' with no explcit or implied warranties
 in respect of any properties, including, but not limited to, correctness 
 and fitness for purpose.
 -------------------------------------------------------------------------
 Issue Date: March 27, 2003
*/
#ifndef AESLIB_H
#define AESLIB_H

#include <LibTraps.h>


/**************************************************************************
 *
 * General Library information for use when loading the library
 *
 **************************************************************************/

#define AESLibCreatorID                 'AESL'
#define AESLibTypeID                    sysFileTLibrary
#define AESLibName                      "AESLib"

/**************************************************************************
 *
 * Feature Number to control different features of AESLib
 *
 *************************************************************************/

/**************************************************************************
 *
 * Force use of 68k code, even on ARM processors.  If this feature is
 * set and is not zero, then 68k code will be used, instead of the
 * ARM code. (Useful for comparing ARM speed vs. 68k speed).  This feature
 * is ONLY checked during library installation.  You cannot switch to
 * 68k code after the library has been loaded.  You must first close the
 * library and then open it again with the feature set.
 *
 *************************************************************************/

#define AESLibFtrNumForce68K            0x0068

/**************************************************************************
 *
 * Return types and constants for the AES functions. 
 *
 **************************************************************************/

typedef UInt16    aes_fret;
#define aes_bad   0
#define aes_good  1
#define aes_rval  aes_fret

#define AES_128_KEYLEN   16
#define AES_256_KEYLEN   32


/**************************************************************************
 *
 * The AES context data structure (needed for every function call). This
 * structure should be allocated with MemPtrNew and initialized with all
 * zeroes.  MemPtrNew (on Palm OS 5) guarantees that the structure is
 * aligned on a 4 - byte boundary.  The ARM native code which is called on
 * OS / 5 devices requires 4 - byte alignment.  If this data structure is
 * not allocated on a 4 - byte boundary the ARM native code will have to
 * make a local copy of the contents of the structure before performing
 * the AES operations.  This copy time will slow down performance.
 *
 **************************************************************************/

#define AES_BLOCK_SIZE   16
#define AES_KS_LENGTH    4 * AES_BLOCK_SIZE

typedef struct
{
  UInt32  k_sch[AES_KS_LENGTH];     /* the encryption key schedule */
  UInt32  n_rnd;                    /* the number of cipher rounds */
  UInt32  n_blk;                    /* the number of bytes in the state */
} aes_ctx;


/**************************************************************************
 *
 * Trap Number information and macro definition
 *
 **************************************************************************/

#define AESLibEncKey_trapno             sysLibTrapCustom
#define AESLibEncBlk_trapno             sysLibTrapCustom + 1
#define AESLibDecKey_trapno             sysLibTrapCustom + 2
#define AESLibDecBlk_trapno             sysLibTrapCustom + 3

/* New API (Version 3.1) */
#define AESLibEncBigBlk_trapno          sysLibTrapCustom + 4
#define AESLibDecBigBlk_trapno          sysLibTrapCustom + 5

#define AESLIB_TRAP(trapno)             SYS_TRAP(trapno)

/**************************************************************************
 *
 * Required Shared Library API. 
 *
 **************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

Err AESLibOpen(UInt16 refNum)
     AESLIB_TRAP(sysLibTrapOpen);

Err AESLibClose(UInt16 refNum)
     AESLIB_TRAP(sysLibTrapClose);

Err AESLibSleep(UInt16 refNum)
     AESLIB_TRAP(sysLibTrapSleep);

Err AESLibWake(UInt16 refNum)
     AESLIB_TRAP(sysLibTrapWake);

/**************************************************************************
 *
 * Core AES Functions 
 *
 **************************************************************************/

/**************************************************************************
 * AESLibEncKey : Set the key for encryption operations.
 * 
 * refNum   :  The AESLib reference number
 * in_key   :  The byte array containing the key
 * klen     :  The length of the key (16 or 32 bytes)
 * cx       :  The aes context to store the key in.
 *
 **************************************************************************/

aes_rval AESLibEncKey(UInt16 refNum, const unsigned char in_key[],
                      unsigned int klen, aes_ctx cx[1])
     AESLIB_TRAP(AESLibEncKey_trapno);

/**************************************************************************
 * AESLibEncBlk : Encrypt one 16 byte block
 * 
 * refNum   :  The AESLib reference number
 * in_blk   :  The 16 byte block of plaintext to encrypt
 * out_blk  :  The 16 byte block to hold the resulting ciphertext
 * cx       :  The aes context you initialized with AESLibEncKey
 *
 **************************************************************************/

aes_rval AESLibEncBlk(UInt16 refNum, const unsigned char in_blk[],
                      unsigned char out_blk[], const aes_ctx cx[1])
     AESLIB_TRAP(AESLibEncBlk_trapno);

/**************************************************************************
 * AESLibDecKey : Set the key for decryption operations.
 * 
 * refNum   :  The AESLib reference number
 * in_key   :  The byte array containing the key
 * klen     :  The length of the key (16 or 32 bytes)
 * cx       :  The aes context to store the key in.
 *
 **************************************************************************/

aes_rval AESLibDecKey(UInt16 refNum, const unsigned char in_key[],
                      unsigned int klen, aes_ctx cx[1])
     AESLIB_TRAP(AESLibDecKey_trapno);

/**************************************************************************
 * AESLibDecBlk : Decrypt one 16 byte block
 * 
 * refNum   :  The AESLib reference number
 * in_blk   :  The 16 byte block of ciphertext to decrypt
 * out_blk  :  The 16 byte block to hold the resulting plaintext
 * cx       :  The aes context you initialized with AESLibDecKey
 *
 **************************************************************************/

aes_rval AESLibDecBlk(UInt16 refNum, const unsigned char in_blk[],
                      unsigned char out_blk[], const aes_ctx cx[1])
     AESLIB_TRAP(AESLibDecBlk_trapno);

/**************************************************************************
 *
 * New functions in version 3.1
 * 
 * The AESLib{Enc,Dec}BigBlk functions offer Cipher Block Chaining (CBC)
 * and the ability to encrypt/decrypt multiple blocks in one function call.
 * On ARM platforms the speedup is significant versus calling {Enc,Dec}Blk
 * in a loop.  Run the AESSpeedTest from the SDK's Example directory to
 * see the difference.
 *
 *************************************************************************/

/**************************************************************************
 * AESLibEncBigBlk : Encrypt multiple 16 byte blocks at once. Supports
 * Cipher Block Chaining and the use of a initialization vector.
 * 
 * refNum   :  The AESLib reference number
 * in_blk   :  The array of data to encrypt
 * out_blk  :  The array to store the encrypted data in
 * blen     :  On input, contains the number of bytes to encrypt.  On output
 *             contains the number of bytes actually encrypted. The function
 *             will only encrypt multiples of 16 bytes, any extra bytes will
 *             not be processed.
 * cbc_mode :  If true the function will user Cipher Block Chaining.
 * iv       :  Optional initialization vector (16 bytes) for Cipher Block
 *             Chaining.
 * cx       :  The aes context you initialized with AESLibEncKey
 *
 **************************************************************************/

aes_rval AESLibEncBigBlk(UInt16 refNum, const unsigned char in_blk[],
                         unsigned char out_blk[], unsigned long *blen,
                         Boolean cbc_mode, const unsigned char iv[],
                         const aes_ctx cx[1])
     AESLIB_TRAP(AESLibEncBigBlk_trapno);

/**************************************************************************
 * AESLibDecBigBlk : Decrypt multiple 16 byte blocks at once. Supports
 * Cipher Block Chaining and use of a initialization vector.
 *
 * refNum   :  The AESLib reference number
 * in_blk   :  The array of data to decrypt
 * out_blk  :  The array to store the decrypted data in
 * blen     :  On input, contains the number of bytes to decrypt.  On output
 *             contains the number of bytes actually decrypted. The function
 *             will only decrypt multiples of 16 bytes, any extra bytes will
 *             not be processed.
 * cbc_mode :  If true the function will user Cipher Block Chaining.
 * iv       :  Optional initialization vector (16 bytes) for Cipher Block
 *             Chaining.
 * cx       :  The aes context you initialized with AESLibDecKey
 *
 **************************************************************************/
 
aes_rval AESLibDecBigBlk(UInt16 refNum, const unsigned char in_blk[],
                         unsigned char out_blk[], unsigned long *blen,
                         Boolean cbc_mode, const unsigned char iv[],
                         const aes_ctx cx[1])
     AESLIB_TRAP(AESLibDecBigBlk_trapno);

/**************************************************************************
 *
 * Compatibility macros for Gladman functions
 *
 **************************************************************************/

extern UInt16 AESLibRefNum;

#define aes_enc_key(in_key, klen, cx) \
          AESLibEncKey(AESLibRefNum, in_key, klen, cx)

#define aes_enc_blk(in_blk, out_blk, cx) \
          AESLibEncBlk(AESLibRefNum, in_blk, out_blk, cx)

#define aes_dec_key(in_key, klen, cx) \
          AESLibDecKey(AESLibRefNum, in_key, klen, cx)

#define aes_dec_blk(in_blk, out_blk, cx) \
          AESLibDecBlk(AESLibRefNum, in_blk, out_blk, cx)



#ifdef __cplusplus
}
#endif

#define AES_INLINE __inline

/**************************************************************************
 *
 * Inline function to conveniently open the library.  
 *
 **************************************************************************/

AES_INLINE Err AESLib_OpenLibrary(UInt16 *refNumP)
{
  Err error;
  Boolean loaded = false;
  
  /* first try to find the library */
  error = SysLibFind(AESLibName, refNumP);
  
  /* If not found, load the library instead */
  if (error == sysErrLibNotFound)
  {
    error = SysLibLoad(AESLibTypeID, AESLibCreatorID, refNumP);
    loaded = true;
  }
  
  if (error == errNone)
  {
    error = AESLibOpen(*refNumP);
    if (error != errNone)
    {
      if (loaded)
      {
        SysLibRemove(*refNumP);
      }
      
      *refNumP = sysInvalidRefNum;
    }
  }
  
  return error;
}

AES_INLINE Err AESLib_OpenLibrary_Force68K(UInt16 *refNumP)
{
  Err error;
  
  FtrSet (AESLibCreatorID, AESLibFtrNumForce68K, 1);

  error = AESLib_OpenLibrary(refNumP);

  return error;

}

/**************************************************************************
 *
 * Inline function to conveniently load and open the library when it
 * has not been installed separately.  If you have included the AESLib's
 * code resource into your PRC you can use this function to easily install
 * it as a system shared library.  The arguments to this function should
 * be your application or shared library's database type, creator ID, and 
 * the resource type and resource id you used to store the AESLib code 
 * resource.  This function is useful for developer's who wish to include
 * AESLib in their application or shared library and do not want to 
 * distribute a separate PRC.  See the sample projects for an example.
 *
 * This code was adopted from SysLibLoad as presented in the OS 4.0 sources.
 *
 **************************************************************************/

AES_INLINE Err AESLib_LoadLibrary(UInt16 *refNumP, 
                                  UInt32 DbType, UInt32 DbCreatorID,
                                  DmResType type, DmResID id)
{
  Err                         error;
  MemHandle            codeRscH = 0;
  SysLibEntryProcPtr   codeRscP = 0;
  UInt16                 cardNo = 0;
  LocalID                  dbID = 0;
  DmSearchStateType     searchState;
  DmOpenRef                 dbR = 0;
  SysLibTblEntryPtr      entryP = 0;
  Boolean          libInROM = false;
  Boolean            loaded = false;
  
  ErrFatalDisplayIf(!refNumP, "null arg");

  *refNumP = sysInvalidRefNum;

  DmGetNextDatabaseByTypeCreator(true, &searchState, DbType, 
                                 DbCreatorID, true, &cardNo, &dbID);

  if (!dbID)
    return sysErrLibNotFound;

  libInROM = (MemLocalIDKind(dbID) != memIDHandle);

  if (0 == (dbR = DmOpenDatabase(cardNo, dbID, dmModeReadOnly)))
    return sysErrNoFreeRAM;

  if (0 == (codeRscH = DmGet1Resource(type, id)))
  {
    ErrNonFatalDisplay("Can't get library rsrc");
    error = sysErrLibNotFound;
  }
  else
  {
    codeRscP = (SysLibEntryProcPtr)MemHandleLock(codeRscH);
    error = SysLibInstall(codeRscP, refNumP);
    
    if (!error && !libInROM)
      DmDatabaseProtect(cardNo, dbID, true);
  }

  DmCloseDatabase(dbR);
  dbR = 0;

  if (error)
  {
    *refNumP = sysInvalidRefNum;
    
    if (codeRscP && !libInROM)
    {
      MemPtrUnlock((void*)codeRscP);
      DmReleaseResource(codeRscH);
    }
  }
  else
  {
    entryP = SysLibTblEntry(*refNumP);
    entryP->dbID = dbID;
    if (libInROM)
    {
      entryP->codeRscH = 0;
    }
    else
    {
      entryP->codeRscH = codeRscH;
    }
  }
  
  if (error == errNone)
  {
    error = AESLibOpen(*refNumP);
    if (error != errNone)
    {
      if (loaded)
      {
        SysLibRemove(*refNumP);
      }
      
      *refNumP = sysInvalidRefNum;
    }
  }
  
  return error;
}

AES_INLINE Err AESLib_LoadLibrary_Force68K(UInt16 *refNumP, 
                                           UInt32 DbType, UInt32 DbCreatorID,
                                           DmResType type, DmResID id)
{
  Err                         error;

  FtrSet (AESLibCreatorID, AESLibFtrNumForce68K, 1);

  error = AESLib_LoadLibrary(refNumP, DbType, DbCreatorID, type, id);

  return error;
}


/**************************************************************************
 *
 * Inline function to conveniently close the library.  
 *
 **************************************************************************/


AES_INLINE Err AESLib_CloseLibrary(UInt16 refNum)
{
  Err error;
  
  if (refNum == sysInvalidRefNum)
  {
    return sysErrParamErr;
  }
  
  error = AESLibClose(refNum);
  
  if (error == errNone)
  {
    /* no users left, so unload library */
    SysLibRemove(refNum);
  } 
  else if (error != sysErrParamErr)
  {
    /* don't unload library, but mask "still open" from caller  */
    error = errNone;
  }
  
  return error;
}

AES_INLINE Err AESLib_CloseLibrary_Force68K(UInt16 refNum)
{
  Err error;

  FtrUnregister (AESLibCreatorID, AESLibFtrNumForce68K);

  error = AESLib_CloseLibrary(refNum);

  return error;
}


#endif /* AESLIB_H */
