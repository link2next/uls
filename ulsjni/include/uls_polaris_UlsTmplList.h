/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class uls_polaris_UlsTmplList */

#ifndef _Included_uls_polaris_UlsTmplList
#define _Included_uls_polaris_UlsTmplList
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     uls_polaris_UlsTmplList
 * Method:    createTmplList
 * Signature: (II)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_uls_polaris_UlsTmplList_createTmplList
  (JNIEnv *, jclass, jint, jint);

/*
 * Class:     uls_polaris_UlsTmplList
 * Method:    destroyTmplList
 * Signature: (Ljava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsTmplList_destroyTmplList
  (JNIEnv *, jclass, jobject);

/*
 * Class:     uls_polaris_UlsTmplList
 * Method:    resetTmplList
 * Signature: (Ljava/lang/Object;I)V
 */
JNIEXPORT void JNICALL Java_uls_polaris_UlsTmplList_resetTmplList
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     uls_polaris_UlsTmplList
 * Method:    addTmpl
 * Signature: (Ljava/lang/Object;Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsTmplList_addTmpl
  (JNIEnv *, jclass, jobject, jstring, jstring);

#ifdef __cplusplus
}
#endif
#endif