all-local: setup.py compiler.cpp __init__.py
	$(PYTHON) setup.py build

install-exec-local: 
	$(PYTHON) setup.py install --prefix=$(DESTDIR)

EXTRA_DIST = \
	compiler.cpp \
	setup.py \
	__init__.py \
	test.py

distclean: clean

clean:
	$(RM) -Rf build
	$(RM) Makefile

maintainer-clean: clean
	$(RM) Makefile.in
