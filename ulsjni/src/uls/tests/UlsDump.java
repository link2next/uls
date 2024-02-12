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
  <file> UlsDump.java </file>
  <author>
    Stanley Hong <link2next@gmail.com>, Dec. 2013.
  </author>
*/

package uls.tests;

public class UlsDump {
	static sample_xdef[] xdefs;
	int tok_id;

	public void test_basic(Sample1Lex sam_lex) {
		sam_lex.pushInput("Hello Java 1.8 world\n\t\n");

		while (sam_lex.getToken() != sam_lex.EOI) {
			sam_lex.dumpTok("\t", "\n");
		}
	}

	public void test_input_file(Sample1Lex sam_lex, String filename) {
		String str2;

		sam_lex.pushFile(filename);

		while ((tok_id=sam_lex.getToken()) != sam_lex.EOI) {
			if (tok_id == sam_lex.NUMBER) {

				if (sam_lex.isLexemeReal()) {
					double ff = sam_lex.lexemeAsDouble();
					str2 = " ---(str2double)-->> " + ff + "\n";

				} else {
					int ii = sam_lex.lexemeAsInt();
					str2 = " ---(str2int)-->> " + ii + "\n";
				}

			} else {
				str2 = "\n";
			}
			sam_lex.dumpTok("....", str2);
		}
	}

	public void init_xdefs()
	{
		sample_xdef xdef;
		
		xdef = new sample_xdef();
		xdef.tok_id = '*';
		xdef.prec = 27;
		xdef.node_id = 1;
		xdefs[0] = xdef;
		
		xdef = new sample_xdef();
		xdef.tok_id = '+';
		xdef.prec = 26;
		xdef.node_id = 2;
		xdefs[1] = xdef;
		
		xdef = new sample_xdef();
		xdef.tok_id = '/';
		xdef.prec = 25;
		xdef.node_id = 3;
		xdefs[2] = xdef;
		
		xdef = new sample_xdef();
		xdef.tok_id = '&';
		xdef.prec = 24;
		xdef.node_id = 4;
		xdefs[3] = xdef;
		
		xdef = new sample_xdef();
		xdef.tok_id = '^';
		xdef.prec = 23;
		xdef.node_id = 5;
		xdefs[4] = xdef;
	}

	public void test_xdefs(Sample1Lex sam_lex, String fpath)
	{
		sample_xdef  xdef;

		for (int i = 0; i < 5; i++) {
			xdef = xdefs[i];
			System.out.println(" ** xdefs[ " + i + "] = " + xdef.tok_id);
			sam_lex.setExtraTokdef(xdef.tok_id, xdef);
		}

		sam_lex.pushFile(fpath);

		for ( ; ; ) {
			if (sam_lex.getTok() == sam_lex.EOI) break;
			xdef = (sample_xdef) sam_lex.getExtraTokdef();

			sam_lex.dumpTok("\t", "");

			if (xdef != null)
				System.out.println(" prec=" + xdef.prec + " node_id=" + xdef.node_id);
			else
				System.out.println("");
        }
	}

	static void test_sample1_lex(String data_dir, String input_file) {
		String ulc_filename = "sample.ulc";

		System.out.println(" ulc_path = " + ulc_filename);
		System.out.println(" input_file = " + input_file);

		String ulc_path = data_dir + "/" + ulc_filename;
		input_file = data_dir + "/" + input_file;

		try (
			Sample1Lex sam_lex = new Sample1Lex(ulc_path);
		) {
			UlsDump  tst_obj = new UlsDump();
			System.out.println(" -- Input: String -------------------------");
			tst_obj.test_basic(sam_lex);

			System.out.println(" -- Input: File -------------------------");
			tst_obj.test_input_file(sam_lex, input_file);

			System.out.println(" -- Testing Extra Tokdefs -------------------------");
			xdefs = new sample_xdef[5];
			tst_obj.init_xdefs();
			tst_obj.test_xdefs(sam_lex, input_file);
		} catch (Exception ex) {
			System.err.println("IOException !!" + ex.getMessage());
		}
	}

	static void test_sample2_lex(String data_dir, String input_file) {
		String uld_filename = "sample2.uld";

		System.out.println(" ulc_path = " + uld_filename);
		String ulc_path = data_dir + "/" + uld_filename;

		try (
			Sample1Lex sam_lex = new Sample1Lex(ulc_path);
		) {
			UlsDump  tst_obj = new UlsDump();

			System.out.println(" input_file = " + input_file);
			input_file = data_dir + "/" + input_file;

			System.out.println(" -- Input: File -------------------------");
			tst_obj.test_input_file(sam_lex, input_file);
		} catch (Exception ex) {
			System.err.println("IOException !!" + ex.getMessage());
		}
	}

	public static void main(String args[]) {
		String data_dir, input_file;

		if (args.length < 1) {
			System.err.println("usage: UlsDump <input_file> [data_dir]");
			return;
		}
	
		input_file = args[0];

		if (args.length >= 2) {
			data_dir = args[1];
		} else {
			data_dir = ".";
		}

		test_sample1_lex(data_dir, input_file);
		test_sample2_lex(data_dir, input_file);
	}
}

