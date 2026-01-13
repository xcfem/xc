# -*- coding: utf-8 -*-
''' Maximum, minimum, average, ... of some property.'''

__author__= "Luis C. Pérez Tato (LCPT) & Ana Ortega (AO_O)"
__copyright__= "Copyright 2016,LCPT & AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com  ana.ortega@ciccp.es"

import sys
from misc_utils import log_messages as lmsg

def rec_getattr(obj, attr, argv= ''):
    """Get object's attribute. May use dot notation.

    >>> class C(object): pass
    >>> a = C()
    >>> a.b = C()
    >>> a.b.c = 4
    >>> rec_getattr(a, 'b.c')
    4
    """
    retval= None
    if '.' not in attr:
        if(attr=='getProp'):
            retval= obj.getProp(argv)
        else:
            if(callable(obj)):
                retval= getattr(obj(), attr)
                if(callable(retval)):
                    retval= retval()
            else:
                retval= getattr(obj, attr)
    else :
        L = attr.split('.')
        retval= rec_getattr(getattr(obj, L[0]), '.'.join(L[1:]))
    return retval

def rec_setattr(obj, attr, value):
    """Set object's attribute. May use dot notation.

    >>> class C(object): pass
    >>> a = C()
    >>> a.b = C()
    >>> a.b.c = 4
    >>> rec_setattr(a, 'b.c', 2)
    >>> a.b.c
    2
    """
    if '.' not in attr:
        setattr(obj, attr, value)
    else:
        L = attr.split('.')
        rec_setattr(getattr(obj, L[0]), '.'.join(L[1:]), value)

def get_item_with_max_prop(iterable,attrName, argv= ''):
    ''' Return item which maximizes the given property

    :param iterable: iterable to traverse.
    :param propertyName: property name.
    :param argv: argument for the property (when required).
    '''
    retval= None
    if(len(iterable)>0):
        retval= iterable[0]#iter(iterable).next()
        vMax= rec_getattr(retval,attrName,argv)  
        for e in iterable:
            v= rec_getattr(e,attrName,argv)
            if(v>vMax):
                retval= e
                vMax= v
    else:
        lmsg.error('argument container is empty.')
    return retval

def get_item_with_min_prop(iterable,propertyName, argv= ''):
    ''' Return item which minimizes the given property

    :param iterable: iterable to traverse.
    :param propertyName: property name.
    :param argv: argument for the property (when required).
    '''
    retval= None
    if(len(iterable)>0):
        retval= iterable[0]#iter(iterable).next()
        vMin= rec_getattr(retval,propertyName, argv)  
        for e in iterable:
            v= rec_getattr(e,propertyName, argv)
            if(v<vMin):
                retval= e
                vMin= v
    else:
        funcName= sys._getframe(0).f_code.co_name
        lmsg.error(funcName+'; argument container is empty.')
    return retval

def find_property(iterable, propertyName):
    ''' Return the first item which has the given property.

    :param iterable: iterable to traverse.
    :param propertyName: property name.
    '''
    retval= None
    if(len(iterable)>0):
        for item in iterable:
            if(item.hasProp(propertyName)):
                retval= item
                break;
    return retval

def find_attribute(iterable, attributeName):
    ''' Return the first item which has the given attribute.

    :param iterable: iterable to traverse.
    :param attributeName: attribute name.
    '''
    retval= None
    if(len(iterable)>0):
        for item in iterable:
            if(hasattr(item, attributeName)):
                retval= item
                break;
    return retval
