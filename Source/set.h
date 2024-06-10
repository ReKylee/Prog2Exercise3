#ifndef __SET_H__
#define __SET_H__
#include <stdio.h>

/******************************************************************************
 * File: set.h
 * Author: David Avdat
 * Last update: 18.5.23
 *
 * Contents:
 *        This file contains the interface of a Generic Set ADT module.
 *
 * Defined Types :
 *        SetResult        - possible return values of the set interface
 *                           functions.
 *        SetElement       - the element of the set
 *        KeyForSetElement - the type of key for find_by_key and
 *                           filter_by_key functions
 *        Set              - a simple set ADT. Represents a set of generic
 *                           elements (without repetitions)
 *
 * Functions :
 *
 *        setCreate       - create an empty set
 *        setDestroy      - destroys the set
 *        setAdd          - adds a member to the set
 *        setRemove       - removes a member from the set
 *        setGetSize     - get number of elements in a set
 *
 *        setIsIn        - checks if an element is in the set
 *
 *        setFind         - finds a member of the set, which matches a key,
 *                           given as a parameter
 *        setFilter       - create a new subset of the given set, filtered by
 *                           a key, given as a parameter
 *
 *        setUnion        - create a union set of two given sets
 *        setIntersection - create an intersection set of two given sets
 *
 *        setPrint        - prints the set, sorted by the function, supplied
 *                           as a parameter to setCreate
 *        setPrintSorted - prints the set, sorted by a function,
 *                           supplied as a parameter
 *
 *        setGetFirst    - returns some representative element of the set -
 *                           used for primitive iteration over the set
 *        setGetNext     - returns the next element of the set -
 *                           used for primitive iteration over the set
 * Description:
 *    The Set holds copies of the objects supplied by the setAdd function.
 *
 *    The setCreate function requires four function's pointers:
 *    copySetElemFunc, cmpSetElemFunc, freeSetElemFunc and printSetElemFunc.
 *
 *    In case the users want the set to hold the supplied objects themselves
 *    (instead of their "clones"), they should supply a copy function that
 *    returns the same pointer received as a parameter and a free function
 *    that does nothing.
 *
 *    The Set uses cmpSetElemFunc to compare between elements and to print its
 *    elements, sorted by this function. The Set cannot hold identical
 *    elements, as defined by this compare function.
 *
 *    In order to traverse all the members of the set, one can use
 *    setGetFirst and setGetNext functions. Using setGetFirst function,
 *    and after it setGetNext function iteratively until it returns
 *    NULL, guarantees that the user will iterate over all elements of the Set.
 *    The order of traversing the set is undefined.
 *    If the set is modified while iterating (Any direct or in-direct change to one
 *    of its elements) the rest of the iteration is undefined.
 *    In that case you can use setGetFirst to restart the traversal of the set.
 *    Make sure that after any function that alters the set's state, or one of its
 *    members, you do not continue the iteration.
 *
 ******************************************************************************/



/* possible return values of the set interface functions */
typedef enum {
    SET_SUCCESS,
    SET_BAD_ARGUMENTS,
    SET_OUT_OF_MEMORY,
    SET_ELEMENT_EXISTS,
    SET_ELEMENT_DOES_NOT_EXIST
} SetResult;

/* the element of the set */
typedef void* SetElement ;

/* the key for filter_by_key and find_by_key functions */
typedef void* KeyForSetElement;

/* a simple generic set ADT */
typedef struct Set_t *Set ;

/******************************************************************************
 * Name: setCreate
 * Arguments: a pointer to a set to be created
 *            copySetElemFunc  - a function for copying elements of the set
 *            freeSetElemFunc  - a function for deleting elements of the set
 *            cmpSetElemFunc   - a function for comparing elements of the set
 *            printSetElemFunc - a function for printing elements of the set
 *
 *    In case the users want the set to hold the supplied objects themselves
 *    (instead of their "clones"), they should supply a copy function that
 *    returns the same pointer received as a parameter and a free function
 *    that does nothing.
 *
 * Return value:
 *      SET_SUCCESS.
 *      SET_BAD_ARGUMENTS - one of the arguments is NULL (printFunc may be
 *                          NULL, but will disallow using the print functions)
 *      SET_OUT_OF_MEMORY - failed to allocated needed memory
 *
 * Description:
 *      Creates a new empty set.
 *****************************************************************************/
