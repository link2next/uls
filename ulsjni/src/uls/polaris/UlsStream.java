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
  <file> UlsStream.java </file>
  <brief>
	A wrapper class for streaming lexical token sequence.
  </brief>
  <author>
    Stanley Hong <link2next@gmail.com>, Dec. 2013.
  </author>
*/

package uls.polaris;

import uls.polaris.UlsLex;

// <brief>
// The base class of UlsIStream and UlsOStream
// UlsIStream is used for reading uls-file(*.uls).
// UlsOStream is used for creating uls-file(*.uls).
// </brief>
public class UlsStream implements AutoCloseable {
	static {
		System.loadLibrary(UlsLex.NameOfDll);
	}

	private static native void destroyIFile(Object jistr);
	private static native void destroyOFile(Object ostr);

	protected Object uls_hdr;
	protected Boolean read_only;

	// <brief>
	// A Constructor of 'UlsStream'
	// </brief>
	// <parm name="lex">The analyzer associated with this 'UlsStream'</parm>
	public UlsStream(Boolean rdonly) {
		uls_hdr = null;
		read_only = rdonly;
	}

	// <brief>
	// Returns just the stored 'UlsLex' object. 
	// </brief>
	protected Object getCore() {
		return uls_hdr;
	}

	// <brief>
	// Closes the stream (file).
	// </brief>
	@Override
	public void close() {
		if (uls_hdr != null) {
			if (read_only == true) {
				destroyIFile(uls_hdr);
			} else {
				destroyOFile(uls_hdr);
			}
			uls_hdr = null;
		}
	}
}

