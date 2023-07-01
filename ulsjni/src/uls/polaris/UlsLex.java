/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
  <file> UlsLex.java </file>
  <brief>
	A wrapper class of uls lexical analyzer.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Dec. 2013.
  </author>
*/

package uls.polaris;

import java.io.File;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.lang.Integer;

public class UlsLex implements AutoCloseable {
	public static String PathOfDll="";
	public static String NameOfDll="";
	public static String FileEnc="";

	private static native Object createUls(String confname);
	private static native boolean destroyUls(Object juls);

	private static native int toknumEOI(Object juls);
	private static native int toknumEOF(Object juls);
	private static native int toknumERR(Object juls);
	private static native int toknumNONE(Object juls);
	private static native int toknumID(Object juls);
	private static native int toknumNUMBER(Object juls);
	private static native int toknumTMPL(Object juls);

	private static native int tokNum(Object juls);
	private static native String tokStr(Object juls);
	private static native String lexeme(Object juls);
	private static native int getTok(Object juls);

	private static native boolean isLexemeReal(Object juls);
	private static native boolean isLexemeInt(Object juls);
	private static native boolean isLexemeZero(Object juls);
	private static native boolean isQuoteTok(Object juls, int tokid);

	private static native int toInt(Object juls);
	private static native long toLong(Object juls);
	private static native double toReal(Object juls);

	private static native String numberSuffix(Object juls);
	private static native void expect(Object juls, int tokExpected);

	private static native void ungetStr(Object juls, String str);
	private static native void ungetTok(Object juls, String lxm, int tokid);
	private static native void ungetCh(Object juls, int ch);

	private static native void dumpTok(Object juls, String pfx, String suff);
	private static native String tok2keyw(Object juls, int tokid);
	private static native String tok2name(Object juls, int tokid);

	private static native boolean isCharSpace(Object juls, int ch);
	private static native boolean isCharIdFirst(Object juls, int ch);
	private static native boolean isCharId(Object juls, int ch);
	private static native boolean isCharQuote(Object juls, int ch);
	private static native boolean isCharTokCh1(Object juls, int ch);
	private static native boolean isCharTokCh2(Object juls, int ch);

	private static native void skipWhiteSpaces(Object juls);
	private static native Object peekCh(Object juls);
	private static native Object getCh(Object juls);
	private static native int uchFromNextchInfo(Object jch_detail);
	private static native int tokFromNextchInfo(Object jch_detail);
	private static native void putNextchInfo(Object jch_detail);

	private static native String getTag(Object juls);
	private static native void setTag(Object juls, String tag, int lineno);
	private static native int getLineno(Object juls);
	private static native void setLineno(Object juls, int lineno);
	private static native void incLineno(Object juls, int amount);

	private static native boolean pushLine(Object juls, String line, int flags);
	private static native boolean pushFile(Object juls, String filepath, int flags);
	private static native boolean pushIStream(Object juls, Object istr, Object tmpls, int flags);
	private static native boolean setLine(Object juls, String line, int flags);
	private static native boolean setFile(Object juls, String filepath, int flags);

	private static native void popInput(Object juls);
	private static native void popAllInputs(Object juls);

	private static native int[] getTokIdList(Object juls);

	public static native int getFlagWantEofTok();
	public static native int getFlagStreamBinLe();
	public static native int getFlagStreamBinBe();
	public static native int getFlagNextChNone();
	public static native int getFlagTmplsDup();

