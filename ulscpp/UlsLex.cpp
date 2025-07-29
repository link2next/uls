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

#include <uls/uls_lex.h>
#include <uls/uld_conf.h>
#include <uls/uls_fileio.h>
#include <uls/uls_lf_xputs.h>
#ifdef __ULS_WINDOWS__
#include <uls/ms_mbcs_file.h>
#endif

#include <string>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "uls.h"

using namespace std;
using namespace uls::crux;

bool UlsLexUStr::ulslex_inited = false;
bool UlsLexAWStr::ulslexwstr_inited = false;

uls_lf_map_t UlsLexUStr::ulscpp_convspec_nmap;
#ifdef __ULS_WINDOWS__
uls_lf_map_t UlsLexAWStr::ulscpp_convspec_amap;
#endif
uls_lf_map_t UlsLexAWStr::ulscpp_convspec_wmap;

void
#ifdef __GNUC__
__attribute__((constructor))
#endif
_initialize_ulscpp(void)
{
	UlsLexUStr::initialize();
	UlsLexAWStr::initialize();
}

void
#ifdef __GNUC__
__attribute__((destructor))
#endif
_finalize_ulscpp(void)
{
	UlsLexAWStr::finalize();
	UlsLexUStr::finalize();
}

#ifdef __ULS_WINDOWS__
BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
	BOOL rval = TRUE;

	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		_initialize_ulscpp();
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		_finalize_ulscpp();
		break;
	}

	return rval;
}
#endif

#if defined(HAVE_PTHREAD)
// <brief>
//   This Initializes the mutex 'mtx' which is to be used by locked/unlocked method.
//   The mechanism's Implemented by the pthread library in Linux and by Win32 API in Windows.
// </brief>
// <parm name="mtx">Mutex object</parm>
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
//   Deinitialize the 'mtx'. After calling this, 'mtx' shouldn't be used.
// </brief>
// <parm name="mtx">The mutex object used in ULS</parm>
void
uls::deinitMutex(uls::MutexType mtx)
{
	if (pthread_mutex_destroy(mtx->mtx_pthr) != 0)
		err_panic("error: mutex destroy\n");

	uls_mfree(mtx->mtx_pthr);
}

// <brief>
//   The locking API to grab the lock 'mtx'.
// </brief>
// <parm name="mtx">The mutex object initialized by uls::initMutex</parm>
void
uls::lockMutex(uls::MutexType mtx)
{
	if (pthread_mutex_lock(mtx->mtx_pthr) != 0)
		err_panic("error to pthread_mutex_lock");
}

// <brief>
//   The unlocking API to ungrab the lock 'mtx'.
// </brief>
// <parm name="mtx">The mutex object initialized by uls::initMutex</parm>
void
uls::unlockMutex(uls::MutexType mtx)
{
	if (pthread_mutex_unlock(mtx->mtx_pthr) != 0)
		err_panic("error to pthread_mutex_unlock");
}

#elif defined(__ULS_WINDOWS__)
// <brief>
//   Initializes the mutex 'mtx' to be used from then on by locked/unlocked method.
//   Implemented lock/unlock with the mutex of Win32Api in Windows
// </brief>
// <parm name="mtx">Mutex object</parm>
void
uls::initMutex(uls::MutexType mtx)
{
	mtx->hndl = CreateMutex(NULL, FALSE, NULL);

	if (mtx->hndl == INVALID_HANDLE_VALUE)
		err_panic("error: mutex init\n");
}

// <brief>
//   De-initialize the 'mtx'.
//   Hereafter you must not use 'mtx'.
// </brief>
// <parm name="mtx">The mutex object used in ULS</parm>
void
uls::deinitMutex(uls::MutexType mtx)
{
	CloseHandle(mtx->hndl);
	mtx->hndl = INVALID_HANDLE_VALUE;
}

// <brief>
//   A Lock API in ULS.
// </brief>
// <parm name="mtx">The mutex object initialized by uls::initMutex</parm>
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
//   An Unlock API in ULS.
// </brief>
// <parm name="mtx">The mutex object initialized by uls::initMutex</parm>
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

// <brief>
//   This procedure lists the search directories for ulc file, which is suffixed by 'ulc'.
//   uls_create() or uls_init() will search the directories for ulc file in same order as this dumpSearchPathOfUlc().
//   The order of search-directories is affected by whether or not the paramenter 'confname' is suffixed by '.ulc'.
//   If the parameter is suffixed by '.ulc' or has '..' or '.', it's recognized as file path.
//   But if not suffixed by 'ulc' like 'sample', 'dir/sample',
//     the procedure will search the ulc repository preferentially.
// </brief>
// <parm name="confname">The name of lexcial configuration, as like 'sample', 'dir/sample', or 'sample.ulc'</parm>
void uls::dumpSearchPathOfUlc(const string& confname)
{
	const char *austr;
	csz_str_t csz;

	csz_init(&csz, -1);
#ifdef __ULS_WINDOWS__
	austr = uls_astr2ustr(confname.c_str(), -1, &csz);
	if (austr == NULL) {
		err_log("encoding error!");
		csz_deinit(&csz);
		return;
	}
#else
	austr = confname.c_str();
#endif

	ulc_list_searchpath(austr);
	csz_deinit(&csz);
}

void uls::dumpSearchPathOfUlc(const wstring& wconfwname)
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
//   This will list the serach paths, preferentially the location of ulc repository.
// </brief>
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
//   For internal use only:
//   isLexemeReal, isLexemeInteger, isLexemeZero, lexemeAsInt
//   Instead use the class UlsLexUStr APIs
// </brief>

bool
uls::crux::isLexemeZero(const string& lxm)
{
	const char *ptr = lxm.c_str();

	if (ptr[0] == '0' || (ptr[0] == '.' && ptr[1] == '0'))
		return true;

	return false;
}

bool
uls::crux::isLexemeZero(const wstring& lxm)
{
	const wchar_t *ptr = lxm.c_str();

	if (ptr[0] == L'0' || (ptr[0] == L'.' && ptr[1] == L'0'))
		return true;

	return false;
}

bool
uls::crux::isLexemeInt(const string& lxm)
{
	return uls::crux::isLexemeReal(lxm) ? false : true;
}

bool
uls::crux::isLexemeInt(const wstring& lxm)
{
	return uls::crux::isLexemeReal(lxm) ? false : true;
}

bool
uls::crux::isLexemeReal(const string& lxm)
{
	const char *ptr = lxm.c_str();

	if (*ptr == '.') return true;

	return false;

}

bool
uls::crux::isLexemeReal(const wstring& lxm)
{
	const wchar_t *ptr = lxm.c_str();

	if (*ptr == L'.') return true;

	return false;

}

// <brief>
//   In case 'lxm' is a floating point number, this function returns the value of it.
//   The 'lxm' may be the one from the current lexeme of uls-object.
// </brief>
// <parm name="lxm">string</parm>
// <return>true/false</return>
int
uls::crux::LexemeAsInt(const string& lxm)
{
	const char *ptr = lxm.c_str();
	return strtoul(ptr, NULL, 16);
}

int
uls::crux::LexemeAsInt(const wstring& lxm)
{
	const wchar_t *ptr = lxm.c_str();
	return wcstoul(ptr, NULL, 16);
}

double
uls::crux::LexemeAsDouble(const string& lxm)
{
	const char *ptr = lxm.c_str();
	return atof(ptr);
}

double
uls::crux::LexemeAsDouble(const wstring& wlxm)
{
	const wchar_t *ptr = wlxm.c_str();
	double fval;

#ifdef __ULS_WINDOWS__
	fval = _wtof(ptr);
#else
	fval = wcstod(ptr, NULL);
#endif

	return fval;
}

//
// UlsLexUStr
//
void
UlsLexUStr::initialize()
{
	if (ulslex_inited == true) return;

	uls_lf_init_convspec_map(uls_ptr(ulscpp_convspec_nmap), 0);
	uls_add_default_log_convspecs(uls_ptr(ulscpp_convspec_nmap));

	ulslex_inited = true;
}

void
UlsLexUStr::finalize()
{
	if (ulslex_inited == false) return;
	uls_lf_deinit_convspec_map(uls_ptr(ulscpp_convspec_nmap));
	ulslex_inited = false;
}

// <brief>
//   In case the paramenter 'lxm' is a floating point number,
//     this function returns the value of it.
//   The 'lxm' may be the one from the current lexeme of uls-object.
// </brief>
// <parm name="lxm">string</parm>
// <return>true/false</return>
int UlsLexUStr::get_uls_flags(UlsLexUStr::InputOpts fl)
{
	int ret_fl;

	switch (fl) {
	case WantEOF:
		ret_fl = ULS_WANT_EOFTOK;
		break;
	case DoDup:
		ret_fl = ULS_DO_DUP;
		break;
	default:
		ret_fl = -1;
		break;
	}

	return ret_fl;
}

void UlsLexUStr::setInputOpt(UlsLexUStr::InputOpts fl)
{
	int uls_fl = get_uls_flags(fl);
	input_flags |= uls_fl;
}


