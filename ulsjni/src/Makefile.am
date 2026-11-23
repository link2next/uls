include $(top_srcdir)/uls.config

.PHONY: ULSJNI_PREREQ

ULSJNI_BINDIR=../classes
ULSJNI_INCDIR=../include

LIBPROG = uls-$(uls_ver).jar

SRCS1 = \
	uls/polaris/UlsLex.java \
	uls/polaris/UlsStream.java \
	uls/polaris/UlsTmplList.java \
	uls/polaris/UlsIStream.java \
	uls/polaris/UlsOStream.java
OBJS1 = $(patsubst %.java,$(ULSJNI_BINDIR)/%.class,$(SRCS1))

JNI_HFILES_1 = $(patsubst %.java,%,$(SRCS1))
JNI_HFILES_FNAME = $(subst /,_,$(JNI_HFILES_1))
JNI_HFILES_FPATH = $(patsubst %,$(ULSJNI_INCDIR)/%.h,$(JNI_HFILES_FNAME))
JNI_HFILES_DOTS = $(subst /,.,$(JNI_HFILES_1))

CLEANFILES = $(LIBPROG)

all: ULSJNI_PREREQ $(JNI_HFILES_FPATH) $(OBJS1)

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

$(OBJS1): $(SRCS1)
	@for ii in $(SRCS1); do \
		javac -d $(ULSJNI_BINDIR) $${ii}; \
	done

$(LIBPROG): $(OBJS1)
	@jar -cf $(LIBPROG) -C $(ULSJNI_BINDIR) uls

install: $(LIBPROG)
	install -m 644 $(LIBPROG) "$(ax_lib_dir)"

clean-local:
	rm -f $(LIBPROG)