	// <brief>
	// Sets class variables 'NameOfDll', PathOfDll called by class initializer.
	// </brief>
	static void get_dll_path() {
		String sysprops_fpath1, sysprops_fpath, os_name;
		String sysprops_fname = "uls_sysprops.txt";
		int ind;

		NameOfDll = "ulsjni";
		os_name = System.getProperty("os.name");
		if (os_name.indexOf("indow")>=0) {
			sysprops_fpath = System.getenv("CommonProgramFiles");
			sysprops_fpath += "\\UlsWin\\";
			NameOfDll = "libulsjni";
		} else if (os_name.indexOf("inux")>=0 || os_name.indexOf("nix")>=0) {
			sysprops_fpath = "/usr/local/etc/uls/";
			sysprops_fname = "uls.sysprops";

			sysprops_fpath1 = sysprops_fpath + sysprops_fname;
			File file1 = new File(sysprops_fpath1);
			if (file1.exists() == false || file1.isFile() == false) {
				sysprops_fpath = "/tmp/";
				sysprops_fname = "uls_sysprops.txt";
			}
		} else if (os_name.indexOf("Mac")>=0) {
			sysprops_fpath = "/Applications/";

			sysprops_fpath1 = sysprops_fpath + sysprops_fname;
			File file1 = new File(sysprops_fpath1);
			if (file1.exists() == false || file1.isFile() == false) {
				sysprops_fpath = "/tmp/";
				sysprops_fname = "uls_sysprops.txt";
			}
		} else {
			sysprops_fpath = "/tmp/";
		}

		sysprops_fpath += sysprops_fname;

		BufferedReader infile = null;

		try {
			infile = new BufferedReader(new FileReader(sysprops_fpath));
			String line, name, val;

			while ((line=infile.readLine()) != null) {
				line = line.trim();
				if (line.equals("")) continue;

				if ((ind=line.indexOf('=')) < 0) {
					throw new IOException("can't read " + sysprops_fpath);
				}

				name = line.substring(0,ind);
				val = line.substring(ind+1);

				if (name.equals("ULS_DLLPATH")) {
					PathOfDll = val;
					break;
				}
			}

		} catch (IOException e) {
			System.err.println("Error: " + e);

		} finally {
			if (infile != null) {
				try {
					infile.close();
				} catch (IOException e) {
					System.err.println("Error: " + e);
					System.exit(1);
				}
			}
		}
	}

	// <brief>
	// The flag that is defined in the shared library.
	// WANT_EOFTOK : Use this flag in case you want to receive the reserved token EOFat the end of file.
	// </brief>
	public static int WANT_EOFTOK;

	// <brief>
	// The flag used to check the byte-ordef of 'stream file',
	// It's defined in the shared library.
	// </brief>
	public static int STREAM_BIN_LE, STREAM_BIN_BE;

	// <brief>
	// The constant is used to examine the return-value of getCh(), peekCh().
	// The method getCh, peekCh returns NEXTCH_NONE if they comes across a literal-string or EOF, EOI.
	// </brief>
	public static int NEXTCH_NONE;

	// <brief>
	// The constant is used to duplicate the list of template variables.
	// </brief>
	public static int TMPLS_DUP;

	static {
		get_dll_path();
		System.setProperty("jna.library.path", PathOfDll);
		FileEnc = System.getProperty("file.encoding");

		System.loadLibrary(NameOfDll);

		WANT_EOFTOK = getFlagWantEofTok();

		STREAM_BIN_LE = getFlagStreamBinLe();
		STREAM_BIN_BE = getFlagStreamBinBe();
		NEXTCH_NONE = getFlagNextChNone();
		TMPLS_DUP = getFlagTmplsDup();
	}

	// <brief>
	// The embedded structure created by the C-library.
	// </brief>
	private Object uls;

	// <brief>
	// The dictionary to store the user provided object for each token definition.
	// </brief>
	private HashMap<Integer,Object> Dic4ExtraTokdef;

	// <brief>
	// The reserved token numbers defined in the ulc file.
	// </brief>
	public int toknum_EOI, toknum_EOF, toknum_ERR; // read-only
	public int toknum_NONE, toknum_ID, toknum_NUMBER, toknum_TMPL; // read-only

	// This returns the current token number.
	// It can be directly read as declared as public member.
	// It's set to toknum_NONE when the object is created.
	// </brief>
	// <return>token number</return>
	public int TokNum, token;  // read-only

	// <brief>
	// This returns the current token string.
	// It can be directly read as declared as public property.
	// </brief>
	public String TokStr, lexeme; // read-only

	// <brief>
	// Informs user that the current token is toknum_EOI
	// </brief>
	public Boolean isEOI; // read-only

	// <brief>
	// Informs user that the current token is toknum_EOF.
	// </brief>
	public Boolean isEOF; // read-only

	// <brief>
	// These flags is passed to pushInput()
	// 1. WantEOF: At the end of file, getTok() will give you the EOF-token.
	public enum InputOpts
	{
		WantEOF, Unknown
	}

	private int input_flags = 0;

	private int get_uls_flags(InputOpts fl)
	{
		int ret_fl;

		switch (fl) {
		case WantEOF:
			ret_fl = WANT_EOFTOK;
			break;
		default:
			ret_fl = -1;
			break;
		}

		return ret_fl;
	}

