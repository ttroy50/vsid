./cleanup.sh

#set -e

autoconf
aclocal
automake --add-missing
autoconf
