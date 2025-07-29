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

#include <uls/UlsAuw.h>

#include <uls/uls_lex.h>
#include <uls/uld_conf.h>
#include <uls/uls_log.h>

#include <string>
#include <map>

namespace uls {
	typedef uls_mutex_t MutexType;

	// <brief>
	//   Initializes the mutex 'mtx' which is to be used by locked/unlocked method.
	//   The mechanism is implemented by the pthread library on Linux and by win32-api on Windows.
	// </brief>
	// <parm name="mtx">Mutex object</parm>
	ULSCPP_DLL_EXTERN void initMutex(MutexType mtx);

	// <brief>
	//   Deinitializes the 'mtx'. After calling this, 'mtx' shouldn't be used.
	// </brief>
	// <parm name="mtx">the mutex object</parm>
	ULSCPP_DLL_EXTERN void deinitMutex(MutexType mtx);

	// <brief>
	//   The locking API to grab the lock 'mtx'.
	// </brief>
	// <parm name="mtx">The mutex object initialized by uls::initMutex</parm>
	ULSCPP_DLL_EXTERN void lockMutex(MutexType mtx);

	// <brief>
	//   The unlocking API to ungrab the lock 'mtx'.
	// </brief>
	// <parm name="mtx">The mutex object initialized by uls::initMutex</parm>
	ULSCPP_DLL_EXTERN void unlockMutex(MutexType mtx);

	// <brief>
	//   This procedure lists the search directories for ulc file, which is suffixed by 'ulc'.
	//   uls_create() or uls_init() will search the directories for ulc file in same order as shown in dumpSearchPathOfUlc().
	//   The order of search-directories is affected by the format of paramenter 'confname'.
	//   If the parameter is suffixed by '.ulc' or has '..' or '.', it's recognized as file path.
	//   But if not suffixed by 'ulc' like 'sample', 'namespace/sample',
	//     the procedure will search the ulc repository first.
	// </brief>
	// <parm name="confname">lexcial configuration</parm>
	ULSCPP_DLL_EXTERN void dumpSearchPathOfUlc(const std::string& confname);
	ULSCPP_DLL_EXTERN void dumpSearchPathOfUlc(const std::wstring& wconfwname);
	ULSCPP_DLL_EXTERN void listUlcSearchPaths(void);

	ULSCPP_DLL_EXTERN void initialize_ulscpp(void);
	ULSCPP_DLL_EXTERN void finalize_ulscpp(void);

	namespace crux {
		class UlsIStream;

		// <brief>
		//   These package-methods check if the parmeter 'lxm' is an interger or a floating-point number, or zero.
		// </brief>
		// <parm name="lxm">the string to be tested</parm>
		// <return>true/false</return>
		ULSCPP_DLL_EXTERN bool isLexemeZero(const std::string& lxm);
		ULSCPP_DLL_EXTERN bool isLexemeZero(const std::wstring& lxm);

		ULSCPP_DLL_EXTERN bool isLexemeInt(const std::string& lxm);
		ULSCPP_DLL_EXTERN bool isLexemeInt(const std::wstring& lxm);

		ULSCPP_DLL_EXTERN bool isLexemeReal(const std::string& lxm);
		ULSCPP_DLL_EXTERN bool isLexemeReal(const std::wstring& lxm);

		// <brief>
		//   These convert the parameter 'lxm' to the value of int or double.
		// </brief>
		// <parm name="lxm">the string to be tested</parm>
		// <return>the converted native data type from the string.</return>
		ULSCPP_DLL_EXTERN int LexemeAsInt(const std::string& lxm);
		ULSCPP_DLL_EXTERN int LexemeAsInt(const std::wstring& lxm);

		ULSCPP_DLL_EXTERN double LexemeAsDouble(const std::string& lxm);
		ULSCPP_DLL_EXTERN double LexemeAsDouble(const std::wstring& wlxm);

		class IUlsLex {
		public:
			// <brief>
			//   Proceeds the cursor of input to get the next token.
			//   Returns the token number(integer) including negative ones.
			//   The associated token-string(or lexeme) is obtained by calling getTokStr().
			// </brief>
			// <return>the token number</return>
			virtual int next(void) = 0;

			// <brief>
			//   Returns the current token id stored in the object.
			//   It remains same until the next call of next().
			//   The intial value of token-number is NONE which can be defined by user.
			// </brief>
			// <return>token number</return>
			virtual int getTokNum(void) = 0;
			virtual const char *getTokUtf8Str(int *ptr_ulen = NULL) = 0;

