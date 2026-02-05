# -*- coding: utf-8 -*-
''' String manipulation utilities. '''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2014 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import re
import unicodedata
import chardet
import sys
import decimal

if sys.version_info[0] >= 3:
    unicode = str

def get_encoding(input_str):
    ''' Return the encoding of the the string argument.'''
    encoding= chardet.detect(input_str)
    return encoding['encoding']

def is_ascii(input_str):
    ''' Return true if the argument is an ascii string.'''
    encoding= get_encoding(input_str)
    return (encoding == 'ascii')

def to_unicode(input_str):
    ''' Return a unicode string from the input.

    :param input_str: input string.
    '''
    retval= input_str
    if(not isinstance(input_str, unicode)): # input string is Unicode
        encoding= get_encoding(input_str)
        retval= unicode(input_str, encoding)
    return retval    

def remove_accents(input_str):
    ''' Remove the accents from the string argument.

    :param input_str: string to remove accents from. 
    '''
    retval= ''
    if(input_str is not None):
        if(len(input_str)>0):
            tmp= to_unicode(input_str)
            # Remove accents.
            nfkd_form= unicodedata.normalize('NFKD', tmp)
            retval= nfkd_form.encode('ASCII', 'ignore').decode('ascii')
        else:
            retval= input_str
    return retval

def slugify(s):
    """
    Simplifies ugly strings into something URL-friendly.
    >>> print(slugify("[Some] _ Article's Title--"))
    some-articles-title
    """

    # "[Some] _ Article's Title--"
    # "[some] _ article's title--"
    s = s.lower()

    # "[some] _ article's_title--"
    # "[some]___article's_title__"
    for c in [' ', '-', '.', '/']:
        s = s.replace(c, '_')

    # "[some]___article's_title__"
    # "some___articles_title__"
    s = re.sub(r'\W', '', s)

    # "some___articles_title__"
    # "some   articles title  "
    s = s.replace('_', ' ')

    # "some   articles title  "
    # "some articles title "
    s = re.sub(r'\s+', ' ', s)

    # "some articles title "
    # "some articles title"
    s = s.strip()

    # "some articles title"
    # "some-articles-title"
    s = s.replace(' ', '-')

    return s


def get_closest_inlist(lst,value):
    '''Return the number in a list that is 
    the closest to the target value.

    :param lst: list of numbers
    :param value: target value. 
    '''
    closest_value=min(lst, key=lambda list_value : abs(list_value - value))
    return closest_value

def get_index_closest_inlist(lst,value):
    '''Return the index in a list that matches 
    the closest element to the target value.

    :param lst: list of numbers.
    :param value: target value. 
    '''
    closest_value=get_closest_inlist(lst,value)
    return lst.index(closest_value)

def remove_duplicates_list(lst):
    '''Return a list from lst without items duplicated
    '''
    return list(dict.fromkeys(lst))

def remove_close_values(lst,tolerance):
    ''' Remove close values in a list with the specified tolerance
    '''
    ret = list()
    last_added = None
    for i in sorted(lst):
        if (last_added is None) or (last_added + tolerance < i):
            ret.append(i)
            last_added = i
    return ret
    
def atoi(text):
    return int(text) if text.isdigit() else text

def natural_keys(text):
    '''
    alist.sort(key=natural_keys) sorts in human order
    http://nedbatchelder.com/blog/200712/human_sorting.html
    (See Toothy's implementation in the comments)
    '''
    return [ atoi(c) for c in re.split(r'(\d+)', text) ]

def sort_human(l):
    convert = lambda text: float(text) if text.isdigit() else text
    alphanum = lambda key: [convert(c) for c in re.split(r'([-+]?[0-9]*\.?[0-9]*)', key)]
    l.sort(key=alphanum)
    return l

def get_number_decimal_positions(number):
    '''Return the number of decimal positions in the number given as parameter
    '''
    d=decimal.Decimal(str(number))
    retval=-d.as_tuple().exponent
    return retval

def get_max_lstOfLsts_by_col(lstOfLsts, col):
    '''
    Return the maximun value of column 'col' in the list of lists 'lstOfLsts'
    (first column in list is col=0)
    '''
    return max(lstOfLsts, key=lambda x: x[col])[col]

def get_min_lstOfLsts_by_col(lstOfLsts, col):
    '''
    Return the minimum value of column 'col' in the list of lists 'lstOfLsts'
    (first column in list is col=0)
    '''
    return min(lstOfLsts, key=lambda x: x[col])[col]

def sort_two_parallel_lists(list1,list2):
    '''Returns list1 sorted and list2 sorted in parallel to list1.
    e.g.: for list1=[3,5,1,7] and 
              list2=[2, 4, 6, 8]
          returns:
              sorted_list1=[1, 3, 5, 7]
              sorted_list2=[6, 2, 4, 8]

    
    '''
    sorted_list1, sorted_list2=zip(*sorted(zip(list1,list2)))
    return sorted_list1, sorted_list2