void UlsLexUStr::clearInputOpt(UlsLexUStr::InputOpts fl)
{
	int uls_fl = get_uls_flags(fl);
	input_flags &=  ~uls_fl;
}

int UlsLexUStr::getInputOpts(void)
{
	return input_flags;
}

void UlsLexUStr::resetInputOpts(void)
{
	input_flags = 0;
}

void
UlsLexUStr::update_rsvdtoks(void)
{
	toknum_EOI = _uls_toknum_EOI(&lex);
	toknum_EOF = _uls_toknum_EOF(&lex);
	toknum_ERR = _uls_toknum_ERR(&lex);
	toknum_NONE = _uls_toknum_NONE(&lex);
	toknum_ID = _uls_toknum_ID(&lex);
	toknum_NUMBER = _uls_toknum_NUMBER(&lex);
	toknum_TMPL = _uls_toknum_TMPL(&lex);
}

// <brief>
//   The constructor that creates an object for lexical analyzing.
// </brief>
// <parm name="ulc_file">The name or path of its lexical configuration.</parm>
bool
UlsLexUStr::initUlsLex_ustr(const char *ulc_file)
{
	uls_lf_puts_t puts_proc_str, puts_proc_file;

	if (uls_init(&lex, ulc_file) < 0) {
		return false;
	}

	input_flags = 0;
	update_rsvdtoks();

	puts_proc_str = uls_lf_puts_str;
	puts_proc_file = uls_lf_puts_file;

	str_nlf = uls_lf_create(uls_ptr(ulscpp_convspec_nmap), puts_proc_str);
	uls_lf_change_gdat(str_nlf, &lex);

	file_nlf = uls_lf_create(uls_ptr(ulscpp_convspec_nmap), puts_proc_file);
	uls_lf_change_gdat(file_nlf, &lex);

	uls::initMutex(&sysprn_g_mtx);
	sysprn_fp = _uls_stdio_fp(1);
	sysprn_opened = 0;

	uls::initMutex(&syserr_g_mtx);
	if (uls_init_log(&logbase, NULL, &lex) < 0) {
		return false;
	}

	return true;
}

UlsLexUStr::UlsLexUStr()
{
	input_flags = 0;
	names_map = NULL;

	uls_bzero(&lex, sizeof(uls_lex_t));
	toknum_EOI = toknum_EOF = toknum_ERR =
		toknum_NONE = toknum_ID = toknum_NUMBER = toknum_TMPL = 0;
	str_nlf = file_nlf = NULL;

	uls_bzero(&syserr_g_mtx, sizeof(uls_mutex_struct_t));
	uls_bzero(&logbase, sizeof(uls_log_t));

	uls_bzero(&sysprn_g_mtx, sizeof(uls_mutex_struct_t));
	sysprn_opened = 0;
}

UlsLexUStr::UlsLexUStr(const char *ulc_file)
	: UlsLexUStr()
{
	if (initUlsLex_ustr(ulc_file) == false) {
		string errmsg = string(ulc_file) + ": file not proper!";
		throw invalid_argument(errmsg);
	}
}

UlsLexUStr::UlsLexUStr(string& ulc_file)
	: UlsLexUStr()
{
	if (initUlsLex_ustr(ulc_file.c_str()) == false) {
		string errmsg = string(ulc_file) + ": file not proper!";
		throw invalid_argument(errmsg);
	}
}

// <brief>
//   The destructor of UlsLexUStr.
// </brief>
UlsLexUStr::~UlsLexUStr()
{
	uls::deinitMutex(&syserr_g_mtx);
	uls::deinitMutex(&sysprn_g_mtx);

	uls_deinit_log(&logbase);
	uls_destroy(&lex);

	uls_lf_destroy(file_nlf);
	uls_lf_destroy(str_nlf);
}

int UlsLexUStr::prepareUldMap(int bufsiz_uldfile)
{
	names_map = uld_prepare_names(&lex, bufsiz_uldfile);
	return 0;
}

bool UlsLexUStr::finishUldMap()
{
	bool stat = true;

	if (uld_post_names(names_map) < 0) {
		stat = false;
	}
	names_map = NULL;

	return stat;
}

void UlsLexUStr::changeUldNames(const char *name, const char*name2,
	int tokid_valid, int tokid, const char *aliases)
{
	uld_line_t tok_names;
	char *buff;

 	tok_names.name = name;
	tok_names.name2 = name2;
	tok_names.tokid_changed = tokid_valid;
	tok_names.tokid = tokid;

	if (aliases == NULL) aliases = "";
	buff = uls_strdup(aliases, -1);
	tok_names.aliases = buff;

	uld_change_names(names_map, &tok_names);
	uls_mfree(buff);
}

void UlsLexUStr::changeUldNames(const string& name, const string& name2,
	int tokid_valid, int tokid, const string& aliases)
{
	changeUldNames(name.c_str(), name2.c_str(),
		tokid_valid, tokid, aliases.c_str());
}

// <brief>
//   This setter is to set the private data 'FileName' with the parameter 'fname'
// </brief>
// <parm name="fname">The new string value of 'FileName'</parm>
void UlsLexUStr::getTag(string& fname)
{
	const char *ustr = uls_get_tag(&lex);
	fname = ustr;
}

void UlsLexUStr::setTag(const char *fname)
{
	uls_set_tag(&lex, fname, -1);
}

void UlsLexUStr::setTag(const string& fname)
{
	setTag(fname.c_str());
}

// <brief>
//   Sets the internal data 'LineNum' to 'lineno' forcibly.
// </brief>
// <parm name="lineno">The new value of 'LineNum'</parm>
void UlsLexUStr::setLineNum(int lineno)
{
	uls_set_lineno(&lex, lineno);
}

// <brief>
//   Adds the current 'LineNum' by 'amount'.
// </brief>
// <parm name="amount">The amount of lines to be added</parm>
void UlsLexUStr::addLineNum(int amount)
{
	uls_inc_lineno(&lex, amount);
}

// <brief>
//   Returns the position in the input
// </brief>
// <return>the current line number</return>
int UlsLexUStr::getLineNum(void)
{
	return uls_get_lineno(&lex);
}

// <brief>
//   Sets the log level of the object, UlsLexUStr.
//   The possible 'loglvl' are ULS_LOG_EMERG, ULS_LOG_ALERT, ULS_LOG_CRIT, ...
// </brief>
// <parm name="lvl">new value of log level</parm>
void UlsLexUStr::setLogLevel(int lvl)
{
	uls_set_loglevel(&logbase, lvl);
}

// <brief>
//   Clears the log level defined in the uls object.
// </brief>
// <parm name="lvl">the log level to be cleared.</parm>
void UlsLexUStr::clearLogLevel(int lvl)
{
	uls_clear_loglevel(&logbase, lvl);
}

// <brief>
//   Checks if the 'lvl' is set in the internally.
//   'lvl' is a flag such as  ULS_LOG_EMERG, ULS_LOG_ALERT, ULS_LOG_CRIT, ...
// </brief>
// <parm name="lvl">the log level to be checked.</parm>
// <return>true/false</return>
bool UlsLexUStr::isLogLevelSet(int lvl)
{
	return uls_loglevel_isset(&logbase, lvl) ? true : false;
}

// <brief>
//   Changes the literal-string analyzer to 'proc'.
//   The 'proc' will be applied to the quote strings starting with 'pfx'.
// </brief>
// <parm name="pfx">The prefix of literal string that will be processed by 'proc'</parm>
void UlsLexUStr::changeLiteralAnalyzer(const char *pfx, uls_litstr_analyzer_t proc, void* data)
{
	uls_xcontext_change_litstr_analyzer(uls_ptr(lex.xcontext), pfx, proc, data);
}

void UlsLexUStr::changeLiteralAnalyzer(const string& pfx, uls_litstr_analyzer_t proc, void* data)
{
	changeLiteralAnalyzer(pfx.c_str(), proc, data);
}

// <brief>
//   Delete the literal-string analyzer that
//     is processing the quote-strings starting with 'pfx'.
// </brief>
// <parm name="pfx">The literal string analyzer of which the quote type is started with 'pfx'.</parm>
void UlsLexUStr::deleteLiteralAnalyzer(const char *pfx)
{
	uls_xcontext_delete_litstr_analyzer(uls_ptr(lex.xcontext), pfx);
}

void UlsLexUStr::deleteLiteralAnalyzer(const string& pfx)
{
	deleteLiteralAnalyzer(pfx.c_str());
}