			// <brief>
			//   Peeks what is the next character in the input.
			//   getChar() will get the character and advance the cursor of input.
			// </brief>
			// <parm name="isQuote">check if the next char is the first char of a quotation string</parm>
			// <return>The next character</return>
#define ULS_CH_QSTR   0x80000001
#define ULS_CH_EOI    0x80000002
#define ULS_CH_NONE   0x80000003
#define ULS_CH_ERR    0x80000004
			virtual uls_wch_t peekChar(void) = 0;
			virtual uls_wch_t getChar(void) = 0;

			virtual bool ungetStrUtf8(const char *str) = 0;
			virtual bool ungetTokUtf8(int tok_id, const char *lxm = NULL) = 0;
		};

#define LP_TCHAR char
#define LP_TSTR char*
#define LP_CTSTR const char*
#define LP_STRING std::string
#define LP_CONST_STRING const std::string
#define LP_STRING_ALIAS std::string&
#define _ULS_NAME_IF1 IUlsAUPrint
#define _ULS_NAME_IF2 IUlsCompatAUStr

#include "uls/_ulslex_ifs.h"

#undef LP_TCHAR
#undef LP_TSTR
#undef LP_CTSTR
#undef LP_STRING
#undef LP_CONST_STRING
#undef LP_STRING_ALIAS
#undef _ULS_NAME_IF1
#undef _ULS_NAME_IF2

#define LP_TCHAR wchar_t
#define LP_TSTR wchar_t*
#define LP_CTSTR const wchar_t*
#define LP_STRING std::wstring
#define LP_CONST_STRING const std::wstring
#define LP_STRING_ALIAS std::wstring&
#define _ULS_NAME_IF1 IUlsWPrint
#define _ULS_NAME_IF2 IUlsCompatWStr

#include "uls/_ulslex_ifs.h"

#undef LP_TCHAR
#undef LP_TSTR
#undef LP_CTSTR
#undef LP_STRING
#undef LP_CONST_STRING
#undef LP_STRING_ALIAS
#undef _ULS_NAME_IF1
#undef _ULS_NAME_IF2