typedef SetElement (*copySetElemFunc) (SetElement); /* should return NULL if
                                                       fails */
typedef void       (*freeSetElemFunc) (SetElement);

/* cmpSetElemFunc should return a negative number if the first element is less
 *                                  than the second
 *                              0 if the two elements are equal
 *                              a positive number if the first element is greater
 *                                  than the second
 */
typedef int        (*cmpSetElemFunc)   (SetElement, SetElement);
typedef void       (*printSetElemFunc) (FILE*, SetElement);

SetResult setCreate(Set* set /* out */,
    cmpSetElemFunc cmpFunc,
    copySetElemFunc cpyFunc,
    freeSetElemFunc freeFunc,
    printSetElemFunc printFunc);

/******************************************************************************
 * Name: setDestroy
 * Arguments:
 *      set - the set to be destroyed.
 * Return value:
 *      SET_SUCCESS.
 *      SET_BAD_ARGUMENTS - NULL was passed as an argument
 * Description:
 *      Destroys the set and releases the memory resources used by the set.
 *      Uses the freeSetElemFunc function, supplied to the setCreate, to free
 *      the memory allocated for the elements clones.
 *****************************************************************************/
SetResult setDestroy(Set set);

/******************************************************************************
 * Name: setAdd
 * Arguments:
 *      set - the set to which the element is added
 *      elm  - the element to be added
 * Return value:
 *      SET_SUCCESS
 *      SET_BAD_ARGUMENTS - a NULL pointer passed as 'set' or 'elm' parameters
 *      SET_OUT_OF_MEMORY - failed to allocate memory (or when the
 *                          copySetElemFunc, supplied to setCreate,
 *                          returns NULL)
 *      SET_ELEMENT_EXISTS - an identical element (as defined by
 *                           cmpSetElemFunc, supplied to setCreate) already
 *                           exists in the Set
 * Description:
 *      adds a copy of the element (using the copySetElemFunc) to the set, if
 *      the element is not already in the Set, as defined by cmpSetElemFunc.
 *      In case the element is already in the set, the set is not changed and
 *      the element is not added.
 *
 *****************************************************************************/
SetResult setAdd(Set set, SetElement elm);

/******************************************************************************
 * Name: setRemove
 * Arguments:
 *      set - the set from which the element is removed
 *      elm - the element to be removed
 * Return value:
 *      SET_SUCCESS
 *      SET_BAD_ARGUMENTS - a NULL pointer passed as 'set' or 'elm' parameters
 *      SET_ELEMENT_DOES_NOT_EXIST - the identical element (as defined by
 *                                   cmpSetElemFunc, supplied to setCreate)
 *                                   does not exist in the Set
 * Description:
 *      remove the copy of the element (using the freeSetElemFunc) from the
 *      set, if the element is already in the Set, as defined by the compare
 *      function, supplied to setCreate.
 *      In case the element is not in the set, the set is not changed and no
 *      element is not removed.
 *
 *****************************************************************************/
SetResult setRemove(Set set, SetElement elm);

/******************************************************************************
 * Name: setGetSize
 * Arguments: the set
 * Return value: number of elements in given set
 *               or -1 if the 'set' parameter is NULL
 *****************************************************************************/
int setGetSize(Set set);

/*****************************************************************************
 * Name: setIsIn
 *  Arguments:
 *        set             - the set
 *        elm             - the element
 *
 * Return value:
 *      SET_BAD_ARGUMENTS - a NULL pointer passed as 'set' or 'elm' parameters
 *      SET_ELEMENT_EXISTS         - the element is in the set
 *      SET_ELEMENT_DOES_NOT_EXIST - the element is not in the set
 *
 * Description:
 *      checks if the element is in the set, using compare function, provided
 *      in setCreate
 *****************************************************************************/

SetResult setIsIn(Set set,SetElement elm);

/* return 1 if the element matches the key, 0 if not */
typedef int (*matchSetElemFunc)(SetElement,KeyForSetElement);

