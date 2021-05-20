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
    if(len(input_str)>0):
        tmp= to_unicode(input_str)
        # Remove accents.
        nfkd_form= unicodedata.normalize('NFKD', tmp)
        only_ascii= nfkd_form.encode('ASCII', 'ignore').decode('ascii')
    else:
        only_ascii= input_str
    return only_ascii

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
    s = re.sub('\W', '', s)

    # "some___articles_title__"
    # "some   articles title  "
    s = s.replace('_', ' ')

    # "some   articles title  "
    # "some articles title "
    s = re.sub('\s+', ' ', s)

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
