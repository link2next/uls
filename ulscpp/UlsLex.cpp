/*
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
  <file> UlsLex.cpp </file>
  <brief>
 	The wrapper class of ulslex_t, the main structure of ULS c-library, libuls.*
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2012.
  </author>
*/
#include "uls/UlsLex.h"
#include "uls/UlsIStream.h"
#include "uls/UlsUtils.h"

#include <string>
#include <iostream>
#include <stdexcept>

#include <stdlib.h>
#include <uls/uls_lex.h>
#include <uls/uld_conf.h>
#include <uls/uls_fileio.h>
#include <uls/uls_auw.h>

#include "uls/uls_util_wstr.h"
#include "uls/uls_lf_swprintf.h"
#include "uls/uls_wlog.h"

#include "uls.h"

using namespace std;
using namespace uls::crux;

void
#ifdef __GNUC__
__attribute__((constructor))
#endif
_initialize_ulscpp(void)
{
	UlsLex_initialize();
}

void
#ifdef __GNUC__
__attribute__((destructor))
#endif
_finalize_ulscpp(void)
{
	UlsLex_finalize();
}

#ifdef ULS_WINDOWS
BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
	BOOL rval = TRUE;

	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		UlsLex_initialize();
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		UlsLex_finalize();
		break;
	}

	return rval;
}
#endif

#if defined(HAVE_PTHREAD)
// <brief>
// This Initializes the mutex 'mtx' which is to be used by locked/unlocked method.
// The mechanism's Implemented by the pthread library in Linux and by Win32 API in Windows.
// </brief>
// <parm name="mtx">Mutex object</parm>
// <return>none</return>
void
uls::initMutex(uls::MutexType mtx)
{
	uls_mutex_struct_t  *a_mtx = (uls_mutex_struct_t *) mtx;
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);

	if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
		err_panic("error: fail to make the mutex recursive-one\n");

	a_mtx->mtx_pthr = (pthread_mutex_t *) uls_malloc(sizeof(pthread_mutex_t));
	if (pthread_mutex_init(a_mtx->mtx_pthr, &attr) != 0)
		err_panic("error: mutex init\n");

	pthread_mutexattr_destroy(&attr);
}

// <brief>
// Deinitialize the 'mtx'. After calling this, 'mtx' shouldn't be used.
// </brief>
// <parm name="mtx">The mutex object used in ULS</parm>
// <return>none</return>
void
uls::deinitMutex(uls::MutexType mtx)
{
	if (pthread_mutex_destroy(mtx->mtx_pthr) != 0)
		err_panic("error: mutex destroy\n");

	uls_mfree(mtx->mtx_pthr);
}

// <brief>
// The locking API to grab the lock 'mtx'.
// </brief>
// <parm name="mtx">The mutex object initialized by uls::initMutex</parm>
// <return>none</return>
void
uls::lockMutex(uls::MutexType mtx)
{
	if (pthread_mutex_lock(mtx->mtx_pthr) != 0)
		err_panic("error to pthread_mutex_lock");
}

// <brief>
// The unlocking API to ungrab the lock 'mtx'.
// </brief>
// <parm name="mtx">The mutex object initialized by uls::initMutex</parm>
// <return>none</return>
void
uls::unlockMutex(uls::MutexType mtx)
{
	if (pthread_mutex_unlock(mtx->mtx_pthr) != 0)
		err_panic("error to pthread_mutex_unlock");
}

#elif defined(ULS_WINDOWS)
// <brief>
// Initializes the mutex 'mtx' to be used from then on by locked/unlocked method.
// Implemented lock/unlock with the mutex of Win32Api in Windows
// </brief>
// <parm name="mtx">Mutex object</parm>
// <return>none</return>
void
uls::initMutex(uls::MutexType mtx)
{
	mtx->hndl = CreateMutex(NULL, FALSE, NULL);

	if (mtx->hndl == INVALID_HANDLE_VALUE)
		err_panic("error: mutex init\n");
}

// <brief>
// De-initialize the 'mtx'.
// Hereafter you must not use 'mtx'.
// </brief>
// <parm name="mtx">The mutex object used in ULS</parm>
// <return>none</return>
void
uls::deinitMutex(uls::MutexType mtx)
{
	CloseHandle(mtx->hndl);
	mtx->hndl = INVALID_HANDLE_VALUE;
}

// <brief>
// A Lock API in ULS.
// </brief>
// <parm name="mtx">The mutex object initialized by uls::initMutex</parm>
// <return>none</return>
void
uls::lockMutex(uls::MutexType mtx)
{
	DWORD dwWaitResult;

	dwWaitResult = WaitForSingleObject(mtx->hndl, INFINITE);
	if (dwWaitResult != WAIT_OBJECT_0) {
		err_panic("error: mutex lock\n");
	}
}

// <brief>
// An Unlock API in ULS.
// </brief>
// <parm name="mtx">The mutex object initialized by uls::initMutex</parm>
// <return>none</return>
void
uls::unlockMutex(uls::MutexType mtx)
{
	ReleaseMutex(mtx->hndl);
}
#else

void
uls::initMutex(uls::MutexType mtx)
{
}

void
uls::deinitMutex(uls::MutexType mtx)
{
}

void
uls::lockMutex(uls::MutexType mtx)
{
}

void
uls::unlockMutex(uls::MutexType mtx)
{
}
#endif

int
uls::create_fd_wronly(string& fpath)
{
	int fd;
	fd = uls_fd_open(fpath.c_str(), ULS_FIO_CREAT | ULS_FIO_WRITE);
	return fd;
}

int
uls::open_fd_rdonly(string& fpath)
{
	int fd;
	fd = uls_fd_open(fpath.c_str(), ULS_FIO_READ);;
	return fd;
}

void
uls::close_fd(int fd)
{
	uls_fd_close(fd);
}

void
uls::crux::UlsLex_initialize(void)
{
	initialize_uls();

	if (ulscpp_convspec_nmap != NULL) return;
	ulscpp_convspec_nmap = uls_lf_create_convspec_map(0);

	if (ulscpp_convspec_nmap == NULL) {
		err_panic("fail to initialize uls");
	}

	uls_add_default_log_convspecs(ulscpp_convspec_nmap);

	ulscpp_convspec_wmap = uls_lf_create_convspec_wmap(0);
	uls_add_default_log_convspecs(ulscpp_convspec_wmap);
}

void
uls::crux::UlsLex_finalize(void)
{
	if (ulscpp_convspec_nmap != NULL) {
		uls_lf_destroy_convspec_map(ulscpp_convspec_nmap);
		ulscpp_convspec_nmap = NULL;
	}

	if (ulscpp_convspec_wmap != NULL) {
		uls_lf_destroy_convspec_wmap(ulscpp_convspec_wmap);
	}

	finalize_uls();
}

