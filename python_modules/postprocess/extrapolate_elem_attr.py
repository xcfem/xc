# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es  ana.ortega@ciccp.es"

import xc_base
import geom
import xc
import math
from miscUtils import LogMessages as lmsg

def flatten_attribute(elemSet,attributeName, treshold, limit):
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
        #print "v= ", v, " vCorr=", vCorr
        e.setProp(attributeName,vCorr)
        flattened+= 1
    if(flattened>0):
      print "flattened ", flattened, 'values over', len(elemSet)

def create_attribute_at_nodes(xcSet,attributeName,initialValue):
    ''' Create an attribute on the nodes of the set passed as parameter.
    return tags of the affected nodes.

    :param xcSet: nodes that will receive the attribute.
    :param attributeName: name of the attribute to define.
    :param initialValue: initial value to assign to the attribute.
    '''
    nodeTags= {}
    for e in xcSet:
      elemNodes= e.getNodes
      sz= len(elemNodes)
      for i in range(0,sz):
        n= elemNodes[i]
        tag= n.tag
        if tag not in nodeTags:
          nodeTags[tag]= 1
          if(n.hasProp(attributeName)):
            lmsg.warning('node: '+ str(n.tag) + ' already has a property named: \'' + attributeName +'\'.')
          n.setProp(attributeName,initialValue)
        else:
          nodeTags[tag]+=1
    return nodeTags

def extrapolate_elem_function_attr(elemSet,attributeName,function, argument,initialValue= 0.0):
    '''Extrapolate element's function values to the nodes.

    :param elemSet: set of elements.
    :param attributeName: name of the property which will be defined
     at the nodes.
    :param function: name of the function to call for each element.
    :param argument: name of the argument to send to the function (optional).
    :param initialValue: initial value for the attribute defined at the nodes.
    '''
    nodeTags= create_attribute_at_nodes(elemSet,attributeName,initialValue)
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
    preprocessor= elemSet.owner.getPreprocessor
    for tag in nodeTags:
        n= preprocessor.getNodeHandler.getNode(tag)
        denom= nodeTags[tag]
        n.setProp(attributeName,n.getProp(attributeName)/denom)
