# -*- coding: utf-8 -*-

def rec_getattr(obj, attr):
    """Get object's attribute. May use dot notation.

    >>> class C(object): pass
    >>> a = C()
    >>> a.b = C()
    >>> a.b.c = 4
    >>> rec_getattr(a, 'b.c')
    4
    """
    if '.' not in attr:
      if(callable(obj)):
        return getattr(obj(), attr)
      else:
        return getattr(obj, attr)
    else:
      L = attr.split('.')
      return rec_getattr(getattr(obj, L[0]), '.'.join(L[1:]))

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

def getItemWithMaxProp(iterable,attrName):
  ''' Return item wich maximizes property named as indicated in attrName'''
  retval= iter(iterable).next()
  vMax= rec_getattr(retval,attrName)  
  for e in iterable:
    v= rec_getattr(e,attrName)
    if(v>vMax):
      retval= e
      vMax= v
  return retval

def getItemWithMinProp(iterable,attrName):
  ''' Return item wich minimizes property named as indicated in attrName'''
  retval= iter(iterable).next()
  vMin= rec_getattr(retval,attrName)  
  for e in iterable:
    v= rec_getattr(e,attrName)
    if(v<vMin):
      retval= e
      vMin= v
  return retval