// <brief>
// This procedure lists the search directories for ulc file, which is suffixed by 'ulc'.
// uls_create() or uls_init() will search the directories for ulc file in same order as this dumpSearchPathOfUlc().
// The order of search-directories is affected by whether or not the paramenter 'confname' is suffixed by '.ulc'.
// If the parameter is suffixed by '.ulc' or has '..' or '.', it's recognized as file path.
// But if not suffixed by 'ulc' like 'sample', 'dir/sample',
//     the procedure will search the ulc repository preferentially.
// </brief>
// <parm name="confname">The name of lexcial configuration, as like 'sample', 'dir/sample', or 'sample.ulc'</parm>
// <return>none</return>
void uls::dumpSearchPathOfUlc(string& confname)
{
	ulc_list_searchpath(confname.c_str());
}

void uls::dumpSearchPathOfUlc(wstring& wconfwname)
{
	const char *ustr;
	csz_str_t csz;

	csz_init(&csz, -1);

	if ((ustr = uls_wstr2ustr(wconfwname.c_str(), -1, &csz)) == NULL) {
		err_log("encoding error!");
	}
	else {
		ulc_list_searchpath(ustr);
	}

	csz_deinit(&csz);
}

// <brief>
// This will list the serach paths, preferentially the location of ulc repository.
// </brief>
// <return>none</return>
void
uls::listUlcSearchPaths(void)
{
	ulc_list_searchpath("simple");
}

void
uls::initialize_ulscpp(void)
{
	_initialize_ulscpp();
}

void
uls::finalize_ulscpp(void)
{
	_finalize_ulscpp();
}

// <brief>
// For internal use only:
// isLexemeReal, isLexemeInteger, isLexemeZero, lexemeAsInt
// Instead use the class UlsLex APIs
// </brief>

bool
uls::crux::isLexemeZero(string& lxm)
{
	const char *ptr = lxm.c_str();

	if (ptr[0] == '0' || (ptr[0] == '.' && ptr[1] == '0'))
		return true;

	return false;
}

bool
uls::crux::isLexemeZero(wstring& lxm)
{
	const wchar_t *ptr = lxm.c_str();

	if (ptr[0] == L'0' || (ptr[0] == L'.' && ptr[1] == L'0'))
		return true;

	return false;
}

bool
uls::crux::isLexemeInt(string& lxm)
{
	return uls::crux::isLexemeReal(lxm) ? false : true;
}

bool
uls::crux::isLexemeInt(wstring& lxm)
{
	return uls::crux::isLexemeReal(lxm) ? false : true;
}

bool
uls::crux::isLexemeReal(string& lxm)
{
	const char *ptr = lxm.c_str();

	if (*ptr == '.') return true;

	return false;

}

bool
uls::crux::isLexemeReal(wstring& lxm)
{
	const wchar_t *ptr = lxm.c_str();

	if (*ptr == L'.') return true;

	return false;

}

// <brief>
// In case 'lxm' is a floating point number, this function returns the value of it.
// The 'lxm' may be the one from the current lexeme of uls-object.
// </brief>
// <parm name="lxm">string</parm>
// <return>true/false</return>
int
uls::crux::LexemeAsInt(string& lxm)
{
	const char *ptr = lxm.c_str();
	return strtoul(ptr, NULL, 16);
}

int
uls::crux::LexemeAsInt(wstring& lxm)
{
	const wchar_t *ptr = lxm.c_str();
	return wcstoul(ptr, NULL, 16);
}

double
uls::crux::LexemeAsDouble(string& lxm)
{
	const char *ptr = lxm.c_str();
	return atof(ptr);
}

double
uls::crux::LexemeAsDouble(wstring& wlxm)
{
	const wchar_t *ptr = wlxm.c_str();
	double fval;

#ifdef ULS_WINDOWS
	fval = _wtof(ptr);
#else
	fval = wcstod(ptr, NULL);
#endif

	return fval;
}

//
//
// UlsLex
//
//

// <brief>
// In case the paramenter 'lxm' is a floating point number,
//     this function returns the value of it.
// The 'lxm' may be the one from the current lexeme of uls-object.
// </brief>
// <parm name="lxm">string</parm>
// <return>true/false</return>
int UlsLex::get_uls_flags(UlsLex::InputOpts fl)
{
	int ret_fl;

	switch (fl) {
	case WantEOF:
		ret_fl = ULS_WANT_EOFTOK;
		break;
	case DoDup:
		ret_fl = ULS_DO_DUP;
		break;
	case MsMbcsEncoding:
		ret_fl = ULS_FILE_MS_MBCS;
		break;
	case Utf8Encoding:
		ret_fl = ULS_FILE_UTF8;
		break;
	default:
		ret_fl = -1;
		break;
	}

	return ret_fl;
}

void UlsLex::setInputOpt(UlsLex::InputOpts fl)
{
	int uls_fl = get_uls_flags(fl);
	input_flags |= uls_fl;
}


void UlsLex::clearInputOpt(UlsLex::InputOpts fl)
{
	int uls_fl = get_uls_flags(fl);
	input_flags &=  ~uls_fl;
}

int UlsLex::getInputOpts(void)
{
	return input_flags;
}

void UlsLex::resetInputOpts(void)
{
	input_flags = 0;
}