// <brief>
//   This method will push an input string 'istr' on the top of the internal input stack.
//   Then the next() method can be used to get a token from the input.
// </brief>
// <parm name="hdr">the header information</parm>
// <parm name="flags">
//   Set ULS_WANT_EOFTOK if you want EOF-token whenever each file reaches at the end of file
//   Set ULS_NO_EOFTOK if you don't need EOF-token at the end of each input file
//  </parm>
bool UlsLexUStr::pushInput(UlsIStream& in_str, int flags)
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
//   This method pushes (raw) source-file on the internal stack.
//   The raw source-file is just a regular text file.
// </brief>
// <parm name="fd">the file descriptor of input file.</parm>
// <parm name="flags">
//   Set ULS_WANT_EOFTOK if you want EOF-token whenever each file reaches at the end of file
//   Set ULS_NO_EOFTOK if you don't need EOF-token at the end of each input file
// </parm>
bool UlsLexUStr::pushInput(int fd, int flags)
{
	if (flags < 0) {
		flags = getInputOpts();
	}

	if (uls_push_fd(&lex, fd, flags) < 0) {
		return false;
	}

	uls_set_lineno(&lex, 1);
	return true;
}

// <brief>
//   Dismiss the current input stream.
// </brief>
void UlsLexUStr::popInput(void)
{
	uls_pop(&lex);
}

// <brief>
//   Dismiss all the input streams and its state goes back to initial.
// </brief>
void UlsLexUStr::popAllInputs(void)
{
	uls_pop_all(&lex);
}

// <brief>
//   This method pushes (raw) source-file on the internal stack.
//   The raw source-file is just a regular text file.
// </brief>
// <parm name="fd">the file descriptor of input file.</parm>
// <parm name="flags">
//   Set ULS_WANT_EOFTOK if you want receive the reserved token EOF
//       whenever each file reaches at the end of file
//   Set ULS_NO_EOFTOK if you don't need to receive the EOF-token
//       at the end of each input file
// </parm>
void UlsLexUStr::pushFd(int fd, int flags)
{
	if (flags < 0) {
		flags = getInputOpts();
	}

	if (uls_push_fd(&lex, fd, flags) < 0) {
		throw invalid_argument("the param 'fd' is invalid.");
	}

	uls_set_lineno(&lex, 1);
}

void UlsLexUStr::setFd(int fd, int flags)
{
	if (flags < 0) {
		flags = getInputOpts();
	}

	if (uls_set_fd(&lex, fd, flags) < 0) {
		throw invalid_argument("the param 'fd' is invalid.");
	}

	uls_set_lineno(&lex, 1);
}

// <brief>
//   This method will push the input-file on the top of the input stack.
// </brief>
// <parm name="filepath">The file path of input.</parm>
// <parm name="flags">
//   Set ULS_WANT_EOFTOK if you want EOF-token whenever each file reaches at the end of file
//   Set ULS_NO_EOFTOK if you don't need EOF-token at the end of each input file
// </parm>
bool UlsLexUStr::pushFile(const char *filepath, int flags)
{
	bool rval = true;

	if (flags < 0) {
		flags = getInputOpts();
	}

	if (uls_push_file(&lex, filepath, flags) < 0) {
		rval = false;
	}

	return rval;
}

bool UlsLexUStr::pushFile(const string& filepath, int flags)
{
	return pushFile(filepath.c_str(), flags);
}

bool UlsLexUStr::setFile(const char *filepath, int flags)
{
	bool rval = true;

	if (flags < 0) {
		flags = getInputOpts();
	}

	if (uls_set_file(&lex, filepath, flags) < 0) {
		rval = false;
	}

	return rval;
}

bool UlsLexUStr::setFile(const string& filepath, int flags)
{
	return setFile(filepath.c_str(), flags);
}

// <brief>
//   This method will push the string as an input source on the top of the input stack.
// </brief>
// <parm name="line">A input stream as an literal string</parm>
// <parm name="flags">
//   Set ULS_WANT_EOFTOK if you want EOF-token whenever each file reaches at the end of file
//   Set ULS_NO_EOFTOK if you don't need EOF-token at the end of each input file
// </parm>
bool UlsLexUStr::pushLine(const char *line, int len, int flags)
{
	const char *ustr;
	bool rval = true;

	if (flags < 0) {
		flags = getInputOpts();
	}

	ustr = line;
	len = uls_strlen(ustr);

	if (uls_push_line(&lex, line, len, flags | ULS_DO_DUP) < 0) {
		rval = false;
	}

	return rval;
}

bool UlsLexUStr::pushLine(const string& line, int flags)
{
	return pushLine(line.c_str(), (int) line.length(), flags);
}

bool UlsLexUStr::setLine(const char *line, int len, int flags)
{
	const char *ustr;
	bool rval = true;

	if (flags < 0) {
		flags = getInputOpts();
	}

	ustr = line;
	len = uls_strlen(ustr);

	if (uls_set_line(&lex, ustr, len, flags | ULS_DO_DUP) < 0) {
		rval = false;
	}

	return rval;
}

bool UlsLexUStr::setLine(const string& line, int flags)
{
	return setLine(line.c_str(), (int) line.length(), flags);
}

// <brief>
//   Identifies the char-group of 'ch'.
//   Checks if 'ch' can be a blank character.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLexUStr::is_ch_space(uls_wch_t wch)
{
	return uls_canbe_ch_space(lex.ch_context, wch) ? true : false;
}

// <brief>
//   Identifies the char-group of 'ch'.
//   Checks if 'ch' can be a first char of identifier.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLexUStr::is_ch_idfirst(uls_wch_t wch)
{
	return uls_canbe_ch_idfirst(lex.ch_context, wch) ? true : false;
}

// <brief>
//   Identifies the char-group of 'ch'.
//   Checks if 'ch' can be a char of identifier.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLexUStr::is_ch_id(uls_wch_t wch)
{
	return uls_canbe_ch_id(lex.ch_context, wch) ? true : false;
}

// <brief>
//   Identifies the char-group of 'ch'.
//   Checks if 'ch' can be a first char of literal string.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLexUStr::is_ch_quote(uls_wch_t wch)
{
	return uls_canbe_ch_quote(lex.ch_context, wch) ? true : false;
}

// <brief>
//   Identifies the char-group of 'ch'.
//   Checks if 'ch' can be char of one-char token.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLexUStr::is_ch_1ch_token(uls_wch_t wch)
{
	return uls_is_ch_1ch_token(&lex, wch) ? true : false;
}

// <brief>
//   Identifies the char-group of 'ch'.
//   Checks if 'ch' can be a first character of the token
//     which is the keyword of more than one non-alphanumeric char.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLexUStr::is_ch_2ch_token(uls_wch_t wch)
{
	return uls_canbe_ch_2ch_token(lex.ch_context, wch) ? true : false;
}

// <brief>
//   Identifies the char-group of 'ch'.
//   Checks if 'ch' can be a first character of comment.
// </brief>
// <parm name="ch">The char to be tested.</parm>
// <return>bool</return>
bool UlsLexUStr::is_ch_comm(uls_wch_t wch)
{
	return uls_canbe_ch_comm(lex.ch_context, wch) ? true : false;
}

// <brief>
//   Skips the blank chars.
// </brief>
void UlsLexUStr::skipBlanks(void)
{
	uls_skip_blanks(&lex);
}

// <brief>
//   Peeks the next character in the internal buffer.
// </brief>
// <parm name="isQuote">checks if the next char is a first char of some quote-string.</parm>
// <return>The next character</return>
uls_wch_t UlsLexUStr::peekChar(void)
{
	uls_nextch_detail_t detail;
	uls_wch_t wch;
	int qtok;

	if ((wch = uls_peek_ch(&lex, &detail)) == ULS_WCH_NONE) {
		qtok = detail.tok;
		if (detail.qmt != NULL) {
			wch = ULS_CH_QSTR;
		} else if (qtok == toknum_EOI) {
			wch = ULS_CH_EOI;
		} else if (qtok == toknum_EOF || qtok == toknum_NONE) {
			wch = ULS_CH_NONE;
		} else {
			wch = ULS_CH_ERR;
		}
	}

	return wch;
}

// <brief>
//   Extracts the next character.
// </brief>
// <return>The next character</return>
uls_wch_t UlsLexUStr::getChar(void)
{
	uls_nextch_detail_t detail;
	uls_wch_t wch;
	int qtok;

	if ((wch = uls_get_ch(&lex, &detail)) == ULS_WCH_NONE) {
		qtok = detail.tok;
		if (detail.qmt != NULL) {
			wch = ULS_CH_QSTR;
		} else if (qtok == toknum_EOI) {
			wch = ULS_CH_EOI;
		} else if (qtok == toknum_EOF || qtok == toknum_NONE) {
			wch = ULS_CH_NONE;
		} else {
			wch = ULS_CH_ERR;
		}
	}

	return wch;
}

// <brief>
//   Sets the current token to <t,lxm> forcibly.
// </brief>
// <return>The token number</return>
void UlsLexUStr::setTokUStr(int t, const char *lxm)
{
	if (lxm == NULL) lxm = "";
	uls_set_tok(&lex, t, lxm, -1);
}

void UlsLexUStr::setTok(int t, const string lxm)
{
	setTokUStr(t, lxm.c_str());
}

// <brief>
//   Advances the (internal) cursor of the lexical object to the next step.
//   Returns the next token number of UlsLexUStr object.
// </brief>
// <return>The token number</return>
int UlsLexUStr::next(void)
{
	int  t = uls_get_tok(&lex);
	return t;
}