/******************************************************************************
 * Name: setFilter
 * Arguments: set            - the original set
 *            result_set     - the set to hold the filtered elements
 *            filterSetElemFunc - a generic matching function
 *            filterElement  - is passed as the second parameter to
 *                             filterSetElemFunc
 *
 * Return value:
 *      SET_SUCCESS
 *      SET_BAD_ARGUMENTS - one of the arguments is NULL
 *      SET_OUT_OF_MEMORY - failed to allocate the result_set or cpySetElemFunc
 *                          returned NULL on one of the elements
 *
 * Description:
 *      creates a set of elements from given set, for which filterFunc returns
 *      non zero value.
 *      It is the responsibility of the user to destroy the result set.
 *      The copy, free, cmp and print functions of the new set are the same as
 *      in the given set
 *
 *  YOU CAN USE THIS FUNCTION AS COPY FUNCTION (for example: make filterFunc
 *  return 1 always)
 *
 *****************************************************************************/

SetResult setFilter(Set set, Set* result_set /* out */,
    KeyForSetElement filterElement,
    matchSetElemFunc filterFunc);

/******************************************************************************
 * Name: setFind
 *  Arguments:
 *        set             - the set for searching
 *        theFoundElement - pointer to element - the found element will be
 *                          returned there
 *        toFind          - a key by which we want to find the element
 *        matchFunc       - a function that returns 1 iff the element matches
 *                          the key
 *
 * Return value:
 *      SET_SUCCESS                - the element was found
 *      SET_BAD_ARGUMENTS          - a NULL pointer was passed as an argument
 *      SET_ELEMENT_DOES_NOT_EXIST - the element was not found
 *
 * Description:
 *      finds an element in the set, matching toFind according to matchFunc
 *      The found element will point to the element in the set (not to a copy
 *      of the element).
 *****************************************************************************/

SetResult setFind(Set set,SetElement* theFoundElement /* out */,
    KeyForSetElement toFind, matchSetElemFunc matchFunc);

/******************************************************************************
 * Name: setUnion
 * Arguments: set1           - the first set in the union
 *            set2           - the second set in the union
 *            result_set     - the set to hold the result
 *
 * Return value:
 *      SET_SUCCESS
 *      SET_BAD_ARGUMENTS - one of the arguments is NULL
 *      SET_OUT_OF_MEMORY - failed to allocate the result_set or cpySetElemFunc
 *                          returned NULL on one of the elements
 *
 * Description:
 *      creates a union set of two given sets
 *      It is the responsibility of the user to destroy the result set.
 *      The copy, free, cmp and print functions of the new set are the same as
 *      in the given first set. The equality of the members is decided using
 *      the compare function of the first set.
 *
 *****************************************************************************/

SetResult setUnion(Set set1, Set set2, Set* result_set /* out */);

/******************************************************************************
 * Name: setIntersection
 * Arguments: set1           - the first set in the intersection
 *            set2           - the second set in the intersection
 *            result_set     - the set to hold the result
 *
 * Return value:
 *      SET_SUCCESS
 *      SET_BAD_ARGUMENTS - one of the arguments is NULL
 *      SET_OUT_OF_MEMORY - failed to allocate the result_set or cpySetElemFunc
 *                          returned NULL on one of the elements
 *
 * Description:
 *      creates an intersection set of two given sets
 *      It is the responsibility of the user to destroy the result set.
 *      The copy, free, cmp and print functions of the new set are the same as
 *      in the given first set. The equality of the members is decided using
 *      the compare function of the first set.
 *
 *****************************************************************************/

SetResult setIntersection(Set set1, Set set2, Set* result_set /* out */);

/******************************************************************************
 * Name: setDifference
 * Arguments: set1           - the first set in the difference
 *            set2           - the second set in the difference
 *            result_set     - the set to hold the result
 *
 * Return value:
 *      SET_SUCCESS
 *      SET_BAD_ARGUMENTS - one of the arguments is NULL
 *      SET_OUT_OF_MEMORY - failed to allocate the result_set or cpySetElemFunc
 *                          returned NULL on one of the elements
 *
 * Description:
 *      creates a difference set of two given sets *result_set = A/B
 *      A/B set of all elements of A, which are not in B
 *      It is the responsibility of the user to destroy the result set.
 *      The copy, free, cmp and print functions of the new set are the same as
 *      in the given first set. The equality of the members is decided using
 *      the compare function of the first set.
 *
 *****************************************************************************/

