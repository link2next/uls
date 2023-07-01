
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "uls_polaris_UlsTmplList.h"
#include "uls_polaris_UlsIStream.h"
#include <uls/uls_istream.h>
#include <uls/uls_lex.h>

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsTmplList_createTmplList(JNIEnv *env, jobject obj, jint n_alloc, jint jflags)
{
	uls_tmpl_list_ptr_t tmpls;
	jobject jtmpls;

	if ((tmpls = uls_create_tmpls(n_alloc, jflags)) == NULL) {
		return NULL;
	}

	jtmpls = (*env)->NewDirectByteBuffer(env, (void *) tmpls, sizeof(uls_tmpl_list_t));
	return jtmpls;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsTmplList_destroyTmplList(JNIEnv *env, jobject obj, jobject jtmpls)
{
	uls_tmpl_list_ptr_t tmpls;
	int rc;

	tmpls = (uls_tmpl_list_ptr_t) (*env)->GetDirectBufferAddress(env, jtmpls);
	rc = uls_destroy_tmpls(tmpls);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsTmplList_resetTmplList(JNIEnv *env, jobject obj, jobject jtmpls, jint n_alloc)
{
	uls_tmpl_list_ptr_t tmpls = (uls_tmpl_list_ptr_t) (*env)->GetDirectBufferAddress(env, jtmpls);
	uls_reset_tmpls(tmpls, n_alloc);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsTmplList_addTmpl(JNIEnv *env, jobject obj, jobject jtmpls, jstring jname, jstring jval)
{
	uls_tmpl_list_ptr_t tmpls = (uls_tmpl_list_ptr_t) (*env)->GetDirectBufferAddress(env, jtmpls);
	const char *name, *val;
	int rc;

	name = (*env)->GetStringUTFChars(env, jname, 0);
	val = (*env)->GetStringUTFChars(env, jval, 0);

	rc = ulsjava_add_tmpl(tmpls, name, -1, val, -1);

	(*env)->ReleaseStringUTFChars(env, jname, name);
	(*env)->ReleaseStringUTFChars(env, jval, val);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsIStream_openIFile(JNIEnv *env, jobject obj, jstring jfilepath)
{
	const char *filepath;
	uls_istream_ptr_t istr;
	jobject jistr;

	filepath = (*env)->GetStringUTFChars(env, jfilepath, 0);
	istr = ulsjava_open_istream_file(filepath, -1);
	(*env)->ReleaseStringUTFChars(env, jfilepath, filepath);

	if (istr == NULL) {
		return NULL;
	}

	jistr = (*env)->NewDirectByteBuffer(env, (void *) istr, sizeof(uls_istream_t));
	return jistr;
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsIStream_destroyIFile(JNIEnv *env, jobject obj, jobject jistr)
{
	uls_istream_ptr_t istr = (uls_istream_ptr_t) (*env)->GetDirectBufferAddress(env, jistr);
	uls_destroy_istream(istr);
}
