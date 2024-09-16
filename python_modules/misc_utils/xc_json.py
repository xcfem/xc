# -*- coding: utf-8 -*-
''' JSON encoder and decoder for XC classes.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"


import sys
import json
import xc
import importlib
import inspect
from misc_utils import log_messages as lmsg
from misc_utils import compat

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
            object_dict= {'class_name':className} # store the name of the class.
            objectId= id(obj)
            if(objectId in self.alreadyExportedObjects):
                object_dict.update({'object_ref':objectId}) # reference to the object.
                if(className.startswith('xc.')): # if XC object.
                    # append the data needed to create a new
                    # empty object of this type.
                    tmp= obj.getDict()
                    if('tag' in tmp):
                        object_dict['tag']= tmp['tag']
                    if('name' in tmp):
                        object_dict['name']= tmp['name']
            else:
                object_dict.update({'object_id':objectId}) # store the object itself.
                object_dict.update(obj.getDict())
                self.alreadyExportedObjects[objectId]= obj
            retval= object_dict
        else:
            retval= super().default(obj)
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
        self.definitionPending= set()

    def broke_xc_object(self, fullyQualifiedClassName, tag, name, dct):
        ''' Return an xc object from the data in the given dictionary.

        :param fullyQualifiedClassName: fully qualified class name.
        :param tag: object tag.
        :param name: object name.
        :param dct: Python dictionary.
        '''
        retval= None
        prefix= 'xc.'
        className= fullyQualifiedClassName[len(prefix):]
        if(className=='Vector'):
            values= dct['values']
            retval= xc.Vector(values)
        elif(className=='ElasticShearSection3d'):
            materialHandler= self.preprocessor.getMaterialHandler
            if(tag is not None):
                matTag= tag
                if(materialHandler.materialExists(matTag)): # already defined.
                    retval= materialHandler.getMaterial(matTag)
                else:
                    if(name is not None):
                        matName= name
                        retval= materialHandler.newMaterial("elasticShearSection3d", matName)
                    else:
                       lmsgClassName= type(self).__name__
                       lmsgMethodName= sys._getframe(0).f_code.co_name
                       lmsg.error(lmsgClassName+'.'+lmsgMethodName+'; name argument is mandatory for class: '+str(className) + " (name argument was: '"+str(name)+"').")
                       exit(1)

                    if(dct):
                        retval.setFromDict(dct)
            else:
                lmsgClassName= type(self).__name__
                lmsgMethodName= sys._getframe(0).f_code.co_name
                lmsg.error(lmsgClassName+'.'+lmsgMethodName+'; tag argument is mandatory for class: '+str(className) + " (tag argument was: '"+str(tag)+"').")
                exit(1)
                        
        else:
            lmsgClassName= type(self).__name__
            lmsgMethodName= sys._getframe(0).f_code.co_name
            lmsg.error(lmsgClassName+'.'+lmsgMethodName+'; class: '+str(className) + ' not implemented yet.')
        return retval
            
    def broke_object(self, objId, fullyQualifiedClassName, tag= None, name= None, dct= None):
        ''' Create XC object from dictionary.

        :param objId: object identifier.
        :param fullyQualifiedClassName: fully qualified class name.
        :param tag: tag of the XC object.
        :param name: name of the XC object.
        :param dct: Python dictionary containing the values for the object members.
        '''
        if(fullyQualifiedClassName.startswith('xc.')):
            retval= self.broke_xc_object(fullyQualifiedClassName= fullyQualifiedClassName, tag= tag, name= name, dct= dct)
        else:
            className= fullyQualifiedClassName.split('.')[-1]
            if sys.version_info < (3,9):
                moduleName= compat.removesuffix(fullyQualifiedClassName,'.'+className)
            else:
                moduleName= fullyQualifiedClassName.removesuffix('.'+className)
            importedClass= getattr(importlib.import_module(moduleName), className)
            retval= importedClass.newFromDict(dct)
        self.objectDict[objId]= retval # for future reference.
        return retval
        

    def object_hook(self, dct):
        ''' Create XC object from dictionary.

        :param dct: Python dictionary.
        '''
        retval= None
        objId= None
        if('object_id' in dct): # Read object definition.
            objId= dct['object_id']
            if(objId in self.objectDict): # already loaded.
                retval= self.objectDict[objId]
                if(objId in self.definitionPending): # not fully defined yet.
                    retval.setFromDict(dct)
                    self.definitionPending.remove(objId) # not pending anymore.
            else: # new object.
                fullyQualifiedClassName= dct['class_name']
                tag= None
                if('tag' in dct):
                    tag= int(dct['tag'])
                name= None
                if('name' in dct):
                    name= dct['name']
                retval= self.broke_object(objId= objId, fullyQualifiedClassName= dct['class_name'], tag= tag, name= name, dct= dct)
        elif('object_ref' in dct):
            objId= dct['object_ref']
            if(objId in self.objectDict): # already defined.
                retval= self.objectDict[objId]
            else: # not defined yet.
                tag= None
                if('tag' in dct):
                    tag= int(dct['tag'])
                name= None
                if('name' in dct):
                    name= dct['name']
                retval= self.broke_object(objId= objId, fullyQualifiedClassName= dct['class_name'], tag= tag, name= name, dct= None)
                self.definitionPending.add(objId)
        else:
            retval= dct
        return retval
