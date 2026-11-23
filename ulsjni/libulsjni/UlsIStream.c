
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "uls_polaris_UlsTmplList.h"
#include "uls_polaris_UlsIStream.h"
#include <uls/uls_istream.h>
#include <uls/uls_lex.h>

extern const char *GetStringUTFChars_Safely(JNIEnv *env, jstring jstr);
extern void ReleaseStringUTFChars_Safely(JNIEnv *env, jstring jstr, const char *cstr);
extern jobject new_jniobj(JNIEnv *env, void *vptr);

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsTmplList_createTmplList(JNIEnv *env, jobject obj,
	jint n_alloc, jint jflags)
{
	uls_tmpl_list_ptr_t tmpls;
	tmpls = uls_create_tmpls(n_alloc, jflags);

	return new_jniobj(env, tmpls);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsTmplList_destroyTmplList(JNIEnv *env, jobject obj, jobject jtmpls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jtmpls);
	uls_tmpl_list_ptr_t tmpls = (uls_tmpl_list_ptr_t) jobj->vptr;
	int rc;

	rc = uls_destroy_tmpls(tmpls);
	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsTmplList_resetTmplList(JNIEnv *env, jobject obj,
	jobject jtmpls, jint n_alloc)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jtmpls);
	uls_tmpl_list_ptr_t tmpls = (uls_tmpl_list_ptr_t) jobj->vptr;
	uls_reset_tmpls(tmpls, n_alloc);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsTmplList_addTmpl(JNIEnv *env, jobject obj,
	jobject jtmpls, jstring jname, jstring jval)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jtmpls);
	uls_tmpl_list_ptr_t tmpls = (uls_tmpl_list_ptr_t) jobj->vptr;
	const char *name, *val;
	int rc;

	name = (*env)->GetStringUTFChars(env, jname, 0);
	val = GetStringUTFChars_Safely(env, jval);

	rc = ulsjava_add_tmpl(tmpls, name, -1, val, -1);

	(*env)->ReleaseStringUTFChars(env, jname, name);
	ReleaseStringUTFChars_Safely(env, jval, val);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsIStream_openIFile(JNIEnv *env, jobject obj, jstring jfilepath)
{
	const char *filepath;
	uls_istream_ptr_t istr;


	filepath = (*env)->GetStringUTFChars(env, jfilepath, 0);
	istr = ulsjava_open_istream_file(filepath, -1);
	(*env)->ReleaseStringUTFChars(env, jfilepath, filepath);

	return new_jniobj(env, istr);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsIStream_destroyIFile(JNIEnv *env, jobject obj, jobject jistr)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jistr);
	uls_istream_ptr_t istr = (uls_istream_ptr_t) jobj->vptr;
	uls_destroy_istream(istr);
}