		// <brief>
		//   This is the main class of ulscpp library.
		//   You can instantiate this class with a parameter representing lexical configuration.
		//   The object created is used for lexical analysis.
		// </brief>
		class ULSCPP_DLL_EXTERN UlsLexUStr :
			public uls::crux::IUlsLex, public uls::crux::IUlsCompatAUStr {
			static const int N_CHARS_LOGBUF = 128;
			static const int N_TMPLVAL_SIZE = 64;

			uls_lex_t lex;
			uld_names_map_t *names_map;
			int input_flags;

			std::map<int,void*> extra_tokdefs;

			uls_lf_t  *str_nlf;
			uls_lf_t  *file_nlf;

			static bool ulslex_inited;
			static uls_lf_map_t ulscpp_convspec_nmap;
			uls_log_t logbase;

			FILE *sysprn_fp;
			int sysprn_opened;

		protected:
			uls_mutex_struct_t syserr_g_mtx;
			uls_mutex_struct_t sysprn_g_mtx;

			UlsLexUStr();

			uls_lex_ptr_t getCore(void) {
				return &lex;
			}

			// <brief>
			//   This checks whether the user-provided data exists
			//   for given token number 't'
			// </brief>
			// <parm name="t">token number to be tested</parm>
			// <return>true/false</return>
			bool existTokdefInHashMap(int t);

			// <brief>
			//   changeUldNames
			// </brief>
			int prepareUldMap(int bufsiz_uldfile);
			bool finishUldMap();

			virtual void changeUldNames(const char *name, const char *name2,
				int tokid_valid, int tokid, const char *aliases) override;
			void changeUldNames(const std::string& name, const std::string& name2,
				int tokid_valid, int tokid, const std::string& aliases);

			bool initUlsLex_ustr(const char *ulc_file);
			void update_rsvdtoks(void);

			bool openOutput_ustr(const char *out_file);
			FILE *get_sysprn_fp(void) {
				return sysprn_fp;
			}

		public:
			static void initialize(void);
			static void finalize(void);

			// <brief>
			//   These flags affects the input stream process and is set in calling pushInput().
			//     1. WantEOF: At the end of file, getTok() will give you the reserved token EOF.
			//     2. DoDup: This will copy the input source for character string or file descriptor if possible.
			// </brief>
			enum InputOpts {
				WantEOF = 0, DoDup
			};

			// <brief>
			//   This will convert the 'fl' to get the flag to interface with 'libuls.so'.
			// </brief>
			int get_uls_flags(InputOpts fl);

			// <brief>
			//   These procedures is to maninpulate the flags described above.
			//   setInputOpts() sets the the intenal flag 'input_flags' to the flag 'fl'
			//   clearInputOpts() clears the flag 'fl' in the the intenal flag 'input_flags'.
			//   getInputOpts() just returns the intenal flag 'input_flags'.
			// </brief>
			void setInputOpt(InputOpts fl);
			void clearInputOpt(InputOpts fl);
			int getInputOpts(void);
			void resetInputOpts(void);

			// <brief>
			//   These are constructors that create objects for lexical analysis.
			//   The 'ulc_file' is a name of language specification in the ulc repository or
			//     simply the file path of an ulc file.
			//   To see the available ulc names, you can use the -q-option of the command 'ulc2class'.
			// </brief>
			// <parm name="ulc_file">The name-path or file path of the lexical configuration.</parm>
			UlsLexUStr(const char *ulc_file);
			UlsLexUStr(std::string& ulc_file);

			// <brief>
			//   The destructor of UlsLexUStr.
			// </brief>
			virtual ~UlsLexUStr();

			// <brief>
			//   The class 'UlsLexUStr' can be the internal class of wrapper classes.
			//   This returns the pointer of the embedded structure of the library, libuls.
			// </brief>
			// <return>The pointer of 'uls_lex_t'</return>
			uls_lex_ptr_t getLexCore(void) { return &lex; }

			// <brief>
			//   Checks if the cursor reaches the end of input stream.
			// </brief>
			virtual bool isEOI(void) {
				return (getTokNum() == _uls_toknum_EOI(&lex)) ? true : false;
			}

			// <brief>
			//   Checks if the cursor reaches the end of the end of file.
			//   There may be lots of (nested) files in the internal input stack of ULS object.
			//   A streaming seesion will be ended by EOI, end of input.
			// </brief>
			virtual bool isEOF(void) {
				return (getTokNum() == _uls_toknum_EOF(&lex)) ? true : false;
			}

			// <brief>
			//   The list of reserved token numbers
			//   The reserved token can be renamed by user in the ulc configuration.
			//   So the values of reserved tokens can be varied according to the language spec.
			// </brief>
			int toknum_EOI, toknum_EOF, toknum_ERR;
			int toknum_NONE, toknum_ID, toknum_NUMBER, toknum_TMPL;

			// <brief>
			//   This will get or update the tag string of current processed file.
			// </brief>
			// <parm name="fname">The current or new tag string</parm>
			virtual void getTag(std::string& fname) override;
			virtual void setTag(const char *fname) override;
			void setTag(const std::string& fname);

			// <brief>
			//   Set the location of the current token in the input file.
			//   This can be used with the tag string described above.
			//   The field 'LineNum' is automatically updated by calling getTok() but
			//     can be changed it forcibly.
			// </brief>
			// <parm name="lineno">The new value of 'LineNum' to be updated</parm>
			void setLineNum(int lineno);

			// <brief>
			//   Use this method to add some lines to the current line number 'LineNum' forcibly.
			//   If the resultant line number is negative the 'LineNum' won't be updated.
			// </brief>
			// <parm name="amount">The amount of lines to be added. It may be negative</parm>
			void addLineNum(int amount);

			// <brief>
			//   Get the number of current line indicated by the input cursor
			// </brief>
			// <return>the current line number</return>
			int getLineNum(void);

			// <brief>
			//   Changes the literal-string analyzer to 'proc'.
			//   The 'proc' will be applied to the quote type starting with 'pfx'.
			// </brief>
			// <parm name="pfx">The prefix of literal string that will be processed by 'proc'</parm>
			virtual void changeLiteralAnalyzer(const char *pfx, uls_litstr_analyzer_t proc, void *data) override;
			void changeLiteralAnalyzer(const std::string& pfx, uls_litstr_analyzer_t proc, void *data);

			// <brief>
			//   Delete the literal-string analyzer starting with 'pfx' if it's defined in the object.
			//   The 'literal-string analyzer' is to recognize the quotation string starting with 'pfx'.
			// </brief>
			// <parm name="pfx">The literal string analyzer of which the quote type is started with 'pfx'.</parm>
			virtual void deleteLiteralAnalyzer(const char *pfx) override;
			void deleteLiteralAnalyzer(const std::string& pfx);

			// <brief>
			//   This method will push an input string 'istr' on the top of the internal input stack.
			//   Then the getTok() method can be used to get a token from the input.
			// </brief>
			// <parm name="istr">input stream object!
			//   You can create the input object from text file, literal-string, or uls-file.</parm>
			// <return>true/false</return>
			bool pushInput(UlsIStream& istr, int flags=-1);

			// <brief>
			//   This makes file descriptor 'fd' prepared on the internal stack.
			//   Processing of the previous input is postponed until the completion of the input specified by 'fd'.
			// </brief>
			// <parm name="fd">the file descriptor of input file</parm>
			// <return>true/false</return>
			bool pushInput(int fd, int flags=-1);

			// <brief>
			//   popInput() dismisses the current input source in the internal stack.
			// </brief>
			void popInput(void);

			// <brief>
			//   popAllInputs() dismisses all the input sources in the internal (input) stack.
			//     and goes back to the initial state.
			//   In the initial state you should get the NONE as current token.
			// </brief>
			void popAllInputs(void);

			// <brief>
			//   These mothods make an input-file to be ready to tokenized
			//     by putting it on the internal stack.
			// </brief>
			// <parm name="filepath">The file path of input</parm>
			// <parm name="line">string encoded by utf-8</parm>
			// <parm name="fd">file descriptor</parm>
			// <return>true/false</return>
			virtual bool pushFile(const char *filepath, int flags = -1) override;
			bool pushFile(const std::string& filepath, int flags = -1);

			virtual bool setFile(const char *filepath, int flags = -1) override;
			bool setFile(const std::string& filepath, int flags = -1);

			virtual bool pushLine(const char *line, int len = -1, int flags = -1) override;
			bool pushLine(const std::string& line, int flags = -1);

			virtual bool setLine(const char *line, int len = -1, int flags = -1) override;
			bool setLine(const std::string& line, int flags = -1);

			void pushFd(int fd, int flags = -1);
			void setFd(int fd, int flags = -1);

			// <brief>
			//   identify the char group of 'wch'.
			//   the char group = { space, the first-char of id, the char of id, the first of quotation string,
			//     the first char of punctuation, ...}
			// </brief>
			// <parm name="wch">The decoded (wide) char to be tested</parm>
			// <return>true/false</return>
			bool is_ch_space(uls_wch_t wch);
			bool is_ch_idfirst(uls_wch_t wch);
			bool is_ch_id(uls_wch_t wch);
			bool is_ch_quote(uls_wch_t wch);
			bool is_ch_1ch_token(uls_wch_t wch);
			bool is_ch_2ch_token(uls_wch_t wch);
			bool is_ch_comm(uls_wch_t wch);

			// <brief>
			//   Skips the white chars.
			//   The white chars are to be defined by the spec. written by user
			// </brief>
			void skipBlanks(void);

			// <brief>
			//   peekChar() peeks the next character in the input.
			// </brief>
			// <return>the next character</return>
			virtual uls_wch_t peekChar(void) override;

			// <brief>
			//   This extracts the next character.
			//   If the return value is UCH_CH_NONE,
			//     you can check the value of parameter 'isQuote'
			//     to see that the current token is a literal string.
			//   If the return value is UCH_CH_NONE and *isQuote == false, the current position is EOF or EOI.
			// </brief>
			// <return>The next character</return>
			virtual uls_wch_t getChar(void) override;

			// <brief>
			//   This is one of the main methods of the uls lexical analyzer object.
			//
			//   It proceeds the cursor of input to get the next token
			//     and gets a token(integer) and its associated token-string(lexeme), advancing the cursor of input.
			//   The token number is stored internally.
			//   The lexeme can be obtained by calling getTokStr() as well
			//   Use getTokNum() to get the current token number and getTokStr() to get the token.
			//
			//   At the end of input you will get a special token EOI.
			//   Notice the EOI token number should be the value of 'toknum_EOI()
			//     as it's a dynamic number varied for each lexical configuration.
			// </brief>
			// <return>the token number</return>
			virtual int next(void) override;
			inline int getTok(void) {
				return next();
			}

			// <brief>
			//   Returns the current token string internally stored in the object.
			//   It is only valid until the next call of getTok().
			//   getTokStr() return the current token string in os/native form.
			//     a) utf-8 on Linux
			//     b) ms-mbcs on Windows
			//   getTokUtf8Str() returns the utf-8 string on both Linux and Windows.
			// </brief>
			// <parm name="lxm">the buffer to store lexeme</parm>
			// <parm name="ptr_ulen">the pointer for the length of utf-8 strig</parm>
			// <return>the utf-8 string</return>
			virtual const char *getTokUtf8Str(int *ptr_ulen = NULL) override;
			virtual void getTokStr(std::string& lxm) override;

			// <brief>
			//   Returns the current token id in the object obtained by getTok().
			//   It is the same as the return value of getTok().
			//   This value is only valid until the next call of getTok().
			// </brief>
			// <return>integer</return>
			virtual int getTokNum(void) override;
			int getTokId(void) {
				return getTokNum();
			}

			// <brief>
			//   These methods check if the lexeme of the current token is
			//     an integer or a floating-point number, or zero.
			// </brief>
			// <return>true/false</return>
			bool isLexemeInt(void);
			bool isLexemeReal(void);
			bool isLexemeZero(void);

			// <brief>
			//   These convert the current token string to primitive values.
			//     1. lexemeAsInt() will recognize the current token string as an 'int'.
			//     2. lexemeAsUInt() will recognize the current token string as an 'unsigned int'.
			//     3. lexemeAsLongLong() recognizes the current lexeme as an 'long long'.
			//     4. lexemeAsULongLong() recognizes the current lexeme as an 'unsigned long long'.
			//     5. lexemeAsDouble() recognizes the current lexeme as an 'double'.
			//   They return the value after converting the token string to the primitive type as much as possible.
			//   Note that the range of numbers that uls manipulate can exceed the range that OS can cover.
			//   Make sure the current token is a number before calling these methods.
			// </brief>
			// <return>primitive data types</return>
			int lexemeAsInt(void);
			unsigned int lexemeAsUInt(void);
			long long lexemeAsLongLong(void);
			unsigned long long lexemeAsULongLong(void);
			double lexemeAsDouble(void);

			// <brief>
			//   In case that the current token is NUMBER, this'll return the suffix of the number if it exists.
			//   It's obvious that suffix of number is to inform explicitly compiler
			//     of its (number) data types defined by the programming language.
			// </brief>
			// <return>suffix string</return>
			std::string lexemeNumberSuffix(void);

			// <brief>s
			//   Sets the current token to <t,lxm> forcibly.
			//   setTokUStr sets the current token string as the parameter 'lxm'
			// </brief>
			virtual void setTok(int t, const std::string lxm) override;
			void setTokUStr(int t, const char *lxm = NULL);

			// <brief>
			//   If the current token number is not 'TokExpected', An exception will be thrown.
			// </brief>
			// <parm name="TokExpected">The expected token number</parm>
			void expect(int TokExpected);

			// <brief>
			//   This sets extra token definition 'extra_tokdef' which is void-type, provided by user.
			//   The stored data of token number 't' can be later retrieved by getExtraTokdef().
			//   For a detailed example, refer to the source files in tests/dump_toks.
			// </brief>
			// <parm name="t">The target token number with which user data is associated</parm>
			// <parm name="extra_tokdef">An opaque data</parm>
			void setExtraTokdef(int t, void *extra_tokdef);

			// <brief>
			//   Use these methods to get the user-defined token information, which is set by setExtraTokdef().
			//   See the example in tests/dump_toks for the usage.
			// </brief>
			// <parm name="t">The target token id of data the you want to retrieve.</parm>
			// <return>The opaque data given by user</return>
			void *getExtraTokdef(void);
			void *getExtraTokdef(int t);
			void *&operator[](int t);

			// <brief>
			//   Call ungetCurrent() if you want to get the current token again after getTok().
			//   Call ungetTok() if you want to push a lexeme at the front of the current input source.
			// </brief>
			// <parm name="tok_id">The token id pushed</parm>
			// <parm name="lxm">The token string with which the token id 'tok_id' is paired.</parm>
			virtual bool ungetTokUtf8(int tok_id, const char *lxm = NULL) override;
			virtual bool ungetTok(int tok_id, const char *lxm = NULL) override;
			bool ungetTok(int tok_id, const std::string& lxm);
			bool ungetCurrent(void);

			// <brief>
			//   Push a string into the input string
			// </brief>
			// <parm name="str">The string that is pushed into the input stream</parm>
			virtual bool ungetStrUtf8(const char *str) override;
			virtual bool ungetStr(const char *str) override;
			bool ungetStr(const std::string& str);
			bool ungetChar(uls_wch_t wch);

			// <brief>
			//   This dumps the current token in the explanatory form.
			//   The line is composed of the string 'pfx','suff', and the information string of the token.
			//   For example, if pfx is '\t' and suff is '\n',
			//     the output line to the terminal may be like as follows:
			//     '\t' [___ID] main '\n'
			//
			//   The dumpTok() without parameters will prepend '\t' and append '\n' to the string by default.
			// </brief>
			// <parm name="pfx">The 'pfx' is prepended at the front of the basic description of token.</parm>
			// <parm name="suff">The 'suff' is appended at the end of the basic description of token.</parm>
			virtual void dumpTok(const char *pfx, const char *suff) override;
			void dumpTok(void);

			// <brief>
			//   This opens a file for writing content line by line sequentially.
			//   To write to the output file, use print() defined below.
			// </brief>
			// <parm name="out_file">The output file path</parm>
			virtual bool openOutput(const char *out_file) override;
			bool openOutput(const std::string& out_file);

			// <brief>
			//   This flushes the buffer of the output and closes the output file.
			//   After using the calls of print() method, don't forget to call closeOutput().
			// </brief>
			void closeOutput(void);

			// <brief>
			//   This will print the formatted string to the output opened by openOutput().
			//   Note that the keyword 'virtual' is used
			//     because both(!) vprint and print are needed to be overrided at the supclass 'UlsLexAWStr'
			// </brief>
			// <parm name="fmt">the format string</parm>
			// <return># of bytes printed</return>
			virtual int vprint(const char *fmt, va_list args) override;
			int print(const char *fmt, ...);

			// <brief>
			//   This procedure makes a formatted string from the format string 'fmt' and the variadic arguments list.
			//   The 'buf' is a buffer of size 'bufsiz' where the formatted string is stored.
			//   In case of the resultant string being excess of the 'buf',
			//     the string will be truncated at the 'bufsiz - 1'.
			// </brief>
			// <parm name="buf">The output buffer for the formatted string</parm>
			// <parm name="bufsiz">The capacity of 'buf'</parm>
			// <parm name="fmt">the format string</parm>
			// <return># of bytes filled except for '\0'</return>
			virtual int vsnprintf(char *buf, int bufsiz, const char *fmt, va_list args) override;

			// <brief>
			//   This stores the formatted string to the 'buf' with its size unknown.
			//   Recommended to use snprintf().
			// </brief>
			// <parm name="buf">The output buffer for the formatted string</parm>
			// <parm name="fmt">The template for argumented string</parm>
			// <return># of bytes filled</return>
			int snprintf(char *buf, int bufsiz, const char *fmt, ...);
			int sprintf(char *buf, const char *fmt, ...);

			// <brief>
			//   The formatted string by 'fmt' and variadic arguments will be emitted to file.
			//   The formatted string by 'fmt' and the arguments
			//     will be emitted to the 'standard output', stdout.
			// </brief>
			// <parm name="fp">The pointer of FILE, which is opened by fopen()</parm>
			// <parm name="fmt">format string</parm>
			// <return># of bytes written</return>
			virtual int vfprintf(FILE* fp, const char *fmt, va_list args) override;
			int fprintf(FILE* fp, const char *fmt, ...);
			int printf(const char *fmt, ...);

			// <brief>
			//   Changes the associated procedure with 'percent_name', a converion specification.
			//   The procedure will output string with puts_proc which can be set by 'changePuts'
			// </brief>
			// <parm name="percent_name">The converion specification string that follows '%'.</parm>
			// <parm name="proc">The user provided procedure to process '%<percent_name>'</parm>
			virtual void changeConvSpec(const char *percent_name, uls_lf_convspec_t proc) override;
			void changeConvSpec(std::string& percent_name, uls_lf_convspec_t proc);

			// <brief>
			//   Sets the log level of the object, UlsLexUStr.
			//   The possible 'loglvl' are ULS_LOG_EMERG, ULS_LOG_ALERT, ULS_LOG_CRIT, ...
			// </brief>
			// <parm name="lvl">new value of log level</parm>
			void setLogLevel(int lvl);

			// <brief>
			//   Clears the log level defined in this object, UlsLexUStr.
			// </brief>
			// <parm name="lvl">the log level to be cleared.</parm>
			void clearLogLevel(int lvl);

			// <brief>
			//   Checks if the 'lvl' is set in the internally.
			// </brief>
			// <parm name="lvl">the log level to be checked.</parm>
			// <return>true/false</return>
			bool isLogLevelSet(int lvl);

			// <brief>
			//   Logs formatted messages
			//
			//   It's capable of printing the keyword string of token and input coordinate,
			//     as well as the default conversion specifications.
			//     a) %w: use to print the coordinate of the current input file or source.
			//          The 'coordinate' is composed of the tag-string and line-number.
			// </brief>
			// <parm name="fmt">format string</parm>
			// <parm name="args">The list of args</parm>
			virtual void vlog(int loglvl, const char *fmt, va_list args) override;

			// <brief>
			//   This is one of the main methods of ULS logging framework.
			//   This emits the formatted string from 'fmt' and 'args' to the error port.
			//   No need to append '\n' to the end of the format string 'fmt'.
			//   The output is by default 'stderr' of 'FILE'.
			// </brief>
			void log(const char *fmt, ...);

			// <brief>
			//   This informs the user of the occurrence of a system error with a formatted message.
			//   The program will be aborted.
			// </brief>
			// <parm name="fmt">format string</parm>
			void panic(const char *fmt, ...)
#ifdef __GNUC__
__attribute__((noreturn))
#endif
			;
		};