// <brief>
// The constructor that creates an object for lexical analyzing.
// </brief>
// <parm name="ulc_file">The name or path of its lexical configuration.</parm>
// <return>none</return>
bool
UlsLex::initUlsLex_ustr(const char *ulc_file)
{
	FILE  *cstdio_out = _uls_stdio_fp(1);
	uls_lf_puts_t puts_proc_str, puts_proc_file;

	if (uls_init(&lex, ulc_file) < 0) {
		throw std::invalid_argument("Failed to create the uls object!");
		return false;
	}

	input_flags = 0;
	lxm_nstr = new string("");
	lxm_wstr = new wstring(L"");
	lxm_id = uls_toknum_none(&lex);

	FileNameBuf = new string("");

	auwcvt = new UlsAuw();

	toknum_EOI = _uls_toknum_EOI(&lex);
	toknum_EOF = _uls_toknum_EOF(&lex);
	toknum_ERR = _uls_toknum_ERR(&lex);
	toknum_NONE = _uls_toknum_NONE(&lex);
	toknum_ID = _uls_toknum_ID(&lex);
	toknum_NUMBER = _uls_toknum_NUMBER(&lex);
	toknum_TMPL = _uls_toknum_TMPL(&lex);

	puts_proc_str = uls_lf_puts_str;
	puts_proc_file = uls_lf_puts_file;
#define uls_nlf_create uls_lf_create

	str_nlf = uls_nlf_create(ulscpp_convspec_nmap, NULL, puts_proc_str);
	uls_lf_change_gdat(str_nlf, &lex);

	file_nlf = uls_nlf_create(ulscpp_convspec_nmap, cstdio_out, puts_proc_file);
	uls_lf_change_gdat(file_nlf, &lex);

	prn_nlf = uls_nlf_create(ulscpp_convspec_nmap, cstdio_out, puts_proc_file);
	uls_lf_change_gdat(prn_nlf, &lex);

	str_wlf = uls_wlf_create(ulscpp_convspec_wmap, NULL, uls_lf_wputs_str);
	uls_lf_change_gdat(str_wlf, &lex);

	file_wlf = uls_wlf_create(ulscpp_convspec_wmap, cstdio_out, uls_lf_wputs_file);
	uls_lf_change_gdat(file_wlf, &lex);

	prn_wlf = uls_wlf_create(ulscpp_convspec_wmap, cstdio_out, uls_lf_wputs_file);
	uls_lf_change_gdat(prn_wlf, &lex);

	uls::initMutex(&sysprn_g_mtx);
	sysprn_opened = 0;

	uls::initMutex(&syserr_g_mtx);
	if (uls_init_log(&logbase, NULL, &lex) < 0) {
		err_panic("fail to initialize uls");
	}

	uls_log_change(&logbase, (void *) cstdio_out, puts_proc_file);

	extra_tokdefs = new std::map<int,void*>();

	return true;
}

UlsLex::UlsLex(const char *ulc_file)
{
	initUlsLex_ustr(ulc_file);
}

UlsLex::UlsLex(const wchar_t *ulc_wfile)
{
	const char *ustr;
	csz_str_t csz;

	csz_init(&csz, -1);

	if ((ustr = uls_wstr2ustr(ulc_wfile, -1, &csz)) == NULL) {
		err_log("encoding error!");
	}
	else {
		initUlsLex_ustr(ustr);
	}

	csz_deinit(&csz);
}

UlsLex::UlsLex(string& ulc_file)
{
	initUlsLex_ustr(ulc_file.c_str());
}

UlsLex::UlsLex(wstring& ulc_wfile)
{
	const char *ustr;
	csz_str_t csz;

	csz_init(&csz, -1);

	if ((ustr = uls_wstr2ustr(ulc_wfile.c_str(), -1, &csz)) == NULL) {
		err_log("encoding error!");
	}
	else {
		initUlsLex_ustr(ustr);
	}

	csz_deinit(&csz);
}

// <brief>
// The destructor of UlsLex.
// </brief>
// <return>none</return>
UlsLex::~UlsLex()
{
	delete lxm_nstr;
	delete lxm_wstr;
	delete FileNameBuf;
	delete extra_tokdefs;
	delete auwcvt;

	uls::deinitMutex(&syserr_g_mtx);
	uls::deinitMutex(&sysprn_g_mtx);

	uls_deinit_log(&logbase);
	uls_destroy(&lex);

#define uls_nlf_destroy uls_lf_destroy
	uls_wlf_destroy(prn_wlf);
	uls_wlf_destroy(file_wlf);
	uls_wlf_destroy(str_wlf);

	uls_nlf_destroy(prn_nlf);
	uls_nlf_destroy(file_nlf);
	uls_nlf_destroy(str_nlf);
}

int UlsLex::prepareUldMap()
{
	names_map = uld_prepare_names(&lex);
	return 0;
}

bool UlsLex::finishUldMap()
{
	bool stat = true;

	if (uld_post_names(names_map) < 0) {
		stat = false;
	}
	names_map = NULL;

	return stat;
}

void UlsLex::changeUldNames(const char *name, const char*name2, int tokid_valid, int tokid, const char *aliases)
{
	uld_line_t tok_names;

 	tok_names.name = name;
	tok_names.name2 = name2;
	tok_names.tokid_changed = tokid_valid;
	tok_names.tokid = tokid;
	tok_names.aliases = aliases;

	uld_change_names(names_map, &tok_names);
}

// <brief>
// This setter is to set the private data 'FileName' with the parameter 'fname'
// </brief>
// <parm name="fname">The new string value of 'FileName'</parm>
// <return>none</return>
void UlsLex::setTag_ustr(const char *fname)
{
	const char *nstr;

	uls_set_tag(&lex, fname, 1);
	_ULSCPP_USTR2NSTR(uls_get_tag(&lex), nstr, 1);
	*FileNameBuf = string(nstr);
}

void UlsLex::setTag(string& fname)
{
	const char *ustr;
	_ULSCPP_NSTR2USTR(fname.c_str(), ustr, 0);
	setTag_ustr(ustr);
}

void UlsLex::setTag(wstring& wfname)
{
	const char *ustr;
	_ULSCPP_WSTR2USTR(wfname.c_str(), ustr, 0);
	setTag_ustr(ustr);
}

void UlsLex::setFileName(string& fname)
{
	setTag(fname);
}

void UlsLex::setFileName(wstring& fname)
{
	setTag(fname);
}

void UlsLex::getTag(string& fname)
{
	fname = *FileNameBuf;
}

void UlsLex::getTag(std::wstring& wfname)
{
	const wchar_t *wstr;
	_ULSCPP_NSTR2WSTR(FileNameBuf->c_str(), wstr, 0);
	wfname = wstr;
}

void UlsLex::getFileName(string& fname)
{
	getTag(fname);
}

void UlsLex::getFileName(std::wstring& fname)
{
	getTag(fname);
}

// <brief>
// Sets the internal data 'LineNum' to 'lineno' forcibly.
// </brief>
// <parm name="lineno">The new value of 'LineNum'</parm>
// <return>none</return>
void UlsLex::setLineNum(int lineno)
{
	uls_set_tag(&lex, NULL, lineno);
	LineNum = uls_get_lineno(&lex);
}

// <brief>
// Adds the current 'LineNum' by 'amount'.
// </brief>
// <parm name="amount">The amount of lines to be added</parm>
// <return>none</return>
void UlsLex::addLineNum(int amount)
{
	_uls_inc_lineno(&lex, amount);
	LineNum = uls_get_lineno(&lex);
}

// <brief>
// Sets the log level of the object, UlsLex.
// The possible 'loglvl' are ULS_LOG_EMERG, ULS_LOG_ALERT, ULS_LOG_CRIT, ...
// </brief>
// <parm name="lvl">new value of log level</parm>
// <return>none</return>
void UlsLex::setLogLevel(int lvl)
{
	uls_set_loglevel(&logbase, lvl);
}

