# pg_ctl -D /var/lib/postgres/data4 -l logfile stop
# pg_ctl -D /var/lib/postgres/data4 -l logfile start
# Create rrule_archive
# https://riptutorial.com/git/example/9513/create-an-archive-of-git-repository
# git archive --format tar HEAD | gzip > pg_rrule-HEAD.tar.gz
# Add rrule to cross-pg container:
# get to container
# docker exec -it cross-pg /bin/sh
# inside container
# alpine packages: https://pkgs.alpinelinux.org/packages


apk add tar qt5-qtbase-dev g++ clang libical-dev postgresql-dev make  \
\
&& cd /usr/src/app \
# \ # download tar.gz from github
# && wget -O pg_rrule.tar.gz "https://raw.githubusercontent.com/ondrej-111/pg_rrule/master/rrule-HEAD.tar.gz" \
# \
# && tar --extract --file pg_rrule.tar.gz \
# \
# && rm pg_rrule.tar.gz \
# \
&& cd src \
&& qmake-qt5 \
&& make \
# && cp libpg_rrule.so /usr/lib/postgresql/pg_rrule.so \
&& cd .. \
&& make \
# && cp pg_rrule.control /usr/share/postgresql/extension/ \
# && cp sql/pg_rrule.sql /usr/share/postgresql/extension/pg_rrule--0.2.0.sql

&& cp pg_rrule.control /usr/local/share/postgresql/extension/ \
&& cp sql/pg_rrule.sql /usr/local/share/postgresql/extension/pg_rrule--0.2.0.sql

# v2 
apk add tar g++ clang libical-dev postgresql-dev make  \
\
&& mkdir -p /usr/src/app \
&& cd /usr/src/app \
\
&& wget -O pg_rrule.tar.gz "https://raw.githubusercontent.com/ondrej-111/pg_rrule/master/rrule-HEAD.tar.gz" \
\
&& tar --extract --file pg_rrule.tar.gz \
\
&& rm pg_rrule.tar.gz \
&& make

# HELPERS:
/usr/lib/postgresql
# mkdir postgresql
# cd postgresql
cd /usr/lib
rrule docker: https://github.com/fizix-io/docker-postgres-rrule/blob/master/Dockerfile
alpine docker: https://github.com/appropriate/docker-postgis/blob/f6d28e4a1871b1f72e1c893ff103f10b6d7cb6e1/10-2.4/alpine/Dockerfile
alpine packages: https://pkgs.alpinelinux.org/packages