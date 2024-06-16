/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class uls_polaris_UlsLex */

#ifndef _Included_uls_polaris_UlsLex
#define _Included_uls_polaris_UlsLex
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     uls_polaris_UlsLex
 * Method:    createUls
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_uls_polaris_UlsLex_createUls
  (JNIEnv *, jclass, jstring);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    destroyUls
 * Signature: (Ljava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_destroyUls
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    toknumEOI
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumEOI
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    toknumEOF
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumEOF
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    toknumERR
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumERR
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    toknumNONE
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumNONE
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    toknumID
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumID
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    toknumNUMBER
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumNUMBER
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    toknumTMPL
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumTMPL
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    tokNum
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_tokNum
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    tokStr
 * Signature: (Ljava/lang/Object;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_tokStr
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    lexeme
 * Signature: (Ljava/lang/Object;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_lexeme
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    getTok
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getTok
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    isLexemeReal
 * Signature: (Ljava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isLexemeReal
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    isLexemeInt
 * Signature: (Ljava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isLexemeInt
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    isLexemeZero
 * Signature: (Ljava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isLexemeZero
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    isQuoteTok
 * Signature: (Ljava/lang/Object;I)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isQuoteTok
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    toInt
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toInt
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    toLong
 * Signature: (Ljava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL Java_uls_polaris_UlsLex_toLong
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    toReal
 * Signature: (Ljava/lang/Object;)D
 */
JNIEXPORT jdouble JNICALL Java_uls_polaris_UlsLex_toReal
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    numberSuffix
 * Signature: (Ljava/lang/Object;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_numberSuffix
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    expect
 * Signature: (Ljava/lang/Object;I)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_expect
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    ungetStr
 * Signature: (Ljava/lang/Object;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_ungetStr
  (JNIEnv *, jclass, jobject, jstring);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    ungetTok
 * Signature: (Ljava/lang/Object;Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_ungetTok
  (JNIEnv *, jclass, jobject, jstring, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    ungetCh
 * Signature: (Ljava/lang/Object;I)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_ungetCh
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    dumpTok
 * Signature: (Ljava/lang/Object;Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_dumpTok
  (JNIEnv *, jclass, jobject, jstring, jstring);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    tok2keyw
 * Signature: (Ljava/lang/Object;I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_tok2keyw
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    tok2name
 * Signature: (Ljava/lang/Object;I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_tok2name
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    isCharSpace
 * Signature: (Ljava/lang/Object;I)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharSpace
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    isCharIdFirst
 * Signature: (Ljava/lang/Object;I)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharIdFirst
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    isCharId
 * Signature: (Ljava/lang/Object;I)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharId
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    isCharQuote
 * Signature: (Ljava/lang/Object;I)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharQuote
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    isCharTokCh1
 * Signature: (Ljava/lang/Object;I)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharTokCh1
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    isCharTokCh2
 * Signature: (Ljava/lang/Object;I)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharTokCh2
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    skipWhiteSpaces
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_skipWhiteSpaces
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    peekCh
 * Signature: (Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_uls_polaris_UlsLex_peekCh
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    getCh
 * Signature: (Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_uls_polaris_UlsLex_getCh
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    uchFromNextchInfo
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_uchFromNextchInfo
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    tokFromNextchInfo
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_tokFromNextchInfo
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    putNextchInfo
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_putNextchInfo
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    getTag
 * Signature: (Ljava/lang/Object;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_getTag
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    setTag
 * Signature: (Ljava/lang/Object;Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_setTag
  (JNIEnv *, jclass, jobject, jstring, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    getLineno
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getLineno
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    setLineno
 * Signature: (Ljava/lang/Object;I)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_setLineno
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    incLineno
 * Signature: (Ljava/lang/Object;I)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_incLineno
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    pushLine
 * Signature: (Ljava/lang/Object;Ljava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_pushLine
  (JNIEnv *, jclass, jobject, jstring, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    pushFile
 * Signature: (Ljava/lang/Object;Ljava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_pushFile
  (JNIEnv *, jclass, jobject, jstring, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    pushIStream
 * Signature: (Ljava/lang/Object;Ljava/lang/Object;Ljava/lang/Object;I)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_pushIStream
  (JNIEnv *, jclass, jobject, jobject, jobject, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    setLine
 * Signature: (Ljava/lang/Object;Ljava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_setLine
  (JNIEnv *, jclass, jobject, jstring, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    setFile
 * Signature: (Ljava/lang/Object;Ljava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_setFile
  (JNIEnv *, jclass, jobject, jstring, jint);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    popInput
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_popInput
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    popAllInputs
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_popAllInputs
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    getTokIdList
 * Signature: (Ljava/lang/Object;)[I
 */
JNIEXPORT jintArray JNICALL Java_uls_polaris_UlsLex_getTokIdList
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    getFlagWantEofTok
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getFlagWantEofTok
  (JNIEnv *, jclass);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    getFlagStreamBinLe
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getFlagStreamBinLe
  (JNIEnv *, jclass);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    getFlagStreamBinBe
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getFlagStreamBinBe
  (JNIEnv *, jclass);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    getFlagNextChNone
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getFlagNextChNone
  (JNIEnv *, jclass);

/*
 * Class:     uls_polaris_UlsLex
 * Method:    getFlagTmplsDup
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getFlagTmplsDup
  (JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif
#endif
