/*
 +---------------------------------------------------------------------+
 | Extension Manager for PHP                                           |
 +---------------------------------------------------------------------+
 | Copyright (c) 2014 NewEraCracker (http://www.planet-dl.org)         |
 +---------------------------------------------------------------------+
 | Permission is hereby granted, free of charge, to any person         |
 | obtaining a copy of this software and associated documentation      |
 | files (the "Software"), to deal in the Software without             |
 | restriction, including without limitation the rights to use, copy,  |
 | modify, merge, publish, distribute, sublicense, and/or sell copies  |
 | of the Software, and to permit persons to whom the Software is      |
 | furnished to do so, subject to the following conditions:            |
 |                                                                     |
 | The above copyright notice and this permission notice shall be      |
 | included in all copies or substantial portions of the Software.     |
 |                                                                     |
 | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,     |
 | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES     |
 | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND            |
 | NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS |
 | BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN  |
 | ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN   |
 | CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE    |
 | SOFTWARE.                                                           |
 |                                                                     |
 | Except as contained in this notice, the name(s) of the above        |
 | copyright holders shall not be used in advertising or otherwise to  |
 | promote the sale, use or other dealings in this Software without    |
 | prior written authorization.                                        |
 +---------------------------------------------------------------------+
 | Author: NewEraCracker <neweracracker@gmail.com>                     |
 +---------------------------------------------------------------------+
*/

#ifndef PHP_MANAGER_H
#define PHP_MANAGER_H

/** Include important internal PHP headers */
#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_variables.h"
#include "TSRM.h"
#include "SAPI.h"
#include "zend_alloc.h"
#include "zend_compile.h"
#include "zend_extensions.h"
#include "zend_globals.h"
#include "zend_hash.h"
#include "zend_llist.h"
#include "zend_operators.h"
#include "ext/standard/base64.h"
#include "ext/standard/info.h"
#include "main/php_version.h"

/** Include important system headers */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>

/** Include string.h */
#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

/** Include other system headers */
#ifdef PHP_WIN32
#include "win32/param.h"
#include "win32/winutil.h"
#else
#include <sys/param.h>
#endif

/** An important macro */
#ifndef PHP_VERSION_ID
#define PHP_VERSION_ID (PHP_MAJOR_VERSION * 10000 + PHP_MINOR_VERSION * 100 + PHP_RELEASE_VERSION)
#endif

/** Define our entry point */
extern zend_module_entry manager_module_entry;
#define phpext_manager_ptr &manager_module_entry

/** Define extension constants */
#define MANAGER_NAME "Extension Manager"
#define MANAGER_VERSION "1.0.4"
#define MANAGER_AUTHOR "NewEraCracker"

/** Our globals */
ZEND_BEGIN_MODULE_GLOBALS(manager)
	char * modules_dir;
	char * modules_list;
	zend_bool check_build;
	zend_bool load_stealth;
	zend_bool replace_info;
ZEND_END_MODULE_GLOBALS(manager)

/** Macros to access globals */
#ifdef ZTS
#define MANAGER_G(v) TSRMG(manager_globals_id, zend_manager_globals *, v)
#else
#define MANAGER_G(v) (manager_globals.v)
#endif

#endif /* PHP_MANAGER_H */