// <brief>
// Clears the log level defined in the uls object.
// </brief>
// <parm name="lvl">the log level to be cleared.</parm>
// <return>void</return>
void UlsLex::clearLogLevel(int lvl)
{
	uls_clear_loglevel(&logbase, lvl);
}

// <brief>
// Checks if the 'lvl' is set in the internally.
// 'lvl' is a flag such as  ULS_LOG_EMERG, ULS_LOG_ALERT, ULS_LOG_CRIT, ...
// </brief>
// <parm name="lvl">the log level to be checked.</parm>
// <return>true/false</return>
bool UlsLex::isLogLevelSet(int lvl)
{
	return uls_loglevel_isset(&logbase, lvl) ? true : false;
}

// <brief>
// Delete the literal-string analyzer that
//     is processing the quote-strings starting with 'pfx'.
// </brief>
// <parm name="pfx">The literal string analyzer of which the quote type is started with 'pfx'.</parm>
// <return>none</return>
void UlsLex::deleteLiteralAnalyzer(string& pfx)
{
	const char *ustr;
	_ULSCPP_NSTR2USTR(pfx.c_str(), ustr, 0);
	uls_xcontext_delete_litstr_analyzer(_uls_get_xcontext(&lex), ustr);
}

void UlsLex::deleteLiteralAnalyzer(wstring& wpfx)
{
	const char *ustr;
	_ULSCPP_WSTR2USTR(wpfx.c_str(), ustr, 0);
	uls_xcontext_delete_litstr_analyzer(_uls_get_xcontext(&lex), ustr);
}

// <brief>
// Changes the literal-string analyzer to 'proc'.
// The 'proc' will be applied to the quote strings starting with 'pfx'.
// </brief>
// <parm name="pfx">The prefix of literal string that will be processed by 'proc'</parm>
// <return>void</return>
void UlsLex::changeLiteralAnalyzer(string pfx, uls_litstr_analyzer_t proc, void* data)
{
	const char *ustr;
	_ULSCPP_NSTR2USTR(pfx.c_str(), ustr, 0);
	uls_xcontext_change_litstr_analyzer(_uls_get_xcontext(&lex), ustr, proc, data);
}

void UlsLex::changeLiteralAnalyzer(wstring wpfx, uls_litstr_analyzer_t proc, void* data)
{
	const char *ustr;
	_ULSCPP_WSTR2USTR(wpfx.c_str(), ustr, 0);
	uls_xcontext_change_litstr_analyzer(_uls_get_xcontext(&lex), ustr, proc, data);
}

// <brief>
// This method will push an input string 'istr' on the top of the internal input stack.
// Then the getTok() method can be used to get a token from the input.
// </brief>
// <parm name="hdr">the header information</parm>
// <parm name="flags">
//   Set ULS_WANT_EOFTOK if you want EOF-token whenever each file reaches at the end of file
//   Set ULS_NO_EOFTOK if you don't need EOF-token at the end of each input file
//  </parm>
// <return>none</return>
bool UlsLex::pushInput(UlsIStream& in_str, int flags)
{
	uls_tmpl_list_t tmpl_list_exp;
	bool rval;

	if (flags < 0) {
		flags = getInputOpts();
	}

	uls_init_tmpls(&tmpl_list_exp, N_TMPLVAL_SIZE, ULS_TMPLS_DUP);
	in_str.exportTmpls(&tmpl_list_exp);

	if (uls_push_istream(&lex, in_str.getCore(), &tmpl_list_exp, flags) < 0) {
		cerr << "fail to prepare input-stream" << endl;
		rval = false;
	} else {
		rval = true;
	}

	uls_deinit_tmpls(&tmpl_list_exp);
	return rval;
}

// <brief>
// This method pushes (raw) source-file on the internal stack.
// The raw source-file is just a regular text file.
// </brief>
// <parm name="fd">the file descriptor of input file.</parm>
// <parm name="flags">
//   Set ULS_WANT_EOFTOK if you want EOF-token whenever each file reaches at the end of file
//   Set ULS_NO_EOFTOK if you don't need EOF-token at the end of each input file
// </parm>
// <return>none</return>
bool UlsLex::pushInput(int fd, int flags)
{
	if (flags < 0) {
		flags = getInputOpts();
	}

	if (uls_push_fd(&lex, fd, flags) < 0) {
		return false;
	}

	uls_set_tag(&lex, NULL, 1);
	return true;
}

// <brief>
// Dismiss the current input stream.
// </brief>
// <return>
// </return>
void UlsLex::popInput(void)
{
	uls_pop(&lex);
}

// <brief>
// Dismiss all the input streams and its state goes back to initial.
// </brief>
// <return>none</return>
void UlsLex::popAllInputs(void)
{
	uls_pop_all(&lex);
}

// <brief>
// This method pushes (raw) source-file on the internal stack.
// The raw source-file is just a regular text file.
// </brief>
// <parm name="fd">the file descriptor of input file.</parm>
// <parm name="flags">
//   Set ULS_WANT_EOFTOK if you want receive the reserved token EOF
//       whenever each file reaches at the end of file
//   Set ULS_NO_EOFTOK if you don't need to receive the EOF-token
//       at the end of each input file
// </parm>
// <return>none</return>
void UlsLex::pushFd(int fd, int flags)
{
	if (flags < 0) {
		flags = getInputOpts();
	}

	if (uls_push_fd(&lex, fd, flags) < 0) {
		throw std::invalid_argument("the param 'fd' is invalid.");
	}

	uls_set_tag(&lex, NULL, 1);
}

void UlsLex::setFd(int fd, int flags)
{
	if (flags < 0) {
		flags = getInputOpts();
	}

	if (uls_set_fd(&lex, fd, flags) < 0) {
		throw std::invalid_argument("the param 'fd' is invalid.");
	}

	uls_set_tag(&lex, NULL, 1);
}

// <brief>
// This method will push the input-file on the top of the input stack.
// </brief>
// <parm name="filepath">The file path of input.</parm>
// <parm name="flags">
//   Set ULS_WANT_EOFTOK if you want EOF-token whenever each file reaches at the end of file
//   Set ULS_NO_EOFTOK if you don't need EOF-token at the end of each input file
// </parm>
// <return>none</return>

bool UlsLex::pushFile(string& filepath, int flags)
{
	const char *ustr;

	if (flags < 0) {
		flags = getInputOpts();
	}

	_ULSCPP_NSTR2USTR(filepath.c_str(), ustr, 0);

	if (uls_push_file(&lex, ustr, flags) < 0) {
		return false;
	}

	return true;
}

bool UlsLex::pushFile(std::wstring& wfilepath, int flags)
{
	const char *ustr;

	if (flags < 0) {
		flags = getInputOpts();
	}

	_ULSCPP_WSTR2USTR(wfilepath.c_str(), ustr, 0);

	if (uls_push_file(&lex, ustr, flags) < 0) {
		return false;
	}

	return true;
}

