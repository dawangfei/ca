/******************************************************************
*  $Id: hashtable.c,v 1.1 2004/10/29 09:28:25 snowdrop Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2003-2004  Ferhat Ayaz
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA  02111-1307, USA.
*
* Email: ferhatayaz@yahoo.com
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "hash.h"

static const unsigned int primes[] = {
    53, 97, 193, 389,
    769, 1543, 3079, 6151,
    12289, 24593, 49157, 98317,
    196613, 393241, 786433, 1572869,
    3145739, 6291469, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189,
    805306457, 1610612741
};
const int prime_table_length = sizeof(primes)/sizeof(primes[0]);

static void *(*hashAllocator) (size_t size);
static void (*hashDeallocator)(void *ptr);

static int pointercmp(const void *pointer1, const void *pointer2);
static int stringcmp(const void *pointer1, const void *pointer2);
static int isProbablePrime(long number);
static long calculateIdealNumOfBuckets(hashtable_t *hashTable);
static unsigned long hash(const hashtable_t *hashTable, const void *key);

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_create() - creates a new HashTable
 *  DESCRIPTION:
 *      Creates a new HashTable.  When finished with this HashTable, it
 *      should be explicitly destroyed by calling the hashtable_destroy()
 *      function.
 *  EFFICIENCY:
 *      O(1)
 *  ARGUMENTS:
 *      numOfBuckets - the number of buckets to start the HashTable out with.
 *                     Must be greater than zero, and should be prime.
 *                     Ideally, the number of buckets should between 1/5
 *                     and 1 times the expected number of elements in the
 *                     HashTable.  Values much more or less than this will
 *                     result in wasted memory or decreased performance
 *                     respectively.  The number of buckets in a HashTable
 *                     can be re-calculated to an appropriate number by
 *                     calling the hashtable_rehash() function once the
 *                     HashTable has been populated.  The number of buckets
 *                     in a HashTable may also be re-calculated
 *                     automatically if the ratio of elements to buckets
 *                     passes the thresholds set by hashtable_set_ideal_ration().
 *  RETURNS:
 *      HashTable    - a new Hashtable, or NULL on error
\*--------------------------------------------------------------------------*/