const char*
UlsLexUStr::getTokUtf8Str(int *ptr_ulen)
{
	const char *ustr = uls_tokstr(&lex);

	if (ptr_ulen != NULL) {
		*ptr_ulen = uls_tokstr_len(&lex);
	}

	return ustr;
}

void
UlsLexUStr::getTokStr(string& lxm)
{
	lxm = getTokUtf8Str();
}

int
UlsLexUStr::getTokNum(void)
{
	return uls_tok(&lex);
}

// <brief>
//   This function checks if 'lxm' is a floating point number.
//   Make sure the current token is a number before calling these methods.
// </brief>
// <parm name="lxm">A string</parm>
// <return>true/false</return>
bool
UlsLexUStr::isLexemeReal(void)
{
	return uls_is_real(&lex) ? true : false;
}

bool
UlsLexUStr::isLexemeInt(void)
{
	return uls_is_int(&lex) ? true : false;
}

bool
UlsLexUStr::isLexemeZero(void)
{
	return uls_is_zero(&lex) ? true : false;
}

int
UlsLexUStr::lexemeAsInt(void)
{
	return uls_lexeme_d(&lex);
}

unsigned int
UlsLexUStr::lexemeAsUInt(void)
{
	return uls_lexeme_u(&lex);
}

long long
UlsLexUStr::lexemeAsLongLong(void)
{
	return uls_lexeme_lld(&lex);
}

unsigned long long
UlsLexUStr::lexemeAsULongLong(void)
{
	return uls_lexeme_llu(&lex);
}

double
UlsLexUStr::lexemeAsDouble(void)
{
	return uls_lexeme_double(&lex);
}

string
UlsLexUStr::lexemeNumberSuffix(void)
{
	return uls_number_suffix(&lex);
}

// <brief>
//   If the current token-id is not 'TokExpected', An Exception will be thrown.
// </brief>
// <parm name="TokExpected">The expected token number</parm>
void UlsLexUStr::expect(int TokExpected)
{
	uls_expect(&lex, TokExpected);
}

bool
UlsLexUStr::existTokdefInHashMap(int t)
{
	map<int,void*>::iterator it;
	bool rval;

	it = extra_tokdefs.find(t);
	if (it != extra_tokdefs.end()) {
		rval = true;
	} else {
		rval = false;
	}

	return rval;
}

// <brief>
//   Get the extra token definition corresponding to 'tok_id'.
// </brief>
// <parm name="tok_id">The token id for which you want its (extra) token definition.</parrm>
// <return>The extra tokdef of (void *)</return>
void*&
UlsLexUStr::operator[](int t)
{
	if (existTokdefInHashMap(t) == false) {
		extra_tokdefs[t] = NULL;
	}

	return extra_tokdefs[t];
}

// <brief>
//   This sets extra token definition 'extra_tokdef' which is provided by user.
//   The stored data of token number 't' can be later retrieved by getExtraTokdef().
// </brief>
// <parm name="tok_id">The target token id of which extra tokdef is set.</parm>
// <parm name="extra_tokdef">The opaque data provided by user</parm>
void
UlsLexUStr::setExtraTokdef(int t, void* extra_tokdef)
{
	extra_tokdefs[t] = extra_tokdef;
}

void*
UlsLexUStr::getExtraTokdef(void)
{
	return getExtraTokdef(getTokNum());
}

void*
UlsLexUStr::getExtraTokdef(int t)
{
	if (existTokdefInHashMap(t) == false) {
		return NULL;
	}
	return extra_tokdefs[t];
}

// <brief>
//   Call ungetTok if you want push lexeme to the current input stream.
// </brief>
// <parm name="tok_id">
//   The corresponding token string of 'tok_id'.
// </parm>
// <parm name="tok_id">
//   The parameter 'tok_id' should be normally TOK_NONE.
//   If you want 'str' to be a quote-string, specify the corresponding tok-id of the quote-string.
// </parm>
bool UlsLexUStr::ungetTokUtf8(int tok_id, const char *lxm)
{
	int rval;
	rval = uls_unget_tok(&lex, tok_id, lxm);
	return rval < 0 ? false : true;
}

bool UlsLexUStr::ungetTok(int tok_id, const char *lxm)
{
	return ungetTokUtf8(tok_id, lxm);
}


bool UlsLexUStr::ungetTok(int tok_id, const string& lxm)
{
	return ungetTok(tok_id, lxm.c_str());
}

bool UlsLexUStr::ungetCurrent(void)
{
	return ungetTok(getTokNum());
}

// <brief>
//   Push a string into the input stream.
// </brief>
bool UlsLexUStr::ungetStrUtf8(const char *str)
{
	return uls_unget_str(&lex, str);
}

bool UlsLexUStr::ungetStr(const char *str)
{
	return ungetStrUtf8(str);
}

bool UlsLexUStr::ungetStr(const string& str)
{
	return ungetStr(str.c_str());
}

// <brief>
//   Pushes the 'ch' to the buffer so that next char with a call getChar() may be 'ch'.
// </brief>
bool UlsLexUStr::ungetChar(uls_wch_t wch)
{
	char ch_str[ULS_UTF8_CH_MAXLEN + 1];
	int rc;

	if ((rc = uls_encode_utf8(wch, ch_str, -1)) <= 0) {
		err_log("encoding error!");
		return false;
	}

	ch_str[rc] = '\0';
	return UlsLexUStr::ungetStr(ch_str);
}

// <brief>
//   Prints the information on the current token.
// </brief>
void UlsLexUStr::dumpTok(const char *pfx, const char *suff)
{
	if (pfx == NULL) pfx = "\t";
	if (suff == NULL) suff = "\n";
	uls_dump_tok(&lex, pfx, suff);
}

void UlsLexUStr::dumpTok(void)
{
	dumpTok(NULL, NULL);
}

// <brief>
//   A method that emits the formatted message from the string 'fmt' with 'args'
//   Logs formatted messages if the 'loglvl' is set in the object.
// The available loglvl are ULS_LOG_EMERG, ULS_LOG_ALERT, ULS_LOG_CRIT, ...
// </brief>
// <parm name="fmt">The format string, a template for printing</parm>
// <parm name="args">The list of args</parm>
void UlsLexUStr::vlog(int loglvl, const char *fmt, va_list args)
{
	if (logbase.log_mask & ULS_LOGLEVEL_FLAG(loglvl)) {
		uls::lockMutex(&syserr_g_mtx);
		uls_vlog(&logbase, fmt, args);
		uls::unlockMutex(&syserr_g_mtx);
	}
}

// <brief>
//   Logs formatted messages
//   No need to append '\n' to the end of line 'fmt'
// </brief>
// <parm name="fmt">The template for message string</parm>
void UlsLexUStr::log(const char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	vlog(ULS_LOG_WARN, fmt, args);
	va_end(args);
}

// <brief>
//   Logs formatted messages and the program will be aborted.
//   No need to append '\n' to the end of line 'fmt'
// </brief>
// <parm name="fmt">The template for message string</parm>
#if defined(__ULS_WINDOWS__) && !defined(ULS_DOTNET)
[[noreturn]]
#endif
void UlsLexUStr::panic(const char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);
	vlog(ULS_LOG_CRIT, fmt, args);
	va_end(args);

	exit(1);
}

// <brief>
//   Opens a file for output.
//   After the calls of print() or wprint(), be sure to call closeOutput().
// </brief>
// <parm name="out_file">The output file path</parm>
bool UlsLexUStr::openOutput_ustr(const char *out_file)
{
	bool stat = true;
	FILE *fp;

	if (out_file != NULL) {
		if ((fp = uls_fp_open(out_file, ULS_FIO_WRITE)) == NULL) {
			throw invalid_argument("fail to create an output file.");
		}
	} else {
		fp = NULL;
	}

	uls::lockMutex(&sysprn_g_mtx);

	if (sysprn_opened) {
		if (fp != NULL) uls_fp_close(fp);
		uls::unlockMutex(&sysprn_g_mtx);
		stat = false;
	} else if ((sysprn_fp = fp) != NULL) {
		sysprn_opened = 1;
	}

	return stat;
}

bool UlsLexUStr::openOutput(const char *out_file)
{
	return openOutput_ustr(out_file);
}

bool UlsLexUStr::openOutput(const string& out_file)
{
	return openOutput(out_file.c_str());
}

// <brief>
//   This flushes the buffer of the output and closes the output file.
//   After using the calls of print() method, don't forget to call closeOutput().
// </brief>
void UlsLexUStr::closeOutput(void)
{
	if (sysprn_opened) {
		uls_fp_close(sysprn_fp);
		sysprn_fp = _uls_stdio_fp(1);
		sysprn_opened = 0;
		uls::unlockMutex(&sysprn_g_mtx);
	}
}

