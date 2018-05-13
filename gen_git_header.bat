@echo off

git log -n 1 --format=format:"#define GIT_HEAD_REV \"%%H\"%%n" HEAD > sekvyu\git_tmp.h

fc sekvyu\git_tmp.h sekvyu\git.h >NUL 2>NUL && goto _no_change || goto _gen_header

:_gen_header
MOVE /Y sekvyu\git_tmp.h sekvyu\git.h
EXIT

:_no_change
DEL sekvyu\git_tmp.h
