#
include ../cfg.mk

outdir = classes

SRCS1 = uls/tests/Sample1Lex.java \
	uls/tests/Sample2Lex.java
OBJS1 = $(patsubst %.java,$(outdir)/%.class,$(SRCS1))

SRCS2 = uls/tests/UlsDump.java \
	uls/tests/UlsTestStream.java
OBJS2 = $(patsubst %.java,$(outdir)/%.class,$(SRCS2))

all: $(OBJS1) $(OBJS2)

$(outdir)/uls/tests/%.class: uls/tests/%.java
	@javac -cp .:$(uls_libdir)/$(uls_jar) -d $(outdir) $<

uls/tests/Sample1Lex.java: sample1.ulc
	@$(ulc2cls) -o $@ -ljava -n uls.tests.Sample1Lex $< 

uls/tests/Sample2Lex.java: sample2.uld 
	@$(ulc2cls) -o $@ -ljava -n uls.tests.Sample2Lex $< 

check: all
	@./run_java_tests.sh ../../tools/uls_stream $(uls_libdir) $(uls_jar)

clean-local:
	@rm -rf $(outdir); mkdir $(outdir) 
	@rm -f a?.uls $(SRCS1)

