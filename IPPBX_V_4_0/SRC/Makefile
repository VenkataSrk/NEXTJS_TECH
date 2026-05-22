# ///////////////////////////////////////////////////////////////////////////
# 
#            Copyright 2020 by Vectone Mobile.
#            All rights reserved.
# 
# ///////////////////////////////////////////////////////////////////////////

top_dir = ./

include $(top_dir)/Makefile.conf

pkg_name=libxgatesip-$(shell date +%d%m%Y)
tar_file=$(pkg_name).tar
pkg=$(tar_file).gz

all:
	@mkdir -p lib
	@make -C common
	@make -C sip
help:
	@echo 'make dep   # generate dependency file'
	@echo 'make dbg   # to make debug build'
	@echo 'make opt   # to make optimized build'
	@echo 'make doc   # run doxygen and generate documentation'

doc:
	doxygen etc/doxygen-devel.conf

binpkg:
	bin/release.sh $(PWD)

dep:
	$(MAKE) -C common $@
	$(MAKE) -C sip $@

test:
	$(MAKE) -C $@

clean:
	$(MAKE) -C common $@
	$(MAKE) -C sip $@
	$(MAKE) -C test $@

allclean: clean
	$(MAKE) -C test $@
	@rm -rf lib

.PHONY: doc common sip test
