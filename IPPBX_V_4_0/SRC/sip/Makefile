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

src:
	#mkdir -p lib
	$(MAKE) -C src

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
	$(MAKE) -C src $@

clean:
	-rm -f src/*.o
	#-rm -f lib/*.so
 
.PHONY: doc src
