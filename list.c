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
 * Code file for a general-purpose double linked list which is
 * implemented using circular nodes. Provided are functions for
 * addition and removal of data from the list. Warning: NULL data
 * cannot be stored by the list. Additionally, the list provides
 * the means to add sorted data, but it's best to keep in mind that
 * this process might not be as fast as needed.
 */

#include "list.h"

/*********************************************************************************
 *	- Constructor: Linkedlist * createList()
 *		Creates a new Linkedlist and returns its pointer. Returns NULL if
 *		no space can be aquired to store the list.
 *********************************************************************************/
Linkedlist * linkedlist_create( int (*cmp)(void *, void *) , void (*print)(void *), void (*destroy)(void *) )
{
	if(cmp == NULL)
		return NULL;
	if(print == NULL)
		return NULL;

	{
		/* Obtain space for the head and tail nodes */
		Linkedlist * list = malloc(sizeof(Linkedlist));
		if( list != NULL )
		{
			/* Make sure memory contents are set to NULL */
			memset(list, 0, sizeof(Linkedlist));

			/* Init */
			list->head = malloc(sizeof(Node));
			list->tail = malloc(sizeof(Node));

			/* Link circular list */
			if(list->head != NULL && list->tail != NULL)
			{
				/* Make sure memory contents are set to NULL */
				memset(list->head, 0, sizeof(Node));
				memset(list->tail, 0, sizeof(Node));

				/* Init head */
				list->head->next = list->tail;
				list->head->prev = list->tail;

				/* Init tail */
				list->tail->next = list->head;
				list->tail->prev = list->head;

				/* Init others */
				list->cmp = cmp;
				list->print = print;
				list->destroy = destroy;
			}
			else
			{
				/* Clear if no space aquired */
				free(list->head);
				free(list->tail);
				free(list);
				list = NULL;
			}
		}
		return list;
	}
}

/*********************************************************************************
 *	- Destructor: void destroyList(Linkedlist* list)
 *		Completely frees all the allocated space for the Linkedlist and will
 *		destroy all its nodes.
 *********************************************************************************/
void linkedlist_destroy(Linkedlist * list)
{
	if(list)
	{
		/* Obtain the first node in the list */
		Node * node = list->head;

		/* Destroy all the nodes */
		while(node != list->tail)
		{
			Node * to_destroy = node;
			node = node->next;

			/* Run element destructor if supplied */
			if(to_destroy->element != NULL && list->destroy != NULL)
				list->destroy(to_destroy->element);

			/* Deallocate the node */
			free(to_destroy);
		}

		/* Free the space for the list */
		free(list->head);
		free(list->tail);
		free(list);
	}
}

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
Node * linkedlist_addNode(Linkedlist * list, Node * left, Node * right, void * elem)
{
	Node * new_node = malloc(sizeof(Node));

	if(new_node != NULL)
	{
		/* Make sure memory contents are set to NULL */
		memset(new_node, 0, sizeof(Node));

		/* Linking node pointers */
		new_node->element = elem;
		new_node->prev = left;
		new_node->next = right;

		/* Update left and right nodes */
		left->next = new_node;
		right->prev = new_node;

		/* Increase elements number */
		list->count++;
	}

	return new_node;

}

int linkedlist_addFirst(Linkedlist * list, void * elem)
{
	if(list == NULL)
		return LINKEDLIST_ERROR_NULL_LIST;
	if(elem == NULL)
		return LINKEDLIST_ERROR_NULL_DATA;

	/* Insert verifying memory usage */
	if(linkedlist_addNode( list, list->head, list->head->next, elem ) == NULL)
		return LINKEDLIST_ERROR_NO_MEMORY;

	return 1;
}

int linkedlist_addLast(Linkedlist * list, void * elem)
{
	if(list == NULL)
		return LINKEDLIST_ERROR_NULL_LIST;
	if(elem == NULL)
		return LINKEDLIST_ERROR_NULL_DATA;

	/* Insert verifying memory usage */
	if(linkedlist_addNode(list, list->tail->prev, list->tail, elem) == NULL)
		return LINKEDLIST_ERROR_NO_MEMORY;

	return 1;
}