// <brief>
//   Changes the associated procedure with 'percent_name', a converion specification.
//   The procedure will output string with puts_proc which can be set by 'changePuts'
// </brief>
// <parm name="percent_name">A converion specification void of the percent character itself.</parm>
// <parm name="proc">The user-defined procedure for processing '%percent_name'</parm>
void
UlsLexUStr::changeConvSpec(const char *percent_name, uls_lf_convspec_t proc)
{
	const char *ustr;

	ustr = percent_name;
	uls_lf_register_convspec(uls_ptr(ulscpp_convspec_nmap), ustr, proc);
}

// <brief>
//   This will print the formatted string to the output port which
//     is initialized by calling openOutput.
//   The default port is stdout.
// </brief>
// <parm name="fmt">The template for message string</parm>
// <return># of chars printed</return>
int
UlsLexUStr::vprint(const char *fmt, va_list args)
{
	int len;

	if (sysprn_fp != NULL) {
		len = UlsLexUStr::vfprintf(sysprn_fp, fmt, args);
	} else {
		len = 0;
	}

	return len;
}

int
UlsLexUStr::print(const char *fmt, ...)
{
	va_list	args;
	int len;

	va_start(args, fmt);
	len = vprint(fmt, args);
	va_end(args);

	return len;
}

// <brief>
//   This stores the formatted string to the 'buf'.
// </brief>
// <parm name="buf">The output buffer for the formatted string</parm>
// <parm name="bufsiz">The size of 'buf'</parm>
// <parm name="fmt">The template for message string</parm>
// <return># of chars filled except for '\0'</return>
int
UlsLexUStr::vsnprintf(char *buf, int bufsiz, const char *fmt, va_list args)
{
	int len;

	uls_lf_lock(str_nlf);
	len = __uls_lf_vsnprintf(buf, bufsiz, str_nlf, fmt, args);
	uls_lf_unlock(str_nlf);

	return len;
}

int
UlsLexUStr::snprintf(char *buf, int bufsiz, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vsnprintf(buf, bufsiz, fmt, args);
	va_end(args);

	return len;
}

// <brief>
//   This stores the formatted string to the 'buf' with its size unknown.
//   Recommended not to use this but snprintf().
// </brief>
// <parm name="buf">The output buffer for the formatted string</parm>
// <parm name="fmt">The template for message string</parm>
// <return># of chars filled</return>
int
UlsLexUStr::sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vsnprintf(buf, (~0U) >> 1, fmt, args);
	va_end(args);

	return len;
}

// <brief>
//   This stores the formatted string to the 'fp', pointer of FILE.
// </brief>
// <parm name="fp">An output port to be written</parm>
// <parm name="fmt">The template for message string</parm>
// <return># of chars written</return>
int
UlsLexUStr::vfprintf(FILE* fp, const char *fmt, va_list args)
{
	int len;
	len = uls_lf_vxprintf(fp, file_nlf, fmt, args);
	return len;
}

int
UlsLexUStr::fprintf(FILE* fp, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vfprintf(fp, fmt, args);
	va_end(args);

	return len;
}

// <brief>
//   This prints the formatted string to the 'stdout'.
// </brief>
// <parm name="fmt">The template for message string</parm>
// <return># of chars printed</return>
int
UlsLexUStr::printf(const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vfprintf(_uls_stdio_fp(1), fmt, args);
	va_end(args);

	return len;
}

// ========================================================================================
//
// UlsLexAWstr
//
// ========================================================================================
#ifdef __ULS_WINDOWS__
int
UlsLexAWStr::lf_init_convspec_amap(uls_lf_map_ptr_t lf_map, int flags)
{
	if (uls_lf_init_convspec_map(lf_map, flags) < 0)
		return -1;

	uls_lf_register_convspec(lf_map, "s", fmtproc_as);
	uls_lf_register_convspec(lf_map, "S", fmtproc_ws);

	return 0;
}

void
UlsLexAWStr::lf_deinit_convspec_amap(uls_lf_map_ptr_t lf_map)
{
	uls_lf_deinit_convspec_map(lf_map);
}
#endif

int
UlsLexAWStr::lf_init_convspec_wmap(uls_lf_map_ptr_t lf_map, int flags)
{
	if (uls_lf_init_convspec_map(lf_map, flags) < 0)
		return -1;

	uls_lf_register_convspec(lf_map, "s", fmtproc_ws);
	uls_lf_register_convspec(lf_map, "S", fmtproc_ws);

	return 0;
}

void
UlsLexAWStr::lf_deinit_convspec_wmap(uls_lf_map_ptr_t lf_map)
{
	uls_lf_deinit_convspec_map(lf_map);
}

void
UlsLexAWStr::initialize()
{
	if (ulslexwstr_inited == true) return;
#ifdef __ULS_WINDOWS__
	lf_init_convspec_amap(uls_ptr(ulscpp_convspec_amap), 0);
	uls_add_default_log_convspecs(uls_ptr(ulscpp_convspec_amap));
#endif
	lf_init_convspec_wmap(uls_ptr(ulscpp_convspec_wmap), 0);
	uls_add_default_log_convspecs(uls_ptr(ulscpp_convspec_wmap));

	ulslexwstr_inited = true;
}

void
UlsLexAWStr::finalize()
{
	if (ulslexwstr_inited == false) return;
#ifdef __ULS_WINDOWS__
	lf_deinit_convspec_amap(uls_ptr(ulscpp_convspec_amap));
#endif
	lf_deinit_convspec_wmap(uls_ptr(ulscpp_convspec_wmap));

	ulslexwstr_inited = false;
}

bool
UlsLexAWStr::initUlsLex_awstr(const char *ustr)
{
	UlsLexUStr::initUlsLex_ustr(ustr);

#ifdef __ULS_WINDOWS__
	str_alf = uls_lf_create(uls_ptr(ulscpp_convspec_amap), uls_lf_aputs_str);
	uls_lf_change_gdat(str_alf, getCore());

	file_alf = uls_lf_create(uls_ptr(ulscpp_convspec_amap), uls_lf_puts_aufile);
	uls_lf_change_gdat(file_alf, getCore());

	csz_init(uls_ptr(fmtstr1), 128);

	if (uls_init_log(&alogbase, NULL, getCore()) < 0) {
		err_log("fail to initialize uls");
		return false;
	}
#endif

	str_wlf = uls_lf_create(uls_ptr(ulscpp_convspec_wmap), uls_lf_wputs_str);
	uls_lf_change_gdat(str_wlf, getCore());

	file_wlf = uls_lf_create(uls_ptr(ulscpp_convspec_wmap), uls_lf_puts_aufile);
	uls_lf_change_gdat(file_wlf, getCore());

	csz_init(uls_ptr(fmtstr2), 128 * sizeof(wchar_t));

	if (uls_init_log(&wlogbase, NULL, getCore()) < 0) {
		err_log("fail to initialize uls");
		return false;
	}

	return true;
}

UlsLexAWStr::~UlsLexAWStr()
{
#ifdef __ULS_WINDOWS__
	uls_lf_destroy(file_alf);
	uls_lf_destroy(str_alf);
	uls_deinit_log(&alogbase);

	csz_deinit(uls_ptr(fmtstr1));
#endif
	uls_lf_destroy(file_wlf);
	uls_lf_destroy(str_wlf);
	uls_deinit_log(&wlogbase);

	csz_deinit(uls_ptr(fmtstr2));
}

// ========================================================================================
//
// UlsLexAWstr(AStr)
//
// ========================================================================================
#ifdef __ULS_WINDOWS__
UlsLexAWStr::UlsLexAWStr(const char *ulc_afile)
	: UlsLexUStr()
{
	const char *ustr;
	csz_str_t csz;

	csz_init(&csz, -1);

	if ((ustr = uls_astr2ustr(ulc_afile, -1, &csz)) == NULL) {
		throw invalid_argument("Invalid ulc-name in UlsLexAWStr!");
	}

	if (initUlsLex_awstr(ustr) == false) {
		string errmsg = ": file not proper!";
		throw invalid_argument(errmsg);
	}

	csz_deinit(&csz);
}

UlsLexAWStr::UlsLexAWStr(string& ulc_afile)
	: UlsLexAWStr(ulc_afile.c_str())
{
}

int
UlsLexAWStr::push_fp_astr(uls_lex_ptr_t uls, FILE *fp, int flags)
{
	uls_tempfile_ptr_t tmpfile_utf8;
	FILE *fp2;

	tmpfile_utf8 = uls_create_tempfile();

	if ((fp2 = cvt_ms_mbcs_fp(fp, tmpfile_utf8, flags)) == NULL) {
//		err_alog("%s: encoding error!", __func__);
		uls_destroy_tempfile(tmpfile_utf8);
		return -1;
	}

	if (uls_push_fp(uls, fp2, flags) < 0) {
//		err_alog("%s: can't prepare input!", __func__);
		uls_destroy_tempfile(tmpfile_utf8);
		return -1;
	}

	uls_register_ungrabber(uls, 0, ms_mbcs_tmpf_ungrabber, tmpfile_utf8);
	return 0;
}

