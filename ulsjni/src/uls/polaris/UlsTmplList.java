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
  <file> UlsTmplVal.java </file>
  <author>
    Stanley Hong <link2next@gmail.com>, Dec. 2013.
  </author>
*/

package uls.polaris;

import java.util.HashMap;

// <brief>
// The class to define the name-value pair.
// This is used to pass the array argument of name-value pairs
//    in processing the template uls-stream class.
// </brief>
public class UlsTmplList implements AutoCloseable {
	static {
		System.loadLibrary(UlsLex.NameOfDll);
	}

	private static native Object createTmplList(int n_alloc, int flags);
	private static native boolean destroyTmplList(Object tmpls);
	private static native void resetTmplList(Object tmpls, int n_alloc);
	private static native boolean addTmpl(Object tmpls, String name, String val);

	private HashMap<String,String> hashtbl;
	private Object tmpl_list;

	// <brief>
	// UlsTmplList is to store a list of template variables and their values.
	// Notice that a template variable is composed of a name and its (string) value.
	// It can be passed to the argument of UlsIStream().
	// </brief>
    public UlsTmplList()
    {
		hashtbl = new HashMap<String,String>();
		tmpl_list = createTmplList(8, UlsLex.TMPLS_DUP);
    }

	// <brief>
	// This clears the internal list of variables. The length of the list will be zero.
	// </brief>
	// <return>none</return>
	public void clear()
	{
		hashtbl.clear();
	}

	// <brief>
	// Checks if the variable 'tnam' exists in the list.
	// </brief>
	// <return>true/false</return>
	public Boolean exist(String nam)
    {
		Boolean rval;

		if (hashtbl.containsKey(nam)) {
			rval = true;
		} else {
			rval = false;
		}

        return rval;
    }

	// <brief>
	// It returns the length of the internal list. It's less than or equal to the capacity of the list.
	// </brief>
	// <return># of variables</return>
    public int length()
    {
        return hashtbl.size();
    }

	// <brief>
	// Append a pair <tnam, tval> to the list increasing the length of the list.
	// </brief>
	// <parm name="tnam">the name of template variable</parm>
	// <parm name="tval">the value of the 'tnam'</parm>
	// <return>none</return>
    public void add(String nam, String val)
    {
		if (val == null) val = "";
		hashtbl.put(nam, val);
    }

	// <brief>
	// It returns the value of 'tnam'.
	// If there is no template variable named 'tnam', it returns 'null'.
	// </brief>
	// <parm name="tnam">the name of template variable</parm>
	// <return>the value of the 'tnam'</return>
	public String getValue(String nam)
	{
		String str;

		if (hashtbl.containsKey(nam)) {
			str = hashtbl.get(nam);
		} else {
			str = null;
		}

        return str;
    }
 
	// <brief>
	// This modifies the pair <tnam, tval> in the internal list if the item named 'tnam' exists.
	// </brief>
	// <parm name="tnam">the name of template variable</parm>
	// <parm name="tval">the value of the 'tnam'</parm>
	// <return>true/false</return>
	public Boolean setValue(String nam, String val)
	{
		Boolean rval;

		if (hashtbl.containsKey(nam)) {
			hashtbl.put(nam, val);
			rval = true;
		} else {
			rval = false;
		}

        return rval;
	}

	// <brief>
	// This exports the internal list to another UlsTmplList object.
	// </brief>
	// <return>Opaque Data</return>
	public Object exportTmpls()
	{
		resetTmplList(tmpl_list, length());

		for (String nam : hashtbl.keySet()) {
			String val = hashtbl.get(nam);

			addTmpl(tmpl_list, nam, val);
		}

		return tmpl_list;
	}

	// <brief>
	// Export the internal list to another UlsTmplList object.
	// </brief>
	// <parm name="tmpl_list_exp">A new UlsTmplList object to which the current list is exported.</parm>
	// <return>the length of the list</return>
    public int exportTmpls(UlsTmplList tmpl_list_exp)
    {
		int n=0;

		for (String tnam : hashtbl.keySet()) {
			String tval = hashtbl.get(tnam);
			System.out.println("tnam = '" + tnam + "' tval='" + tval + "'");
			tmpl_list_exp.add(new String(tnam), new String(tval));
			++n;
		}

        return n;
    }

	// <brief>
	// Dumps the internal list of pairs <tnam,tval> to stdout.
	// </brief>
	// <return>none</return>
    public void dump()
    {
		int i=0;

		for (String nam : hashtbl.keySet())
		{
			String val = hashtbl.get(nam);
			System.out.println("[" + i + "] " + nam + " :: '" + val + "'");
			i++;
		}
	}

	@Override
	public void close() {
		destroyTmplList(tmpl_list);
		tmpl_list = null;
	}
}