hashtable_t *hashtable_create(long numOfBuckets) {
    hashtable_t *hashTable;
    int i;

    assert(numOfBuckets > 0);

    /* Enforce size as prime */
    for (i=0; i < prime_table_length; i++) {
        if ((long)primes[i] > numOfBuckets)
        {
            numOfBuckets = (long)primes[i];
            break;
        }
    }

    if (hashAllocator == NULL)
    {
        hashtable_set_construction_function(malloc, free);
    }

    hashTable = (hashtable_t *) hashAllocator(sizeof(hashtable_t));
    if (hashTable == NULL)
        return NULL;

    hashTable->bucketArray = (hashpair_t **)
                        hashAllocator(numOfBuckets * sizeof(hashpair_t *));
    if (hashTable->bucketArray == NULL) {
        hashDeallocator(hashTable);
        return NULL;
    }
    
    hashTable->numOfBuckets = numOfBuckets;
    hashTable->numOfElements = 0;

    for (i=0; i<numOfBuckets; i++)
        hashTable->bucketArray[i] = NULL;

    hashTable->idealRatio = 3.0;
    hashTable->lowerRehashThreshold = 0.0;
    hashTable->upperRehashThreshold = 15.0;

    hashTable->keycmp = stringcmp; /*pointercmp;*/
    hashTable->valuecmp = pointercmp;
    hashTable->hashFunction = hashtable_string_hash_function; /*pointerHashFunction;*/
    hashTable->keyDeallocator = NULL;
    hashTable->valueDeallocator = NULL;

    return hashTable;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_destroy() - destroys an existing HashTable
 *  DESCRIPTION:
 *      Destroys an existing HashTable.
 *  EFFICIENCY:
 *      O(n)
 *  ARGUMENTS:
 *      hashTable    - the HashTable to destroy
 *  RETURNS:
 *      <nothing>
\*--------------------------------------------------------------------------*/

void hashtable_destroy(hashtable_t *hashTable) {
    int i;
    hashpair_t *pair, *nextPair;

    for (i=0; i<hashTable->numOfBuckets; i++) {
        pair = hashTable->bucketArray[i];
        while (pair != NULL) {
            nextPair = pair->next;

            if (hashTable->keyDeallocator != NULL)
                hashTable->keyDeallocator((void *) pair->key);
            if (hashTable->valueDeallocator != NULL)
                hashTable->valueDeallocator(pair->value);
            hashDeallocator(pair);

            pair = nextPair;
        }
    }

    hashDeallocator(hashTable->bucketArray);
    hashDeallocator(hashTable);
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_contains_key() - checks the existence of a key in a HashTable
 *  DESCRIPTION:
 *      Determines whether or not the specified HashTable contains the
 *      specified key.  Uses the comparison function specified by
 *      hashtable_set_key_comparision_function().
 *  EFFICIENCY:
 *      O(1), assuming a good hash function and element-to-bucket ratio
 *  ARGUMENTS:
 *      hashTable    - the HashTable to search
 *      key          - the key to search for
 *  RETURNS:
 *      bool         - whether or not the specified HashTable contains the
 *                     specified key.
\*--------------------------------------------------------------------------*/

int hashtable_contains_key(const hashtable_t *hashTable, const void *key) {
    return (hashtable_get(hashTable, key) != NULL);
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_contains_value()
 *                         - checks the existence of a value in a HashTable
 *  DESCRIPTION:
 *      Determines whether or not the specified HashTable contains the
 *      specified value.  Unlike hashtable_contains_key(), this function is
 *      not very efficient, since it has to scan linearly looking for a
 *      match.  Uses the comparison function specified by
 *      hashtable_set_value_comparision_function().
 *  EFFICIENCY:
 *      O(n)
 *  ARGUMENTS:
 *      hashTable    - the HashTable to search
 *      value        - the value to search for
 *  RETURNS:
 *      bool         - whether or not the specified HashTable contains the
 *                     specified value.
\*--------------------------------------------------------------------------*/

int hashtable_contains_value(const hashtable_t *hashTable, const void *value) {
    int i;
    hashpair_t *pair;

    for (i=0; i<hashTable->numOfBuckets; i++) {
        pair = hashTable->bucketArray[i];
        while (pair != NULL) {
            if (hashTable->valuecmp(value, pair->value) == 0)
                return 1;
            pair = pair->next;
        }
    }

    return 0;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_put() - adds a key/value pair to a HashTable
 *  DESCRIPTION:
 *      Adds the specified key/value pair to the specified HashTable.  If
 *      the key already exists in the HashTable (determined by the comparison
 *      function specified by hashtable_set_key_comparision_function()), its value
 *      is replaced by the new value.  May trigger an auto-rehash (see
 *      hashtable_set_ideal_ration()).  It is illegal to specify NULL as the
 *      key or value.
 *  EFFICIENCY:
 *      O(1), assuming a good hash function and element-to-bucket ratio
 *  ARGUMENTS:
 *      hashTable    - the HashTable to add to
 *      key          - the key to add or whose value to replace
 *      value        - the value associated with the key
 *  RETURNS:
 *      err          - 0 if successful, -1 if an error was encountered
\*--------------------------------------------------------------------------*/

int hashtable_put(hashtable_t *hashTable, const void *key, void *value) {
    long hashValue;
    unsigned long hashValueOriginal;
    hashpair_t *pair;

    assert(key != NULL);
    assert(value != NULL);

    hashValueOriginal = hash(hashTable, key);
    hashValue = hashValueOriginal % hashTable->numOfBuckets;
    pair = hashTable->bucketArray[hashValue];

    while (pair != NULL && hashValueOriginal != pair->h
            && hashTable->keycmp(key, pair->key) != 0)
        pair = pair->next;

    if (pair) {
        if (pair->key != key) {
            if (hashTable->keyDeallocator != NULL)
                hashTable->keyDeallocator((void *) pair->key);
            pair->key = key;
        }
        if (pair->value != value) {
            if (hashTable->valueDeallocator != NULL)
                hashTable->valueDeallocator(pair->value);
            pair->value = value;
        }
    }
    else {
        hashpair_t *newPair = (hashpair_t *) hashAllocator(sizeof(hashpair_t));
        if (newPair == NULL) {
            return -1;
        }
        else {
            newPair->h = hashValueOriginal;
            newPair->key = key;
            newPair->value = value;
            newPair->next = hashTable->bucketArray[hashValue];
            hashTable->bucketArray[hashValue] = newPair;
            hashTable->numOfElements++;

            if (hashTable->upperRehashThreshold > hashTable->idealRatio) {
                float elementToBucketRatio = (float) hashTable->numOfElements /
                                             (float) hashTable->numOfBuckets;
                if (elementToBucketRatio > hashTable->upperRehashThreshold)
                    hashtable_rehash(hashTable, 0);
            }
        }
    }

    return 0;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      HashTableGet() - retrieves the value of a key in a HashTable
 *  DESCRIPTION:
 *      Retrieves the value of the specified key in the specified HashTable.
 *      Uses the comparison function specified by
 *      hashtable_set_key_comparision_function().
 *  EFFICIENCY:
 *      O(1), assuming a good hash function and element-to-bucket ratio
 *  ARGUMENTS:
 *      hashTable    - the HashTable to search
 *      key          - the key whose value is desired
 *  RETURNS:
 *      void *       - the value of the specified key, or NULL if the key
 *                     doesn't exist in the HashTable
\*--------------------------------------------------------------------------*/

void *hashtable_get(const hashtable_t *hashTable, const void *key) {
    unsigned long hashValueOriginal = hash(hashTable, key);
    long hashValue = hashValueOriginal % hashTable->numOfBuckets;
    hashpair_t *pair = hashTable->bucketArray[hashValue];

    while (pair != NULL && hashValueOriginal != pair->h
            && hashTable->keycmp(key, pair->key) != 0)
        pair = pair->next;

    return (pair == NULL)? NULL : pair->value;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_remove() - removes a key/value pair from a HashTable
 *  DESCRIPTION:
 *      Removes the key/value pair identified by the specified key from the
 *      specified HashTable if the key exists in the HashTable.  May trigger
 *      an auto-rehash (see hashtable_set_ideal_ration()).
 *  EFFICIENCY:
 *      O(1), assuming a good hash function and element-to-bucket ratio
 *  ARGUMENTS:
 *      hashTable    - the HashTable to remove the key/value pair from
 *      key          - the key specifying the key/value pair to be removed
 *  RETURNS:
 *      <nothing>
\*--------------------------------------------------------------------------*/

void hashtable_remove(hashtable_t *hashTable, const void *key) {
    unsigned long hashValueOriginal = hash(hashTable, key);
    long hashValue = hashValueOriginal % hashTable->numOfBuckets;
    hashpair_t *pair = hashTable->bucketArray[hashValue];
    hashpair_t *previousPair = NULL;

    while (pair != NULL && hashValueOriginal != pair->h
            && hashTable->keycmp(key, pair->key) != 0) {
        previousPair = pair;
        pair = pair->next;
    }

    if (pair != NULL) {
        if (hashTable->keyDeallocator != NULL)
            hashTable->keyDeallocator((void *) pair->key);
        if (hashTable->valueDeallocator != NULL)
            hashTable->valueDeallocator(pair->value);
        if (previousPair != NULL)
            previousPair->next = pair->next;
        else
            hashTable->bucketArray[hashValue] = pair->next;
        hashDeallocator(pair);
        hashTable->numOfElements--;

        if (hashTable->lowerRehashThreshold > 0.0) {
            float elementToBucketRatio = (float) hashTable->numOfElements /
                                         (float) hashTable->numOfBuckets;
            if (elementToBucketRatio < hashTable->lowerRehashThreshold)
                hashtable_rehash(hashTable, 0);
        }
    }
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_remove_all() - removes all key/value pairs from a HashTable
 *  DESCRIPTION:
 *      Removes all key/value pairs from the specified HashTable.  May trigger
 *      an auto-rehash (see hashtable_set_ideal_ration()).
 *  EFFICIENCY:
 *      O(n)
 *  ARGUMENTS:
 *      hashTable    - the HashTable to remove all key/value pairs from
 *  RETURNS:
 *      <nothing>
\*--------------------------------------------------------------------------*/

void hashtable_remove_all(hashtable_t *hashTable) {
    int i;

    for (i=0; i<hashTable->numOfBuckets; i++) {
        hashpair_t *pair = hashTable->bucketArray[i];
        while (pair != NULL) {
            hashpair_t *nextPair = pair->next;
            if (hashTable->keyDeallocator != NULL)
                hashTable->keyDeallocator((void *) pair->key);
            if (hashTable->valueDeallocator != NULL)
                hashTable->valueDeallocator(pair->value);
            hashDeallocator(pair);
            pair = nextPair;
        }
        hashTable->bucketArray[i] = NULL;
    }

    hashTable->numOfElements = 0;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_is_empty() - determines if a HashTable is empty
 *  DESCRIPTION:
 *      Determines whether or not the specified HashTable contains any
 *      key/value pairs.
 *  EFFICIENCY:
 *      O(1)
 *  ARGUMENTS:
 *      hashTable    - the HashTable to check
 *  RETURNS:
 *      bool         - whether or not the specified HashTable contains any
 *                     key/value pairs
\*--------------------------------------------------------------------------*/

int hashtable_is_empty(const hashtable_t *hashTable) {
    return (hashTable->numOfElements == 0);
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_size() - returns the number of elements in a HashTable
 *  DESCRIPTION:
 *      Returns the number of key/value pairs that are present in the
 *      specified HashTable.
 *  EFFICIENCY:
 *      O(1)
 *  ARGUMENTS:
 *      hashTable    - the HashTable whose size is requested
 *  RETURNS:
 *      long         - the number of key/value pairs that are present in
 *                     the specified HashTable
\*--------------------------------------------------------------------------*/

long hashtable_size(const hashtable_t *hashTable) {
    return hashTable->numOfElements;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      HashTableGetNumBuckets() - returns the number of buckets in a HashTable
 *  DESCRIPTION:
 *      Returns the number of buckets that are in the specified HashTable.
 *      This may change dynamically throughout the life of a HashTable if
 *      automatic or manual rehashing is performed.
 *  EFFICIENCY:
 *      O(1)
 *  ARGUMENTS:
 *      hashTable    - the HashTable whose number of buckets is requested
 *  RETURNS:
 *      long         - the number of buckets that are in the specified
 *                     HashTable
\*--------------------------------------------------------------------------*/

long hashtable_get_num_buckets(const hashtable_t *hashTable) {
    return hashTable->numOfBuckets;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_set_key_comparision_function()
 *              - specifies the function used to compare keys in a HashTable
 *  DESCRIPTION:
 *      Specifies the function used to compare keys in the specified
 *      HashTable.  The specified function should return zero if the two
 *      keys are considered equal, and non-zero otherwise.  The default
 *      function is one that simply compares pointers.
 *  ARGUMENTS:
 *      hashTable    - the HashTable whose key comparison function is being
 *                     specified
 *      keycmp       - a function which returns zero if the two arguments
 *                     passed to it are considered "equal" keys and non-zero
 *                     otherwise
 *  RETURNS:
 *      <nothing>
\*--------------------------------------------------------------------------*/

void hashtable_set_key_comparision_function(hashtable_t *hashTable,
        int (*keycmp)(const void *key1, const void *key2)) {
    assert(keycmp != NULL);
    hashTable->keycmp = keycmp;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_set_value_comparision_function()
 *              - specifies the function used to compare values in a HashTable
 *  DESCRIPTION:
 *      Specifies the function used to compare values in the specified
 *      HashTable.  The specified function should return zero if the two
 *      values are considered equal, and non-zero otherwise.  The default
 *      function is one that simply compares pointers.
 *  ARGUMENTS:
 *      hashTable    - the HashTable whose value comparison function is being
 *                     specified
 *      valuecmp     - a function which returns zero if the two arguments
 *                     passed to it are considered "equal" values and non-zero
 *                     otherwise
 *  RETURNS:
 *      <nothing>
\*--------------------------------------------------------------------------*/

void hashtable_set_value_comparision_function(hashtable_t *hashTable,
        int (*valuecmp)(const void *value1, const void *value2)) {
    assert(valuecmp != NULL);
    hashTable->valuecmp = valuecmp;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_set_hashfunction()
 *              - specifies the hash function used by a HashTable
 *  DESCRIPTION:
 *      Specifies the function used to determine the hash value for a key
 *      in the specified HashTable (before modulation).  An ideal hash
 *      function is one which is easy to compute and approximates a
 *      "random" function.  The default function is one that works
 *      relatively well for pointers.  If the HashTable keys are to be
 *      strings (which is probably the case), then this default function
 *      will not suffice, in which case consider using the provided
 *      hashtable_string_hash_function() function.
 *  ARGUMENTS:
 *      hashTable    - the HashTable whose hash function is being specified
 *      hashFunction - a function which returns an appropriate hash code
 *                     for a given key
 *  RETURNS:
 *      <nothing>
\*--------------------------------------------------------------------------*/

void hashtable_set_hashfunction(hashtable_t *hashTable,
        unsigned long (*hashFunction)(const void *key))
{
    assert(hashFunction != NULL);
    hashTable->hashFunction = hashFunction;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_rehash() - reorganizes a HashTable to be more efficient
 *  DESCRIPTION:
 *      Reorganizes a HashTable to be more efficient.  If a number of
 *      buckets is specified, the HashTable is rehashed to that number of
 *      buckets.  If 0 is specified, the HashTable is rehashed to a number
 *      of buckets which is automatically calculated to be a prime number
 *      that achieves (as closely as possible) the ideal element-to-bucket 
 *      ratio specified by the hashtable_set_ideal_ration() function.
 *  EFFICIENCY:
 *      O(n)
 *  ARGUMENTS:
 *      hashTable    - the HashTable to be reorganized
 *      numOfBuckets - the number of buckets to rehash the HashTable to.
 *                     Should be prime.  Ideally, the number of buckets
 *                     should be between 1/5 and 1 times the expected
 *                     number of elements in the HashTable.  Values much
 *                     more or less than this will result in wasted memory
 *                     or decreased performance respectively.  If 0 is
 *                     specified, an appropriate number of buckets is
 *                     automatically calculated.
 *  RETURNS:
 *      <nothing>
\*--------------------------------------------------------------------------*/

void hashtable_rehash(hashtable_t *hashTable, long numOfBuckets) {
    hashpair_t **newBucketArray;
    int i;

    assert(numOfBuckets >= 0);
    if (numOfBuckets == 0)
        numOfBuckets = calculateIdealNumOfBuckets(hashTable);

    if (numOfBuckets == hashTable->numOfBuckets)
        return; /* already the right size! */

    newBucketArray = (hashpair_t **)
                                hashAllocator(numOfBuckets * sizeof(hashpair_t *));
    if (newBucketArray == NULL) {
        /* Couldn't allocate memory for the new array.  This isn't a fatal
         * error; we just can't perform the rehash. */
        return;
    }

    for (i=0; i<numOfBuckets; i++)
        newBucketArray[i] = NULL;

    for (i=0; i<hashTable->numOfBuckets; i++) {
        hashpair_t *pair = hashTable->bucketArray[i];
        while (pair != NULL) {
            hashpair_t *nextPair = pair->next;
            long hashValue = hash(hashTable, pair->key) % numOfBuckets;
            pair->next = newBucketArray[hashValue];
            newBucketArray[hashValue] = pair;
            pair = nextPair;
        }
    }

    hashDeallocator(hashTable->bucketArray);
    hashTable->bucketArray = newBucketArray;
    hashTable->numOfBuckets = numOfBuckets;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_set_ideal_ration()
 *              - sets the ideal element-to-bucket ratio of a HashTable
 *  DESCRIPTION:
 *      Sets the ideal element-to-bucket ratio, as well as the lower and
 *      upper auto-rehash thresholds, of the specified HashTable.  Note
 *      that this function doesn't actually perform a rehash.
 *
 *      The default values for these properties are 3.0, 0.0 and 15.0
 *      respectively.  This is likely fine for most situations, so there
 *      is probably no need to call this function.
 *  ARGUMENTS:
 *      hashTable    - a HashTable
 *      idealRatio   - the ideal element-to-bucket ratio.  When a rehash
 *                     occurs (either manually via a call to the
 *                     hashtable_rehash() function or automatically due the
 *                     the triggering of one of the thresholds below), the
 *                     number of buckets in the HashTable will be
 *                     recalculated to be a prime number that achieves (as
 *                     closely as possible) this ideal ratio.  Must be a
 *                     positive number.
 *      lowerRehashThreshold
 *                   - the element-to-bucket ratio that is considered
 *                     unacceptably low (i.e., too few elements per bucket).
 *                     If the actual ratio falls below this number, a
 *                     rehash will automatically be performed.  Must be
 *                     lower than the value of idealRatio.  If no ratio
 *                     is considered unacceptably low, a value of 0.0 can
 *                     be specified.
 *      upperRehashThreshold
 *                   - the element-to-bucket ratio that is considered
 *                     unacceptably high (i.e., too many elements per bucket).
 *                     If the actual ratio rises above this number, a
 *                     rehash will automatically be performed.  Must be
 *                     higher than idealRatio.  However, if no ratio
 *                     is considered unacceptably high, a value of 0.0 can
 *                     be specified.
 *  RETURNS:
 *      <nothing>
\*--------------------------------------------------------------------------*/

void hashtable_set_ideal_ration(hashtable_t *hashTable, float idealRatio,
        float lowerRehashThreshold, float upperRehashThreshold) {
    assert(idealRatio > 0.0);
    assert(lowerRehashThreshold < idealRatio);
    assert(upperRehashThreshold == 0.0 || upperRehashThreshold > idealRatio);

    hashTable->idealRatio = idealRatio;
    hashTable->lowerRehashThreshold = lowerRehashThreshold;
    hashTable->upperRehashThreshold = upperRehashThreshold;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_set_deallocation_function()
 *              - sets the key and value deallocation functions of a HashTable
 *  DESCRIPTION:
 *      Sets the key and value deallocation functions of the specified
 *      HashTable.  This determines what happens to a key or a value when it
 *      is removed from the HashTable.  If the deallocation function is NULL
 *      (the default if this function is never called), its reference is
 *      simply dropped and it is up to the calling program to perform the
 *      proper memory management.  If the deallocation function is non-NULL,
 *      it is called to free the memory used by the object.  E.g., for simple
 *      objects, an appropriate deallocation function may be free().
 *
 *      This affects the behaviour of the hashtable_destroy(), hashtable_put(),
 *      hashtable_remove() and hashtable_remove_all() functions.
 *  ARGUMENTS:
 *      hashTable    - a HashTable
 *      keyDeallocator
 *                   - if non-NULL, the function to be called when a key is
 *                     removed from the HashTable.
 *      valueDeallocator
 *                   - if non-NULL, the function to be called when a value is
 *                     removed from the HashTable.
 *  RETURNS:
 *      <nothing>
\*--------------------------------------------------------------------------*/

void hashtable_set_deallocation_function(hashtable_t *hashTable,
        void (*keyDeallocator)(void *key),
        void (*valueDeallocator)(void *value)) {
    hashTable->keyDeallocator = keyDeallocator;
    hashTable->valueDeallocator = valueDeallocator;
}

/*--------------------------------------------------------------------------*\
 *  NAME:
 *      hashtable_string_hash_function() - a good hash function for strings
 *  DESCRIPTION:
 *      A hash function that is appropriate for hashing strings.  Note that
 *      this is not the default hash function.  To make it the default hash
 *      function, call hashtable_set_hashfunction(hashTable,
 *      hashtable_string_hash_function).
 *  ARGUMENTS:
 *      key    - the key to be hashed
 *  RETURNS:
 *      unsigned long - the unmodulated hash value of the key
\*--------------------------------------------------------------------------*/

unsigned long hashtable_string_hash_function(const void *key) {
    const unsigned char *str = (const unsigned char *) key;
    unsigned long hashValue = 0;
    int i;

    for (i=0; str[i] != '\0'; i++)
        hashValue = hashValue * 37 + str[i];

    return hashValue;
}



static int stringcmp(const void *pointer1, const void *pointer2) {
    return (strcmp((const char*)pointer1, (const char*)pointer2));
}

static int pointercmp(const void *pointer1, const void *pointer2) {
    return (pointer1 != pointer2);
}

#if 0
static unsigned long pointerHashFunction(const void *pointer);
static unsigned long pointerHashFunction(const void *pointer) {
    return ((unsigned long) pointer) >> 4;
}
#endif

static int isProbablePrime(long oddNumber) {
    long i;

    for (i=3; i<51; i+=2)
        if (oddNumber == i)
            return 1;
        else if (oddNumber%i == 0)
            return 0;

    return 1; /* maybe */
}

static long calculateIdealNumOfBuckets(hashtable_t *hashTable) {
    long idealNumOfBuckets = hashTable->numOfElements / hashTable->idealRatio;
    if (idealNumOfBuckets < 5)
        idealNumOfBuckets = 5;
    else
        idealNumOfBuckets |= 0x01; /* make it an odd number */
    while (!isProbablePrime(idealNumOfBuckets))
        idealNumOfBuckets += 2;

    return idealNumOfBuckets;
}

static unsigned long hash(const hashtable_t *hashTable, const void *key) {
    /* Aim to protect against poor hash functions by adding logic here
     * - logic taken from java 1.4 hashtable source */
    unsigned long i = hashTable->hashFunction(key);

    i += ~(i << 9);
    i ^=  ((i >> 14) | (i << 18)); /* >>> */
    i +=  (i << 4);
    i ^=  ((i >> 10) | (i << 22)); /* >>> */

    return i;
}


void hashtable_set_construction_function(
        void *(*_hashAllocator)(size_t size),
        void  (*_hashDeallocator)(void *ptr)) {

    hashAllocator   = _hashAllocator;
    hashDeallocator = _hashDeallocator;
}


/*
 * Copyright (c) 2002, Christopher Clark
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * * Neither the name of the original author; nor the names of any contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
