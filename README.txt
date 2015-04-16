Extension Manager for PHP
=========================

What is Extension Manager?
--------------------------
Extension Manager is a free open source extension manager for PHP. It increases
compatibility in environments that require loading of zend extensions made by
third-parties.

Extension Manager stores extensions loaded into its own structures, defeating
most common checks performed by third-party closed source zend extensions.

It will hook into the first loaded zend extension and execute other extension
methods from there if stealth mode is enabled.

There are some advanced options for more skilled users, such as disabling build
id checks and disabling info replacement.

Compatibility
-------------
Extension Manager works with PHP version 5.2 and higher.
It is being used on Linux and Windows but other OS may be supported as well.

Quick install
-------------

Note(1): for Microsoft Windows installation, please refer to README-win32.txt file.

Step 1. Compiling Extension Manager

  export PHP_PREFIX="/usr"

  $PHP_PREFIX/bin/phpize

  ./configure \
  --enable-manager=shared \
  --with-php-config=$PHP_PREFIX/bin/php-config

  make

  You must specify the real prefix where PHP is installed in the "export"
  command. It may be "/usr" "/usr/local", or something else.

Step 2. Installing Extension Manager

  make install

Step 3. Configuring Extension Manager

Extension Manager can be installed as a PHP extension.

If you have /etc/php.d directory, you should copy manager.ini inside and modify
default values.

If not, you need to edit your php.ini file (usually /etc/php.ini).

Configuration Options
---------------------

manager.modules_dir
	Directory where the loadable zend extensions reside
	Default: Empty

manager.modules_list
	List of zend extensions to be loaded separated by colons
	Default: Empty

manager.check_build
	Enable build id check
	Only applicable in PHP 5.3 and higher
	Keep it turned on unless you really know what you are doing
	Default: 1 (Turned on)

manager.load_stealth
	Enable stealth loading
	Default: 0 (Turned off)

manager.replace_info
	Enable information replacement
	Only applicable when stealth loading is enabled
	Default: 1 (Turned on)

Contact us
----------
You can contact us with questions, patches or bugs on GitHub and BitBucket:
https://github.com/NewEraCracker/manager/issues
https://bitbucket.org/NewEraCracker/manager/issues