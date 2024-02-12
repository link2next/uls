include $(top_srcdir)/uls.config

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
JNI_HFILES_2 = $(patsubst %.java,%,$(SRCS2))
JNI_HFILES_FNAME = $(subst /,_,$(JNI_HFILES_1))
JNI_HFILES_FPATH = $(patsubst %,$(ULSJNI_INCDIR)/%.h,$(JNI_HFILES_FNAME))
JNI_HFILES_DOTS = $(subst /,.,$(JNI_HFILES_1))

all: $(JNI_HFILES_FPATH) $(OBJS2)

$(LIBPROG): $(OBJS2)
	jar -cf $(LIBPROG) -C $(ULSJNI_BINDIR) uls

# java --> jni-header
define rule_java2hfile
$(ULSJNI_INCDIR)/$1: $2
	@if which javah > /dev/null; then \
		javah -o $(ULSJNI_INCDIR)/$1 $3; \
	else \
		javac -h $(ULSJNI_INCDIR) $2; \
	fi
endef
$(foreach T,$(JNI_HFILES_DOTS),$(eval $(call rule_java2hfile,$(subst .,_,$(T)).h,$(subst .,/,$(T)).java,$(T))))

# java --> class
define rule_java2class
$(ULSJNI_BINDIR)/$1: $2
	javac -d $(ULSJNI_BINDIR) $2
endef
$(foreach T,$(JNI_HFILES_2),$(eval $(call rule_java2class,$(T).class,$(T).java)))

install: $(LIBPROG)
	install -m 644 $(LIBPROG) $(libdir)

clean-local:
	@find $(ULSJNI_BINDIR) -name '*.class' -exec rm {} \;
	rm -f $(LIBPROG)
