
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "uls_polaris_UlsLex.h"

#include <uls/uls_lex.h>

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsLex_createUls(JNIEnv *env, jobject obj, jstring confname)
{
	const char *str = (*env)->GetStringUTFChars(env, confname, 0);
	uls_lex_t *uls;
	jobject juls;

	uls = uls_create(str);
	(*env)->ReleaseStringUTFChars(env, confname, str);

	if (uls == NULL) {
		// throw exception
		juls = NULL;
	} else {
		juls = (*env)->NewDirectByteBuffer(env, (void *) uls, sizeof(uls_lex_t));
	}

	return juls;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_destroyUls(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_destroy(uls);
	return JNI_TRUE;
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumEOI(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return (jint) _uls_toknum_EOI(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumEOF(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return (jint) _uls_toknum_EOF(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumERR(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return (jint) _uls_toknum_ERR(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumNONE(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return (jint) _uls_toknum_NONE(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumID(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return (jint) _uls_toknum_ID(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumNUMBER(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return (jint) _uls_toknum_NUMBER(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toknumTMPL(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return (jint) _uls_toknum_TMPL(uls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_tokNum(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return _uls_tok_id(uls);
}

JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_tokStr(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	const char *ret_str = _uls_lexeme(uls);
	return (*env)->NewStringUTF(env, ret_str);
}

JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_lexeme(JNIEnv *env, jobject obj, jobject juls)
{
	return Java_uls_polaris_UlsLex_tokStr(env, obj, juls);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getTok(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return uls_get_tok(uls);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isLexemeReal(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return uls_is_real(uls) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isLexemeInt(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return uls_is_int(uls) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isLexemeZero(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return uls_is_zero(uls) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_toInt(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return (jint) uls_lexeme_int32(uls);
}

JNIEXPORT jlong JNICALL Java_uls_polaris_UlsLex_toLong(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return (jlong) uls_lexeme_int64(uls);
}

JNIEXPORT jdouble JNICALL Java_uls_polaris_UlsLex_toReal(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return (jdouble) uls_lexeme_double(uls);
}

JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_numberSuffix(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	const char *ret_str = uls_number_suffix(uls);
	return (*env)->NewStringUTF(env, ret_str);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_expect(JNIEnv *env, jobject obj, jobject juls, jint tokExpected)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_expect(uls, tokExpected);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_ungetStr(JNIEnv *env, jobject obj, jobject juls, jstring jstr)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	const char *str = (*env)->GetStringUTFChars(env, jstr, 0);

	ulsjava_unget_str(uls, (void *) str, -1);
	(*env)->ReleaseStringUTFChars(env, jstr, str);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_ungetTok(JNIEnv *env, jobject obj, jobject juls, jstring jlxm, jint jtokid)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	const char *lxm = (*env)->GetStringUTFChars(env, jlxm, 0);

	ulsjava_unget_lexeme(uls, (void *) lxm, -1, jtokid);
	(*env)->ReleaseStringUTFChars(env, jlxm, lxm);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_ungetCh(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_unget_ch(uls, jch);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_dumpTok(JNIEnv *env, jobject obj, jobject juls, jstring jpfx, jstring jsuff)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	const char *pfx, *suff;

	pfx = (*env)->GetStringUTFChars(env, jpfx, 0);
	suff = (*env)->GetStringUTFChars(env, jsuff, 0);

	ulsjava_dump_tok(uls, pfx, -1, suff, -1);

	(*env)->ReleaseStringUTFChars(env, jpfx, pfx);
	(*env)->ReleaseStringUTFChars(env, jsuff, suff);
}

JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_tok2keyw(JNIEnv *env, jobject obj, jobject juls, jint jtokid)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	const char *keyw = uls_tok2keyw(uls, jtokid);
	return (*env)->NewStringUTF(env, keyw);
}

JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_tok2name(JNIEnv *env, jobject obj, jobject juls, jint jtokid)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	const char *name = uls_tok2name(uls, jtokid);
	return (*env)->NewStringUTF(env, name);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharSpace(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return _uls_is_ch_space(uls, jch) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharIdFirst(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return _uls_is_ch_idfirst(uls, jch) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharId(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return _uls_is_ch_id(uls, jch) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharQuote(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return _uls_is_ch_quote(uls, jch) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharTokCh1(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return _uls_is_ch_1ch_token(uls, jch) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isCharTokCh2(JNIEnv *env, jobject obj, jobject juls, jint jch)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return _uls_is_ch_2ch_token(uls, jch) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_skipWhiteSpaces(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_skip_white_spaces(uls);
}

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsLex_peekCh(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_nextch_detail_ptr_t ch_detail;
	jobject jch_detail;

	ch_detail = ulsjava_peek_nextch_info(uls);
	jch_detail = (*env)->NewDirectByteBuffer(env, (void *) ch_detail, sizeof(uls_nextch_detail_t));

	return jch_detail;
}

JNIEXPORT jobject JNICALL Java_uls_polaris_UlsLex_getCh(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_nextch_detail_ptr_t ch_detail;
	jobject jch_detail;

	ch_detail = ulsjava_get_nextch_info(uls);
	jch_detail = (*env)->NewDirectByteBuffer(env, (void *) ch_detail, sizeof(uls_nextch_detail_t));

	return jch_detail;
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_uchFromNextchInfo(JNIEnv *env, jobject obj, jobject jch_detail)
{
	uls_nextch_detail_ptr_t ch_detail = (uls_nextch_detail_ptr_t) (*env)->GetDirectBufferAddress(env, jch_detail);
	int wch;
	wch =  ulsjava_get_uch_from_nextch(ch_detail);
	return wch;
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_tokFromNextchInfo(JNIEnv *env, jobject obj, jobject jch_detail)
{
	uls_nextch_detail_ptr_t ch_detail = (uls_nextch_detail_ptr_t) (*env)->GetDirectBufferAddress(env, jch_detail);
	int tokid;
	tokid = ulsjava_get_tok_from_nextch(ch_detail);
	return tokid;
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_putNextchInfo(JNIEnv *env, jobject obj, jobject jch_detail)
{
	uls_nextch_detail_ptr_t ch_detail = (uls_nextch_detail_ptr_t) (*env)->GetDirectBufferAddress(env, jch_detail);
	ulsjava_put_nextch_info(ch_detail);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_isQuoteTok(JNIEnv *env, jobject obj, jobject juls, jint jtokid)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return uls_is_quote_tok(uls, jtokid) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL Java_uls_polaris_UlsLex_getTag(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	const char *tag = _uls_get_tag(uls);
	return (*env)->NewStringUTF(env, tag);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_setTag(JNIEnv *env, jobject obj, jobject juls, jstring jtag, jint jlineno)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	const char *tag;

	tag = (*env)->GetStringUTFChars(env, jtag, 0);
	ulsjava_set_tag(uls, tag, -1, jlineno);
	(*env)->ReleaseStringUTFChars(env, jtag, tag);
}

JNIEXPORT jint JNICALL Java_uls_polaris_UlsLex_getLineno(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	return _uls_get_lineno(uls);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_setLineno(JNIEnv *env, jobject obj, jobject juls, jint jlineno)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	_uls_set_lineno(uls, jlineno);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_incLineno(JNIEnv *env, jobject obj, jobject juls, jint jamount)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	_uls_inc_lineno(uls, jamount);
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_pushLine(JNIEnv *env, jobject obj, jobject juls, jstring jline, jint jflags)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	const char *line;
	int rc;

	line = (*env)->GetStringUTFChars(env, jline, 0);
	rc = ulsjava_push_line(uls, line, -1, (int) jflags);
	(*env)->ReleaseStringUTFChars(env, jline, line);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_pushFile(JNIEnv *env, jobject obj, jobject juls, jstring jfilepath, jint jflags)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
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
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_istream_ptr_t istr = (uls_istream_ptr_t) (*env)->GetDirectBufferAddress(env, jistr);
	uls_tmpl_list_ptr_t tmpls = (uls_tmpl_list_ptr_t) (*env)->GetDirectBufferAddress(env, jtmpls);
	int rc;

	rc = uls_push_istream(uls, istr, tmpls, (int) jflags);
	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_setLine(JNIEnv *env, jobject obj, jobject juls, jstring jline, jint jflags)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	const char *line;
	int rc;

	line = (*env)->GetStringUTFChars(env, jline, 0);
	rc = ulsjava_set_line(uls, line, -1, (int) jflags);
	(*env)->ReleaseStringUTFChars(env, jline, line);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_uls_polaris_UlsLex_setFile(JNIEnv *env, jobject obj, jobject juls, jstring jfilepath, jint jflags)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	const char *filepath;
	int rc;

	filepath = (*env)->GetStringUTFChars(env, jfilepath, 0);
	rc = ulsjava_set_file(uls, filepath, -1, (int) jflags);
	(*env)->ReleaseStringUTFChars(env, jfilepath, filepath);

	return rc < 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_popInput(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_pop(uls);
}

JNIEXPORT void JNICALL Java_uls_polaris_UlsLex_popAllInputs(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
	uls_pop_all(uls);
}

JNIEXPORT jintArray JNICALL Java_uls_polaris_UlsLex_getTokIdList(JNIEnv *env, jobject obj, jobject juls)
{
	uls_lex_ptr_t uls = (uls_lex_ptr_t) (*env)->GetDirectBufferAddress(env, juls);
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