int linkedlist_addSorted(Linkedlist * list, void * elem)
{
	/* Verify valid arguments */
	if(list == NULL)
		return LINKEDLIST_ERROR_NULL_LIST;
	if(elem == NULL)
		return LINKEDLIST_ERROR_NULL_DATA;

	{
		/* Insert with comparison */
		Node * itr = list->head;
		while(itr != list->tail)
		{
			/* Compare the current data with next data */
			int cmp1 = ((itr->element != NULL) ? list->cmp(itr->element, elem) : 0);
			int cmp2 = ((itr->element != NULL) ? (0 - (list->cmp(elem, itr->next->element))) : 0);

			/* Verify if the spot has been found */
			if(cmp1 >= 0 && cmp2 <= 0)
			{
				/* Insert verifying memory usage */
				if( linkedlist_addNode(list, itr, itr->next, elem) == NULL )
					return LINKEDLIST_ERROR_NO_MEMORY;

				/* Return that a element has been added */
				return LINKEDLIST_INSERT_SUCCESSEFUL;
			}

			/* Advance iterator */
			itr = itr->next;
		}
	}

	/* Program shouldn't reach here */
	return LINKEDLIST_ERROR_UNKNOWN;
}

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
void * linkedlist_remove(Linkedlist * list, Node * left, Node * right)
{
	/* Obtain data and remove node */
	void * elem = left->next->element;
	free(left->next);

	/* Reset the linking of the left and right nodes */
	left->next  = right;
	right->prev = left;

	/* Decrease elements number */
	list->count--;

	/* Return the data */
	return elem;
}

void * linkedlist_removeFirst(Linkedlist * list)
{
	if(list == NULL)
		return NULL;

	return linkedlist_remove(list, list->head, list->head->next->next);
}

void * linkedlist_removeLast(Linkedlist * list)
{
	if(list == NULL)
		return NULL;

	return linkedlist_remove(list, list->tail->prev->prev, list->tail);
}

void * linkedlist_removeSorted(Linkedlist * list, void * elem)
{
	/* Verify valid arguments */
	if(list == NULL || elem == NULL)
	{
		return NULL;
	}

	{
		Node * itr = list->head->next;
		while(itr != list->tail)
		{
			int cmp1  = list->cmp(elem, itr->element);

			/* Compare elements */
			if(cmp1 == 0)
				return linkedlist_remove(list, itr->prev, itr->next);

			if(cmp1 > 0)
				return NULL;

			/* Advance iterator */
			itr = itr->next;
		}
	}

	return NULL;
}

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
Node * linkedlist_contains(Linkedlist * list, void * elem)
{
	/* Verify valid arguments */
	if(list == NULL)
		return NULL;
	if(elem == NULL)
		return NULL;

	{
		Node * itr = list->head->next;
		while(itr != list->tail)
		{
			/* Compare elements */
			if(list->cmp(elem, itr->element) == 0)
				return itr;

			/* Advance iterator */
			itr = itr->next;
		}
	}

	return NULL;
}

Node * linkedlist_containsSorted(Linkedlist * list, void * elem)
{
	/* Verify valid arguments */
	if(list == NULL || elem == NULL)
	{
		return NULL;
	}

	{
		Node * itr = list->head->next;
		while(itr != list->tail)
		{
			int cmp1 = list->cmp(elem, itr->element);

			/* Compare elements */
			if(cmp1 == 0)
				return itr;

			if(cmp1 > 0)
				return NULL;

			/* Advance iterator */
			itr = itr->next;
		}
	}

	return NULL;
}

void * linkedlist_search(Linkedlist * list, void * elem)
{
	Node * aux = linkedlist_contains(list, elem);

	if(aux == NULL)
		return NULL;

	return aux->element;
}

void * linkedlist_searchSorted(Linkedlist * list, void * elem)
{
	Node * aux = linkedlist_containsSorted(list, elem);

	if(aux == NULL)
		return NULL;

	return aux->element;
}

/**********************************************************************************
 *	- Function: int isEmpty(Linkedlist* ll)
 *		Returns wether the current list is empty or not. Returns 0 when it
 *		is not empty or the list is NULL or not 0 when it is empty.
 *********************************************************************************/
int linkedlist_isEmpty(Linkedlist * list)
{
	return list->count==0?1:0;
}

/**********************************************************************************
 *	- Function: int getCount(Linkedlist * list)
 *		Returns the number of elements currently stored inside the linked
 *		list passed as the argument. Returns ERROR_NULL_LIST when list is
 *		NULL.
 *********************************************************************************/
int linkedlist_getCount(Linkedlist * list)
{
	return list->count;
}

/**********************************************************************************
 *	- Function: void print(Linkedlist * list)
 *		This function iterates the list while printing the elements using
 *		function print supplied by parameter when the list was constructed
 *********************************************************************************/
void linkedlist_print(Linkedlist * list)
{
	if(list == NULL)
	{
		printf("Invalid list!");
		return;
	}

	printf(" ^\t|\n");
	printf(" |\tV\n");

	{
		/* Obtain the first node in the list */
		Node * itr = list->head->next;

		/* Print */
		while(itr != list->tail)
		{
			printf("--------------\n");
			list->print(itr->element);
			printf("--------------\n");
			itr = itr->next;

			printf(" ^\t|\n");
			printf(" |\tV\n");
		}
	}
}