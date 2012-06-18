#  Copyright (c) 2012 Antti-Juhani Kaijanaho
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#  2. Redistributions in binary form must reproduce the above
#     copyright notice, this list of conditions and the following
#     disclaimer in the documentation and/or other materials provided
#     with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
#  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
#  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
#  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

SHELL = /bin/sh

echoexec = sh echoexec.sh

all :
	exit 1

# see Makefile.in distcheck
run_distcheck :
	tar zxf $(tgz)
	mv $(base) $(base).orig
	@echo "# TEST INSTALL"
	tar zxf $(tgz)
	cd $(base) && ./configure
	$(MAKE) -C $(base) DESTDIR=../dest install
	test -f dest/usr/local/lib/libpub.a
	test ! -x dest/usr/local/lib/libpub.a
	test -f dest/usr/local/include/publib.h
	test ! -x dest/usr/local/include/publib.h
	@set -e ; for f in $(includes) ; do \
	  bn=`basename $$f` ;\
	  $(echoexec) test -f dest/usr/local/include/publib/$$bn ;\
	  $(echoexec) test ! -x dest/usr/local/include/publib/$$bn ;\
	done
	@set -e ; for f in $(manpages) ; do \
	  bn=`basename $$f` ;\
	  $(echoexec) test -f dest/usr/local/share/man/man3/$$bn.gz ;\
	  $(echoexec) test ! -x dest/usr/local/share/man/man3/$$bn.gz ;\
	done
	rm -rf $(base) dest
	@echo "# TEST DIST"
	tar zxf $(tgz)
	cd $(base) && ./configure
	$(MAKE) -C $(base) DESTDIR=../dest/ dist
	test -f dest/$(tgz)
	cd dest && tar zxf $(tgz)
	touch dest/$(base)/foobar
	diff -Naur $(base).orig dest/$(base)
