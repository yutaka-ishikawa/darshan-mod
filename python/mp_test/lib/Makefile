PYTHON=/mnt2/python/bin/python3
BUILD_DIR=build/lib.linux-`uname -p`-`$(PYTHON) --version | sed -e 's/Python \([0-9].[0-9]\).*/\1/'`
darshan:
	$(PYTHON) darshan-compile.py build
	cp $(BUILD_DIR)/darshan*.so darshan.so
#	$(PYTHON) darshan-compile.py install
#	$(PYTHON) darshan-compile.py sdist



