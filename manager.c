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

#include "list.h"
#include "manager.h"
#include "manager_logo.h"

/** Declare our module globals */
ZEND_DECLARE_MODULE_GLOBALS(manager)

/** Linked list position */
static zend_llist_position lp = NULL;

/** Last zend_extension pointer */
static zend_extension *ze = NULL;

/** Original pointers to hooked zend extension methods */
static int  (*orig_startup)(zend_extension *extension) = NULL;
static void (*orig_shutdown)(zend_extension *extension) = NULL;
static void (*orig_activate)(void) = NULL;
static void (*orig_deactivate)(void) = NULL;
static void (*orig_message_handler)(int message, void *arg) = NULL;
static void (*orig_op_array_handler)(zend_op_array *op_array) = NULL;
static void (*orig_statement_handler)(zend_op_array *op_array) = NULL;
static void (*orig_fcall_begin_handler)(zend_op_array *op_array) = NULL;
static void (*orig_fcall_end_handler)(zend_op_array *op_array) = NULL;
static void (*orig_op_array_ctor)(zend_op_array *op_array) = NULL;
static void (*orig_op_array_dtor)(zend_op_array *op_array) = NULL;

/** Zend extensions loaded by our manager and hooked into the 1st loaded zend extension */
static Linkedlist * manager_extension_list;

/** Hold the original value of zend extensions list */
static zend_llist backup_zend_extensions;

/** You will never defeat the cracker of New Era... */
#define BACKUP_ZEND_EXTENSIONS_LIST()   backup_zend_extensions = zend_extensions;
#define REPLACE_ZEND_EXTENSIONS_LIST(a) zend_extensions = a;
#define RESTORE_ZEND_EXTENSIONS_LIST()  zend_extensions = backup_zend_extensions;

/** Stealth macro for functions with no arguments */
#define STEALTH_FUNCTION_VOID(name)                 \
  static void stealth_##name(void){                 \
    Linkedlist     * list = manager_extension_list; \
    Node           * itr  = list->head->next;       \
    zend_llist     * zll  = NULL;                   \
    zend_extension * ex   = NULL;                   \
    if(orig_##name != NULL) {                       \
      orig_##name();                                \
    }                                               \
    BACKUP_ZEND_EXTENSIONS_LIST()                   \
    while(itr != list->tail) {                      \
      zll = (zend_llist *)itr->element;             \
      ex  = (zend_extension *)&zll->head->data;     \
      REPLACE_ZEND_EXTENSIONS_LIST(*zll)            \
      if(ex->##name != NULL) {                      \
        ex->##name();                               \
      }                                             \
      itr = itr->next;                              \
    }                                               \
    RESTORE_ZEND_EXTENSIONS_LIST()                  \
  }

/** Stealth extension activation */
STEALTH_FUNCTION_VOID(activate)

/** Stealth extension deactivation */
STEALTH_FUNCTION_VOID(deactivate)

/** Stealth message_handler */
static void stealth_message_handler(int message, void * arg)
{
	Linkedlist     * list = manager_extension_list;
	Node           * itr  = list->head->next;
	zend_llist     * zll  = NULL;
	zend_extension * ex   = NULL;

	/* Run message handler of the extension we hooked into */
	if(orig_message_handler != NULL) {
		orig_message_handler(message, arg);
	}

	BACKUP_ZEND_EXTENSIONS_LIST()

	/* Handle our list of extensions */
	while(itr != list->tail)
	{
		/* Current extension being iterated */
		zll = (zend_llist *)itr->element;
		ex  = (zend_extension *)&zll->head->data;

		REPLACE_ZEND_EXTENSIONS_LIST(*zll)

		if(ex->message_handler != NULL)
		{
			ex->message_handler(message, arg);
		}

		itr = itr->next;
	}

	RESTORE_ZEND_EXTENSIONS_LIST()
}

