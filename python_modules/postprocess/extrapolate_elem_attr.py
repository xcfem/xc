# -*- coding: utf-8 -*-
''' Extrapolate attribute values of elements.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es  ana.ortega@ciccp.es"

import math
import sys
import geom
import xc
from misc_utils import log_messages as lmsg

def flatten_attribute(elemSet, attributeName, treshold, limit):
    '''Reduce higher values which hide attribute variation over the model.

    :param elemSet: set of elements to deal with.
    :param attributeName: attribute to be flattened.
    :param treshold: starting value to apply flatten.
    :param limit: limit 
 
    '''
    flattened= 0
    for e in elemSet:
      v= e.getProp(attributeName)
      if(v>treshold):
        vCorr= 2*math.atan(v)/math.pi*(limit-treshold)+treshold
        e.setProp(attributeName,vCorr)
        flattened+= 1
    if(flattened>0):
        lmsg.info("flattened "+str(flattened)+' values over '+str(elemSet))

def create_attribute_at_nodes(xcSet,attributeName,initialValue):
    ''' Create an attribute on the nodes of the set passed as parameter.
    Returns a dictionary with the number of elements connected to
    a node indexed by the node tag.

    :param xcSet: nodes that will receive the attribute.
    :param attributeName: name of the attribute to define.
    :param initialValue: initial value to assign to the attribute.
    '''
    touchedNodes= {}
    for e in xcSet:
        elemNodes= e.getNodes
        sz= len(elemNodes)
        for i in range(0,sz):
            n= elemNodes[i]
            nodeTag= n.tag
            if nodeTag not in touchedNodes:
                touchedNodes[nodeTag]= 1
                # 9/11/2023 LP: it's normal to redefine a property
                # so there is no need of this message:
                #if(n.hasProp(attributeName)):
                    #lmsg.warning('node: '+ str(n.tag) + ' already has a property named: \'' + attributeName +'\'.')
                n.setProp(attributeName,initialValue)
            else:
                touchedNodes[nodeTag]+=1
    return touchedNodes

def average_on_nodes(preprocessor, touchedNodes, attributeName):
    ''' Divide by number of elements in the set that touch the node.'''
    for tag in touchedNodes:
        n= preprocessor.getNodeHandler.getNode(tag)
        denom= touchedNodes[tag]
        n.setProp(attributeName, n.getProp(attributeName)*(1.0/denom))

def extrapolate_elem_function_attr(elemSet,attributeName,function, argument,initialValue= 0.0, preprocessor= None):
    '''Extrapolate element's function values to the nodes.

    :param elemSet: set of elements.
    :param attributeName: name of the property which will be defined
     at the nodes.
    :param function: name of the function to call for each element.
    :param argument: name of the argument for the function call function (optional).
    :param initialValue: initial value for the attribute defined at the nodes.
    '''
    touchedNodes= create_attribute_at_nodes(elemSet,attributeName,initialValue)
    #Calculate totals.
    for e in elemSet:
        elemNodes= e.getNodes
        sz= len(elemNodes)
        for i in range(0,sz):
            n= elemNodes[i]
            f= getattr(e,function)
            value= f(argument)
            if(value):
                oldValue= n.getProp(attributeName)
                n.setProp(attributeName,oldValue+value)
    #Divide by number of elements in the set that touch the node.
    if(not preprocessor):
        preprocessor= elemSet.owner.getPreprocessor
    average_on_nodes(preprocessor,touchedNodes, attributeName)

def transform_to_local_coordinates(element, value):
    '''Transform the given value to local coordinates in the given element.

    :param element: element to get local coordinates from.
    :param value: value to transform:
    '''
    elementDimension= element.getDimension
    if(elementDimension==2):
        cooSys= element.get2DCoordinateSystem(True)
    elif(elementDimension==3):
        cooSys= element.getCoordinateSystem(True)
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; not implemented for elements of dimension: '+elementDimension)
        
    size= len(value)
    if(size==1):
        retval= value
    else:
        if(elementDimension==2):
            if(size==3):
                valueTensor= geom.Matrix([[value[0], value[2]],
                                          [value[2], value[1]]])
                tmp= cooSys.getLocalMatrix(valueTensor)
                retval= xc.Vector([tmp(0,0), tmp(1,1), tmp(1,2)])
            else:
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(methodName+'; expected size was 3, got a list of size: '+size)
                retval= None                
        elif(elementDimension==3):
            if(size==6):
                valueTensor= geom.Matrix([[value[0], value[3], value[4]],
                                          [value[3], value[1], value[5]],
                                          [value[4], value[5], value[2] ]])
                tmp= cooSys.getLocalMatrix(valueTensor)
                retval= xc.Vector([tmp(1,1), tmp(2,2), tmp(3,3), tmp(1,2), tmp(1,3), tmp(2,3)])
            else:
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(methodName+'; expected size was 6, got a list of size: '+size)
                retval= None
    return retval

def extrapolate_elem_data_to_nodes(elemSet, attributeName, function, argument= None, initialValue= 0.0, transformToLocalCoord= False):
    '''Extrapolate element's function values to the nodes.

    :param elemSet: set of elements.
    :param attributeName: name of the property which will be defined
                          at the nodes.
    :param function: name of the method to call for each element.
    :param argument: name of the argument for the function call function (optional).
    :param initialValue: initial value for the attribute defined at the nodes.
    :param transformToLocalCoord: if true (and appropriate), express the obtained result in local coordinates.
    '''
    touchedNodes= create_attribute_at_nodes(elemSet, attributeName, initialValue)
    #Calculate totals.
    for e in elemSet:
        elemNodes= e.getNodes
        if(argument):
            values= function(e, argument)
        else:
            values= function(e)
        szV= len(values)
        sz= len(elemNodes)
        if(szV==0): # no values returned
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(methodName+'; no values returned for attribute name: '+attributeName)
            values= sz*[None]
        for i in range(0,sz):
            n= elemNodes[i]
            valueAtNode= values[i]                
            if(valueAtNode):
                if(transformToLocalCoord): # if transform to local coordinates
                    valueAtNode= transform_to_local_coordinates(element= e, value= valueAtNode)
                oldValue= n.getProp(attributeName)
                newValue= oldValue+valueAtNode
                n.setProp(attributeName,newValue)
    #Divide by number of elements in the set that touch the node.
    preprocessor= elemSet.owner.getPreprocessor
    average_on_nodes(preprocessor= preprocessor, touchedNodes= touchedNodes, attributeName= attributeName)
