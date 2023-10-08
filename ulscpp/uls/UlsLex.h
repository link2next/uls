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
  <file> UlsLex.h </file>
  <brief>
 	The wrapper class of ulslex_t, the main structure of ULS c-library, libuls.*
    This file is part of ULS, Unified Lexical Scheme.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, 2011.
  </author>
*/

#pragma once

#include <string>
#include <map>

#include <uls/uls_lex.h>
#include <uls/uld_conf.h>
#include <uls/uls_log.h>
#include <stdarg.h>

namespace uls {
	typedef uls_mutex_t MutexType;

	// <brief>
	// This Initializes the mutex 'mtx' which is to be used by locked/unlocked method.
	// The mechanism's Implemented by the pthread library in Linux and by Win32 API in Windows.
	// </brief>
	// <parm name="mtx">Mutex object</parm>
	// <return>none</return>
	ULSCPP_DLL_EXTERN void initMutex(MutexType mtx);

	// <brief>
	// Deinitialize the 'mtx'. After calling this, 'mtx' shouldn't be used.
	// </brief>
	// <parm name="mtx">The mutex object</parm>
	// <return>none</return>
	ULSCPP_DLL_EXTERN void deinitMutex(MutexType mtx);

	// <brief>
	// The locking API to grab the lock 'mtx'.
	// </brief>
	// <parm name="mtx">The mutex object initialized by uls::initMutex</parm>
	// <return>none</return>
	ULSCPP_DLL_EXTERN void lockMutex(MutexType mtx);

	// <brief>
	// The unlocking API to ungrab the lock 'mtx'.
	// </brief>
	// <parm name="mtx">The mutex object initialized by uls::initMutex</parm>
	// <return>none</return>
	ULSCPP_DLL_EXTERN void unlockMutex(MutexType mtx);

	// <brief>
	// Creates a file descriptor 'fd' and return it.
	// The 'fd' is created by the system call open().
	// The file is used to write content sequentially.
	// </brief>
	// <parm name="fpath">the file path to be created</parm>
	// <return>a file descriptor</return>
	ULSCPP_DLL_EXTERN int create_fd_wronly(const std::string& fpath);

	// <brief>
	// Opens file and return the file descriptor 'fd'.
	// The file content is randomly read and read only.
	// </brief>
	// <parm name="fpath">the file path to be read</parm>
	// <return>a file descriptor</return>
	ULSCPP_DLL_EXTERN int open_fd_rdonly(const std::string& fpath);

	// <brief>
	// Closes the 'fd' opened by open_fd_rdonly() or create_fd_wronly().
	// </brief>
	// <parm name="fd">file decriptor</parm>
	// <return>none</return>
	ULSCPP_DLL_EXTERN void close_fd(int fd);

	// <brief>
	// This procedure lists the search directories for ulc file, which is suffixed by 'ulc'.
	// uls_create() or uls_init() will search the directories for ulc file in same order as this dumpSearchPathOfUlc().
	// The order of search-directories is affected by whether or not the paramenter 'confname' is suffixed by '.ulc'.
	// If the parameter is suffixed by '.ulc' or has '..' or '.', it's recognized as file path.
	// But if not suffixed by 'ulc' like 'sample', 'dir/sample',
	//     the procedure will search the ulc repository preferentially.
	// </brief>
	// <parm name="confname">lexcial configuration</parm>
	// <return>none</return>
	ULSCPP_DLL_EXTERN void dumpSearchPathOfUlc(const std::string& confname);
	ULSCPP_DLL_EXTERN void dumpSearchPathOfUlc(const std::wstring& confname);

	// <brief>
	// This will list the serach paths, preferentially the location of ulc repository.
	// </brief>
	// <return>none</return>
	ULSCPP_DLL_EXTERN void listUlcSearchPaths(void);

	ULSCPP_DLL_EXTERN void initialize_ulscpp(void);
	ULSCPP_DLL_EXTERN void finalize_ulscpp(void);

	namespace crux {
		class UlsIStream;
		class UlsAuw;

		// <brief>
		// This calls checks if the string 'lxm' is interger or floating-point number, or zero.
		// These are static methods of uls::crux.
		// It's recommended to use the class methods in uls::crux::UlsLex.
		// </brief>
		// <parm name="lxm">A string</parm>
		// <return>bool</return>
		ULSCPP_DLL_EXTERN bool isLexemeZero(const std::string& lxm);
		ULSCPP_DLL_EXTERN bool isLexemeZero(const std::wstring& lxm);