	// <brief>
	// setInputOpts() sets the the intenal flag 'input_flags' to the flag 'fl'
	// </brief>
	public void setInputOpt(InputOpts fl)
	{
		int uls_fl = get_uls_flags(fl);
		input_flags |=  uls_fl;
	}

	// <brief>
	// clearInputOpts() clears the flag 'fl' in the the intenal flag 'input_flags'.
	// </brief>
	public void clearInputOpt(InputOpts fl)
	{
		int uls_fl = get_uls_flags(fl);
		input_flags &=  ~uls_fl;
	}

	// <brief>
	// getInputOpts() will just return the intenal flag 'input_flags'.
	// </brief>
	public int getInputOpts()
	{
		return input_flags;
	}

	// <brief>
	// resetInputOpts will clear the internal 'input_flags'.
	// </brief>
	public void resetInputOpts()
	{
		input_flags = 0;
	}

	public Object getCore() {
		return uls;
	}

	// <brief>
	// It updates the token information after advancing the cursor of the input buffer.
	// It changes the state of the analyzer, TokNum, TokStr, FileName, LineNum, ..
	// </brief>
	// <return>token id</return>
	private int update_token_lex() {
		int t;

		TokNum = token = t = getTok(uls);
		TokStr = lexeme = lexeme(uls);

		isEOI = (t == toknum_EOI);
		isEOF = (t == toknum_EOF);

		return t;
	}

	// <brief>
	// This is a constructor that creates an object for lexical analysis.
	// The 'ulc_file' is a name of language specification in the ulc system repository or
	//   simply a file path of ulc file.
	// To see the available ulc names, use the -q-option of the command 'ulc2class'.
	// </brief>
	// <parm name="ulc_file">The name/path of the lexical configuration.</parm>
	// <return>none</return>
	public UlsLex(String ulc_file) {
		try {
			uls = createUls(ulc_file);
		} catch (Exception e) {
			System.err.println(e);
			System.exit(1);
		}

		if (uls == null) {
			System.err.println("Can't open the file '" + ulc_file + "'");
			System.exit(1);
		}

		toknum_EOI = toknumEOI(uls);
		toknum_EOF = toknumEOF(uls);
		toknum_ERR = toknumERR(uls);
		toknum_NONE = toknumNONE(uls);
		toknum_ID = toknumID(uls);
		toknum_NUMBER = toknumNUMBER(uls);
		toknum_TMPL = toknumTMPL(uls);

		Dic4ExtraTokdef = new HashMap<Integer,Object>();

		int[] tokid_jarr = getTokIdList(uls);

		for (int i = 0; i < tokid_jarr.length; i++) {
			int t = tokid_jarr[i];
			Dic4ExtraTokdef.put(t, null);
		}

		update_token_lex();
	}

	// <brief>
	// move the cursor of input to the first nonblank character.
	// </brief>
	// <return>the character to which the cursor is pointing.</return>
	public int skipBlanks() {
		skipWhiteSpaces(uls);
		return peekCh();
	}

	// <brief>
	// return the character to which the cursor of input is pointing.
	// This getCh() will advance the cursor to the next character.
	// If the cursor is pointing to a part of literal string,
	//    it'll just return NEXTCH_NONE without advancing the cursor.
	// </brief>
	// <return>the character to which the cursor is pointing.</return>
	public int peekCh() {
		Object jch_detail;
		int ch;

		jch_detail = peekCh(uls);
		ch = uchFromNextchInfo(jch_detail);
		putNextchInfo(jch_detail);

		return ch;
	}

	// <brief>
	// return the character to which the cursor of input is pointing.
	// This getCh() will advance the cursor to the next character.
	// If the cursor is pointing to a part of literal string,
	//    it'll just return NEXTCH_NONE without advancing the cursor.
	// </brief>
	// <return>the character to which the cursor is pointing.</return>
	public int getCh() {
		Object jch_detail;
		int ch;

		jch_detail = getCh(uls);
		ch = uchFromNextchInfo(jch_detail);
		putNextchInfo(jch_detail);

		return ch;
	}

	// <brief>
	// Checks whether 'tok_id' is an literal string or not
	// </brief>
	// <parm name="tok_id">The number to examine</parm>
	// <return>true if the 'tok_id' belongs to the gourp of literal-string tokens.</return>
	public Boolean isQuoteTok(int tok_id) {
		return isQuoteTok(uls, tok_id);
	}

