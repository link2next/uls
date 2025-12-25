include $(top_srcdir)/uls.config

.PHONY: ULSJNI_PREREQ

ACLOCAL_AMFLAGS = -I m4

ULSJNI_BINDIR=../classes
ULSJNI_INCDIR=../include

LIBPROG = uls-$(uls_ver).jar

SRCS1 = \
	uls/polaris/UlsLex.java \
	uls/polaris/UlsStream.java \
	uls/polaris/UlsTmplList.java \
	uls/polaris/UlsIStream.java \
	uls/polaris/UlsOStream.java

SRCS2 = $(SRCS1) \
	uls/tests/UlsDump.java \
	uls/tests/UlsTestStream.java
OBJS2 = $(patsubst %.java,$(ULSJNI_BINDIR)/%.class,$(SRCS2))

JNI_HFILES_1 = $(patsubst %.java,%,$(SRCS1))
JNI_HFILES_FNAME = $(subst /,_,$(JNI_HFILES_1))
JNI_HFILES_FPATH = $(patsubst %,$(ULSJNI_INCDIR)/%.h,$(JNI_HFILES_FNAME))
JNI_HFILES_DOTS = $(subst /,.,$(JNI_HFILES_1))

all: ULSJNI_PREREQ $(JNI_HFILES_FPATH) $(OBJS2)

ULSJNI_PREREQ:
	@if [ ! -e "$(ULSJNI_BINDIR)" ]; then \
		mkdir "$(ULSJNI_BINDIR)"; \
	fi
	@if [ ! -e "$(ULSJNI_INCDIR)" ]; then \
		mkdir "$(ULSJNI_INCDIR)"; \
	fi

$(JNI_HFILES_FPATH): $(SRCS1)
	@echo "Generating jni-header files..."
	@for ii in $(SRCS1); do \
		javac -h $(ULSJNI_INCDIR) $${ii}; \
	done

$(OBJS2): $(SRCS2)
	@echo "Generating object files..."
	@./make_SampleLex.sh ../../tools/ulc2class/ulc2class $(TMP_SYSPROPS) .
	@for ii in $(SRCS2); do \
		javac -d $(ULSJNI_BINDIR) $${ii}; \
	done

$(LIBPROG): $(OBJS2)
	jar -cf $(LIBPROG) -C $(ULSJNI_BINDIR) uls

install: $(LIBPROG)
	install -m 644 $(LIBPROG) $(libdir)

clean-local:
	@find $(ULSJNI_BINDIR) -name '*.class' -exec rm {} \;
	@find . -name '*.class' -exec rm {} \;
	rm -f $(LIBPROG)
