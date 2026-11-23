
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "uls_polaris_UlsOStream.h"
#include <uls/uls_core.h>
#include <uls/uls_ostream.h>

extern const char *GetStringUTFChars_Safely(JNIEnv *env, jstring jstr);
extern void ReleaseStringUTFChars_Safely(JNIEnv *env, jstring jstr, const char *cstr);
extern jobject new_jniobj(JNIEnv *env, void *vptr);

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsOStream_createOFile(
	JNIEnv *env, jobject obj, jstring jfilepath, jobject juls, jstring jsubname)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;

	uls_ostream_ptr_t ostr;
	const char *filepath, *subname;

	filepath = (*env)->GetStringUTFChars(env, jfilepath, 0);
	subname = GetStringUTFChars_Safely(env, jsubname);

	ostr = ulsjava_create_ostream_file(filepath, -1, uls, subname, -1);
	(*env)->ReleaseStringUTFChars(env, jfilepath, filepath);
	ReleaseStringUTFChars_Safely(env, jsubname, subname);

	return new_jniobj(env, ostr);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsOStream_destroyOFile(
	JNIEnv *env, jobject obj, jobject jostr)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jostr);
	uls_ostream_ptr_t ostr = (uls_ostream_ptr_t) jobj->vptr;
	uls_destroy_ostream(ostr);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsOStream_printTok(
	JNIEnv *env, jobject obj, jobject jostr, jint jtokid, jstring jtokstr)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jostr);
	uls_ostream_ptr_t ostr = (uls_ostream_ptr_t) jobj->vptr;
	const char *tokstr;
	int rc;

	tokstr = (*env)->GetStringUTFChars(env, jtokstr, 0);
	rc = ulsjava_print_tok(ostr, jtokid, tokstr, -1);
	(*env)->ReleaseStringUTFChars(env, jtokstr, tokstr);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsOStream_printTokLinenum(
	JNIEnv *env, jobject obj, jobject jostr, jint jlno, jstring jtag)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jostr);
	uls_ostream_ptr_t ostr = (uls_ostream_ptr_t) jobj->vptr;
	const char *tag;
	int rc;

	tag = GetStringUTFChars_Safely(env, jtag);
	rc = ulsjava_print_tok_linenum(ostr, jlno, tag, -1);
	ReleaseStringUTFChars_Safely(env, jtag, tag);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsOStream_startStream(
	JNIEnv *env, jobject obj, jobject jostr, jint jflags)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jostr);
	uls_ostream_ptr_t ostr = (uls_ostream_ptr_t) jobj->vptr;
	int rc;

	rc = uls_start_stream(ostr, (int) jflags);
	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsOStream_getFlagLineNumbering(JNIEnv *env, jobject obj)
{
	return _uls_const_LINE_NUMBERING();
}