		class ULSCPP_DLL_EXTERN UlsLexAWStr :
			public uls::crux::UlsLexUStr, public uls::crux::IUlsCompatWStr {
			UlsAuw auwcvt;

			static bool ulslexwstr_inited;
			bool initUlsLex_awstr(const char *ulc_file);

#ifdef __ULS_WINDOWS__
			static uls_lf_map_t ulscpp_convspec_amap;

			static int lf_init_convspec_amap(uls_lf_map_ptr_t lf_map, int flags);
			static void lf_deinit_convspec_amap(uls_lf_map_ptr_t lf_map);

			uls_lf_t  *str_alf;
			uls_lf_t  *file_alf;
			csz_str_t fmtstr1;

			int lf_vxaprintf(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const char* afmt, va_list args);
			int lf_vsnaprintf(char* abuf, int abuf_siz, uls_lf_ptr_t uls_lf, const char* afmt, va_list args);

			uls_log_t alogbase;

			int lflog_vaprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *afmt, va_list args);
			int lflog_aprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const char *afmt, ...);
			void lflog_aflush(csz_str_ptr_t csz, uls_voidptr_t data, uls_lf_puts_t lf_puts);
			void lf_valog(uls_log_ptr_t log, const char *afmt, va_list args);

			int push_fp_astr(uls_lex_ptr_t uls, FILE *fp, int flags);
			int push_fp_wstr(uls_lex_ptr_t uls, FILE *fp, int flags);
#endif
			static uls_lf_map_t ulscpp_convspec_wmap;

