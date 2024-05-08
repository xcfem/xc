# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"


import json
import xc
import importlib
import inspect

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
    ''' JSON encoder for XC objects.

    :ivar alreadyExportedObjects: objects already exported.
    '''
    
    def __init__(self, *args, **kwargs):
        ''' Constructor.'''
        kwargs['check_circular'] = False  # no need to check anymore
        super().__init__(*args, **kwargs)
        self.alreadyExportedObjects= dict()
        
    def default(self, obj):
        ''' Overloads encoding method.

        :param obj: object to encode.
        '''
        if(hasattr(obj, "getDict")):
            className= fully_qualified_classname(obj)
            objectId= id(obj)
            if(objectId in self.alreadyExportedObjects):
                object_dict= {'object_ref':objectId}
            else:
                object_dict= {'class_name':className, 'object_id':objectId}
                object_dict.update(obj.getDict())
                self.alreadyExportedObjects[objectId]= obj
            retval= object_dict
        else:
            retval= super().default(obj)
        return retval

def broke_xc_object(preprocessor, dct):
    ''' Return an xc object from the data in the given dictionary.

    :param preprocessor: preprocessor of the FE problem.
    :param dct: Python dictionary.
    '''
    retval= None
    prefix= 'xc.'
    fullyQualifiedClassName= dct['class_name']
    className= fullyQualifiedClassName[len(prefix):]
    if(className=='Vector'):
        values= dct['values']
        retval= xc.Vector(values)
    elif(className=='ElasticShearSection3d'):
        materialHandler= preprocessor.getMaterialHandler
        matTag= dct['tag']
        if(materialHandler.materialExists(matTag)): # already defined.
            retval= materialHandler.getMaterial(matTag)
        else:
            matName= dct['name']
            retval= materialHandler.newMaterial("elasticShearSection3d", matName)
            retval.setFromDict(dct)
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; class: '+str(className) + ' not implemented yet.')
        lmsg.error
    return retval
    
        
class XCJSONDecoder(json.JSONDecoder):
    ''' JSON encoder for XC objects.

    :ivar objectDict: XC object readed from a JSON file.
    '''
    
    def __init__(self, **kwargs):
        ''' Constructor.'''
        kwargs["object_hook"] = self.object_hook
        super().__init__(**kwargs)
        self.objectDict= dict()

    def object_hook(self, dct):
        ''' Create XC object from dictionary.

        :param dct: Python dictionary.
        '''
        if('class_name' in dct):
            objId= dct['object_id']
            obj= None
            if(objId in self.objectDict): # already loaded.
                obj= self.objectDict[objId]
            else: # new object.
                fullyQualifiedClassName= dct['class_name']
                if(fullyQualifiedClassName.startswith('xc.')):
                    obj= broke_xc_object(preprocessor= self.preprocessor, dct= dct)
                else:
                    className= fullyQualifiedClassName.split('.')[-1]
                    moduleName= fullyQualifiedClassName.removesuffix('.'+className)
                    importedClass= getattr(importlib.import_module(moduleName), className)
                    obj= importedClass.newFromDict(dct)
                self.objectDict[objId]= obj # for future reference.
            return obj
        elif('object_ref' in dct):
            objId= dct['object_ref']
            obj= self.objectDict[objId]
            return obj
        else:
            return dct