	// <brief>
	// push back the 'ch' into the input-buffer
	//     so that the next call of getCh, getTok will consider it.
	// </brief>
	// <parm name="ch">The character to push back</parm>
	public void ungetCh(char ch) {
		if (ch != NEXTCH_NONE) {
			ungetCh(uls, ch);
			update_token_lex();
		}
	}

	// <brief>
	// Checks if the 'ch' is a space character in the context of the current lexical object.
	// </brief>
	// <parm name="ch">The character to examine</parm>
	// <return>true/false</return>
	public Boolean isSpace(char ch) {
		return isCharSpace(uls, ch);
	}

	// <brief>
	// Checks if the 'ch' is a member of first character group of identifier
	// in the context of the current lexical object.
	// </brief>
	// <parm name="ch">The character to examine</parm>
	// <return>true/false</return>
	public Boolean isIdfirst(char ch) {
		return isCharIdFirst(uls, ch);
	}

	// <brief>
	// Checks if the 'ch' is a part of identifier
	// in the context of the current lexical object.
	// </brief>
	// <parm name="ch">The character to examine</parm>
	// <return>true/false</return>
	public Boolean isId(char ch) {
		return isCharId(uls, ch);
	}

	// <brief>
	// Checks if the 'ch' is a member of the first character group of literal-strings.
	// </brief>
	// <parm name="ch">The character to examine</parm>
	// <return>true/false</return>
	public Boolean isQuote(char ch) {
		return isCharQuote(uls, ch);
	}

	// <brief>
	// Checks if the 'ch' is a punctuation character
	//    in the context of the current lexical object.
	// </brief>
	// <parm name="ch">The character to examine</parm>
	// <return>true/false</return>
	public Boolean is1CharTok(char ch) {
		return isCharTokCh1(uls, ch);
	}

	// <brief>
	// Checks if the 'ch' is a part of lexeme that consists of
	//     two or more punctuation characters.
	// </brief>
	// <parm name="ch">The character to examine</parm>
	// <return>true/false</return>
	public Boolean is2CharTok(char ch) {
		return isCharTokCh2(uls, ch);
	}

	// <brief>
	// This is one of the main methods of the uls lexical analyzer object.
	// It gets the next token and its associated lexeme by advancing the cursor of input.
	//
	// At the end of input you will get the reserved token EOI.
	// Notice the last token number should be compared with the field 'toknum_EOI'
	//    as it's a dynamic number varied from each lexical configuration.
	//
	// Even if it returns the token number, you can get it again from the object.
	// Use the property 'TokNum' to get the current token number and
	//    the property 'TokStr' to get the associated lexeme.
	// </brief>
	// <return>the token number</return>
	public int getTok() {
		return update_token_lex();
	}

	// <brief>
	// An alias of update_token_lex().
	// </brief>
	// <return>token id</return>
	public int getToken() {
		return update_token_lex();
	}

	// <brief>
	// An alias of update_token_lex().
	// </brief>
	// <return>token id</return>
	public int next() {
		return update_token_lex();
	}

	// <brief>
	// In case that the current token is NUMBER, this'll return the suffix of the number if it exists.
	// It's obvious that suffix of number is to inform explicitly compiler
	//     of its data types in programming language.
	// </brief>
	// <return>suffix string</return>
	public String lexemeNumberSuffix()
	{
		String str;
		str = numberSuffix(uls);
		if (str == null) str = "";
		return str;
	}

	// <brief>
	// This method checks if the lexeme of the current token is a floating-point number.
	// </brief>
	public Boolean isLexemeReal()
	{
		return isLexemeReal(uls);
	}

	// <brief>
	// This method checks if the lexeme of the current token is an integer.
	// </brief>
	public Boolean isLexemeInt()
	{
		return isLexemeInt(uls);
	}

	// <brief>
	// This method checks if the lexeme of the current token is zero.
	// </brief>
	public Boolean isLexemeZero()
	{
		return isLexemeZero(uls);
	}

	// <brief>
	// lexemeAsInt() will recognize the current token string as a primitive type 'int'.
	// It returns the value after converting the lexeme to the primitive type 'int'.
	// Make sure the current token is a number.
	// </brief>
	// <return>primitive data type 'int'</return>
	public int lexemeAsInt32()
	{
		return toInt(uls);
	}

	public int lexemeAsInt()
	{
		return lexemeAsInt32();
	}

