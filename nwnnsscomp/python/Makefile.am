EXTRA_DIST = setup.py test.py

all-local: setup.py compiler.cpp __init__.py
	$(PYTHON) setup.py build

install-exec-local: 
	$(PYTHON) setup.py install --prefix=$(DESTDIR)

distclean: clean

clean:
	$(RM) -Rf build
	$(RM) Makefile

maintainer-clean: clean
	$(RM) Makefile.in