			static int lf_init_convspec_wmap(uls_lf_map_ptr_t lf_map, int flags);
			static void lf_deinit_convspec_wmap(uls_lf_map_ptr_t lf_map);

			uls_lf_t  *str_wlf;
			uls_lf_t  *file_wlf;

			csz_str_t fmtstr2;

			int lf_vxwprintf(uls_voidptr_t x_dat, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args);
			int lf_vsnwprintf(wchar_t *wbuf, int wbuf_siz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args);

			uls_log_t wlogbase;

			int lflog_vwprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, va_list args);
			int lflog_wprintf(csz_str_ptr_t csz, uls_lf_ptr_t uls_lf, const wchar_t *wfmt, ...);
			void lflog_wflush(csz_str_ptr_t csz, uls_voidptr_t data, uls_lf_puts_t lf_puts);
			void lf_vwlog(uls_log_ptr_t log, const wchar_t *wfmt, va_list args);

		public:
			static void initialize();
			static void finalize();

			UlsLexAWStr() = delete;
			virtual ~UlsLexAWStr();

			using UlsLexUStr::pushFile;
			using UlsLexUStr::setFile;

			using UlsLexUStr::pushLine;
			using UlsLexUStr::setLine;

			using UlsLexUStr::getTokStr;
			using UlsLexUStr::setTok;

			using UlsLexUStr::getTag;
			using UlsLexUStr::setTag;
			using UlsLexUStr::dumpTok;

			using UlsLexUStr::ungetStr;
			using UlsLexUStr::ungetTok;

			using UlsLexUStr::changeLiteralAnalyzer;
			using UlsLexUStr::deleteLiteralAnalyzer;
			using UlsLexUStr::changeUldNames;

			using UlsLexUStr::vsnprintf;
			using UlsLexUStr::snprintf;
			using UlsLexUStr::sprintf;

			using UlsLexUStr::vfprintf;
			using UlsLexUStr::fprintf;
			using UlsLexUStr::printf;
			using UlsLexUStr::changeConvSpec;

			using UlsLexUStr::openOutput;
			using UlsLexUStr::vprint;
			using UlsLexUStr::print;

			using UlsLexUStr::vlog;
			using UlsLexUStr::log;
			using UlsLexUStr::panic;

