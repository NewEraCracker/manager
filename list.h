/*
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
*/

/**
 * Include file for a general-purpose double linked list which is
 * implemented using circular nodes. Provided are functions for
 * addition and removal of data from the list. Warning: NULL data
 * cannot be stored by the list. Additionally, the list provides
 * the means to add sorted data, but it's best to keep in mind that
 * this process might not be as fast as needed.
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

	#include <stdio.h>
	#include <stddef.h>
	#include <string.h>
	#include <malloc.h>

	#define LINKEDLIST_INSERT_SUCCESSEFUL	 1
	#define LINKEDLIST_ERROR_UNKNOWN		 0
	#define LINKEDLIST_ERROR_NULL_LIST		-1
	#define LINKEDLIST_ERROR_NULL_DATA		-2
	#define LINKEDLIST_ERROR_NULL_COMPARE	-3
	#define LINKEDLIST_ERROR_REPEATED_DATA	-4
	#define LINKEDLIST_ERROR_NO_MEMORY		-5

	/*****************************************************************************
	 *
	 * Structure Declaration
	 *		- struct Node, describes a private structure that is a node in a
	 *		  Linkedlist. The node contains three fields, a pointer to the previous
	 *		  element, another pointer to the next element and a void* data field.
	 *
	 *		- struct Linkedlist, describes the List structure composed of Node.
	 *		  At the basic level, the list contains only three fields, which are
	 *		  head and tail nodes, for addition at the begining and end of list and
	 *		  an integer that keeps track of the number of nodes in the list.
	 *
	 *		  	         ----------------------------------+
	 *		  	         |                                 |
	 *		  	    +----|------+                   +------|----+
	 *		  	    |    v O----|-------------------|----> O    |
	 *		  	    +-----------+                   +-----------+
	 *		  	+---|----O <----|-------------------|----O <----|---+
	 *		  	|   +-----------+                   +-----------+   |
	 *		  	|   |   NULL    |                   |   NULL    |   |
	 *		  	|   |    DATA   |                   |     DATA  |   |
	 *		  	|   +-----------+                   +-----------+   |
	 *		  	|     Head Node                       Tail Node     |
	 *		  	|                                                   |
	 *		  	+---------------------------------------------------+
	 *
	 *			The connection of an empty list, is as shown, a circular connection.
	 *			This makes it easy to add and remove any nodes, since all the data
	 *			will be contained and constrained by the Head and Tail Nodes.
	 *
	 *******************************************************************************/

	typedef struct Node
	{
		struct Node * next;
		struct Node * prev;
		void * element;
	} Node;

	typedef struct
	{
		Node * head;
		Node * tail;
		int count;
		int (*cmp)(void *, void *);
		void (*print)(void *);
		void (*destroy)(void *);
	} Linkedlist;

	/*********************************************************************************
	 *	- Constructor: Linkedlist * createList()
	 *		Creates a new Linkedlist and returns its pointer. Returns NULL if
	 *		no space can be aquired to store the list.
	 *********************************************************************************/
	Linkedlist * linkedlist_create( int (*cmp)(void *, void *), void (*print)(void *), void (*destroy)(void *) );


	/*********************************************************************************
	 *	- Destructor: void destroyList(Linkedlist* list)
	 *		Completely frees all the allocated space for the Linkedlist and will
	 *		destroy all its nodes.
	 *********************************************************************************/
	void linkedlist_destroy(Linkedlist * list);

	/**********************************************************************************
	 *	- Insertion:
	 *		Provided are these basic functions for working with the linked list
	 *		these are:
	 *			o int addFirst(Linkedlist* list, void * elem)
	 *			o int addLast(Linkedlist* list, void * elem)
	 *			o int addSorted(Linkedlist* list, void * elem,
	 *							int (*cmp)(void * e1, void * e2))
	 *
	 *		The functions addFirst and addLast will add a single new element
	 *		to the begining or end of the list, respectively. The function
	 *		addSorted has the same funcionality, but will add the element in
	 *		a sorted fashion, as long as a comparator function is present.
	 *		The compare function should return a negative value when argument1
	 *		is less than argument2, a positive value when argument1 is more than
	 *		argument2 or zero, when equal. NULL data pointers are not allowed
	 *		for any of the functions. Returns an integer that when positive
	 *		means the number of added elements to the list or when negative,
	 *		means an error code. Error codes are defined below.
	 *********************************************************************************/
	int linkedlist_addFirst(Linkedlist * list, void * elem);

	int linkedlist_addLast(Linkedlist * list, void * elem);

	int linkedlist_addSorted(Linkedlist * list, void * elem);

	/**********************************************************************************
	 *	- Removal:
	 *		The removal works exactly the same way as the insertion process.
	 *		Provided are the following functions:
	 *			o void* removeFirst(Linkedlist* list)
	 *			o void* removeLast(Linkedlist* list)
	 *			o void* removeSorted(Linkedlist* list, void * elem,
	 *					int (*cmp)(void * e1, void * e2))
	 *
	 *		All the functions have similar behaviour. The functions removeFirst
	 *		and removeLast, remove the elements at the start and end of the
	 *		list respectively. The function remove sorted is useful when the
	 *		data is sorted, since most of the time, it is not necessary to run
	 *		through all the elements in the list. Assumed are valid linked list
	 *		pointers and for the removeSorted function, a valid comparator
	 *		function. These functions return the removed element, or NULL in
	 *		case of an error or non-existant element.
	 *********************************************************************************/
	void * linkedlist_removeFirst(Linkedlist * list);

	void * linkedlist_removeLast(Linkedlist * list);

	void * linkedlist_removeSorted(Linkedlist * list, void * elem);

	/**********************************************************************************
	 *	- Lookup:
	 *		Lookup functions serve as a way to test if the list contains a given
	 *		element. These functions are:
	 *			o int contains(Linkedlist * list, void * elem)
	 *			o int containsSorted(Linkedlist * list, void * elem,
	 *					int (*cmp)(void * e1, void * e2))
	 *		Both functions work in a similar way. They both search for the
	 *		given element inside the list and both return an integer for boolean
	 *		logic (0 = not in the list, not 0 = in the list). The function
	 *		containsSorted is smarter since it can stop when the list is sorted,
	 *		so searching the entire list is not needed. Both data pointers
	 *		and compare functions cannot be NULL.
	 *********************************************************************************/

	Node * linkedlist_contains(Linkedlist * list, void * elem);

	Node * linkedlist_containsSorted(Linkedlist * list, void * elem);

	void * linkedlist_search(Linkedlist * list, void * elem);

	void * linkedlist_searchSorted(Linkedlist * list, void * elem);

	/**********************************************************************************
	 *	- Function: int isEmpty(Linkedlist* ll)
	 *		Returns wether the current list is empty or not. Returns 0 when it
	 *		is not empty or the list is NULL or not 0 when it is empty.
	 *********************************************************************************/
	int linkedlist_isEmpty(Linkedlist * list);

	/**********************************************************************************
	 *	- Function: int getCount(Linkedlist * list)
	 *		Returns the number of elements currently stored inside the linked
	 *		list passed as the argument. Returns ERROR_NULL_LIST when list is
	 *		NULL.
	 *********************************************************************************/
	int linkedlist_getCount(Linkedlist * list);

	/**********************************************************************************
	 *	- Function: void print(Linkedlist * list)
	 *		This function iterates the list while printing the elements using
	 *		function print supplied by parameter when the list was constructed
	 *********************************************************************************/
	 void linkedlist_print(Linkedlist * list);
#endif