	// <brief>
	// lexemeAsLong() will recognize the current lexeme as a primitive type 'long'.
	// It returns the value after converting the lexeme to the primitive type.
	// Make sure the current token is a number.
	// </brief>
	// <return>primitive data type 'long'</return>
	public long lexemeAsInt64()
	{
		return toLong(uls);
	}

	public long lexemeAsLong()
	{
		return lexemeAsInt64();
	}

	// <brief>
	// lexemeAsDouble() will recognize the current lexeme as a primitive type 'double'.
	// It returns the value after converting the lexeme to the primitive type.
	// Make sure the current token is a number.
	// </brief>
	// <return>primitive data type 'double'</return>
	public double lexemeAsDouble()
	{
		return toReal(uls);
	}

	// <brief>
	// This method 'expect' the current token to be 'TokExpected'
	// If not, the current thread will be terminated abnormally.
	// </brief>
	// <parm name="TokExpected">The expected token number</parm>
	public void expect(int TokExpected) {
		expect(uls, TokExpected);
	}

	// <brief>
	// Pushes the 'str' into the buffer
	//     so that the next call will consider it.
	// </brief>
	// <parm name="str">The string to push back</parm>
	public void ungetStr(String str) {
		ungetStr(uls, str);
		update_token_lex();
	}

	// <brief>
	// Pushes the 'tok_id' and it lexeme 'lxm' into the buffer
	//     so that the next call will consider it.
	// </brief>
	// <parm name="lxm">The lexeme to push back</parm>
	// <parm name="tok_id">The token number to push back</parm>
	public void ungetTok(String lxm, int tok_id) {
		ungetTok(uls, lxm, tok_id);
		update_token_lex();
	}

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
	public void dumpTok(String pfx, String suff) {
		dumpTok(uls, pfx, suff);
	}

	// <brief>
	// The dumpTok() without parameters will prepend '\t' and append '\n' to the string.
	// </brief>
	public void dumpTok() {
		dumpTok("\t", "\n");
	}

	// <brief>
	// Returns the keyword string corresponding to the token number 't'
	// </brief>
	// <parm name="t">A token number</parm>
	// <return>keyword string</return>
	public String keyword(int t) {
		String keyw = tok2keyw(uls, t);

		if (keyw == null) keyw = "<none>";
		return keyw;
	}

	// <brief>
	// Returns the keyword string corresponding to the current token number.
	// </brief>
	// <return>keyword string</return>
	public String keyword() {
		return keyword(TokNum);
	}

	// <brief>
	// Returns the name string corresponding to the token number 't'
	// The name is defined in the ulc-file
	// </brief>
	// <parm name="t">A token number</parm>
	// <return>The token name string defined in the ulc-file</return>
	public String nameOf(int t) {
		String name = tok2name(uls, t);

		if (name  == null) name = "<none>";
		return name;
	}

	public String nameOf() {
		return nameOf(TokNum);
	}

	// <brief>
	// The field 'LineNum' is automatically updated by calling getTok() but
	//     if you want to change it forcibly use this method.
	// </brief>
	// <parm name="lineno">The new value of 'LineNum' to be updated</parm>
	public void setLineNum(int lineno) {
		setLineno(uls, lineno);
	}

	public int getLineNum() {
		return getLineno(uls);
	}

	// <brief>
	// Use this to add some number to the current line number 'LineNum' forcibly.
	// If the resultant line number is negative the 'LineNum' won't be updated.
	// </brief>
	// <parm name="amount">The amount of lines to be added. It may be negative</parm>
	public void addLineNum(int amount) {
		incLineno(uls, amount);
	}

	// <brief>
	// This makes user set forcibly the tag of current input.
	// </brief>
	// <parm name="fname">the tag to be set</parm>
	public void setTag(String tag) {
		setTag(uls, tag, 1);
	}

	public void setFileName(String fname) {
		setTag(fname);
	}

	// <brief>
	// This makes user set forcibly the tag of current input.
	// </brief>
	// <parm name="fname">the tag to be set</parm>
	public String getTag() {
		return getTag(uls);
	}

	public String getFileName() {
		return getTag();
	}

	// <brief>
	// Pushes the parameter 'line' onto the current input.
	// The current input buffer of 'line' will be the top of stack of input-buffers.
	// </brief>
	// <parm name="line">A new input that you want to tokenize</parm>
	public void pushInput(String line) {
		int flags = getInputOpts();
		boolean rval;

		rval = pushLine(uls, line, flags);
		if (rval == false) {
			System.err.println("Invalid input string!");
			System.exit(1);
		}
	}