		ULSCPP_DLL_EXTERN bool isLexemeInt(const std::string& lxm);
		ULSCPP_DLL_EXTERN bool isLexemeInt(const std::wstring& lxm);

		ULSCPP_DLL_EXTERN bool isLexemeReal(const std::string& lxm);
		ULSCPP_DLL_EXTERN bool isLexemeReal(const std::wstring& lxm);

		// <brief>
		// These convert the string 'lxm' to the value of int or double.
		// It's recommended to use the class methods lexemeAsInt(), lexemeAsDouble() in uls::crux::UlsLex.
		// </brief>
		// <parm name="lxm">A string</parm>
		// <return>the converted values from the string.</return>
		ULSCPP_DLL_EXTERN int LexemeAsInt(const std::string& lxm);
		ULSCPP_DLL_EXTERN int LexemeAsInt(const std::wstring& lxm);

		ULSCPP_DLL_EXTERN double LexemeAsDouble(const std::string& lxm);
		ULSCPP_DLL_EXTERN double LexemeAsDouble(const std::wstring& lxm);

		class ULSCPP_DLL_EXTERN IPrintf {
		public:
			// <brief>
			// This procecure stores the formatted string to the 'buf'.
			// </brief>
			// <parm name="buf">The output buffer for the formatted string</parm>
			// <parm name="bufsiz">The size of 'buf'</parm>
			// <parm name="fmt">The template string</parm>
			// <return># of bytes filled</return>
			virtual int vsnprintf(char* buf, int bufsiz, const char *fmt, va_list args) = 0;

			// <brief>
			// This procedure stores the formatted string to the standard FILE pointed by 'fp'.
			// </brief>
			// <parm name="fp">The file pointer to be wrriten</parm>
			// <parm name="fmt">The template string for 'args'</parm>
			// <return># of bytes written</return>
			virtual int vfprintf(FILE* fp, const char *fmt, va_list args) = 0;

			// <brief>
			// This procedure will change the associated procedure with 'percent_name', a converion specification.
			// </brief>
			// <parm name="percent_name">A conversion specification string without the percent character '%'.
			//  For example, if you intend to process the '%Fz' with procdure a_proc(),
			//      call this procedure like 'changeConvSpec("Fz", a_proc)'.
			// </parm>
			// <parm name="proc">The user-provided procedure for processing '%percent_name'</parm>
			// <return>none</return>
			virtual void changeConvSpec(const char* percent_name, uls_lf_convspec_t proc) = 0;

			// <brief>
			// You can use this method to change the default output interface.
			// </brief>
			// <parm name="puts_proc">A newly output interface</parm>
			// <return>none</return>
			virtual void changePuts(void *xdat, uls_lf_puts_t puts_proc) = 0;
		};

		class ULSCPP_DLL_EXTERN ILoggable {
		public:
			// <brief>
			// This procedure logs formatted messages if the 'loglvl' is set in the object.
			// The possible loglvl are ULS_LOG_EMERG, ULS_LOG_ALERT, ULS_LOG_CRIT, ...
			// </brief>
			// <parm name="loglvl">This message will be printed if 'loglvl' is set.</parm>
			// <return>none</return>
			virtual void log(int loglvl, const char* fmt, ...) = 0;

			// <brief>
			// This Logs a formatted message.
			// No need to append '\n' to the end of line 'fmt' as log() will automatically append it.
			// You can use %t %w to print the current token information and its location.
			// No need argument for %t and %w.
			// </brief>
			// <parm name="fmt">format string</parm>
			// <return>void</return>
			virtual void log(const char* fmt, ...) = 0;

			// <brief>
			// This Logs a formatted message and the program will be aborted.
			// No need to append '\n' to the end of line 'fmt' as panic() will append it.
			// </brief>
			// <parm name="fmt">format string</parm>
			// <return>none</return>
			virtual void panic(const char* fmt, ...) = 0;
		};

		class ULSCPP_DLL_EXTERN IUlsLex {
		public:
			// <brief>
			// Proceeds the cursor of input to get the next token.
			// Returns the token number, which is also stored internally.
			// The associated token string is obtained by calling getTokStr().
			// </brief>
			// <return>the token number</return>
			virtual int getTok(void) = 0;

