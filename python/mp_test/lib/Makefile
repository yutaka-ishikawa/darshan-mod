PYTHON=python3
darshan3: darshan.c
	echo "#define PYTHON3" >darshan.c
	cat darshan.c.in >>darshan.c
	$(PYTHON) darshan-compile.py build --build-lib ./

darshan-k: darshan.c
	echo "#define PYTHON2" >darshan.c
	cat darshan.c.in >>darshan.c
	pjsub run-compile-k.sh
run-test-k:
	pjsub run-test-k.sh
clean:
	rm -rf build run-compile-k.sh.{e,o,s}* run-test-k.sh.{e,o,s}*
cleanall:
	rm -rf darshan.so build run-compile-k.sh.{e,o,s}* run-test-k.sh.{e,o,s}*
