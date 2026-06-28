# -*- coding: utf-8 -*-
''' Common utilities for steel shapes dictionaries.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es"

import re

def common_prefix(a:str, b:str):
    ''' Return the common prefix of both strings.

    :param a: first string.
    :param b: second string.
    '''
    retval= ''
    for ch1, ch2 in zip(a,b):
        if(ch1==ch2):
            retval+=ch1
        else:
            break
    return retval

def find_nearest_steel_shape_by_depth(shapeDict, namePattern:str, depth:float):
    ''' Return the steel shape whose name matches the pattern argument
        and whose depth is the nearest to the argument.

    :param shapeDict: dictionary containing the properties of the steel shape.
    :param namePattern: regular expression to match.
    :param depth: depth of the steel shape.
    '''
    # Find the dictionary to search in.
    retval= None
    regex= re.compile(namePattern)
    for tableKey in shapeDict:
        c_prefix= common_prefix(namePattern, tableKey)
        if(c_prefix==tableKey):
            shapeDict= shapeDict[tableKey]
            # initial values.
            retval= None
            diff= 6.023e23
            # iterate through dictionary
            for shapeKey in shapeDict:
                if(regex.match(shapeKey)):
                    shape= shapeDict[shapeKey]
                    tmp= abs(depth-shape['h'])
                    if(tmp<diff):
                        diff= tmp
                        retval= shape
                        if(diff==0.0): break;
        if(retval): break;
    return retval

def get_steel_shape_depth_range(shapeDict, namePattern:str, minDepth:float, maxDepth):
    ''' Return the steel shapes whose name matches the pattern argument
        and whose depth is belongs to the range (minDepth, maxDepth)

    :param shapeDict: dictionary containing the properties of the steel shape.
    :param namePattern: regular expression to match.
    :param minDepth: minimum depth of the steel shape.
    :param maxDepth: maximum depth of the steel shape.
    '''
    # Find the dictionary to search in.
    retval= list()
    regex= re.compile(namePattern)
    for tableKey in shapeDict:
        c_prefix= common_prefix(namePattern, tableKey)
        if(c_prefix==tableKey):
            shapeDict= shapeDict[tableKey]
            # iterate through dictionary
            for shapeKey in shapeDict:
                if(regex.match(shapeKey)):
                    shape= shapeDict[shapeKey]
                    depth= shape['h']
                    if((depth>=minDepth) and (depth<=maxDepth)):
                        retval.append(shape)
    return retval
