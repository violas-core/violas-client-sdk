# remove all temp directories
rm -rf autom4te.cache bin build m4 
# remove all temp files
rm -rf  aclocal.m4 ar-lib autoscan.log \
        compile config.guess config.h.in* config.sub configure configure.scan\
        depcomp \
        install-sh ltmain.sh Makefile.in missing
# remove all Makefile.in
find ./ -name Makefile.in | xargs  rm -f