void UlsLex::setFile(string& filepath, int flags)
{
	const char *ustr;

	if (flags < 0) {
		flags = getInputOpts();
	}

	_ULSCPP_NSTR2USTR(filepath.c_str(), ustr, 0);

	if (uls_set_file(&lex, ustr, flags) < 0) {
		throw std::invalid_argument("the param 'filepath' is invalid.");
	}
}

void UlsLex::setFile(wstring& wfilepath, int flags)
{
	const char *ustr;

	if (flags < 0) {
		flags = getInputOpts();
	}

	_ULSCPP_WSTR2USTR(wfilepath.c_str(), ustr, 0);

	if (uls_set_file(&lex, ustr, flags) < 0) {
		throw std::invalid_argument("the param 'filepath' is invalid.");
	}
}

// <brief>
// This method will push the string as an input source on the top of the input stack.
// </brief>
// <parm name="line">A input stream as an literal string</parm>
// <parm name="flags">
//   Set ULS_WANT_EOFTOK if you want EOF-token whenever each file reaches at the end of file
//   Set ULS_NO_EOFTOK if you don't need EOF-token at the end of each input file
// </parm>
// <return>none</return>
void UlsLex::pushLine(const char* line, int len, int flags)
{
	const char *ustr;

	if (flags < 0) {
		flags = getInputOpts();
	}

	len = _ULSCPP_NSTR2USTR(line, ustr, 0);
#ifdef ULS_WINDOWS
	flags |= ULS_DO_DUP;
#endif
	if (uls_push_line(&lex, line, len, flags) < 0) {
		throw std::invalid_argument("the param 'line' is invalid.");
	}
}

void UlsLex::pushLine(const wchar_t* wline, int len, int flags)
{
	const char *ustr;

	if (flags < 0) {
		flags = getInputOpts();
	}

	len = _ULSCPP_WSTR2USTR(wline, ustr, 0);
#ifdef ULS_WINDOWS
	flags |= ULS_DO_DUP;
#endif
	if (uls_push_line(&lex, ustr, len, flags) < 0) {
		throw std::invalid_argument("the param 'line' is invalid.");
	}
}

void UlsLex::setLine(const char* line, int len, int flags)
{
	const char *ustr;

	if (flags < 0) {
		flags = getInputOpts();
	}

	len = _ULSCPP_NSTR2USTR(line, ustr, 0);
#ifdef ULS_WINDOWS
	flags |= ULS_DO_DUP;
#endif

	if (uls_set_line(&lex, ustr, len, flags) < 0) {
		throw std::invalid_argument("the param 'line' is invalid.");
	}
}

void UlsLex::setLine(const wchar_t* wline, int len, int flags)
{
	const char *ustr;

	if (flags < 0) {
		flags = getInputOpts();
	}

	len = _ULSCPP_WSTR2USTR(wline, ustr, 0);
#ifdef ULS_WINDOWS
	flags |= ULS_DO_DUP;
#endif
	if (uls_set_line(&lex, ustr, len, flags) < 0) {
		throw std::invalid_argument("the param 'line' is invalid.");
	}
}

void UlsLex::popCurrent(void)
{
	popInput();
}

void UlsLex::dismissAllInputs(void)
{
	popAllInputs();
}


// <brief>
// Identifies the char-group of 'ch'.
// Checks if 'ch' can be a blank character.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLex::is_ch_space(uls_uch_t uch)
{
	return uls_is_ch_space(&lex, uch) ? true : false;
}

// <brief>
// Identifies the char-group of 'ch'.
// Checks if 'ch' can be a first char of identifier.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLex::is_ch_idfirst(uls_uch_t uch)
{
	return uls_is_ch_idfirst(&lex, uch) ? true : false;
}

// <brief>
// Identifies the char-group of 'ch'.
// Checks if 'ch' can be a char of identifier.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLex::is_ch_id(uls_uch_t uch)
{
	return uls_is_ch_id(&lex, uch) ? true : false;
}

// <brief>
// Identifies the char-group of 'ch'.
// Checks if 'ch' can be a first char of literal string.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLex::is_ch_quote(uls_uch_t uch)
{
	return uls_is_ch_quote(&lex, uch) ? true : false;
}

// <brief>
// Identifies the char-group of 'ch'.
// Checks if 'ch' can be char of one-char token.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLex::is_ch_1ch_token(uls_uch_t uch)
{
	return uls_is_ch_1ch_token(&lex, uch) ? true : false;
}

// <brief>
// Identifies the char-group of 'ch'.
// Checks if 'ch' can be a first character of the token
//    which is the keyword of more than one non-alphanumeric char.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</re turn>
bool UlsLex::is_ch_2ch_token(uls_uch_t uch)
{
	return uls_is_ch_2ch_token(&lex, uch) ? true : false;
}

// <brief>
// Identifies the char-group of 'ch'.
// Checks if 'ch' can be a first character of comment.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLex::is_ch_comm(uls_uch_t uch)
{
	return uls_is_ch_comm(&lex, uch) ? true : false;
}

// <brief>
// Skips the blank chars.
// </brief>
// <return>none</return>
void UlsLex::skipBlanks(void)
{
	uls_skip_blanks(&lex);
}

// <brief>
// Peeks the next character in the internal buffer.
// </brief>
// <parm name="isQuote">checks if the next char is a first char of some quote-string.</parm>
// <return>The next character</return>
uls_uch_t UlsLex::peekCh(bool* isQuote)
{
	uls_uch_t uch;
	uls_nextch_detail_t detail;
	bool is_quote = false;

	if ((uch = uls_peek_uch(&lex, &detail)) == ULS_UCH_NONE && detail.qmt != NULL) {
		is_quote = true;
	}

	if (isQuote) {
		*isQuote = is_quote;
	}

	return uch;
}

// <brief>
// Peeks the next character in the internal buffer.
// </brief>
// <return>The next character</return>
uls_uch_t UlsLex::peekCh(void)
{
	return peekCh(NULL);
}

// <brief>
// Extracts the next character.
// </brief>
// <parm name="isQuote">
//  checks if the extracted char is a first char of some quote-string.
// </parm>
// <return>The next character</return>
uls_uch_t UlsLex::getCh(bool* isQuote)
{
	uls_uch_t uch;
	uls_nextch_detail_t detail;
	bool is_quote = false;

	if ((uch = uls_get_uch(&lex, &detail)) == ULS_UCH_NONE && detail.qmt != NULL) {
		is_quote = true;
	}

	if (isQuote) {
		*isQuote = is_quote;
	}

	return uch;
}

