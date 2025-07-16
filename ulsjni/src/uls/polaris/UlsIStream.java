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
  <file> UlsIStream.java </file>
  <brief>
	A wrapper class for streaming lexical token sequence.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Dec. 2013.
  </author>
*/

package uls.polaris;

// <brief>
// UlsIStream is used for reading uls-file(*.uls).
// The base class of UlsIStream is UlsStream.
// </brief>
public class UlsIStream extends UlsStream {
	UlsTmplList tmpl_vars;
	static {
		System.loadLibrary(UlsLex.NameOfDll);
	}

	private static native Object openIFile(String filepath);
	private static native void destroyIFile(Object jistr);

	private void makeIStream(String filepath, UlsTmplList tmpls) {
		uls_hdr = openIFile(filepath);
		if (uls_hdr == null) {
			System.err.println("Can't create in-stream!");
			return;
		}

		tmpl_vars = new UlsTmplList();
		if (tmpls != null) {
			tmpls.exportTmpls(tmpl_vars);
		}
	}

	// <brief>
	// UlsIStream is used as an input file of UlsLex.
	// It's an abstraction of text (program source) file or token sequence file 'uls'.
	// The parameter 'uls_tmpls' is optional.
	// In case that the file is a token sequence file and has template variables,
	//     the parameter 'uls_tmpls' is needed.
	// </brief>
	// <parm name="filepath">inputs of UlsLex</parm>
	// <parm name="uls_tmpls">A list of template variables having its values too.</parm>
	public UlsIStream(String filepath, UlsTmplList tmpls) {
		super(true);
		makeIStream(filepath, tmpls);
	}

	public UlsIStream(String filepath) {
		super(true);
		makeIStream(filepath, null);
	}

	public UlsTmplList getTmplList()
	{
		return tmpl_vars;
	}

	public void clearTmpls()
	{
		tmpl_vars.clear();
	}

	public void addTmpl(String tnam, String tval)
	{
		tmpl_vars.add(tnam, tval);
	}

	public String getTmplValue(String tnam)
	{
		return tmpl_vars.getValue(tnam);
	}

	public Boolean setTmplValue(String tnam, String tval)
	{
		return tmpl_vars.setValue(tnam, tval);
	}

	@Override
	public void close() {
	    if (uls_hdr != null) {
	        destroyIFile(uls_hdr);
	        uls_hdr = null;
	    }
	    tmpl_vars.close();
	    tmpl_vars = null;
	    super.close();
	}
}