			// <brief>
			// Call ungetTok if you want to get the current token again by next calling getTok().
			// </brief>
			// <return>none</return>
			virtual void ungetTok(void) = 0;

			// <brief>
			// Returns the current token string internally stored by getTok().
			// This value is only valid until the next call of getTok().
			// </brief>
			// <return>the alias of string stored internally</return>
			virtual void getTokStr(std::string **pp_lxm) = 0;

			// <brief>
			// Returns the current token id in the object by getTok().
			// It should be the same as the return value of getTok().
			// This value is only valid until the next call of getTok().
			// </brief>
			// <return>token number</return>
			virtual int getTokNum(void) = 0;

			// <brief>
			// Peeks what is the next character in the input.
			// getCh() will get the character and advance the cursor of input.
			// </brief>
			// <return>The next character</return>
			virtual uls_wch_t peekCh(bool* isQuote) = 0;
			virtual uls_wch_t getCh(bool* isQuote) = 0;

			// <brief>
			// This will push back the character wch in order to get it again
			//    when peekCh() or getCh() is called.
			// getCh() will get the character and advance the cursor of input.
			// </brief>
			// <return>none</return>
			virtual void ungetCh(uls_wch_t wch) = 0;
		};

		// <brief>
		// This is the major class of ulscpp library.
		// You can instantiate this class with a parameter, lexical configuration(*.ulc),
		//    to obtain an object for lexical analysis.
		// </brief>
		class ULSCPP_DLL_EXTERN UlsLex :
			public uls::crux::IUlsLex, public uls::crux::IPrintf, public uls::crux::ILoggable {
			static const int N_CHARS_LOGBUF = 128;
			static const int N_TMPLVAL_SIZE = 64;

			uls_lex_t lex;
			uld_names_map_t *names_map;
			int input_flags;

			// <brief>
			// 'lxm_id' is the internal data for the current token id.
			// It's updated by calling the methods 'getTok()' or 'next()'.
			// </brief>
			int lxm_id;

			// <brief>
			// 'lxm_str' is the internal data for the lexeme of the current token.
			// It's only valid until the next call of 'getTok()' or 'next()'.
			// </brief>
			std::string lxm_nstr;
			std::wstring lxm_wstr;

			std::map<int,void*> *extra_tokdefs;

			std::string *FileNameBuf;

			uls_mutex_struct_t syserr_g_mtx;
			uls_log_t logbase;

			uls_lf_t  *str_nlf;
			uls_lf_t  *file_nlf;
			uls_lf_t  *prn_nlf;

			uls_lf_t  *str_wlf;
			uls_lf_t  *file_wlf;
			uls_lf_t  *prn_wlf;

			uls_mutex_struct_t sysprn_g_mtx;
			int sysprn_opened;

			UlsAuw    *auwcvt;

			static bool ulscpp_inited;
			static uls_lf_map_t *ulscpp_convspec_nmap;
			static uls_lf_map_t *ulscpp_convspec_wmap;

		protected:
			// <brief>
			// Emits the formatted message by the string 'fmt' with 'args'
			// </brief>
			// <parm name="fmt">The format string, a template for formatting</parm>
			// <parm name="args">The list of args</parm>
			// <return>none</return>
			void vlog(const char* fmt, va_list args);
			void vwlog(const wchar_t* fmt, va_list args);

			// <brief>
			// This checks whether the user provided extra token definition exists
			//    in the internal dictionary of the lexical object.
			// </brief>
			// <parm name="t">token number</parm>
			// <return>true/false</return>
			bool existTokdefInHashMap(int t);

			// <brief>
			// A internal procedure to sync the state of object.
			// </brief>
			void update_token_lex(void);

			// <brief>
			// A internal procedure to set the current token forcibly.
			// </brief>
			void set_token(int t, const std::string& lxm);
			void set_token(int t, const std::wstring& lxm);

			// <brief>
			// changeUldNames
			// </brief>
			int prepareUldMap();
			bool finishUldMap();
			void changeUldNames(const char *name, const char *name2, int tokid_valid, int tokid, const char *aliases);
			void changeUldNames(const wchar_t *name, const wchar_t *name2, int tokid_valid, int tokid, const wchar_t *aliases);

			// <brief>
			// setTag
			// </brief>
			void setTag_ustr(const char *fname);
			void openOutput_ustr(const char* out_file, uls_lf_puts_t puts_proc);

		public:
			static void initialize();
			static void finalize();

			// <brief>
			// These flags affects the input stream process and is set in calling pushInput().
			// 1. WantEOF: At the end of file, getTok() will give you the reserved token EOF.
			// 2. DoDup: This will copy the input source for character string or file descriptor if possible.
			// </brief>
			enum InputOpts {
				WantEOF=0, DoDup
			};

			// <brief>
			// This will convert the 'fl' to get the flag to interface with 'libuls.so'.
			// </brief>
			int get_uls_flags(InputOpts fl);

			// <brief>
			// These procedures is to maninpulate the flags described above.
			// setInputOpts() sets the the intenal flag 'input_flags' to the flag 'fl'
			// clearInputOpts() clears the flag 'fl' in the the intenal flag 'input_flags'.
			// getInputOpts() will just return the intenal flag 'input_flags'.
			// </brief>
			void setInputOpt(InputOpts fl);
			void clearInputOpt(InputOpts fl);
			int getInputOpts(void);
			void resetInputOpts(void);

			// <brief>
			// This is a constructor that creates an object for lexical analysis.
			// The 'ulc_file' is a name of language specification in the ulc repository or
			//   simply the file path of an ulc file.
			// To see the available ulc names, use the -q-option of the command 'ulc2class'.
			// </brief>
			// <parm name="ulc_file">The name/path of the lexical configuration.</parm>
			// <return>none</return>
			bool initUlsLex_ustr(const char *ulc_file);

			UlsLex(const char *ulc_file);
			UlsLex(const wchar_t *ulc_wfile);

			UlsLex(std::string& ulc_file);
			UlsLex(std::wstring& ulc_file);

			// <brief>
			// The destructor of UlsLex.
			// </brief>
			// <return>none</return>
			virtual ~UlsLex();

			// <brief>
			// The class 'UlsLex' is wrapper class of C library 'libuls.so'.
			// This returns the pointer of embedded structure of the library.
			// </brief>
			// <return>The pointer of 'uls_lex_t'</return>
			uls_lex_t* getLexCore(void) { return &lex; }

			// <brief>
			// Checks if the current position in input stream is at the end of input.
			// </brief>
			bool isEOI;

			// <brief>
			// Checks if the current position in input stream is at the end of file.
			// There may be a lots of (nested) files in the internal input stack of ULS object.
			// A streaming seesion will be ended with EOI, end of input.
			// </brief>
			bool isEOF;

			// <brief>
			// The list of token number of reserved tokens
			// The reserved token can be renamed by user in the ulc configuration.
			// So the reserver tokens varies according to each language spec.
			// </brief>
			int toknum_EOI, toknum_EOF, toknum_ERR;
			int toknum_NONE, toknum_ID, toknum_NUMBER, toknum_TMPL;

			// <brief>
			// This attribute is the location of the current token in input.
			// It can be read directly as declared as public member.
			// This can be used with 'FileName' described below.
			// </brief>
			int LineNum;

			// <brief>
			// This will update the value 'FileName' with the parameter 'fname'
			// </brief>
			// <parm name="fname">The new string value to be updated</parm>
			// <return>none</return>
			void setTag(const std::string& fname);
			void setTag(const std::wstring& fname);

			void setFileName(const std::string& fname);
			void setFileName(const std::wstring& wfname);

			void getTag(std::string& fname);
			void getTag(std::wstring& fname);

			void getFileName(std::string& fname);
			void getFileName(std::wstring& fname);

			// <brief>
			// The field 'LineNum' is automatically updated by calling getTok() but
			//  if you want to change it forcibly use this method.
			// </brief>
			// <parm name="lineno">The new value of 'LineNum' to be updated</parm>
			// <return>none</return>
			void setLineNum(int lineno);

			// <brief>
			// Use this method to add some lines to the current line number 'LineNum' forcibly.
			// If the resultant line number is negative the 'LineNum' won't be updated.
			// </brief>
			// <parm name="amount">The amount of lines to be added. It may be negative</parm>
			// <return>none</return>
			void addLineNum(int amount);

			// <brief>
			// Delete the literal-string analyzer starting with 'pfx' if it's defined in the object.
			// The 'literal-string analyzer' is to recognize the quote string starting with 'pfx'.
			// </brief>
			// <parm name="pfx">The literal string analyzer of which the quote type is started with 'pfx'.</parm>
			// <return>none</return>
			void deleteLiteralAnalyzer(const std::string& pfx);
			void deleteLiteralAnalyzer(const std::wstring& pfx);

			// <brief>
			// Changes the literal-string analyzer to 'proc'.
			// The 'proc' will be applied to the quote type starting with 'pfx'.
			// </brief>
			// <parm name="pfx">The prefix of literal string that will be processed by 'proc'</parm>
			// <return>void</return>
			void changeLiteralAnalyzer(const std::string pfx, uls_litstr_analyzer_t proc, void *data);
			void changeLiteralAnalyzer(const std::wstring pfx, uls_litstr_analyzer_t proc, void *data);

			// <brief>
			// This method will push an input string 'istr' on the top of the internal input stack.
			// Then the getTok() method can be used to get a token from the input.
			// </brief>
			// <parm name="istr">input stream object!
			//   You can create the input object from text file, literal-string, or uls-file.</parm>
			// <return>true/false</return>
			bool pushInput(UlsIStream& istr, int flags=-1);

			// <brief>
			// This makes file descriptor 'fd' prepared on the internal stack.
			// Processing of the previous input is postponed until the completion of 'fd'.
			// </brief>
			// <parm name="fd">the file descriptor of input file</parm>
			// <return>true/false</return>
			bool pushInput(int fd, int flags=-1);

			// <brief>
			// popInput() dismisses the current input source.
			// </brief>
			// <return>none</return>
			void popInput(void);

			// <brief>
			// popAllInputs() dismisses all the input sources in the input stack.
			//    and goes back to the initial state.
			// In the initial state you will get the EOI as current token.
			// </brief>
			// <return>none</return>
			void popAllInputs(void);

			void pushFd(int fd, int flags=-1);
			void setFd(int fd, int flags=-1);

			// <brief>
			// This makes an input-file ready to tokenized
			//     by putting it on the internal stack of inputs.
			// </brief>
			// <parm name="filepath">The file path of input</parm>
			// <return>true/false</return>

			bool pushFile(const std::string& filepath, int flags=-1);
			bool pushFile(const std::wstring& wfilepath, int flags=-1);

			void setFile(const std::string& filepath, int flags=-1);
			void setFile(const std::wstring& filepath, int flags=-1);

			void pushLine(const char* line, int len=-1, int flags=-1);
			void pushLine(const wchar_t* line, int len=-1, int flags=-1);

			void setLine(const char* line, int len=-1, int flags=-1);
			void setLine(const wchar_t* line, int len=-1, int flags=-1);

			void popCurrent(void);
			void dismissAllInputs(void);

			// <brief>
			// identify the char group of 'wch'.
			// </brief>
			// <parm name="wch">The char to be tested.</parm>
			// <return>true/false</return>
			bool is_ch_space(uls_wch_t wch);
			bool is_ch_idfirst(uls_wch_t wch);
			bool is_ch_id(uls_wch_t wch);
			bool is_ch_quote(uls_wch_t wch);
			bool is_ch_1ch_token(uls_wch_t wch);
			bool is_ch_2ch_token(uls_wch_t wch);
			bool is_ch_comm(uls_wch_t wch);

			// <brief>
			// Skips the white chars.
			// </brief>
			// <return>none</return>
			void skipBlanks(void);

			// <brief>
			// peekCh() peeks the next character in the input.
			// getCh() will get the character and advance the cursor of the input.
			// </brief>
			// <parm name="isQuote"> If the next token is a literal string 'isQuote' is true.</parm>
			// <return>The next character</return>
			virtual uls_wch_t peekCh(bool* isQuote) override;
			uls_wch_t peekCh(void);

			// <brief>
			// This extracts the next character.
			// If the return value is UCH_CH_NONE,
			//    you can check the value of parameter 'isQuote'
			//    to see that the current token is a literal string.
			// If the return value is UCH_CH_NONE and *isQuote == false, the current position is EOF or EOI.
			// </brief>
			// <parm name="isQuote"> If the next token is a literal string 'isQuote' is true.</parm>
			// <return>The next character</return>
			virtual uls_wch_t getCh(bool* isQuote) override;
			uls_wch_t getCh(void);

			// <brief>
			// This will push back the character wch in order to get it again
			//     at the next call of peekCh() or getCh().
			// </brief>
			// <return>none</return>
			virtual void ungetCh(uls_wch_t wch) override;

			// <brief>
			// This is one of the main methods of the uls lexical analyzer object.
			// It gets a token and its associated lexeme, advancing the cursor of input.
			// At the end of input you will get a special token EOI.
			// Notice the EOI token number should be compared with the field 'toknum_EOI'
			//    as it's a dynamic number varied for each lexical configuration.
			//
			// Even if it returns the token number, you can get it again from the object.
			// Use getTokNum() to get the current token number and getTokStr() to get the associated lexeme.
			// </brief>
			// <return>the token number</return>
			virtual int getTok(void) override;
			inline int getToken(void) {
				return getTok();
			}
			inline int next(void) {
				return getTok();
			}

			// <brief>
			// This method returns the current token string stored in the object by getTok().
			// The value is valid until the next call of getTok().
			// </brief>
			// <return>the alias of string stored internally</return>
			virtual void getTokStr(std::string **pp_lxm) override;
			void getTokStr(std::wstring **pp_lxm);

			void getLexeme(std::string& lxm) {
				std::string *p_lxm;
				getTokStr(&p_lxm);
				lxm = *p_lxm;
			}

			void getLexeme(std::wstring& wlxm) {
				std::wstring *p_wlxm;
				getTokStr(&p_wlxm);
				wlxm = *p_wlxm;
			}

			// <brief>
			// Returns the current token id in the object obtained by getTok().
			// It's the same value as the return value of getTok().
			// </brief>
			// <return>token number</return>
			virtual int getTokNum(void) override;
			int getTokId(void) {
				return getTokNum();
			}

			// <brief>
			// These methods check if the lexeme of the current token is
			//     an integer or floating-point number, or zero.
			// </brief>
			// <return>true/false</return>
			bool isLexemeInt(void);
			bool isLexemeReal(void);
			bool isLexemeZero(void);

			// <brief>
			// These convert the current token string to primitive values.
			// 1. lexemeAsInt() will recognize the current token string as an 'int'.
			// 2. lexemeAsUInt() will recognize the current token string as an 'unsigned int'.
			// 3. lexemeAsLongLong() will recognize the current lexeme as an 'long long'.
			// 4. lexemeAsULongLong() will recognize the current lexeme as an 'unsigned long long'.
			// 5. lexemeAsDouble() will recognize the current lexeme as an 'double'.
			// They return the value after converting the token string to the primitive type.
			// Make sure the current token is a number before calling these methods.
			// </brief>
			// <return>primitive data types</return>
			int lexemeAsInt(void);
			unsigned int lexemeAsUInt(void);
			long long lexemeAsLongLong(void);
			unsigned long long lexemeAsULongLong(void);
			double lexemeAsDouble(void);

			// <brief>
			// In case that the current token is NUMBER, this'll return the suffix of the number if it exists.
			// It's obvious that suffix of number is to inform explicitly compiler
			//     of its data types in programming language.
			// </brief>
			// <return>suffix string</return>
			std::string lexemeNumberSuffix(void);

			// <brief>
			// Sets the current token to <t,lxm> forcibly.
			// </brief>
			// <return>none</return>
			void setTok(int t, const std::string& lxm);
			void setTok(int t, const std::wstring& lxm);

			// <brief>
			// If the current token number is not 'TokExpected', An exception will be thrown.
			// </brief>
			// <parm name="TokExpected">The expected token number</parm>
			// <return>none</return>
			void expect(int TokExpected);

			// <brief>
			// This sets extra token definition 'extra_tokdef' which is provided by user.
			// The stored data of token number 't' can be later retrieved by getExtraTokdef().
			// For detailed example, refer to the source files in tests/dump_toks.
			// </brief>
			// <parm name="t">The target token number with which user data is associated</parm>
			// <parm name="extra_tokdef">An opaque data</parm>
			// <return>none</return>
			void setExtraTokdef(int t, void* extra_tokdef);

			// <brief>
			// Use these methods to get the user-defined token information.
			// See the example in tests/dump_toks for detail.
			// </brief>
			// <parm name="t">The target token id of data the you want to retrieve.</parm>
			// <return>The opaque data given by user</return>
			void* getExtraTokdef(void);
			void* getExtraTokdef(int t);
			void* &operator[](int t);

			// <brief>
			// Call ungetTok() if you want to get the current token again after getTok().
			// Call ungetStr() if you want to push a string to the front of the current input source.
			// Call ungetLexeme() if you want to push a lexeme to the front of the current input source.
			// </brief>
			// <parm name="lxm">The token string with which the token number 'tok_id' is paired.</parm>
			// <return>none</return>
			virtual void ungetTok(void) override;

			void ungetStr(std::string str);
			void ungetStr(std::wstring str);

			void ungetLexeme(std::string lxm, int tok_id);
			void ungetLexeme(std::wstring lxm, int tok_id);

			// <brief>
			// This dumps the current token as explanatory string, which is composed of the string 'pfx',
			//     'suff', and the information string of the token.
			//  For example, if pfx is '\t' and suff is '\n',
			//    the output line to the terminal may be like as follows.
			//  '\t' [___ID] main '\n'
			//
			// The dumpTok() without parameters will prepend '\t' and append '\n' to the string by default.
			// </brief>
			// <parm name="pfx">The 'pfx' is prepended at the front of the basic description of token.</parm>
			// <parm name="suff">The 'suff' is appended at the end of  the basic description of token.</parm>
			// The basic decription of token consists of <TokenName,TokenString>.
			// <return>none</return>
			void dumpTok(std::string pfx, std::string suff);
			void dumpTok(std::wstring pfx, std::wstring suff);
			void dumpTok(void);

			// <brief>
			// This returns the keyword string associated with the token number 't'.
			// For example, if the token number is 1 and it represents the keyword '+=',
			//     getKeywordStr(1) will return the string '+='.
			// </brief>
			// <parm name="t">The token number of which keyword you want know</parm>
			// <return>string</return>

			void Keyword(int t, std::string *ptr_keyw);
			void Keyword(int t, std::wstring *ptr_keyw);

			void getKeywordStr(int t, std::string *ptr_keyw);
			void getKeywordStr(int t, std::wstring *ptr_keyw);

			// <brief>
			// return the keyword string of the current token.
			// </brief>
			// <return>keyword string</return>
			void Keyword(std::string *ptr_keyw);
			void Keyword(std::wstring *ptr_keyw);

			void getKeywordStr(std::string *ptr_keyw);
			void getKeywordStr(std::wstring *ptr_keyw);

			// <brief>
			// This opens a file for writing messages sequentially.
			// To write to the output file, use print() defined below.
			// </brief>
			// <parm name="out_file">The output file path</parm>
			// <return>none</return>
			void openOutput(const std::string& out_file);
			void openOutput(const std::wstring& out_file);

			// <brief>
			// This flushes the buffer of the output and closes the output file.
			// After using the calls of print() method, don't forget to call closeOutput().
			// </brief>
			// <return>none</return>
			void closeOutput(void);

			// <brief>
			// This will print the formatted string to the output opened by openOutput().
			// </brief>
			// <parm name="fmt">format string</parm>
			// <return># of bytes printed</return>
			int print(const char* fmt, ...);
			int print(const wchar_t* fmt, ...);

			// <brief>
			// This procedure makes a formatted string from format string 'fmt' and variable arguments list.
			// The 'buf' is a buffer of size 'bufsiz' where the formatted string is stored.
			// In case of the resultant string being excess of the 'buf',
			//    the string will be truncated at the 'bufsiz - 1'.
			// </brief>
			// <parm name="buf">The output buffer for the formatted string</parm>
			// <parm name="bufsiz">The capacity of 'buf'</parm>
			// <parm name="fmt">format string</parm>
			// <return># of bytes filled except for '\0'</return>
			virtual int vsnprintf(char* buf, int bufsiz, const char *fmt, va_list args) override;
			int vsnprintf(wchar_t* buf, int bufsiz, const wchar_t *fmt, va_list args);

			int snprintf(char* buf, int bufsiz, const char *fmt, ...);
			int snprintf(wchar_t* buf, int bufsiz, const wchar_t *fmt, ...);

			// <brief>
			// This stores the formatted string to the 'buf' with its size unknown.
			// Recommended to use snprintf().
			// </brief>
			// <parm name="buf">The output buffer for the formatted string</parm>
			// <parm name="fmt">The template for message string</parm>
			// <return># of bytes filled</return>
			int sprintf(char * buf, const char *fmt, ...);
			int sprintf(wchar_t * buf, const wchar_t *fmt, ...);

			// <brief>
			// The formatted string by 'fmt' and the arguments will be emitted to file.
			// </brief>
			// <parm name="fp">The pointer of FILE, which is opened by fopen()</parm>
			// <parm name="fmt">format string</parm>
			// <return># of bytes written</return>
			virtual int vfprintf(FILE* fp, const char *fmt, va_list args) override;
			int vfprintf(FILE* fp, const wchar_t *fmt, va_list args);

			int fprintf(FILE* fp, const char *fmt, ...);
			int fprintf(FILE* fp, const wchar_t *fmt, ...);

			// <brief>
			// The formatted string by 'fmt' and the arguments
			//     will be emitted to the 'standard output', stdout.
			// </brief>
			// <parm name="fmt">format string</parm>
			// <return># of bytes printed</return>
			int printf(const char *fmt, ...);
			int printf(const wchar_t *fmt, ...);

			// <brief>
			// Changes the associated procedure with 'percent_name', a converion specification.
			// The procedure will output string with puts_proc which can be set by 'changePuts'
			// </brief>
			// <parm name="percent_name">The converion specification string without '%'.</parm>
			// <parm name="proc">The user provided procedure to process '%percent_name'</parm>
			// <return>none</return>
			virtual void changeConvSpec(const char* percent_name, uls_lf_convspec_t proc) override;
			void changeConvSpec(const wchar_t* percent_name, uls_lf_convspec_t proc);

			// <brief>
			// You can use this method to change the default output interface for logging.
			// </brief>
			// <parm name="puts_proc">A newly output interface</parm>
			// <return>none</return>
			virtual void changePuts(void *xdat, uls_lf_puts_t puts_proc) override;

			// <brief>
			// Sets the log level of the object, UlsLex.
			// The possible 'loglvl' are ULS_LOG_EMERG, ULS_LOG_ALERT, ULS_LOG_CRIT, ...
			// </brief>
			// <parm name="lvl">new value of log level</parm>
			// <return>none</return>
			void setLogLevel(int lvl);

			// <brief>
			// Clears the log level defined in this object, UlsLex.
			// </brief>
			// <parm name="lvl">the log level to be cleared.</parm>
			// <return>void</return>
			void clearLogLevel(int lvl);

			// <brief>
			// Checks if the 'lvl' is set in the internally.
			// </brief>
			// <parm name="lvl">the log level to be checked.</parm>
			// <return>true/false</return>
			bool isLogLevelSet(int lvl);

			// <brief>
			// This is one of the main methods of ULS logging framework.
			// This emits the formatted string from 'fmt' and 'args' to the error port.
			// No need to append '\n' to the end of the format string 'fmt'.
			// The output is by default 'stderr' of 'FILE'.
			// </brief>
			// <parm name="loglvl">This message will be printed if 'loglvl' is set.</parm>
			// <return>none</return>
			virtual void log(int loglvl, const char* fmt, ...) override;
			void log(int loglvl, const wchar_t* fmt, ...);

			// <brief>
			// Logs formatted messages
			// You can use %t %w to print the current token, its location.
			// No need to append '\n' to the end of line 'fmt'.
			//
			// It's capable of printing the keyword string of token and input coordinate,
			//    as well as the default conversion specifications.
			//   a) %t: use to print the name of the current token
			//   b) %k: use to print the keyword string of the current token
			//   c) %w: use to print the coordinate of the current input file or source.
			//          The 'coordinate' is composed of the tag and line number.
			//
			// The 'log' object has already a reference of the lexical object when it's created.
			// Notice that there's no need to give the argument for the conversion specification
			//     to process '%t', '%k', and '%w'
			// </brief>
			// <parm name="fmt">format string</parm>
			// <return>void</return>
			virtual void log(const char* fmt, ...) override;
			void log(const wchar_t* fmt, ...);

			// <brief>
			// This informs user of the occurrence of a system error with a formatted message.
			// The program will be aborted.
			// </brief>
			// <parm name="fmt">format string</parm>
			// <return>none</return>
			virtual void panic(const char* fmt, ...) override;
			void panic(const wchar_t* fmt, ...);
		};
	}
}
