
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "uls_polaris_UlsLex.h"

#include <uls/uls_lex.h>
#include <uls/uld_conf.h>

const char *GetStringUTFChars_Safely(JNIEnv *env, jstring jstr)
{
	const char *cstr;

	if (jstr != NULL) {
		cstr = (*env)->GetStringUTFChars(env, jstr, 0);
	} else {
		cstr = NULL;
	}

	return cstr;
}

void ReleaseStringUTFChars_Safely(JNIEnv *env, jstring jstr, const char *cstr)
{
	if (cstr != NULL) {
		(*env)->ReleaseStringUTFChars(env, jstr, cstr);
	}
}

jobject new_jniobj(JNIEnv *env, void *vptr)
{
	uls_jniobj_ptr_t jniobj;
	jobject jobj;

	if (!vptr) {
		return NULL;
	}

	jniobj = (uls_jniobj_ptr_t) malloc(sizeof(uls_jniobj_t));
	if (!jniobj) {
		// throw exception
		return NULL;
	}

	jniobj->timestamp = 0;
	jniobj->vptr = vptr;
	jobj = (*env)->NewDirectByteBuffer(env, (void *) jniobj, sizeof(uls_jniobj_t));

	return jobj;
}

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsLex_createUls(JNIEnv *env, jobject obj, jstring jconfname)
{
	uls_lex_t *uls;
	const char *confname;

	confname = (*env)->GetStringUTFChars(env, jconfname, 0);
	uls = uls_create(confname);
	(*env)->ReleaseStringUTFChars(env, jconfname, confname);

	return new_jniobj(env, uls);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_destroyUls(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	uls_destroy(uls);
	return JNI_TRUE;
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumEOI(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return (jint) _uls_toknum_EOI(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumEOF(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return (jint) _uls_toknum_EOF(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumERR(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return (jint) _uls_toknum_ERR(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumNONE(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return (jint) _uls_toknum_NONE(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumID(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return (jint) _uls_toknum_ID(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumNUMBER(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return (jint) _uls_toknum_NUMBER(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumTMPL(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return (jint) _uls_toknum_TMPL(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_tokNum(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return _uls_tok_id(uls);
}

JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_tokStr(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	const char *ret_str = _uls_lexeme(uls);
	return (*env)->NewStringUTF(env, ret_str);
}

JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_lexeme(JNIEnv *env, jobject obj, jobject juls)
{
	return Java_uls_polaris_UlsLex_tokStr(env, obj, juls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getTok(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return uls_get_tok(uls);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isLexemeReal(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return uls_is_real(uls) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isLexemeInt(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return uls_is_int(uls) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isLexemeZero(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return uls_is_zero(uls) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toInt(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return (jint) uls_lexeme_int32(uls);
}

JNIEXPORT jlong JNICALL Java_uls_polaris_UlsLex_toLong(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return (jlong) uls_lexeme_int64(uls);
}

JNIEXPORT jdouble JNICALL Java_uls_polaris_UlsLex_toReal(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return (jdouble) uls_lexeme_double(uls);
}

JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_numberSuffix(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	const char *ret_str = uls_number_suffix(uls);
	return (*env)->NewStringUTF(env, ret_str);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_expect(JNIEnv *env, jobject obj,
	jobject juls, jint tokExpected)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	uls_expect(uls, tokExpected);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_ungetStr(JNIEnv *env, jobject obj,
	jobject juls, jstring jstr)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	const char *str;
	int rval;

	str = (*env)->GetStringUTFChars(env, jstr, 0);
	rval = ulsjava_unget_str(uls, (void *) str, -1);
	(*env)->ReleaseStringUTFChars(env, jstr, str);

	return (rval > 0) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_ungetTok(JNIEnv *env, jobject obj,
	jobject juls, jint jtokid, jstring jlxm)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	const char *lxm;
	int rval;

	lxm = GetStringUTFChars_Safely(env, jlxm);
	rval = ulsjava_unget_lexeme(uls, jtokid, (void *) lxm, -1);
	ReleaseStringUTFChars_Safely(env, jlxm, lxm);

	return (rval > 0) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_ungetChar(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	int rval;

	rval = uls_unget_ch(uls, jch);

	return (rval < 0) ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_dumpTok(JNIEnv *env, jobject obj,
	jobject juls, jstring jpfx, jstring jsuff)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	const char *pfx, *suff;

	pfx = GetStringUTFChars_Safely(env, jpfx);
	suff = GetStringUTFChars_Safely(env, jsuff);

	ulsjava_dump_tok(uls, pfx, -1, suff, -1);

	ReleaseStringUTFChars_Safely(env, jpfx, pfx);
	ReleaseStringUTFChars_Safely(env, jsuff, suff);
}

JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_tok2name(JNIEnv *env, jobject obj, jobject juls, jint jtokid)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	const char *name = uls_tok2name(uls, jtokid);
	return (*env)->NewStringUTF(env, name);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharSpace(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return _uls_is_wch_space(uls, jch) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharIdFirst(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return _uls_is_wch_idfirst(uls, jch) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharId(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return _uls_is_wch_id(uls, jch) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharQuote(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return _uls_is_wch_quote(uls, jch) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharTokCh1(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return _uls_is_wch_1ch_token(uls, jch) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharTokCh2(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return _uls_is_wch_2ch_token(uls, jch) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_skipWhiteSpaces(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	uls_skip_white_spaces(uls);
}

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsLex_peekCharDetail(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	uls_nextch_detail_ptr_t ch_detail;

	ch_detail = ulsjava_peek_nextch_info(uls);

	return new_jniobj(env, ch_detail);
}

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsLex_getCharDetail(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	uls_nextch_detail_ptr_t ch_detail;

	ch_detail = ulsjava_get_nextch_info(uls);
	return new_jniobj(env, ch_detail);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_uchFromNextchInfo(JNIEnv *env, jobject obj, jobject jch_detail)
{
	uls_nextch_detail_ptr_t ch_detail = (uls_nextch_detail_ptr_t) (*env)->GetDirectBufferAddress(env, jch_detail);
	return ulsjava_get_ch_from_nextch(ch_detail);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_tokFromNextchInfo(JNIEnv *env, jobject obj, jobject jch_detail)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jch_detail);
	uls_nextch_detail_ptr_t ch_detail = (uls_nextch_detail_ptr_t) jobj->vptr;
	return ulsjava_get_tok_from_nextch(ch_detail);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_putNextchInfo(JNIEnv *env, jobject obj, jobject jch_detail)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jch_detail);
	uls_nextch_detail_ptr_t ch_detail = (uls_nextch_detail_ptr_t) jobj->vptr;
	ulsjava_put_nextch_info(ch_detail);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isQuoteTok(JNIEnv *env, jobject obj, jobject juls, jint jtokid)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return (uls_get_quote(uls, jtokid) != NULL) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_getTag(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	const char *tag = _uls_get_tag(uls);
	return (*env)->NewStringUTF(env, tag);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_setTag(JNIEnv *env, jobject obj, jobject juls, jstring jtag, jint jlineno)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	const char *tag;

	tag = GetStringUTFChars_Safely(env, jtag);
	ulsjava_set_tag(uls, tag, -1, jlineno);
	ReleaseStringUTFChars_Safely(env, jtag, tag);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getLineno(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	return uls_get_lineno(uls);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_setLineno(JNIEnv *env, jobject obj, jobject juls, jint jlineno)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	uls_set_lineno(uls, jlineno);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_incLineno(JNIEnv *env, jobject obj, jobject juls, jint jamount)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	uls_inc_lineno(uls, jamount);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_pushLine(JNIEnv *env, jobject obj, jobject juls, jstring jline, jint jflags)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	const char *line;
	int rc;

	line = (*env)->GetStringUTFChars(env, jline, 0);
	rc = ulsjava_push_line(uls, line, -1, (int) jflags);
	(*env)->ReleaseStringUTFChars(env, jline, line);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_pushFile(JNIEnv *env, jobject obj, jobject juls, jstring jfilepath, jint jflags)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	const char *filepath;
	int rc;

	filepath = (*env)->GetStringUTFChars(env, jfilepath, 0);
	rc = ulsjava_push_file(uls, filepath, -1, (int) jflags);
	(*env)->ReleaseStringUTFChars(env, jfilepath, filepath);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_pushIStream(JNIEnv *env, jobject obj, jobject juls, jobject jistr,
	jobject jtmpls, jint jflags)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;

	jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jistr);
	uls_istream_ptr_t istr = (uls_istream_ptr_t) jobj->vptr;

	jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jtmpls);
	uls_tmpl_list_ptr_t tmpls = (uls_tmpl_list_ptr_t) jobj->vptr;

	int rc;

	rc = uls_push_istream(uls, istr, tmpls, (int) jflags);
	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_setLine(JNIEnv *env, jobject obj, jobject juls, jstring jline, jint jflags)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	const char *line;
	int rc;

	line = (*env)->GetStringUTFChars(env, jline, 0);
	rc = ulsjava_set_line(uls, line, -1, (int) jflags);
	(*env)->ReleaseStringUTFChars(env, jline, line);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_setFile(JNIEnv *env, jobject obj, jobject juls, jstring jfilepath, jint jflags)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	const char *filepath;
	int rc;

	filepath = (*env)->GetStringUTFChars(env, jfilepath, 0);
	rc = ulsjava_set_file(uls, filepath, -1, (int) jflags);
	(*env)->ReleaseStringUTFChars(env, jfilepath, filepath);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_popInput(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	uls_pop(uls);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_popAllInputs(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	uls_pop_all(uls);
}

JNIEXPORT jintArray JNICALL Java_uls_polaris_UlsLex_getTokIdList(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	jintArray ret_id_array;
	int *id_array;
	int n;

	n = ulsjava_get_tokid_list(uls, &id_array);
	if (n < 0) {
		return NULL;
	}

	ret_id_array = (*env)->NewIntArray(env, n);
	(*env)->SetIntArrayRegion(env, ret_id_array, 0, n, id_array);

	ulsjava_put_tokid_list(uls, &id_array);
	return ret_id_array;
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getFlagWantEofTok(JNIEnv *env, jobject obj)
{
	return _uls_const_WANT_EOFTOK();
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getFlagStreamBinLe(JNIEnv *env, jobject obj)
{
	return _uls_const_STREAM_BIN_LE();
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getFlagStreamBinBe(JNIEnv *env, jobject obj)
{
	return _uls_const_STREAM_BIN_BE();
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getFlagNextChNone(JNIEnv *env, jobject obj)
{
	return _uls_const_NEXTCH_NONE();
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getFlagTmplsDup(JNIEnv *env, jobject obj)
{
	return _uls_const_TMPLS_DUP();
}

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsLex_prepareUldMapInternal(JNIEnv *env, jobject obj, jobject juls)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_lex_ptr_t uls = (uls_lex_ptr_t) jobj->vptr;
	uld_names_map_ptr_t names_map;

	names_map = uld_prepare_names(uls);
	return new_jniobj(env, names_map);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_changeUldNamesInternal(JNIEnv *env, jobject obj,
	jobject jnames_map, jstring jname, jstring jname2,
	jboolean jtokid_changed, jint jtokid, jstring jaliases)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jnames_map);
	uld_names_map_ptr_t names_map = (uld_names_map_ptr_t) jobj->vptr;
	const char *name, *name2, *aliases;
	int rc, tokid_changed;

	name = (*env)->GetStringUTFChars(env, jname, 0);
	name2 = GetStringUTFChars_Safely(env, jname2);
	aliases = GetStringUTFChars_Safely(env, jaliases);

	tokid_changed = (jtokid_changed == JNI_TRUE) ? 1 : 0;
	rc = uld_change_names_raw(names_map, name, name2, tokid_changed, jtokid, aliases);

	(*env)->ReleaseStringUTFChars(env, jname, name);
	ReleaseStringUTFChars_Safely(env, jname2, name2);
	ReleaseStringUTFChars_Safely(env, jaliases, aliases);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_finishUldMapInternal(JNIEnv *env, jobject obj,
	jobject jnames_map)
{
	uls_jniobj_ptr_t jobj = (uls_jniobj_ptr_t) (*env)->GetDirectBufferAddress(env, jnames_map);
	uld_names_map_ptr_t names_map = (uld_names_map_ptr_t) jobj->vptr;
	jboolean jstat;

	if (uld_post_names(names_map) < 0) {
		jstat = JNI_FALSE;
	} else {
		jstat = JNI_TRUE;
	}

	return jstat;
}