SetResult setDifference(Set set1, Set set2, Set* result_set /* out */);

/******************************************************************************
 * Name: setPrint
 *  Arguments:
 *      set       - the set to print
 *      outstream - the output stream to print the set
 *      n         - first elements to print
 * Return value:
 *      SET_SUCCESS.
 *      SET_BAD_ARGUMENTS - NULL pointers passed as arguments or n < 0
 *
 * Description:
 *      prints the set using the print function, supplied to setCreate
 *      the printed elements are sorted using the compare function, supplied to
 *      setCreate
 * Remarks:
 *      if n >= number of elements in the set, all the set is printed
 *****************************************************************************/
SetResult setPrint(Set set, FILE* outstream, int n);

/******************************************************************************
 * Name: setPrintSorted
 *  Arguments:
 *      set       - the set to print
 *      outstream - the output stream to print the set
 *      n         - first elements to print
 *      cmpFunc   - the function used for sorting
 *
 * Return value:
 *      SET_SUCCESS.
 *      SET_BAD_ARGUMENTS - NULL pointers passed as arguments or n < 0
 *
 * Description:
 *      prints the set using the print function, supplied to setCreate
 *      the printed elements are sorted using the compare function, passed as a
 *      parameter
 *
 * Remarks:
 *      If n >= number of elements in the set, all the set is printed.
 *      This function could be used for printing elements of the set by an
 *      order, other than that, defined by the compare function, passed to
 *      setCreate
 *****************************************************************************/
SetResult setPrintSorted(Set set, FILE* outstream, int n,
    cmpSetElemFunc cmpFunc);

/******************************************************************************
 * Name: setGetFirst
 * Arguments:
 *      set - the given set
 *      elm - the pointer to an element in the set
 *
 * Return value:
 *      SET_SUCCESS.
 *      SET_BAD_ARGUMENTS - a NULL pointer passed as an parameter.
 *      SET_ELEMENT_DOES_NOT_EXIST - the set is empty
 *
 * Description:
 *      returns some representative element of the set -
 *      used for primitive iteration over the set. The returned element 'elm'
 *      will point to the element in the set (not to a copy of the element).
 *
 * PLEASE NOTE:
 *    In order to traverse all the members of the set, one can use
 *    setGetFirst and setGetNext functions. Using setGetFirst function,
 *    and after it setGetNext function iteratively until it returns
 *    SET_ELEMENT_DOES_NOT_EXIST, garantees that the user will iterate over all
 *    the elements of the Set.
 *    The order of the iteration is undefined. Also, the results of iteration
 *    are undefined, if any other functions of the module are called on the
 *    Set between the start and the end of the iteration. These functions must
 *    be ONLY used for traversing ALL the elements of the set, without calling
 *    any other Set functions between the start and the end of the iteration.
 *
 *****************************************************************************/
SetResult setGetFirst(Set set,SetElement* firstElm);

/******************************************************************************
 * Name: setGetNext
 *  Arguments:
 *      set - the given set
 *      elm - the pointer to an element in the set
 *
 * Return value:
 *      SET_SUCCESS.
 *      SET_BAD_ARGUMENTS          - a NULL pointer passed as an parameter.
 *      SET_ELEMENT_DOES_NOT_EXIST - there is no next element in the set,
 *                                   all the set has been traversed
 * Description:
 *      returns the next element of the set -
 *      used for primitive iteration over the set. The returned element 'elm'
 *      will point to the element in the set (not to a copy of the element).
 *
 * PLEASE NOTE:
 *    In order to traverse all the members of the set, one can use
 *    setGetFirst and setGetNext functions. Using setGetFirst function,
 *    and after it setGetNext function iteratively until it returns
 *    SET_ELEMENT_DOES_NOT_EXIST, garantees that the user will iterate over all
 *    the elements of the Set.
 *    The order of the iteration is undefined. Also, the results of iteration
 *    are undefined, if any other functions of the module are called on the
 *    Set between the start and the end of the iteration. These functions must
 *    be ONLY used for traversing ALL the elements of the set, without calling
 *    any other Set functions between the start and the end of the iteration.
 *
 *****************************************************************************/
SetResult setGetNext(Set set,SetElement* NestElm /* out */);

#endif

