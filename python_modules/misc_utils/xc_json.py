# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"


import json
import importlib

def fully_qualified_classname(obj):
    ''' Return the fullyqualified class name of a python object

    :param obj: Python object.
    '''
    cls = type(obj)
    module= cls.__module__
    retval= cls.__qualname__
    if module is not None and module != "__builtin__":
        retval = module + "." + retval
    return retval

class XCJSONEncoder(json.JSONEncoder):
    ''' JSON encoder for XC objects.'''
    def default(self, obj):
        ''' Overloas encoding method.

        :param obj: object to encode.
        '''
        if(hasattr(obj, "getDict")):
            className= fully_qualified_classname(obj)
            object_dict= {'class_name':className, 'object_id':id(obj)}
            object_dict.update(obj.getDict())
            return object_dict
        else:
            return super().default(obj)
        
class XCJSONDecoder(json.JSONDecoder):
    ''' JSON encoder for XC objects.'''
    
    def __init__(self, **kwargs):
        ''' Constructor.'''
        kwargs["object_hook"] = self.object_hook
        super().__init__(**kwargs)
    
    def object_hook(self, dct):
        ''' Create XC object from dictionary.

        :param dct: Python dictionary.
        '''
        if('class_name' in dct):
            fullyQualifiedClassName= dct['class_name']
            className= fullyQualifiedClassName.split('.')[-1]
            moduleName= fullyQualifiedClassName.removesuffix('.'+className)
            importedClass= getattr(importlib.import_module(moduleName), className)
            obj= importedClass(silent= True) # Don't issue warning messages.
            obj.setFromDict(dct)
            return obj
        else:
            return dct