/** Stealth macro for functions with one arguments */
#define STEALTH_FUNCTION_ARG1(name, type, arg)      \
  static void stealth_##name(##type ##arg){         \
    Linkedlist     * list = manager_extension_list; \
    Node           * itr  = list->head->next;       \
    zend_llist     * zll  = NULL;                   \
    zend_extension * ex   = NULL;                   \
    if(orig_##name != NULL) {                       \
      orig_##name(##arg);                           \
    }                                               \
    BACKUP_ZEND_EXTENSIONS_LIST()                   \
    while(itr != list->tail) {                      \
      zll = (zend_llist *)itr->element;             \
      ex  = (zend_extension *)&zll->head->data;     \
      REPLACE_ZEND_EXTENSIONS_LIST(*zll)            \
      if(ex->##name != NULL) {                      \
        ex->##name(##arg);                          \
      }                                             \
      itr = itr->next;                              \
    }                                               \
    RESTORE_ZEND_EXTENSIONS_LIST()                  \
  }

/** Stealth op_array_handler */
STEALTH_FUNCTION_ARG1(op_array_handler, zend_op_array *, op_array)

/** Stealth statement_handler */
STEALTH_FUNCTION_ARG1(statement_handler, zend_op_array *, op_array)

/** Stealth fcall_begin_handler */
STEALTH_FUNCTION_ARG1(fcall_begin_handler, zend_op_array *, op_array)

/** Stealth fcall_end_handler */
STEALTH_FUNCTION_ARG1(fcall_end_handler, zend_op_array *, op_array)

/** Stealth op_array_ctor */
STEALTH_FUNCTION_ARG1(op_array_ctor, zend_op_array *, op_array)

/** Stealth op_array_dtor */
STEALTH_FUNCTION_ARG1(op_array_dtor, zend_op_array *, op_array)

/** Utility macros for saving, hooking and restoring */
#define SAVE_ORIG_ZE(a)    orig_##a = ze->##a;
#define HOOK_STEALTH_ZE(a) ze->##a  = stealth_##a;
#define RESTORE_ORIG_ZE(a) ze->##a  = orig_##a;

/** Stealth extension shutdown */
static void stealth_shutdown(zend_extension *extension)
{
	Linkedlist     * list = manager_extension_list;
	Node           * itr  = list->head->next;
	zend_llist     * zll  = NULL;
	zend_extension * ex   = NULL;

	/* Run shutdown of the extension we hooked into */
	if(orig_shutdown != NULL)
	{
		orig_shutdown(extension);
	}

	BACKUP_ZEND_EXTENSIONS_LIST()

	/* Handle our list of extensions */
	while(itr != list->tail)
	{
		/* Current extension being iterated */
		zll = (zend_llist *)itr->element;
		ex  = (zend_extension *)&zll->head->data;

		REPLACE_ZEND_EXTENSIONS_LIST(*zll)

		/* Shutdown extension */
		if(ex->shutdown != NULL)
		{
			ex->shutdown(ex);
		}

		/* Unload extension */
		if(ex->handle)
		{
			DL_UNLOAD(ex->handle);
		}

		/* Advance iterator */
		itr = itr->next;
	}

	RESTORE_ZEND_EXTENSIONS_LIST()

	/* Unhook everything */
	RESTORE_ORIG_ZE(startup)
	RESTORE_ORIG_ZE(shutdown)
	RESTORE_ORIG_ZE(activate)
	RESTORE_ORIG_ZE(deactivate)
	RESTORE_ORIG_ZE(message_handler)
	RESTORE_ORIG_ZE(op_array_handler)
	RESTORE_ORIG_ZE(statement_handler)
	RESTORE_ORIG_ZE(fcall_begin_handler)
	RESTORE_ORIG_ZE(fcall_end_handler)
	RESTORE_ORIG_ZE(op_array_ctor)
	RESTORE_ORIG_ZE(op_array_dtor)

	/* Destroy the list of extensions loaded by manager */
	linkedlist_destroy(list);

	/* Wipe addresses */
	ze = NULL;
	manager_extension_list = NULL;
}

