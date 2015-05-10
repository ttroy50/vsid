topsrcdir="`dirname $0`"
cd "$topsrcdir"

make -k clean >/dev/null 2>&1
make -k distclean >/dev/null 2>&1
find . -name Makefile.in -exec rm -f {} \;
find . -name Makefile.in -exec rm -f {} \;
rm -rf \
Makefile \
Makefile.in \
aclocal.m4 \
autom4te.cache \
config.log \
config.status \
configure \
build