bool
UlsLexAWStr::pushFile(const char *afilepath, int flags)
{
	uls_outparam_ptr_t parm;
	const char *ustr;
	uls_lex_ptr_t uls = getCore();
	FILE *fp;

	if (afilepath == NULL) {
//		err_alog("%s: Invalid parameter, filepath=%s!", __func__, filepath);
		return false;
	}

	if (flags < 0) {
		flags = getInputOpts();
	}

	_ULSCPP_ASTR2USTR(afilepath, ustr, 0);
	if ((fp = uls_fp_open(ustr, ULS_FIO_READ)) == NULL) {
//		err_alog("%s: Can't open '%s'!", __func__, filepath);
		return false;
	}

	if (push_fp_astr(uls, fp, flags) < 0) {
//		err_alog("%s: Error to uls_push_fp!", __func__);
		uls_fp_close(fp);
		return false;
	}

	setTag(afilepath);

	parm = uls_alloc_object(uls_outparam_t);
	parm->native_data = fp;
	uls_register_ungrabber(uls, 1, ms_mbcs_fp_ungrabber, parm);

	return true;
}

bool
UlsLexAWStr::setFile(const char *afilepath, int flags)
{
	uls_lex_ptr_t uls = getCore();

	if (flags < 0) {
		flags = getInputOpts();
	}

	uls_pop(uls);
	return pushFile(afilepath, flags);
}

bool
UlsLexAWStr::pushLine(const char *aline, int len, int flags)
{
	const char *ustr;

	if (flags < 0) {
		flags = getInputOpts();
	}

	_ULSCPP_ASTR2USTR(aline, ustr, 0);
	len = _ULSCPP_AUWCVT_LEN(0);

	return UlsLexUStr::pushLine(ustr, len, flags);
}

bool
UlsLexAWStr::setLine(const char *aline, int len, int flags)
{
	const char *ustr;

	if (flags < 0) {
		flags = getInputOpts();
	}

	_ULSCPP_ASTR2USTR(aline, ustr, 0);
	len = _ULSCPP_AUWCVT_LEN(0);

	return UlsLexUStr::setLine(ustr, len, flags);
}

void
UlsLexAWStr::getTag(string& afname)
{
	string tag_ustr;
	char *astr;

	UlsLexUStr::getTag(tag_ustr);
	_ULSCPP_USTR2ASTR(tag_ustr.c_str(), astr, 0);
	afname = astr;
}

void
UlsLexAWStr::setTag(const char *afname)
{
	const char *ustr;
	_ULSCPP_ASTR2USTR(afname, ustr, 0);
	UlsLexUStr::setTag(ustr);
}

void
UlsLexAWStr::getTokStr(string& alxm)
{
	const char *ustr = getTokUtf8Str();
	const char *astr;

	_ULSCPP_USTR2ASTR(ustr, astr, 0);
	alxm = astr;
}

void
UlsLexAWStr::setTok(int t, const string alxm)
{
	const char *ustr;

	_ULSCPP_ASTR2USTR(alxm.c_str(), ustr, 0);
	UlsLexUStr::setTokUStr(t, ustr);
}

void
UlsLexAWStr::dumpTok(const char *apfx, const char *asuff)
{
	const char *ustr0, *ustr1;

	if (apfx == NULL) {
		ustr0 = "\t";
	} else {
		_ULSCPP_ASTR2USTR(apfx, ustr0, 0);
	}

	if (asuff == NULL) {
		ustr1 = "\n";
	} else {
		_ULSCPP_ASTR2USTR(asuff, ustr1, 1);
	}

	UlsLexUStr::dumpTok(ustr0, ustr1);
}

// <brief>
//   unget a token <id,lxm>
// </brief>
bool
UlsLexAWStr::ungetTok(int tok_id, const char *alxm)
{
	const char *ustr;
	_ULSCPP_ASTR2USTR(alxm, ustr, 0);
	return ungetTokUtf8(tok_id, ustr);
}

// <brief>
//   Push a string into the input stream.
// </brief>
bool
UlsLexAWStr::ungetStr(const char *astr)
{
	const char *ustr;
	_ULSCPP_ASTR2USTR(astr, ustr, 0);
	return ungetStrUtf8(ustr);
}

void
UlsLexAWStr::changeLiteralAnalyzer(const char *apfx,
	uls_litstr_analyzer_t proc, void *data)
{
	const char *ustr;
	_ULSCPP_ASTR2USTR(apfx, ustr, 0);
	UlsLexUStr::changeLiteralAnalyzer(ustr, proc, data);
}

void
UlsLexAWStr::deleteLiteralAnalyzer(const char *apfx)
{
	const char *ustr;
	_ULSCPP_ASTR2USTR(apfx, ustr, 0);
	UlsLexUStr::deleteLiteralAnalyzer(ustr);
}

void
UlsLexAWStr::changeUldNames(const char *name, const char *name2,
	int tokid_valid, int tokid, const char *aliases)
{
	const char *ustr0, *ustr1, *ustr2;

	_ULSCPP_ASTR2USTR(name, ustr0, 0);

	if (name2 != NULL) {
		_ULSCPP_ASTR2USTR(name2, ustr1, 1);
	} else {
		ustr1 = NULL;
	}

	if (aliases != NULL) {
		_ULSCPP_ASTR2USTR(aliases, ustr2, 2);
	} else {
		ustr2 = NULL;
	}

	UlsLexUStr::changeUldNames(ustr0, ustr1, tokid_valid, tokid, ustr2);
}

int
UlsLexAWStr::lf_vxaprintf(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* afmt, va_list args)
{
	char* ustr;
	int len;

	if ((ustr = uls_astr2ustr(afmt, -1, uls_ptr(fmtstr1))) == NULL) {
		len = -1;
	}
	else {
		len = __uls_lf_vxprintf(x_dat, uls_lf, ustr, args);
	}

	return len;
}

int
UlsLexAWStr::lf_vsnaprintf(char* abuf, int abuf_siz, uls_lf_ptr_t uls_lf, const char* afmt, va_list args)
{
	uls_buf4str_t stdbuf;
	int alen;

	if (abuf_siz <= 1) {
		if (abuf_siz == 1) {
			abuf[0] = '\0';
		}
		return 0;
	}

	stdbuf.flags = 0;
	stdbuf.bufptr = stdbuf.buf = abuf;
	stdbuf.bufsiz = abuf_siz;

	alen = lf_vxaprintf(uls_ptr(stdbuf), uls_lf, afmt, args);
	return alen;
}

int
UlsLexAWStr::vsnprintf(char *abuf, int abufsiz, const char *afmt, va_list args)
{
	int len;

	uls_lf_lock(str_alf);
	len = lf_vsnaprintf(abuf, abufsiz, str_alf, afmt, args);
	uls_lf_unlock(str_alf);

	return len;
}

int
UlsLexAWStr::vfprintf(FILE* fp, const char *afmt, va_list args)
{
	int alen;

	uls_lf_lock(file_alf);
	alen = lf_vxaprintf(fp, file_alf, afmt, args);
	uls_lf_unlock(file_alf);

	return alen;
}

void
UlsLexAWStr::changeConvSpec(const char *percent_aname, uls_lf_convspec_t proc)
{
	const char *ustr;

	_ULSCPP_ASTR2USTR(percent_aname, ustr, 0);
	uls_lf_register_convspec(uls_ptr(ulscpp_convspec_amap), ustr, proc);
}

bool
UlsLexAWStr::openOutput(const char *out_afile)
{
	const char *ustr;

	_ULSCPP_ASTR2USTR(out_afile, ustr, 0);
	return openOutput_ustr(ustr);
}

int
UlsLexAWStr::vprint(const char *afmt, va_list args)
{
	FILE *fp = get_sysprn_fp();
	int len;

	if (fp != NULL) {
		len = UlsLexAWStr::vfprintf(fp, afmt, args);
	} else {
		len = 0;
	}

	return len;
}

int
UlsLexAWStr::lflog_vaprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *afmt, va_list args)
{
	uls_lf_delegate_t delegate;
	csz_str_t fmtbuf;
	char* ustr;
	int len;

	csz_init(uls_ptr(fmtbuf), 128);

	delegate.puts = uls_lf_puts_csz;

	uls_lf_lock(uls_lf);
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));

	if ((ustr = uls_astr2ustr(afmt, -1, uls_ptr(fmtbuf))) == NULL) {
		len = -1;
	}
	else {
		len = __uls_lf_vxprintf(csz, uls_lf, ustr, args);
	}

	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));
	uls_lf_unlock(uls_lf);

	csz_deinit(uls_ptr(fmtbuf));
	return len;
}

int
UlsLexAWStr::lflog_aprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *afmt, ...)
{
	va_list args;
	int len;

	va_start(args, afmt);
	len = lflog_vaprintf(csz, uls_lf, afmt, args);
	va_end(args);

	return len;
}

void
UlsLexAWStr::lflog_aflush(csz_str_ptr_t csz, uls_voidptr_t data, uls_lf_puts_t lf_puts)
{
	char *wrdptr;
	int wrdlen;

	wrdptr = csz_text(csz);
	wrdlen = csz_length(csz);

	lf_puts(data, wrdptr, wrdlen);
	lf_puts(data, "\n", 1);
	lf_puts(data, NULL, 0);

	csz_reset(csz);
}