/** Stealth extension startup */
static int stealth_startup(zend_extension *extension)
{
	/* Have we initialized version info yet? */
	static int initialized = 0;

	/* Other stuff for hooking purposes */
	int retval;
	zend_module_entry *module_entry_ptr;

	/* Pointers used for iteration of our extension list */
	Linkedlist     * list = manager_extension_list;
	Node           * itr  = list->head->next;
	zend_llist     * zll  = NULL;
	zend_extension * ex   = NULL;

	/* Fetch thread information, we'll need this later */
	TSRMLS_FETCH();

	/* Set our module handle to null to avoid race conditions */
	if(zend_hash_find(&module_registry, "manager", sizeof("manager"), (void **)&module_entry_ptr) == SUCCESS)
	{
		module_entry_ptr->handle = NULL;
	}
	else
	{
		return FAILURE;
	}

	/* Hook everything but ignore already hooked startup */
	HOOK_STEALTH_ZE(shutdown)
	HOOK_STEALTH_ZE(activate)
	HOOK_STEALTH_ZE(deactivate)
	HOOK_STEALTH_ZE(message_handler)
	HOOK_STEALTH_ZE(op_array_handler)
	HOOK_STEALTH_ZE(statement_handler)
	HOOK_STEALTH_ZE(fcall_begin_handler)
	HOOK_STEALTH_ZE(fcall_end_handler)
	HOOK_STEALTH_ZE(op_array_ctor)
	HOOK_STEALTH_ZE(op_array_dtor)

	/* Run startup of the extension we hooked into */
	retval = (orig_startup == NULL) ? SUCCESS : orig_startup(extension);

	BACKUP_ZEND_EXTENSIONS_LIST()

	/* Handle our list of extensions */
	while(itr != list->tail)
	{
		/* Current extension being iterated */
		zll = (zend_llist *)itr->element;
		ex  = (zend_extension *)&zll->head->data;

		REPLACE_ZEND_EXTENSIONS_LIST(*zll)

		/* Startup the extension */
		if(ex->startup != NULL)
		{
			ex->startup(ex);
		}

		if(!initialized && MANAGER_G(replace_info))
		{
			/* To hold new information regarding loaded zend extensions */
			char * new_info = NULL;
			unsigned int new_info_length = 0;

			/* Ugly but working hack */
			new_info_length = sizeof("\n    with  v, , by ")
							+ strlen(extension->author)
							+ strlen(ex->name)
							+ strlen(ex->version)
							+ strlen(ex->copyright)
							+ strlen(ex->author);

			new_info = (char *) malloc(new_info_length+1);

			/* Append */
			sprintf(new_info, "%s\n    with %s v%s, %s, by %s", extension->author, ex->name, ex->version, ex->copyright, ex->author);

			/* As we will replace this pointer, free the original */
			free(extension->author);

			/* Replace */
			extension->author = new_info;
		}

		/* Advance iterator */
		itr = itr->next;
	}

	/* Save that we initialized */
	initialized = 1;

	RESTORE_ZEND_EXTENSIONS_LIST()

	return retval;
}

/** Comparator for elements of linked list */
static int manager_cmp(void * e1, void * e2)
{
	return strcmp((((zend_extension *)(((zend_llist *)e2)->head->data))->name), (((zend_extension *)(((zend_llist *)e1)->head->data))->name));
}

/** Print function for element of linked list */
static void manager_print(void * e1)
{
	if(e1 == NULL)
		return;

	printf("%p -> %s", e1, (((zend_extension *)(((zend_llist *)e1)->head->data))->name));
}

/** Destroy element of linked list */
static void manager_destroy(void * e1)
{
	if(e1 == NULL)
		return;

	/* Run destructor of element, in this case zend_llist destructor */
	zend_llist_destroy((zend_llist *)e1);

	/* As we malloc'ed the element, we will destroy that too */
	free(e1);
}

