
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "uls_polaris_UlsOStream.h"
#include <uls/uls_ostream.h>

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsOStream_createOFile(JNIEnv *env, jobject obj, jstring jfilepath, jobject juls, jstring jsubname)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_ostream_ptr_t ostr;
	const char *filepath, *subname;
	jobject jostr;

	filepath = (*env)->GetStringUTFChars(env, jfilepath, 0);
	subname = (*env)->GetStringUTFChars(env, jsubname, 0);

	ostr = ulsjava_create_ostream_file(filepath, -1, uls, subname, -1);
	(*env)->ReleaseStringUTFChars(env, jfilepath, filepath);
	(*env)->ReleaseStringUTFChars(env, jsubname, subname);

	if (ostr == NULL) {
		return NULL;
	}

	jostr = (*env)->NewDirectByteBuffer(env, (void *) ostr, sizeof(uls_ostream_t));
	return jostr;
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsOStream_destroyOFile(JNIEnv *env, jobject obj, jobject jostr)
{
	uls_ostream_ptr_t ostr = (uls_ostream_ptr_t) (*env)->GetDirectBufferAddress(env, jostr);
	uls_destroy_ostream(ostr);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsOStream_printTok(JNIEnv *env, jobject obj, jobject jostr, jint jtokid, jstring jtokstr)
{
	uls_ostream_ptr_t ostr = (uls_ostream_ptr_t) (*env)->GetDirectBufferAddress(env, jostr);
	const char *tokstr;
	int rc;

	tokstr = (*env)->GetStringUTFChars(env, jtokstr, 0);
	rc = ulsjava_print_tok(ostr, jtokid, tokstr, -1);
	(*env)->ReleaseStringUTFChars(env, jtokstr, tokstr);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsOStream_printTokLinenum(JNIEnv *env, jobject obj, jobject jostr, jint jlno, jstring jtag)
{
	uls_ostream_ptr_t ostr = (uls_ostream_ptr_t) (*env)->GetDirectBufferAddress(env, jostr);
	const char *tag;
	int rc;

	tag = (*env)->GetStringUTFChars(env, jtag, 0);
	rc = ulsjava_print_tok_linenum(ostr, jlno, tag, -1);
	(*env)->ReleaseStringUTFChars(env, jtag, tag);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsOStream_startStream(JNIEnv *env, jobject obj, jobject jostr, jint jflags)
{
	uls_ostream_ptr_t ostr = (uls_ostream_ptr_t) (*env)->GetDirectBufferAddress(env, jostr);
	int rc;

	rc = uls_start_stream(ostr, (int) jflags);
	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsOStream_getFlagLineNumbering(JNIEnv *env, jobject obj)
{
	return _uls_const_LINE_NUMBERING();
}