void
UlsLexAWStr::lf_valog(uls_log_ptr_t log, const char *afmt, va_list args)
{
	uls_voidptr_t old_gdat;
	uls_lex_ptr_t uls;
	csz_str_t astr_buff;
	string atag;
	int len;

	if (log == nilptr || (uls = log->uls) == nilptr) {
		return;
	}

	getTag(atag);

	csz_init(uls_ptr(astr_buff), 128);

	uls_log_lock(log);
	old_gdat = uls_lf_change_gdat(log->lf, uls);

	len = lflog_aprintf(uls_ptr(astr_buff), log->lf, "[ULS] [%s:%d] ",
		atag.c_str(), uls_get_lineno(uls));

	len += lflog_vaprintf(uls_ptr(astr_buff), log->lf, afmt, args);

	lflog_aflush(uls_ptr(astr_buff), log->log_port, log->log_puts);

	uls_lf_change_gdat(log->lf, old_gdat);
	uls_log_unlock(log);

	csz_deinit(uls_ptr(astr_buff));
}

void
UlsLexAWStr::vlog(int loglvl, const char *afmt, va_list args)
{
	if (alogbase.log_mask & ULS_LOGLEVEL_FLAG(loglvl)) {
		uls::lockMutex(&syserr_g_mtx);
		lf_valog(&alogbase, afmt, args);
		uls::unlockMutex(&syserr_g_mtx);
	}
}

#endif // __ULS_WINDOWS__

// ========================================================================================
//
// UlsLexAWstr(WStr)
//
// ========================================================================================

UlsLexAWStr::UlsLexAWStr(const wchar_t *ulc_wfile)
	: UlsLexUStr()
{
	const char *ustr;
	csz_str_t csz;

	csz_init(&csz, -1);
	if ((ustr = uls_wstr2ustr(ulc_wfile, -1, &csz)) == NULL) {
		throw invalid_argument("Invalid ulc-name in UlsLexAWStr!");
	}

	if (initUlsLex_awstr(ustr) == false) {
		string errmsg = ": file not proper!";
		throw invalid_argument(errmsg);
	}

	csz_deinit(&csz);
}

UlsLexAWStr::UlsLexAWStr(wstring& ulc_wfile)
	: UlsLexAWStr(ulc_wfile.c_str())
{
}

#ifdef __ULS_WINDOWS__
int
UlsLexAWStr::push_fp_wstr(uls_lex_ptr_t uls, FILE *fp, int flags)
{
	uls_tempfile_ptr_t tmpfile_utf8;
	FILE *fp2;

	tmpfile_utf8 = uls_create_tempfile();

	if ((fp2 = cvt_ms_mbcs_fp(fp, tmpfile_utf8, flags)) == NULL) {
//		err_wlog("%s: encoding error!", __func__);
		uls_destroy_tempfile(tmpfile_utf8);
		return -1;
	}

	if (uls_push_fp(uls, fp2, flags) < 0) {
//		err_wlog("%s: can't prepare input!", __func__);
		uls_destroy_tempfile(tmpfile_utf8);
		return -1;
	}

	uls_register_ungrabber(uls, 0, ms_mbcs_tmpf_ungrabber, tmpfile_utf8);
	return 0;
}
#endif

bool
UlsLexAWStr::pushFile(const wchar_t *wfilepath, int flags)
{
	bool stat = true;
	const char *ustr;
#ifdef __ULS_WINDOWS__
	uls_outparam_ptr_t parm;
	uls_lex_ptr_t uls = getCore();
	FILE *fp;
#endif

	if (wfilepath == NULL) {
//		err_wlog(L"%hs: Invalid parameter, filepath=%s!", __func__, wfilepath);
		return false;
	}

	if (flags < 0) {
		flags = getInputOpts();
	}

	_ULSCPP_WSTR2USTR(wfilepath, ustr, 0);

#ifdef __ULS_WINDOWS__
	if ((fp = uls_fp_open(ustr, ULS_FIO_READ)) == NULL) {
//		err_wlog(L"%hs: Can't open '%s'!", __func__, wfilepath);
		return false;
	}

	if (push_fp_wstr(uls, fp, flags) < 0) {
//		err_wlog(L"%hs: Error to uls_push_fp!", __func__);
		uls_fp_close(fp);
		return false;
	}

	setTag(wfilepath);

	parm = uls_alloc_object(uls_outparam_t);
	parm->native_data = fp;
	uls_register_ungrabber(uls, 1, fp_ungrabber_wstr, parm);
#else
	stat = UlsLexUStr::pushFile(ustr, flags);
#endif

	return stat;
}

bool
UlsLexAWStr::pushFile(const wstring& filepath, int flags)
{
	return pushFile(filepath.c_str(), flags);
}

bool
UlsLexAWStr::setFile(const wchar_t *wfilepath, int flags)
{
	uls_lex_ptr_t uls = getCore();

	if (flags < 0) {
		flags = getInputOpts();
	}

	uls_pop(uls);
	return pushFile(wfilepath, flags);
}

bool
UlsLexAWStr::setFile(const wstring& filepath, int flags)
{
	return setFile(filepath.c_str(), flags);
}

bool
UlsLexAWStr::pushLine(const wchar_t *wline, int len, int flags)
{
	const char *ustr;

	if (flags < 0) {
		flags = getInputOpts();
	}

	_ULSCPP_WSTR2USTR(wline, ustr, 0);
	len = _ULSCPP_AUWCVT_LEN(0);

	return UlsLexUStr::pushLine(ustr, len, flags);
}

bool
UlsLexAWStr::pushLine(const wstring& line, int flags)
{
	return pushLine(line.c_str(), (int) line.length(), flags);
}

bool
UlsLexAWStr::setLine(const wchar_t *wline, int len, int flags)
{
	const char *ustr;

	if (flags < 0) {
		flags = getInputOpts();
	}

	_ULSCPP_WSTR2USTR(wline, ustr, 0);
	len = _ULSCPP_AUWCVT_LEN(0);

	return UlsLexUStr::setLine(ustr, len, flags);
}

bool
UlsLexAWStr::setLine(const wstring& line, int flags)
{
	return setLine(line.c_str(), (int) line.length(), flags);
}

void
UlsLexAWStr::getTag(wstring& wfname)
{
	string tag_ustr;
	wchar_t *wstr;

	UlsLexUStr::getTag(tag_ustr);
	_ULSCPP_USTR2WSTR(tag_ustr.c_str(), wstr, 0);
	wfname = wstr;
}

void
UlsLexAWStr::setTag(const wchar_t *wfname)
{
	const char *ustr;
	_ULSCPP_WSTR2USTR(wfname, ustr, 0);
	UlsLexUStr::setTag(ustr);
}

void
UlsLexAWStr::setTag(const wstring& fname)
{
	setTag(fname.c_str());
}

void
UlsLexAWStr::getTokStr(wstring& wlxm)
{
	const char *ustr = getTokUtf8Str();
	const wchar_t *wstr;

	_ULSCPP_USTR2WSTR(ustr, wstr, 0);
	wlxm = wstr;
}

void
UlsLexAWStr::setTok(int t, const wstring wlxm)
{
	const char *ustr;

	_ULSCPP_WSTR2USTR(wlxm.c_str(), ustr, 0);
	UlsLexUStr::setTokUStr(t, ustr);
}

void
UlsLexAWStr::dumpTok(const wchar_t *wpfx, const wchar_t *wsuff)
{
	const char *ustr0, *ustr1;

	if (wpfx == NULL) {
		ustr0 = "\t";
	} else {
		_ULSCPP_WSTR2USTR(wpfx, ustr0, 0);
	}

	if (wsuff == NULL) {
		ustr1 = "\n";
	} else {
		_ULSCPP_WSTR2USTR(wsuff, ustr1, 1);
	}

	UlsLexUStr::dumpTok(ustr0, ustr1);
}

bool
UlsLexAWStr::ungetTok(int tok_id, const wchar_t *wlxm)
{
	const char *ustr;
	_ULSCPP_WSTR2USTR(wlxm, ustr, 0);
	return ungetTokUtf8(tok_id, ustr);
}

bool
UlsLexAWStr::ungetTok(int tok_id, const wstring& wlxm)
{
	return ungetTok(tok_id, wlxm.c_str());
}

// <brief>
//   Push a string into the input stream.
// </brief>
bool
UlsLexAWStr::ungetStr(const wchar_t *wstr)
{
	const char *ustr;
	_ULSCPP_WSTR2USTR(wstr, ustr, 0);
	return ungetStrUtf8(ustr);
}

bool
UlsLexAWStr::ungetStr(const wstring& wstr)
{
	return ungetStr(wstr.c_str());
}

void
UlsLexAWStr::changeLiteralAnalyzer(const wchar_t *wpfx, uls_litstr_analyzer_t proc, void* data)
{
	const char *ustr;
	_ULSCPP_WSTR2USTR(wpfx, ustr, 0);
	UlsLexUStr::changeLiteralAnalyzer(ustr, proc, data);
}