/** Register a recently loaded zend extension */
static int manager_register_zend_extension(zend_extension * new_extension, DL_HANDLE handle)
{
	zend_extension extension;

	extension = *new_extension;
	extension.handle = handle;

	/* Create a list that will store all zend extensions loaded by manager */
	if(manager_extension_list == NULL)
	{
		manager_extension_list = linkedlist_create(&manager_cmp, &manager_print, &manager_destroy);
	}

	/* Load invisible to other Zend Extensions */
	if(zend_llist_count(&zend_extensions) == 0)
	{
		/* Add extension to the zend engine list */
		zend_llist_add_element(&zend_extensions, &extension);
	}
	else
	{
		/* Create a new Zend Linked List to store that extension entry */
		zend_llist * extension_list = malloc(sizeof(zend_llist));

		/* Add extension entry to recently created list */
		zend_llist_init(extension_list, sizeof(zend_extension), NULL, 1);
		zend_llist_add_element(extension_list, &extension);

		/* Add the list we just created to our custom list */
		linkedlist_addLast(manager_extension_list, (void *)extension_list);
	}

	if(ze == NULL)
	{
		/* Grab the last loaded zend extension */
		ze = (zend_extension *)zend_llist_get_last_ex(&zend_extensions, &lp);

		/* Save pointers to original zend extension methods */
		SAVE_ORIG_ZE(startup)
		SAVE_ORIG_ZE(shutdown)
		SAVE_ORIG_ZE(activate)
		SAVE_ORIG_ZE(deactivate)
		SAVE_ORIG_ZE(message_handler)
		SAVE_ORIG_ZE(op_array_handler)
		SAVE_ORIG_ZE(statement_handler)
		SAVE_ORIG_ZE(fcall_begin_handler)
		SAVE_ORIG_ZE(fcall_end_handler)
		SAVE_ORIG_ZE(op_array_ctor)
		SAVE_ORIG_ZE(op_array_dtor)

		/* At this stage, hook startup */
		HOOK_STEALTH_ZE(startup)
	}

#if 0
	fprintf(stderr, "Loaded %s, version %s\n", extension.name, extension.version);
#endif

	return SUCCESS;
}