	public void pushLine(String line, Boolean want_eof) {
		int flags = 0;

		if (want_eof) {
			flags |= WANT_EOFTOK;
		}

		pushLine(uls, line, flags);
	}

	public void setLine(String line, Boolean want_eof) {
		int flags = 0;

		if (want_eof) {
			flags |= WANT_EOFTOK;
		}

		setLine(uls, line, flags);
	}

	// <brief>
	// The file content will be the top of stack of input-buffers.
	// When the cursor reaches the end-of-file of 'filepath',
	//    the input buffer of the stack will be poped.
	// </brief>
	// <parm name="filepath">A new input that you want to tokenize</parm>
	public Boolean pushFile(String filepath) {
		int flags = getInputOpts();
		boolean rval;

		rval = pushFile(uls, filepath, flags);
		return rval;
	}

	public void pushFile(String filepath, Boolean want_eof) {
		int flags = 0;

		if (want_eof) {
			flags |= WANT_EOFTOK;
		}

		try {
			pushFile(uls, filepath, flags);
		} catch (Exception e) {
			System.err.println(e);
			System.exit(1);
		}
	}

	public void setFile(String filepath, Boolean want_eof) {
		int flags = 0;

		if (want_eof) {
			flags |= WANT_EOFTOK;
		}

		try {
			setFile(uls, filepath, flags);
		} catch (Exception e) {
			System.err.println(e);
			System.exit(1);
		}
	}

	// <brief>
	// This method will push an input string 'istr' on the top of the input stack.
	// Then the getTok() method can be used to get the tokens from the input.
	// You can also create the input object from text file, uls-file, or literal-string.
	// </brief>
	// <parm name="ifile">input stream object!</parm>
	public Boolean pushInput(UlsIStream ifile) {
		int flags = getInputOpts();
		Object tmpl_list;
		boolean rval;

		UlsTmplList TmplList = ifile.getTmplList();
		if (TmplList != null) {
			tmpl_list = TmplList.exportTmpls();
		} else {
			tmpl_list = null;
			return false;
		}

		rval = pushIStream(uls, ifile.getCore(), tmpl_list, flags);
		if (rval == false) {
			System.err.println("Can't open tmpl-stream!");
			return false;
		}

		return true;
	}
	// <brief>
	// popInput() dismisses the current input source.
	// </brief>
	// <return>none</return>
	public void popInput()
	{
		popInput(uls);
	}

	// <brief>
	// popAllInputs() dismisses all the input sources and goes back to the initial state.
	// In the initial state you will get the EOI as current token.
	// </brief>
	public void popAllInputs()
	{
		popAllInputs(uls);
	}

	// <brief>
	// This sets extra token definition 'extra_tokdef' which is provided by user.
	// The stored data of token number 't' can be later retrieved by getExtraTokdef().
	// </brief>
	// <parm name="t">The target token number with which user data is associated</parm>
	// <parm name="o">The user defined token information in form of java object</parm>
	public void setExtraTokdef(int t, Object o) {
		// Overriding the existing one ...
		try {
			Dic4ExtraTokdef.put(t, o);
		} catch (Exception e) {
			System.err.println("Can't insert token (exta) information for tok:" + t + "!");
		}
	}

	// <brief>
	// Use this method to get the user-provided token data,
	//    which has been stored by setExtraTokdef().
	// </brief>
	// <parm name="t">The target token id of data the you want to retrieve.</parm>
	// <return>The object which is associated with token 't'</return>
	public Object getExtraTokdef(int t) {
		Object o = null;
		try {
			if (Dic4ExtraTokdef.containsKey(t)) {
				o = Dic4ExtraTokdef.get(t);
			} else {
				System.err.println("Key:" + t + "Not Found!");
			}
		} catch (Exception e) {
			System.err.println("Can't get token (exta) information for tok:" + t + "!");
		}
		return o;
	}

	// <brief>
	// This returns the user-provided data associated with the current token.
	// It's equivalent to 'getExtraTokdef(TokNum)'.
	// </brief>
	public Object getExtraTokdef() {
		return getExtraTokdef(TokNum);
	}

	@Override
	public void close() {
		destroyUls(uls);
	}
}

