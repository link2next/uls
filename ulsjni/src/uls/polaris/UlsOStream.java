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
  <file> UlsOStream.java </file>
  <brief>
	A wrapper class for creating lexical token sequence file.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Dec. 2013.
  </author>
*/

package uls.polaris;

public class UlsOStream extends UlsStream {
	private static native Object createOFile(String filepath, Object uls, String subname);
	private static native void destroyOFile(Object ostr);
	private static native boolean printTok(Object ostr, int tokid, String tokstr);
	private static native boolean printTokLinenum(Object ostr, int lno, String tag);
	private static native boolean startStream(Object ostr, int flags);

	// <brief>
	// The flag that is defined in the shared library
	// Use this flag in case you want to receive the reserved token LINNUM at the end of line.
	// </brief>
	public static int LINE_NUMBERING;
	public static native int getFlagLineNumbering();

	static {
		System.loadLibrary(UlsLex.NameOfDll);
		LINE_NUMBERING =  getFlagLineNumbering();
	}

	protected UlsLex uls_lex;
	private Boolean do_numbering;

	private Boolean makeOStream(String filepath, UlsLex lex, String subtag, Boolean numbering) {
		uls_lex = lex;
		uls_hdr = createOFile(filepath, uls_lex.getCore(), subtag);

		if (uls_hdr == null) {
			System.err.println("fail to create output stream object!");
			return false;
		}

		do_numbering = numbering;
		return true;
	}

	// <brief>
	// This makes an object that supports write-only sequential IO.
	// Its purpose is to save the token sequence form input source. 
	// </brief>
	// <parm name="filepath">
	// It's a file path for a new uls-stream file.
	// If the file already exists it'll be overwritten.
	// </parm>
	// <parm name="lex">The lexical analyzer associated with uls-stream file</parm>
	// <parm name="subtag">
	// A user provided tag to the file 'filepath'.
	// More often than not, it tends to be the file path.
	// This string will be written to the output uls file.
	// </parm>
	// <parm name="numbering">
	// Specify whether the line number information is inserted or not.
	// The information is automatically inserted whenever the line of source code is changed.
	// </parm>
	public UlsOStream(String filepath, UlsLex lex, String subtag, Boolean numbering) {
		super(false);
		makeOStream(filepath, lex, subtag, numbering);
	}

	// <brief>
	// A constructor of UlsOStream.
	// This creates an uls-file.
	// </brief>
	// <parm name="lex">The lexical analyzer associated with the uls-stream file</parm>
	// <parm name="filepath">uls-stream file(*.uls) path</parm>
	// <parm name="subtag">user provided tag</parm>
	public UlsOStream(String filepath, UlsLex lex, String subtag) {
		super(false);
		makeOStream(filepath, lex, subtag, true);
	}

	public UlsOStream(String filepath, UlsLex lex) {
		super(false);
		makeOStream(filepath, lex, "", true);
	}

	// <brief>
	// This method prints a record of <tokid, tokstr> pair to the output stream.
	// </brief>
	// <parm name="tokid">the token number to be printed</parm>
	// <parm name="tokstr">the lexeme associated with the 'tokid'</parm>
	// <return>none</return>
	public void printTok(int tokid, String tokstr) {
		boolean rval;

		rval = printTok(uls_hdr, tokid, tokstr);
		if (rval == false)
		{
			System.err.println("Can't a print token!");
			System.exit(1);
		}
	}


	// <brief>
	// This method prints an annotation <linenum, tag> pair to the output file.
	// </brief>
	// <parm name="lno">the line number of the source file</parm>
	// <parm name="tagstr">the tag of the source file</parm>
	// <return>none</return>
	public void printTokLineNum(int lno, String tagstr) {
		boolean rval;

		rval = printTokLinenum(uls_hdr, lno, tagstr);
		if (rval == false) {
			System.err.println("Can't a print linenum-token!");
			System.exit(1);
		}
	}

	// <brief>
	// Start writing the lexical streaming with input-stream 'ifile'.
	// </brief>
	// <parm name="ifile">
	// This input uls-stream will be written to the 'UlsOStream' object.
	// </parm>
	public Boolean startStream(UlsIStream ifile) {
		int flags=0;
		if (uls_hdr == null || read_only == true) return false;

		if (do_numbering) flags |= LINE_NUMBERING;

		if (uls_lex.pushInput(ifile) == false) {
			return false;
		}

		if (startStream(uls_hdr, flags) == false) {
			return false;
		}

		return true;
	}

	// <brief>
	// Closes the stream (file).
	// </brief>
	@Override
	public void close() {
		if (uls_hdr != null) {
			destroyOFile(uls_hdr);
			uls_hdr = null;
		}
		super.close();
	}
}