/** Load a zend extension */
static int manager_load_zend_extension(char *path TSRMLS_DC)
{
	DL_HANDLE handle;
	zend_extension *new_extension;
	zend_extension_version_info *extension_version_info;

	handle = DL_LOAD(path);
	if(!handle)
	{
		fprintf(stderr, "Failed loading %s\n", path);
		/* See http://support.microsoft.com/kb/190351 */
		#ifdef PHP_WIN32
			fflush(stderr);
		#endif
		return FAILURE;
	}

	extension_version_info = (zend_extension_version_info *) DL_FETCH_SYMBOL(handle, "extension_version_info");
	if(!extension_version_info)
		extension_version_info = (zend_extension_version_info *) DL_FETCH_SYMBOL(handle, "_extension_version_info");

	new_extension = (zend_extension *) DL_FETCH_SYMBOL(handle, "zend_extension_entry");
	if(!new_extension)
		new_extension = (zend_extension *) DL_FETCH_SYMBOL(handle, "_zend_extension_entry");

	if(!extension_version_info || !new_extension)
	{
		fprintf(stderr, "%s doesn't appear to be a valid Zend extension\n", path);
		/* See http://support.microsoft.com/kb/190351 */
		#ifdef PHP_WIN32
			fflush(stderr);
		#endif
		DL_UNLOAD(handle);
		return FAILURE;
	}

	/* allow extension to proclaim compatibility with any Zend version */
	if(extension_version_info->zend_extension_api_no != ZEND_EXTENSION_API_NO &&(!new_extension->api_no_check || new_extension->api_no_check(ZEND_EXTENSION_API_NO) != SUCCESS))
	{
		if(extension_version_info->zend_extension_api_no > ZEND_EXTENSION_API_NO)
		{
			fprintf(stderr, "%s requires Zend Engine API version %d.\n"
					"The Zend Engine API version %d which is installed, is outdated.\n\n",
					new_extension->name,
					extension_version_info->zend_extension_api_no,
					ZEND_EXTENSION_API_NO);
			/* See http://support.microsoft.com/kb/190351 */
			#ifdef PHP_WIN32
				fflush(stderr);
			#endif
			DL_UNLOAD(handle);
			return FAILURE;
		}
		else if(extension_version_info->zend_extension_api_no < ZEND_EXTENSION_API_NO)
		{
			fprintf(stderr, "%s requires Zend Engine API version %d.\n"
					"The Zend Engine API version %d which is installed, is newer.\n"
					"Contact %s at %s for a later version of %s.\n\n",
					new_extension->name,
					extension_version_info->zend_extension_api_no,
					ZEND_EXTENSION_API_NO,
					new_extension->author,
					new_extension->URL,
					new_extension->name);
			/* See http://support.microsoft.com/kb/190351 */
			#ifdef PHP_WIN32
				fflush(stderr);
			#endif
			DL_UNLOAD(handle);
			return FAILURE;
		}
	}
#if PHP_VERSION_ID < 50300
	else if(ZTS_V!=extension_version_info->thread_safe)
	{
		fprintf(stderr, "Cannot load %s - it %s thread safe, whereas Zend %s\n",
				new_extension->name,
				(extension_version_info->thread_safe?"is":"isn't"),
				(ZTS_V?"is":"isn't"));
		/* See http://support.microsoft.com/kb/190351 */
		#ifdef PHP_WIN32
			fflush(stderr);
		#endif
		DL_UNLOAD(handle);
		return FAILURE;
	}
	else if(ZEND_DEBUG!=extension_version_info->debug)
	{
		fprintf(stderr, "Cannot load %s - it %s debug information, whereas Zend %s\n",
				new_extension->name,
				(extension_version_info->debug?"contains":"does not contain"),
				(ZEND_DEBUG?"does":"does not"));
		/* See http://support.microsoft.com/kb/190351 */
		#ifdef PHP_WIN32
			fflush(stderr);
		#endif
		DL_UNLOAD(handle);
		return FAILURE;
	}
#else
	else if(MANAGER_G(check_build))
	{
		if(strcmp(ZEND_EXTENSION_BUILD_ID, extension_version_info->build_id) &&
			(!new_extension->build_id_check || new_extension->build_id_check(ZEND_EXTENSION_BUILD_ID) != SUCCESS))
		{
			fprintf(stderr, "Cannot load %s - it was built with configuration %s, whereas running engine is %s\n",
					new_extension->name, extension_version_info->build_id, ZEND_EXTENSION_BUILD_ID);
			/* See http://support.microsoft.com/kb/190351 */
			#ifdef PHP_WIN32
				fflush(stderr);
			#endif
			DL_UNLOAD(handle);
			return FAILURE;
		}
	}
#endif

	if(MANAGER_G(load_stealth))
	{
		/* In stealth mode register the extension in our manager */
		return manager_register_zend_extension(new_extension, handle);
	}

	/* In normal mode register the extension in Zend engine manager */
	return zend_register_extension(new_extension, handle);
}

/** Make possible to load a shared module */
#ifdef COMPILE_DL_MANAGER
ZEND_GET_MODULE(manager)
#endif

/** php.ini entries */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("manager.modules_dir", "", PHP_INI_SYSTEM, OnUpdateString, modules_dir, zend_manager_globals, manager_globals)
	STD_PHP_INI_ENTRY("manager.modules_list", "", PHP_INI_SYSTEM, OnUpdateString, modules_list, zend_manager_globals, manager_globals)
	STD_PHP_INI_BOOLEAN("manager.check_build", "1", PHP_INI_SYSTEM, OnUpdateBool, check_build, zend_manager_globals, manager_globals)
	STD_PHP_INI_BOOLEAN("manager.load_stealth", "0", PHP_INI_SYSTEM, OnUpdateBool, load_stealth, zend_manager_globals, manager_globals)
	STD_PHP_INI_BOOLEAN("manager.replace_info", "1", PHP_INI_SYSTEM, OnUpdateBool, replace_info, zend_manager_globals, manager_globals)
PHP_INI_END()