#ifdef __ULS_WINDOWS__
			//
			// UlsLexAStr
			//
			UlsLexAWStr(const char *ulc_file);
			UlsLexAWStr(std::string& ulc_file);

			virtual bool pushFile(const char *filepath, int flags = -1) override;
			virtual bool setFile(const char *filepath, int flags = -1) override;

			virtual bool pushLine(const char *line, int len = -1, int flags = -1) override;
			virtual bool setLine(const char *line, int len = -1, int flags = -1) override;

			virtual void getTag(std::string& fname) override;
			virtual void setTag(const char *fname) override;

			virtual void getTokStr(std::string& lxm) override;
			virtual void setTok(int t, const std::string lxm) override;
			virtual void dumpTok(const char *pfx, const char *suff) override;

			virtual bool ungetStr(const char *str) override;
			virtual bool ungetTok(int tok_id, const char *lxm = NULL) override;

			virtual void changeLiteralAnalyzer(const char *pfx,
				uls_litstr_analyzer_t proc, void *data) override;
			virtual void deleteLiteralAnalyzer(const char *pfx) override;
			virtual void changeUldNames(const char *name, const char *name2,
				int tokid_valid, int tokid, const char *aliases) override;

			virtual int vsnprintf(char *buf, int bufsiz, const char *fmt, va_list args) override;
			virtual int vfprintf(FILE* fp, const char *fmt, va_list args) override;
			virtual void changeConvSpec(const char *percent_name, uls_lf_convspec_t proc) override;

			virtual bool openOutput(const char *out_file) override;
			virtual int vprint(const char *fmt, va_list args) override;
			virtual void vlog(int loglvl, const char *fmt, va_list args) override;