// <brief>
// Extracts the next character.
// </brief>
// <return>The next character</return>
uls_uch_t UlsLex::getCh(void)
{
	return getCh(NULL);
}

// <brief>
// Sets the current token to <t,lxm> forcibly.
// </brief>
// <return>The token number</return>

void UlsLex::set_token(int t, string& lxm)
{
	const wchar_t *wstr;

	_ULSCPP_NSTR2WSTR(lxm.c_str(), wstr, 0);

	lxm_id = t;
	*lxm_nstr = lxm;
	*lxm_wstr = wstr;
}

void UlsLex::set_token(int t, std::wstring& wlxm)
{
	const char *nstr;

	_ULSCPP_WSTR2NSTR(wlxm.c_str(), nstr, 0);

	lxm_id = t;
	*lxm_nstr = nstr;
	*lxm_wstr = wlxm;
}

void UlsLex::setTok(int t, string& lxm)
{
	set_token(t, lxm);
}

void UlsLex::setTok(int t, std::wstring& lxm)
{
	set_token(t, lxm);
}

void UlsLex::update_token_lex(void)
{
	const char *nstr;
	wchar_t *wstr;

	lxm_id = uls_tok(&lex);

	_ULSCPP_USTR2NSTR(uls_lexeme(&lex), nstr, 0);
	*lxm_nstr = nstr;

	_ULSCPP_NSTR2WSTR(nstr, wstr, 1);
	*lxm_wstr = wstr;
}

// <brief>
// push the 'ch' to the buffer so that next char with a call getCh() may be 'ch'.
// </brief>
// <return>none</return>
void UlsLex::ungetCh(uls_uch_t uch)
{
	uls_unget_ch(&lex, uch);
	update_token_lex();
}

// <brief>
// Advances the (internal) cursor of the lexical object to the next step.
// Returns the next token number of UlsLex object.
// </brief>
// <return>The token number</return>
int UlsLex::getTok(void)
{
	int  t = uls_get_tok(&lex);

	isEOI = (uls_tok(&lex) == _uls_toknum_EOI(&lex));
	isEOF = (uls_tok(&lex) == _uls_toknum_EOF(&lex));

	update_token_lex();
	LineNum = uls_get_lineno(&lex);

	return t;
}

void
UlsLex::getTokStr(string** pp_lxm)
{
	*pp_lxm = lxm_nstr;
}

void
UlsLex::getTokStr(std::wstring** pp_wlxm)
{
	*pp_wlxm = lxm_wstr;
}

int
UlsLex::getTokNum(void)
{
	return lxm_id;
}

// <brief>
// This function checks if 'lxm' is a floating point number.
// Make sure the current token is a number before calling these methods.
// </brief>
// <parm name="lxm">A string</parm>
// <return>true/false</return>

bool
UlsLex::isLexemeReal(void)
{
	return uls_is_real(&lex) ? true : false;
}

bool
UlsLex::isLexemeInt(void)
{
	return uls_is_int(&lex) ? true : false;
}

bool
UlsLex::isLexemeZero(void)
{
	return uls_is_zero(&lex) ? true : false;
}

int
UlsLex::lexemeAsInt(void)
{
	return uls_lexeme_d(&lex);
}

unsigned int
UlsLex::lexemeAsUInt(void)
{
	return uls_lexeme_u(&lex);
}

long long
UlsLex::lexemeAsLongLong(void)
{
	return uls_lexeme_lld(&lex);
}

unsigned long long
UlsLex::lexemeAsULongLong(void)
{
	return uls_lexeme_llu(&lex);
}

double
UlsLex::lexemeAsDouble(void)
{
	return uls_lexeme_double(&lex);
}

string
UlsLex::lexemeNumberSuffix(void)
{
	return uls_number_suffix(&lex);
}

// <brief>
// If the current token-id is not 'TokExpected', An Exception will be thrown.
// </brief>
// <parm name="TokExpected">The expected token number</parm>
// <return>none</return>
void UlsLex::expect(int TokExpected)
{
	uls_expect(&lex, TokExpected);
}

bool
UlsLex::existTokdefInHashMap(int t)
{
	std::map<int,void*>::iterator it;
	bool rval;

	it = extra_tokdefs->find(t);

	if (it != extra_tokdefs->end()) {
		rval = true;
	} else {
		rval = false;
	}

	return rval;
}

// <brief>
// Get the extra token definition corresponding to 'tok_id'.
// </brief>
// <parm name="tok_id">The token id for which you want its (extra) token definition.</parrm>
// <return>The extra tokdef of (void *)</return>
void*&
UlsLex::operator[](int t)
{
	if (existTokdefInHashMap(t) == false) {
		(*extra_tokdefs)[t] = NULL;
	}

	return (*extra_tokdefs)[t];
}

// <brief>
// This sets extra token definition 'extra_tokdef' which is provided by user.
// The stored data of token number 't' can be later retrieved by getExtraTokdef().
// </brief>
// <parm name="tok_id">The target token id of which extra tokdef is set.</parm>
// <parm name="extra_tokdef">The extra tokdef is provided by user</parm>
// <return>0 if success, -1 otherwise</return>
void
UlsLex::setExtraTokdef(int t, void* extra_tokdef)
{
	(*extra_tokdefs)[t] = extra_tokdef;
}

void*
UlsLex::getExtraTokdef(void)
{
	return getExtraTokdef(lxm_id);
}

void*
UlsLex::getExtraTokdef(int t)
{
	if (existTokdefInHashMap(t) == false) {
		return NULL;
	}
	return (*extra_tokdefs)[t];
}

// <brief>
// Call ungetTok if you want get the current token again at the next call of getTok().
// </brief>
// <return>none</return>
void UlsLex::ungetTok(void)
{
	uls_unget_tok(&lex);
	update_token_lex();
}

// <brief>
// Call ungetStr if you want push string to the current input stream.
// </brief>
// <return>none</return>
void UlsLex::ungetStr(string str)
{
	const char *ustr;

	_ULSCPP_NSTR2USTR(str.c_str(), ustr, 0);
	uls_unget_str(&lex, ustr);

	update_token_lex();
}

void UlsLex::ungetStr(wstring str)
{
	const char *ustr;

	_ULSCPP_WSTR2USTR(str.c_str(), ustr, 0);
	uls_unget_str(&lex, ustr);

	update_token_lex();
}

// <brief>
// Call ungetLexeme if you want push lexeme to the current input stream.
// </brief>
// <parm name="tok_id">
//   The corresponding token string of 'tok_id'.
// </parm>
// <parm name="tok_id">
//   The parameter 'tok_id' should be normally TOK_NONE.
//   If you want 'str' to be a quote-string, specify the corresponding tok-id of the quote-string.
// </parm>
// <return>none</return>
void UlsLex::ungetLexeme(string lxm, int tok_id)
{
	const char *ustr;

	_ULSCPP_NSTR2USTR(lxm.c_str(), ustr, 0);
	uls_unget_lexeme(&lex, ustr, tok_id);

	update_token_lex();
}

