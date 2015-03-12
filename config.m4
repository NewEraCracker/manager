PHP_ARG_ENABLE(manager, whether to enable manager support,
[  --enable-manager        Enable manager support])

if test "$PHP_MANAGER" != "no"; then
  PHP_NEW_EXTENSION(manager, manager.c, $ext_shared)
fi