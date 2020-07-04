# -*- coding: utf-8 -*-

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

def getItemWithMaxProp(iterable,attrName, argv= ''):
    ''' Return item wich maximizes property named as indicated in attrName'''
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

def getItemWithMinProp(iterable,attrName, argv= ''):
    ''' Return item wich minimizes property named as indicated in attrName'''
    retval= None
    if(len(iterable)>0):
        retval= iterable[0]#iter(iterable).next()
        vMin= rec_getattr(retval,attrName, argv)  
        for e in iterable:
            v= rec_getattr(e,attrName, argv)
            if(v<vMin):
                retval= e
                vMin= v
    else:
        lmsg.error('argument container is empty.')                
    return retval