void UlsLex::ungetLexeme(wstring wlxm, int tok_id)
{
	const char *ustr;

	_ULSCPP_WSTR2USTR(wlxm.c_str(), ustr, 0);
	uls_unget_lexeme(&lex, ustr, tok_id);

	update_token_lex();
}

// <brief>
// Prints the information on the current token.
// </brief>
// <return>none</return>
void UlsLex::dumpTok(string pfx, string suff)
{
	const char *ustr0, *ustr1;

	_ULSCPP_NSTR2USTR(pfx.c_str(), ustr0, 0);
	_ULSCPP_NSTR2USTR(suff.c_str(), ustr1, 1);

	uls_dump_tok(&lex, ustr0, ustr1);
}

void UlsLex::dumpTok(wstring wpfx, wstring wsuff)
{
	const char *ustr0, *ustr1;

	_ULSCPP_WSTR2USTR(wpfx.c_str(), ustr0, 0);
	_ULSCPP_WSTR2USTR(wsuff.c_str(), ustr1, 1);

	uls_dump_tok(&lex, ustr0, ustr1);
}

void UlsLex::dumpTok(void)
{
	uls_dump_tok(&lex, "\t", "\n");
}

// <brief>
// Return the keyword string of the token number.
// </brief>
// <parm name="t">A token number</parm>
// <return>string</return>
void
UlsLex::Keyword(int t, string *ptr_keyw)
{
	const char *keyw, *nstr;

	if ((keyw = uls_tok2keyw(&lex, t)) == NULL)
		keyw = "<unknown>";

	_ULSCPP_USTR2NSTR(keyw, nstr, 0);
	*ptr_keyw = nstr;
}

void
UlsLex::Keyword(int t, wstring *ptr_keyw)
{
	const char *keyw;
	const wchar_t *wstr;

	if ((keyw = uls_tok2keyw(&lex, t)) == NULL)
		keyw = "<unknown>";

	_ULSCPP_USTR2WSTR(keyw, wstr, 0);
	*ptr_keyw = wstr;
}

void
UlsLex::getKeywordStr(int t, string *ptr_keyw)
{
	Keyword(t, ptr_keyw);
}

void
UlsLex::getKeywordStr(int t, wstring *ptr_keyw)
{
	Keyword(t, ptr_keyw);
}

// <brief>
// return the keyword string of the current token.
// </brief>
// <return>keyword string</return>
void
UlsLex::Keyword(string *ptr_keyw)
{
	Keyword(lxm_id, ptr_keyw);
}

void
UlsLex::Keyword(std::wstring *ptr_keyw)
{
	Keyword(lxm_id, ptr_keyw);
}

void
UlsLex::getKeywordStr(string *ptr_keyw)
{
	Keyword(ptr_keyw);
}

void
UlsLex::getKeywordStr(std::wstring *ptr_keyw)
{
	Keyword(ptr_keyw);
}

// <brief>
// A method that emits the formatted message from the string 'fmt' with 'args'
// </brief>
// <parm name="fmt">The format string, a template for printing</parm>
// <parm name="args">The list of args</parm>
// <return>none</return>
void UlsLex::vlog(const char* fmt, va_list args)
{
	uls::lockMutex(&syserr_g_mtx);
	uls_vlog(&logbase, fmt, args);
	uls::unlockMutex(&syserr_g_mtx);
}

void UlsLex::vwlog(const wchar_t* wfmt, va_list args)
{
	uls::lockMutex(&syserr_g_mtx);
	uls_vwlog(&logbase, wfmt, args);
	uls::unlockMutex(&syserr_g_mtx);
}

// <brief>
// Logs formatted messages if the 'loglvl' is set in the object.
// The available loglvl are ULS_LOG_EMERG, ULS_LOG_ALERT, ULS_LOG_CRIT, ...
// </brief>
// <parm name="loglvl">This will print the formatted message if 'loglvl' is set.</parm>
// <return>none</return>
void UlsLex::log(int loglvl, const char* fmt, ...)
{
	va_list	args;

	if (!(logbase.log_mask & ULS_LOGLEVEL_FLAG(loglvl)))
		return;

	va_start(args, fmt);
	vlog(fmt, args);
	va_end(args);
}

void UlsLex::log(int loglvl, const wchar_t* wfmt, ...)
{
	va_list	args;

	if (!(logbase.log_mask & ULS_LOGLEVEL_FLAG(loglvl)))
		return;

	va_start(args, wfmt);
	vwlog(wfmt, args);
	va_end(args);
}

// <brief>
// Logs formatted messages
// No need to append '\n' to the end of line 'fmt'
// You can use %t %w to print the current token, its location.
// No need argument for %t, %w.
// </brief>
// <parm name="fmt">The template for message string</parm>
// <return>void</return>
void UlsLex::log(const char* fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	vlog(fmt, args);
	va_end(args);
}

void UlsLex::log(const wchar_t* wfmt, ...)
{
	va_list	args;

	va_start(args, wfmt);
	vwlog(wfmt, args);
	va_end(args);
}

// <brief>
// Logs formatted messages and the program will be aborted.
// No need to append '\n' to the end of line 'fmt'
// </brief>
// <parm name="fmt">The template for message string</parm>
// <return>none</return>
void UlsLex::panic(const char* fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	vlog(fmt, args);
	va_end(args);

	exit(1);
}

void UlsLex::panic(const wchar_t* wfmt, ...)
{
	va_list	args;

	va_start(args, wfmt);
	vwlog(wfmt, args);
	va_end(args);

	exit(1);
}

// <brief>
// Opens a file for output.
// After the calls of print() or wprint(), be sure to call closeOutput().
// </brief>
// <parm name="out_file">The output file path</parm>
// <return>none</return>
void UlsLex::openOutput_ustr(const char* out_file, uls_lf_puts_t puts_proc)
{
	FILE *fp;

	if ((fp = uls_fp_open(out_file, ULS_FIO_CREAT | ULS_FIO_WRITE)) == NULL) {
		throw std::invalid_argument("fail to create an output file.");
	}

	uls::lockMutex(&sysprn_g_mtx);

	if (sysprn_opened) {
		uls::unlockMutex(&sysprn_g_mtx);
	}
	else {
		sysprn_opened = 1;
		prn_nlf->x_dat = (void *)fp;
		prn_nlf->uls_lf_puts = puts_proc;
	}
}

