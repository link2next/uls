      
              ULS(Unified Lexical Scheme)

  What is it?
  -----------
   ULS(Unified Lexical Scheme) is a class library for creating lexical analyzer 
   from language specification file. The project ULS project has been started 
   for the purpose of providing compiler creators with a intuitive, practical, flexible language design.
   It consists of C/C++, C#/Java libraries with a few of tools and runs on Linux, Windows, and macOS.
   The version 1.0 of ULS was released by Stanley Hong <link2next@gmail.com>, 2011.

  Key Features
  -------------

   ULS has the following key features.
     o It can tokenize the input file which encoded by UTF-8, UTF-16, UTF-32.
       The input files may contain the words in localized language as identifier.

     o It can map the tokens that you defined to any number of all range of 32-integer.
       Even 1-character token like '+', '-', '=', '&' can be mapped to any number other than ASCII codes,
          if you're ready to the confliction among those tokens.

     o It can instantiate multiple objects for one language or other languages in one application
          as it's been designed as class library.

     o It can store the tokens from many text or uls(*.uls) input files to a uls-file(*.uls).
        The output file can be again replayed.

     o Since ULS manipulates the real or integer number as text string format,
          it can deal with the input source having the arbitrary number of digits.

     o It can be used as a replacement of 'lex/flex' for token-provider to yacc/bison parser.


   Examples
  -------------
   Refer to 'examples' and 'tests' directory to see the scenarios using ULS API.
   The programs in 'tests' are tested for the core library source.
     You can run all of them with command 'make check' in batch mode.

     o dump_toks : This is a basic example of ULS. dumps the tokens from input files.

     o yacc      : There's an ULS-object which provides yacc parser with token sequence.

     o getch     : ULS supports for getting or ungetting one char in input-file in restricted manner.
                   To see how to manipulate the individual character, refer to 'getch'.

     o id_stat   : This shows the statistics of identifiers from input-files recognizing the IDs.

     o line      : You can give a literal line as input by calling uls_set_line(), uls_push_line().

     o log       : ULS has a unique logging system(uls_lf_*) with vararg style. 
                   The 'log' demonstrates how to use the uls_lf.
                   With this logging system, you can log error-messages.
                      It includes the error-location(%w) and token-name(%k).

     o many_keys : This example has ulc-file, which defines plenty of token.
                   ULS supports for the languages having many keywords with little loss of performance.

     o nest      : It demonstrates to process the input that contains sub-language in it
                     by instantiating two ULS-objects.

     o strings   : This shows that ULS is able to extract only the literal strings from input files.

     o tok_remap : ULS can remap the token numbers of 1-char ACSII,
                    such as '(', '{', '=', ... to arbitrary integers.

     o two_lex   : An example using two ULS-objects.

     o cpp_hello : Demonstrates the usage of ULS-class in C++.
                  This is also included in the Windows version as an example.

     o wdump_toks : This is the wide string version of the 'dump_toks'.

   There are also examples in the directory 'examples' which should be built after installing ULS.
     You can build and run all of them with command 'make check' in batch mode after installing ULS.

     o Css3      : Using the uls-c++ library, it scans css3 file and dump its tokens to the standard output.

     o Html5     : Using the uls-c++ library, it scans html file and dump its tokens to the standard output.
 
     o Shell     : Using the uls-c++ library, it scans shell-script file and dump its tokens to the standard output.

     o Mkf       : Using the uls-c++ library, it scans 'Makefile' file and dump its tokens to the standard output.

     o tokseq    : Tests the token streamp apis, writing and reading uls-files.

   In Linux or macOS, entering the directory 'examples', build them by command 'make'.
   In Windows, There is a (VS2012) solution file in 'UlsWin\UlsCppExamples'.



  Documentation
  -------------
   There are an user-guide uls_ug-0.8.x.pdf and manuals on tools, ulc2class.txt, ulf_gen.txt, uls_stream in 'doc'. 

   You can see the manuals on 'ulc2class', 'uls_stream', 'ulf_gen' on linux after installing the package.
   To see the man-pages, type
      man ulc2class
      man uls_stream
      man ulf_gen

   If you installed ULS in directory other than '/usr/local',
    you may need to add the path to the environment variable 'MANPATH' to see them.



  Installation
  ------------
   On Debian platforms like Debian, Ubuntu, Mint, two deb-files is provided. 
      o uls_1.x.y_amd64.deb: for 64-bit mach(x86_64, amd64)
      o uls_1.x.y_i686.deb: for 32-bit mach
   To install the deb-file, download the proper file on the site(http://www.sourceforge.net/projects/uls).

   Enter the command as follows.
      sudo gdebi uls_1.x.y_amd64.deb

   To test the ULS, setup the examples of ULS by entering the command 'setup_uls_examples'.
      setup_uls_examples <dest-dir>
   There'll be a directory called 'uls_examples' in the <dest-dir>.

   To build the examples, do the followings
      1. ./configure
      2. make
   Refer to the section 'Demos' below to examine the executions.

   ULS The installation process of ULS from code sources on Linux platforms is routine.
      1. ./configure
      2. make
      3. make check
      4. sudo make install

   The default installation directory is '/usr/local', which is arranged by 'configure' script.
   If you want to install ULS to directory other than '/usr/local', start 'configure' with typing
      'configure --prefix=<dir-you-want-install>'.

   On Windows platforms(Windows7 or 10),
     There's a directory called UlsWin in the source code. 
     Unzip the zip-file.
     Run the Visual Studio and open the solution 'UlsWin.sln' in the directory.


  Demos
  ------------

   After installing the libraries, you can build the demo programs in 'examples'.
   Go to the directory, build the program by running 'make'.

   All the demo programs accept an input file specified in command line like this.
      LD_LIBRARY_PATH=/usr/local/lib64 <demo-prog> <input-file>
         where demo-prog = DumpToks, Css3Toks, Html5Toks, ...
      o test_1st test_1st_sam.txt in the 'test_1st' directory.
      o ulc2xml in the 'yacc' directory.
      o tokseq tokseq_sam.txt
      o DumpToks input1.txt
      o Css3Toks css3_sam.txt
      o Html5Toks html5_sam.txt
      o ShellToks shell_sam.txt
      o MkfToks mkf_sam.txt

   You may need to set the environment variable 'LD_LIBRARY_PATH' for the shared library loader.
   For example,
      LD_LIBRARY_PATH=/usr/local/lib64 ./test_1st input1.txt
      LD_LIBRARY_PATH=/usr/local/lib ./test_1st input1.txt or simply
      ./test_1st input1.txt

   Please see the file called INSTALL.txt to see how to use ULS in Windows or Eclipse.



  Tested Platforms
  ------------
   ULS is tested on following platforms.
      o Ubuntu 18.04.2
      o Linux Mint 18.1
      o Centos 6.8
      o Windows 7 & 10
   Maybe, it works on similar or later version of above platforms.


  Prerequisite packages
  ------------
   o glibc
   o gcc
   o g++ 
   o make
   o libtool
   o help2man 
   o bison
   o autoconf
   o bash
  The above packages is needed to build ULS from code sources.
  The package is dependant on libc6(glibc) to run.


  The Latest Version
  ------------------
   The main site of ULS is http://sourceforge.net/projects/uls.
   You can find the latest version of ULS in https://github.com/link2next/uls