void
UlsLexAWStr::changeLiteralAnalyzer(wstring& pfx,
				uls_litstr_analyzer_t proc, void *data)
{
	changeLiteralAnalyzer(pfx.c_str(), proc, data);
}

void
UlsLexAWStr::deleteLiteralAnalyzer(const wchar_t *wpfx)
{
	const char *ustr;
	_ULSCPP_WSTR2USTR(wpfx, ustr, 0);
	UlsLexUStr::deleteLiteralAnalyzer(ustr);
}

void
UlsLexAWStr::deleteLiteralAnalyzer(wstring& pfx)
{
	deleteLiteralAnalyzer(pfx.c_str());
}

void
UlsLexAWStr::changeUldNames(const wchar_t *name, const wchar_t *name2,
	int tokid_valid, int tokid, const wchar_t *aliases)
{
	const char *ustr0, *ustr1, *ustr2;

	_ULSCPP_WSTR2USTR(name, ustr0, 0);

	if (name2 != NULL) {
		_ULSCPP_WSTR2USTR(name2, ustr1, 1);
	} else {
		ustr1 = NULL;
	}

	if (aliases != NULL) {
		_ULSCPP_WSTR2USTR(aliases, ustr2, 2);
	} else {
		ustr2 = NULL;
	}

	UlsLexUStr::changeUldNames(ustr0, ustr1, tokid_valid, tokid, ustr2);
}

void
UlsLexAWStr::changeUldNames(wstring& name, wstring& name2,
	int tokid_valid, int tokid, wstring& aliases)
{
	changeUldNames(name.c_str(), name2.c_str(),tokid_valid, tokid, aliases.c_str());
}

int
UlsLexAWStr::lf_vxwprintf(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args)
{
	char *ustr;
	int len;

	if ((ustr = uls_wstr2ustr(wfmt, -1, uls_ptr(fmtstr2))) == NULL) {
		len = -1;
	} else {
		len = __uls_lf_vxprintf(x_dat, uls_lf, ustr, args);
	}

	return len;
}

int
UlsLexAWStr::lf_vsnwprintf(wchar_t *wbuf, int wbuf_siz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args)
{
	uls_buf4wstr_t stdwbuf;
	int len;

	if (wbuf_siz <= 1) {
		if (wbuf_siz == 1) {
			wbuf[0] = L'\0';
		}
		return 0;
	}

	stdwbuf.flags = 0;
	stdwbuf.wbufptr = stdwbuf.wbuf = wbuf;
	stdwbuf.wbufsiz = wbuf_siz;

	len = lf_vxwprintf(uls_ptr(stdwbuf), uls_lf, wfmt, args);
	if (len > 0) len /= sizeof(wchar_t);
	return len;
}

int
UlsLexAWStr::vsnprintf(wchar_t *wbuf, int bufsiz, const wchar_t *wfmt, va_list args)
{
	int len;

	uls_lf_lock(str_wlf);
	len = lf_vsnwprintf(wbuf, bufsiz, str_wlf, wfmt, args);
	uls_lf_unlock(str_wlf);

	return len;
}

int
UlsLexAWStr::snprintf(wchar_t *wbuf, int bufsiz, const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = vsnprintf(wbuf, bufsiz, wfmt, args);
	va_end(args);

	return len;
}

int
UlsLexAWStr::sprintf(wchar_t *wbuf, const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = vsnprintf(wbuf, (~0U) >> 1, wfmt, args);
	va_end(args);

	return len;
}

int
UlsLexAWStr::vfprintf(FILE* fp, const wchar_t *wfmt, va_list args)
{
	int aulen;

	uls_lf_lock(file_wlf);
	aulen = lf_vxwprintf(fp, file_wlf, wfmt, args);
	uls_lf_unlock(file_wlf);

	return aulen;
}

int
UlsLexAWStr::fprintf(FILE* fp, const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = vfprintf(fp, wfmt, args);
	va_end(args);

	return len;
}

int
UlsLexAWStr::printf(const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = vfprintf(_uls_stdio_fp(1), wfmt, args);
	va_end(args);

	return len;
}

void
UlsLexAWStr::changeConvSpec(const wchar_t *percent_wname, uls_lf_convspec_t proc)
{
	const char *ustr;

	_ULSCPP_WSTR2USTR(percent_wname, ustr, 0);
	uls_lf_register_convspec(uls_ptr(ulscpp_convspec_wmap), ustr, proc);
}

void
UlsLexAWStr::changeConvSpec(const wstring& percent_wname, uls_lf_convspec_t proc)
{
	changeConvSpec(percent_wname.c_str(), proc);
}

bool
UlsLexAWStr::openOutput(const wchar_t *out_wfile)
{
	const char *ustr;

	_ULSCPP_WSTR2USTR(out_wfile, ustr, 0);
	return openOutput_ustr(ustr);
}

bool
UlsLexAWStr::openOutput(const wstring& out_wfile)
{
	return openOutput(out_wfile.c_str());
}

int
UlsLexAWStr::vprint(const wchar_t *wfmt, va_list args)
{
	FILE *fp = get_sysprn_fp();
	int len;

	if (fp != NULL) {
		len = UlsLexAWStr::vfprintf(fp, wfmt, args);
	} else {
		len = 0;
	}

	return len;
}

int
UlsLexAWStr::print(const wchar_t *fmt, ...)
{
	va_list	args;
	int len;

	va_start(args, fmt);
	len = vprint(fmt, args);
	va_end(args);

	return len;
}

int
UlsLexAWStr::lflog_vwprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args)
{
	uls_lf_delegate_t delegate;
	csz_str_t fmtbuf;
	char* ustr;
	int len;

	csz_init(uls_ptr(fmtbuf), 128 * sizeof(wchar_t));

	delegate.puts = uls_lf_puts_csz;

	uls_lf_lock(uls_lf);
	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));

	if ((ustr = uls_wstr2ustr(wfmt, -1, uls_ptr(fmtbuf))) == NULL) {
		len = -1;
	} else {
		len = __uls_lf_vxprintf(csz, uls_lf, ustr, args);
	}

	__uls_lf_change_puts(uls_lf, uls_ptr(delegate));
	uls_lf_unlock(uls_lf);

	csz_deinit(uls_ptr(fmtbuf));
	return len;
}

int
UlsLexAWStr::lflog_wprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...)
{
	va_list args;
	int len;

	va_start(args, wfmt);
	len = lflog_vwprintf(csz, uls_lf, wfmt, args);
	va_end(args);

	return len;
}

void
UlsLexAWStr::lflog_wflush(csz_str_ptr_t csz, uls_voidptr_t data, uls_lf_puts_t lf_puts)
{
	char *wrdptr;
	int wrdlen;

	wrdptr = csz_text(csz);
	wrdlen = csz_length(csz);

	lf_puts(data, wrdptr, wrdlen);
	lf_puts(data, "\n", 1);
	lf_puts(data, NULL, 0);

	csz_reset(csz);
}

void
UlsLexAWStr::lf_vwlog(uls_log_ptr_t log, const wchar_t *wfmt, va_list args)
{
	uls_voidptr_t old_gdat;
	uls_lex_ptr_t uls;
	csz_str_t wstr_buff;
	wstring wtag;
	int len;

	if (log == nilptr || (uls = log->uls) == nilptr) {
		return;
	}

	getTag(wtag);

	csz_init(uls_ptr(wstr_buff), 128*sizeof(wchar_t));

	uls_log_lock(log);
	old_gdat = uls_lf_change_gdat(log->lf, uls);

	len = lflog_wprintf(uls_ptr(wstr_buff), log->lf, L"[ULS] [%s:%d] ",
		wtag.c_str(), uls_get_lineno(uls));
	len += lflog_vwprintf(uls_ptr(wstr_buff), log->lf, wfmt, args);

	lflog_wflush(uls_ptr(wstr_buff), log->log_port, log->log_puts);

	uls_lf_change_gdat(log->lf, old_gdat);
	uls_log_unlock(log);

	csz_deinit(uls_ptr(wstr_buff));
}

void
UlsLexAWStr::vlog(int loglvl, const wchar_t *wfmt, va_list args)
{
	if (wlogbase.log_mask & ULS_LOGLEVEL_FLAG(loglvl)) {
		uls::lockMutex(&syserr_g_mtx);
		lf_vwlog(&wlogbase, wfmt, args);
		uls::unlockMutex(&syserr_g_mtx);
	}
}

void
UlsLexAWStr::log(const wchar_t *wfmt, ...)
{
	va_list	args;

	va_start(args, wfmt);
	vlog(ULS_LOG_WARN, wfmt, args);
	va_end(args);
}

#if defined(__ULS_WINDOWS__) && !defined(ULS_DOTNET)
[[noreturn]]
#endif
void
UlsLexAWStr::panic(const wchar_t *wfmt, ...)
{
	va_list	args;

	va_start(args, wfmt);
	vlog(ULS_LOG_CRIT, wfmt, args);
	va_end(args);

	exit(1);
}