void UlsLex::openOutput(string& out_file)
{
	const char *ustr;
	uls_lf_puts_t puts_proc;

	puts_proc = uls_lf_puts_file;
	_ULSCPP_NSTR2USTR(out_file.c_str(), ustr, 0);
	openOutput_ustr(ustr, puts_proc);
}

void UlsLex::openOutput(wstring& out_wfile)
{
	uls_lf_puts_t puts_proc;

	puts_proc = uls_lf_wputs_file;

	const char *ustr;
	_ULSCPP_WSTR2USTR(out_wfile.c_str(), ustr, 0);

	openOutput_ustr(ustr, puts_proc);
}

// <brief>
// This flushes the buffer of the output and closes the output file.
// After using the calls of print() method, don't forget to call closeOutput().
// </brief>
// <return>none</return>
void UlsLex::closeOutput(void)
{
	FILE  *cstdio_out = _uls_stdio_fp(1);
	uls_lf_puts_t puts_proc;
	FILE *fp;

	puts_proc = uls_lf_puts_file;

	if (sysprn_opened) {
		fp = (FILE *) prn_nlf->x_dat;
		uls_fp_close(fp);

		prn_nlf->x_dat = (void *) cstdio_out;
		prn_nlf->uls_lf_puts = puts_proc;
		sysprn_opened = 0;
		uls::unlockMutex(&sysprn_g_mtx);
	}
}

// <brief>
// Changes the associated procedure with 'percent_name', a converion specification.
// The procedure will output string with puts_proc which can be set by 'changePuts'
// </brief>
// <parm name="percent_name">A converion specification void of the percent character itself.</parm>
// <parm name="proc">The user-defined procedure for processing '%percent_name'</parm>
// <return>none</return>
void
UlsLex::changeConvSpec(const char* percent_name, uls_lf_convspec_t proc)
{
	const char *ustr;

	_ULSCPP_NSTR2USTR(percent_name, ustr, 0);
	uls_lf_register_convspec(ulscpp_convspec_nmap, ustr, proc);
}

void
UlsLex::changeConvSpec(const wchar_t* percent_wname, uls_lf_convspec_t proc)
{
	const char *ustr;

	_ULSCPP_WSTR2USTR(percent_wname, ustr, 0);
	uls_lf_register_convspec(ulscpp_convspec_wmap, ustr, proc);
}

// <brief>
// You can use this method to change the default output interface for logging.
// </brief>
// <parm name="puts_proc">A newly output interface(puts-style)</parm>
// <return>none</return>
void
UlsLex::changePuts(void *xdat, uls_lf_puts_t puts_proc)
{
	uls_lf_delegate_t delegate;

	delegate.xdat = xdat;
	delegate.puts = puts_proc;

	uls_lf_change_puts(prn_nlf, &delegate);
}

// <brief>
// This will print the formatted string to the output port which
//     is initialized by calling openOutput.
// The default port is stdout.
// </brief>
// <parm name="fmt">The template for message string</parm>
// <return># of chars printed</return>
int UlsLex::print(const char* fmt, ...)
{
	va_list	args;
	int len;

	va_start(args, fmt);
	len = uls_lf_vxprintf(prn_nlf, fmt, args);
	va_end(args);

	return len;
}

int UlsLex::print(const wchar_t* fmt, ...)
{
	va_list	args;
	int len;

	va_start(args, fmt);
	len = uls_lf_vxwprintf(prn_wlf, fmt, args);
	va_end(args);

	return len;
}

// <brief>
// This stores the formatted string to the 'buf'.
// </brief>
// <parm name="buf">The output buffer for the formatted string</parm>
// <parm name="bufsiz">The size of 'buf'</parm>
// <parm name="fmt">The template for message string</parm>
// <return># of chars filled except for '\0'</return>
int
UlsLex::vsnprintf(char* buf, int bufsiz, const char *fmt, va_list args)
{
	int len;

	uls_lf_lock(str_nlf);
	len = __uls_lf_vsnprintf(buf, bufsiz, str_nlf, fmt, args);
	uls_lf_unlock(str_nlf);

	return len;
}

int
UlsLex::vsnprintf(wchar_t* wbuf, int bufsiz, const wchar_t *wfmt, va_list args)
{
	int len;

	uls_lf_lock(str_wlf);
	len = __uls_lf_vsnwprintf(wbuf, bufsiz, str_wlf, wfmt, args);
	uls_lf_unlock(str_wlf);

	return len;
}

int
UlsLex::snprintf(char* buf, int bufsiz, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vsnprintf(buf, bufsiz, fmt, args);
	va_end(args);

	return len;
}

int
UlsLex::snprintf(wchar_t* wbuf, int bufsiz, const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = vsnprintf(wbuf, bufsiz, wfmt, args);
	va_end(args);

	return len;
}

// <brief>
// This stores the formatted string to the 'buf' with its size unknown.
// Recommended not to use this but snprintf().
// </brief>
// <parm name="buf">The output buffer for the formatted string</parm>
// <parm name="fmt">The template for message string</parm>
// <return># of chars filled</return>
int
UlsLex::sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = snprintf(buf, (~0U) >> 1, fmt, args);
	va_end(args);

	return len;
}

int
UlsLex::sprintf(wchar_t * wbuf, const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = snprintf(wbuf, (~0U) >> 1, wfmt, args);
	va_end(args);

	return len;
}

// <brief>
// This stores the formatted string to the 'fp', pointer of FILE.
// </brief>
// <parm name="fp">An output port to be written</parm>
// <parm name="fmt">The template for message string</parm>
// <return># of chars written</return>
int
UlsLex::vfprintf(FILE* fp, const char *fmt, va_list args)
{
	int len;
	len = uls_lf_vxprintf_generic(fp, file_nlf, fmt, args);
	return len;
}

int
UlsLex::vfprintf(FILE* fp, const wchar_t *wfmt, va_list args)
{
	int len;

	uls_lf_lock(file_wlf);
	len = __uls_lf_vfwprintf(fp, file_wlf, wfmt, args);
	uls_lf_unlock(file_wlf);

	return len;
}

int
UlsLex::fprintf(FILE* fp, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vfprintf(fp, fmt, args);
	va_end(args);

	return len;
}

int
UlsLex::fprintf(FILE* fp, const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = vfprintf(fp, wfmt, args);
	va_end(args);

	return len;
}

// <brief>
// This prints the formatted string to the 'stdout'.
// </brief>
// <parm name="fmt">The template for message string</parm>
// <return># of chars printed</return>
int
UlsLex::printf(const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vfprintf(_uls_stdio_fp(1), fmt, args);
	va_end(args);

	return len;
}

int
UlsLex::printf(const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = vfprintf(_uls_stdio_fp(1), wfmt, args);
	va_end(args);

	return len;
}

