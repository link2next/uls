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
  <file> UlsTestStream.java </file>
  <author>
    Stanley Hong <link2next@gmail.com>, Dec. 2013.
  </author>
*/

package uls.tests;

import uls.polaris.*;

public class UlsTestStream {
	static String data_dir = ".";
	int tok_id;

	public void dump_input_file(Sample1Lex sam_lex, String infile) {
		sam_lex.pushFile(infile);

		while ((tok_id=sam_lex.getTok()) != sam_lex.EOI) {
			sam_lex.dumpTok("\t", "\n");
		}
	}

	public void read_stream_file(Sample1Lex sam_lex, String Tval, String infile) {
		UlsTmplList tmpls = new UlsTmplList();

		if (Tval.equals("")) {
		} else {
			tmpls.add("T", Tval);
		}

		UlsIStream in_hdr = new UlsIStream(infile, tmpls);

		try {
			sam_lex.pushInput(in_hdr);
			while ((tok_id=sam_lex.getTok()) != sam_lex.EOI) {
				sam_lex.dumpTok("\t", "\n");
			}
		} finally {
			in_hdr.close();
			tmpls.close();
		}
	}

	public Boolean test_streaming(Sample1Lex sam_lex, String input_file)
	{
		String uls_file = data_dir + "/a2.uls";
		String lxm;
		int t;

		UlsOStream ofile = new UlsOStream(uls_file, sam_lex, "<<tag>>");
		UlsIStream ifile = new UlsIStream(input_file);

		try {

			ofile.startStream(ifile);
		} finally {
			ofile.close();
			ifile.close();
		}

		ifile = new UlsIStream(uls_file);
		try {
			sam_lex.pushInput(ifile);

			for ( ; ; ) {
				t = sam_lex.next();
				if (t == sam_lex.toknum_EOI) break;
				lxm = sam_lex.lexeme;
				System.out.println("\t[" + t + "] " + lxm);
			}
		} finally {
			ifile.close();
		}

	    return true;
	}

	public Boolean test_tokseq(Sample1Lex sam_lex, String input_file)
	{
		UlsTmplList  TmplLst = new UlsTmplList();
		String uls_file = data_dir + "/a3.uls";
		String lxm;
		int t;

		TmplLst.add("TVAR1", "unsigned long long");
		TmplLst.add("TVAR2", "long double");

		// Write a output-stream
		UlsOStream ofile = new UlsOStream(uls_file, sam_lex, "<<tag>>");

		try {
		    sam_lex.pushFile(input_file);

		    for ( ; ; ) {
			    t = sam_lex.getTok();

			    if (t == sam_lex.toknum_ERR) {
				    return false;
			    }

			    if (t == sam_lex.toknum_EOF || t == sam_lex.toknum_EOI) break;
			    lxm = sam_lex.lexeme;

			    if (t == sam_lex.toknum_ID && TmplLst.exist(lxm) == true) {
				    t = sam_lex.toknum_TMPL;
			    }

			    ofile.printTok(t, lxm);
		  	}
		} finally {
			ofile.close();
			TmplLst.close();
		}

		UlsIStream ifile = new UlsIStream(uls_file, TmplLst);

		try {
		    sam_lex.pushInput(ifile);

		    for ( ; ; ) {
			    t = sam_lex.getTok();
			    if (t == sam_lex.toknum_EOI) break;
			    lxm = sam_lex.lexeme;
			    System.out.println("\t[" + t + "] " + lxm);
		    }

		} finally {
			ifile.close();
		}

	    return true;
	}

	public static void main(String args[]) {
		String ulc_path, input_file;
		UlsTestStream  tst_obj = new UlsTestStream();

		if (args.length < 1) {
			System.err.println("usage: UlsTestStream [data_dir]");
			return;
		}

		if (args.length >= 1) {
			data_dir = args[0];
		} else {
			data_dir = ".";
		}

		ulc_path = data_dir + "/sample.ulc";
//		System.out.println("ulc_path = " + ulc_path);

		Sample1Lex sam_lex = new Sample1Lex(ulc_path);
		System.out.println("** Test Start");

		try {
			// Test-1: read the input file.
			input_file = "tmpl_ex.txt";
			System.out.println("Test-1: input_file = " + input_file);
			input_file = data_dir + "/" + input_file;
			tst_obj.dump_input_file(sam_lex, input_file);

			// Test-2: read the input file replacing the template var.
			input_file = "tmpl_ex.uls";
			System.out.println("Test-2: input_file = " + input_file);
			input_file = data_dir + "/" + input_file;
			String tmpl_val = "unsigned long long";
			tst_obj.read_stream_file(sam_lex, tmpl_val, input_file);

			// Test-3:
			input_file = "tmpl_ex.txt";
			System.out.println("Test-3: input_file = " + input_file);
			input_file = data_dir + "/" + input_file;
			tst_obj.test_streaming(sam_lex, input_file);

			// Test-4:
			input_file = "tmpl_ex.txt";
			System.out.println("Test-4: input_file = " + input_file);
			input_file = data_dir + "/" + input_file;
			tst_obj.test_tokseq(sam_lex, input_file);

		} finally {
			sam_lex.close();
		}

		System.out.println("** Test End");
	}
}

