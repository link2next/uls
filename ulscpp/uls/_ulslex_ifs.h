
class _ULS_NAME_IF1 {
public:
	// <brief>
	// Store the formatted string into buffer
	// </brief>
	// <parm name="fmt">The format string</parm>
	// <parm name="args">The list of args</parm>
	// <return>#-of-bytes</return>
	virtual int vsnprintf(LP_TSTR buf, int bufsiz, LP_CTSTR fmt, va_list args) = 0;

	// <brief>
	// Store the formatted string into file
	// </brief>
	// <parm name="fmt">The format string</parm>
	// <parm name="args">The list of args</parm>
	// <return>#-of-bytes</return>
	virtual int vfprintf(FILE* fp, LP_CTSTR fmt, va_list args) = 0;

	// <brief>
	// Add user-defined converion-specification
	// </brief>
	// <parm name="percent_name">the string for conversion-specification</parm>
	// <parm name="proc">user-provided procedure for conversion-specification</parm>
	virtual void changeConvSpec(LP_CTSTR percent_name, uls_lf_convspec_t proc) = 0;

	// <brief>
	// Open a file 'out_file' for writing
	// </brief>
	// <return>success/fail</return>
	virtual bool openOutput(LP_CTSTR out_file) = 0;

	// <brief>
	// Store the formatted string into the internal file
	// </brief>
	// <parm name="fmt">The format string</parm>
	// <parm name="args">The list of args</parm>
	// <return>#-of-bytes</return>
	virtual int vprint(LP_CTSTR fmt, va_list args) = 0;

	// <brief>
	// Emits a log messages the formatted message by the string 'fmt' with 'args'
	// </brief>
	// <parm name="fmt">The format string, a template for formatting</parm>
	// <parm name="args">The list of args</parm>
	virtual void vlog(int loglvl, LP_CTSTR fmt, va_list args) = 0;
};

class _ULS_NAME_IF2 : public uls::crux::_ULS_NAME_IF1 {
public:
	// <brief>
	//   These mothods make an input-file to be ready to tokenized
	//     by putting it on the internal stack.
	// </brief>
	// <parm name="filepath">The file path of input</parm>
	// <parm name="line">string encoded by utf-8</parm>
	// <return>true/false</return>
	virtual bool pushFile(LP_CTSTR filepath, int flags = -1) = 0;
	virtual bool setFile(LP_CTSTR filepath, int flags = -1) = 0;

	virtual bool pushLine(LP_CTSTR line, int len = -1, int flags = -1) = 0;
	virtual bool setLine(LP_CTSTR line, int len = -1, int flags = -1) = 0;

	// <brief>
	//   This will get or update the tag string of current processed file.
	// </brief>
	// <parm name="fname">The current or new tag string</parm>
	virtual void getTag(LP_STRING_ALIAS fname) = 0;
	virtual void setTag(LP_CTSTR fname) = 0;

	// <brief>
	// Returns the current token string internally stored by next().
	// This value is only valid until the next call of next().
	// </brief>
	// <return>the alias of string stored internally</return>
	virtual void getTokStr(LP_STRING_ALIAS lxm) = 0;
	virtual void setTok(int t, LP_CONST_STRING lxm) = 0;

	// <brief>
	//   Call ungetTok() if you want to push a lexeme at the front of the current input source.
	//   ungetStr() pushes back a string in order to get it again
	// </brief>
	// <parm name="tok_id">The token id pushed</parm>
	// <parm name="lxm">The token string with which the token id 'tok_id' is paired.</parm>
	virtual bool ungetTok(int tok_id, LP_CTSTR lxm = NULL) = 0;
	virtual bool ungetStr(LP_CTSTR str) = 0;

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
	virtual void dumpTok(LP_CTSTR pfx, LP_CTSTR suff) = 0;

	// <brief>
	//   Changes the literal-string analyzer to 'proc'.
	//   The 'proc' will be applied to the quote type starting with 'pfx'.
	// </brief>
	// <parm name="pfx">The prefix of literal string that will be processed by 'proc'</parm>
	virtual void changeLiteralAnalyzer(LP_CTSTR pfx,
		uls_litstr_analyzer_t proc, void *data) = 0;
	virtual void deleteLiteralAnalyzer(LP_CTSTR pfx) = 0;

	// <brief>
	//   Changes the token-name and token-id inherited from super class to new ones.
	// </brief>
	// <parm name="name">find the token information named 'name'</parm>
	// <parm name="name2">replace the name with new one</parm>
	virtual void changeUldNames(LP_CTSTR name, LP_CTSTR name2,
		int tokid_valid, int tokid, LP_CTSTR aliases) = 0;
};