#endif // __ULS_WINDOWS__
			//
			// UlsLexWStr
			//
			UlsLexAWStr(const wchar_t *ulc_wfile);
			UlsLexAWStr(std::wstring& ulc_wfile);

			virtual bool pushFile(const wchar_t *filepath, int flags = -1) override;
			bool pushFile(const std::wstring& filepath, int flags = -1);

			virtual bool setFile(const wchar_t *filepath, int flags = -1) override;
			bool setFile(const std::wstring& filepath, int flags = -1);

			virtual bool pushLine(const wchar_t *line, int len = -1, int flags = -1) override;
			bool pushLine(const std::wstring& line, int flags = -1);

			virtual bool setLine(const wchar_t *line, int len = -1, int flags = -1) override;
			bool setLine(const std::wstring& line, int flags = -1);

			virtual void getTag(std::wstring& fname) override;
			virtual void setTag(const wchar_t *fname) override;
			void setTag(const std::wstring& fname);

			virtual void getTokStr(std::wstring& lxm) override;
			virtual void setTok(int t, const std::wstring lxm) override;
			virtual void dumpTok(const wchar_t *pfx, const wchar_t *suff) override;

			virtual bool ungetStr(const wchar_t *str) override;
			bool ungetStr(const std::wstring& str);

			virtual bool ungetTok(int tok_id, const wchar_t *lxm = NULL) override;
			bool ungetTok(int tok_id, const std::wstring& lxm);

			virtual void changeLiteralAnalyzer(const wchar_t *pfx,
				uls_litstr_analyzer_t proc, void *data) override;
			void changeLiteralAnalyzer(std::wstring& pfx,
				uls_litstr_analyzer_t proc, void *data);

			virtual void deleteLiteralAnalyzer(const wchar_t *pfx) override;
			void deleteLiteralAnalyzer(std::wstring& pfx);

			virtual void changeUldNames(const wchar_t *name, const wchar_t *name2,
				int tokid_valid, int tokid, const wchar_t *aliases) override;
			void changeUldNames(std::wstring& name, std::wstring& name2,
				int tokid_valid, int tokid, std::wstring& aliases);

			virtual int vsnprintf(wchar_t *wbuf, int bufsiz, const wchar_t *wfmt, va_list args) override;
			int snprintf(wchar_t *wbuf, int bufsiz, const wchar_t *wfmt, ...);
			int sprintf(wchar_t * wbuf, const wchar_t *wfmt, ...);

			virtual int vfprintf(FILE* fp, const wchar_t *fmt, va_list args) override;
			int fprintf(FILE* fp, const wchar_t *wfmt, ...);
			int printf(const wchar_t *wfmt, ...);

			virtual void changeConvSpec(const wchar_t *percent_name, uls_lf_convspec_t proc) override;
			void changeConvSpec(const std::wstring& percent_name, uls_lf_convspec_t proc);

			virtual bool openOutput(const wchar_t *out_file) override;
			bool openOutput(const std::wstring& out_file);
			virtual int vprint(const wchar_t *fmt, va_list args) override;
			int print(const wchar_t *fmt, ...);

			virtual void vlog(int loglvl, const wchar_t *fmt, va_list args) override;
			void log(const wchar_t *fmt, ...);
			void panic(const wchar_t *fmt, ...)
#ifdef __GNUC__
__attribute__((noreturn))
#endif
			;
		};
#if defined(__ULS_WINDOWS__) || defined(ULS_USE_WSTR)
		typedef UlsLexAWStr UlsLex;
#else
		typedef UlsLexUStr UlsLex;
#endif
	}
}