/** Module Globals constructor */
static void ZEND_MODULE_GLOBALS_CTOR_N(manager)(void *arg TSRMLS_DC)
{
	zend_manager_globals *manager_globals = arg;

	/* Initialize module globals */
	manager_globals->modules_dir  = NULL;
	manager_globals->modules_list = NULL;
	manager_globals->check_build  = 1;
	manager_globals->load_stealth = 0;
	manager_globals->replace_info = 1;
}

/** Module init */
PHP_MINIT_FUNCTION(manager)
{
	/* Register entries in php.ini */
	REGISTER_INI_ENTRIES();

	{
		/* Module configuration */
		char * mod_dir  = MANAGER_G(modules_dir);
		char * mod_list = MANAGER_G(modules_list);
		int mod_dir_len = strlen(mod_dir);

		if(mod_dir && mod_list && *mod_dir != '\0' && *mod_list != '\0')
		{
			/* Tokenize modules list */
			char * buf = NULL;
			char * lst = strdup(mod_list);
			char * ptr = php_strtok_r(lst, ":", &buf);

			while(ptr != NULL)
			{
				/* Path to load */
				char * libpath = NULL;

				if(IS_SLASH(mod_dir[mod_dir_len-1])) {
					spprintf(&libpath, 0, "%s%s", mod_dir, ptr);
				} else {
					spprintf(&libpath, 0, "%s%c%s", mod_dir, DEFAULT_SLASH, ptr);
				}

				/* Load extension */
				manager_load_zend_extension(libpath TSRMLS_CC);

				/* Advance to the next */
				ptr = php_strtok_r(NULL, ":", &buf);

				/* Free allocation made by spprintf */
				efree(libpath);
			}

			/* We can free the memory used by the duplicated string */
			free(lst);
		}

		return SUCCESS;
	}
}

/** Module shutdown */
PHP_MSHUTDOWN_FUNCTION(manager)
{
	/* Unregister entries from php.ini */
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

/** Module info */
PHP_MINFO_FUNCTION(manager)
{
	/* Box header */
	if(!sapi_module.phpinfo_as_text)
		php_info_print_box_start(0);
	else
		PUTS("\n");

	/* Logo */
	if(!sapi_module.phpinfo_as_text)
	{
		char *enc_logo;
		int ret;

		PUTS("<img border=\"0\" src=\"data:image/jpeg;base64,");
		enc_logo=(char *)php_base64_encode(manager_logo, sizeof(manager_logo), &ret);
		if(enc_logo) {
			PUTS(enc_logo);
			efree(enc_logo);
		}
		PUTS("\" alt=\"Manager logo\" />\n");
	}

	/* Box text */
	PUTS("This server is enhanced by " MANAGER_AUTHOR "'s " MANAGER_NAME);
	PUTS(!sapi_module.phpinfo_as_text?"<br /><br />":"\n");
	PUTS("Copyright (c) 2014 " MANAGER_AUTHOR);

	/* Box footer */
	if(!sapi_module.phpinfo_as_text)
		php_info_print_box_end();
	else
		PUTS("\n");

	/* Information table */
	php_info_print_table_start();
	php_info_print_table_row(2, MANAGER_NAME " Support", "enabled");
	php_info_print_table_row(2, MANAGER_NAME " Version", MANAGER_VERSION);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

/** Module entry */
zend_module_entry manager_module_entry = {
	STANDARD_MODULE_HEADER,
	"manager",                    /* name */
	NULL,                         /* function entry */
	PHP_MINIT(manager),           /* module startup */
	PHP_MSHUTDOWN(manager),       /* module shutdown */
	NULL,                         /* request startup */
	NULL,                         /* request shutdown */
	PHP_MINFO(manager),           /* info func */
	MANAGER_VERSION,              /* version */
	PHP_MODULE_GLOBALS(manager),  /* globals descriptor */
	PHP_GINIT(manager),           /* globals ctor */
	NULL,                         /* globals dtor */
	NULL,                         /* post deactivate */
	STANDARD_MODULE_PROPERTIES_EX
};