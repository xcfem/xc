# -*- coding: utf-8 -*-
''' Strain loads represented as scalar fields.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2025, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
import xc
from misc_utils import log_messages as lmsg
from postprocess.xcVtk.fields import fields
from postprocess.xcVtk.fields import load_vector_field as lvf
from postprocess import extrapolate_elem_attr

class StrainLoadsField(fields.ScalarField):
    ''' Represent strain loads components as scalar fields.

    :ivar name: name of the strain loads field.
    :ivar setToDisplay: set over which to display the loads.
    '''
    def __init__(self, name, setToDisplay, fUnitConv= 1e3, rgMinMax= None):
        '''
        Constructor.

        :param name: name of the vector field.
        :param setToDisplay: set over which to display the loads.
        :param fUnitConv: unit conversion factor.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax  in red (defaults to None)
        '''
        super().__init__(name= name, functionName= 'getProp', component= None, fUnitConv= fUnitConv)
        self.setToDisplay= setToDisplay

    def getPropertyName(self):
        ''' Return the name of the property that will be used to store
            the values of the strain loads in the nodes.'''
        return self.name+'_strain_load_values'
        
    def sumElementalStrainLoads(self, activeLoadPatterns):
        ''' Iterate over active load patterns and cumulate on elements 
            their elemental uniform loads.

        :param activeLoadPatterns: list of active load patterns.
        '''
        retval= dict()
        for lp in activeLoadPatterns:
            lIter= lp.loads.getElementalLoadIter
            preprocessor= lp.getDomain.getPreprocessor
            spaceDimension= preprocessor.getNodeHandler.dimSpace
            elementLoad= lIter.next()
            eTagsSet= self.setToDisplay.getElements.getTags()
            while(elementLoad):
                category= elementLoad.category
                if('strain' in category):
                    if hasattr(elementLoad,'getStrainsMatrix'):
                        tags= elementLoad.elementTags
                        strainMatrix= elementLoad.getStrainsMatrix()
                        for i, eTag in enumerate(tags):
                            if eTag in eTagsSet:
                                element= self.setToDisplay.getElements.findTag(eTag)
                                dim= element.getDimension
                                nDOFperNode= element.getNumDOF()/element.numNodes
                                # Strain loads on beam elements are displayed
                                # as diagrams.
                                beamElement= (dim==1) and (nDOFperNode>spaceDimension)
                                if(not beamElement):
                                    if eTag in retval:
                                        retval[eTag]+= strainMatrix
                                    else:
                                        retval[eTag]= strainMatrix
                    else:
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        warningMsg= '; display of loads of type '+str(type(elementLoad))
                        warningMsg+= ' not implemented yet.'
                        lmsg.warning(className+'.'+methodName+warningMsg)
                else:
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.warning(className+'.'+methodName+'; category: '+str(category)+' unknown.')
                elementLoad= lIter.next()
        return retval

    def extrapolateStrainLoadsToNodes(self, elementalStrainLoads):
        '''Extrapolate element's function values to the nodes and store the as
           as an attribute with the given name.

        :param elementalStrainLoads: values of the strain load at the elements.
        '''
        elemSet= self.setToDisplay.elements
        firstValue= next(iter(elementalStrainLoads.values()), None)
        if(firstValue is not None):
            attributeName= self.getPropertyName()
            initialValue= xc.Vector(firstValue.noCols, 0.0)
            touchedNodes= extrapolate_elem_attr.create_attribute_at_nodes(elemSet, attributeName, initialValue)
            #Calculate totals.
            for e in elemSet:
                elemNodes= e.getNodes
                values= elementalStrainLoads[e.tag]
                extrapolatedValues= e.getExtrapolatedValues(values)
                szV= extrapolatedValues.noRows
                sz= len(elemNodes)
                if(szV==0): # no values returned
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.error(className+'.'+methodName+'; no values returned for element: '+str(e.tag))
                    extrapolatedValues= sz*[None]
                for i in range(0,sz):
                    n= elemNodes[i]
                    valueAtNode= extrapolatedValues.getRow(i)
                    if(valueAtNode):
                        oldValue= n.getProp(attributeName)
                        newValue= oldValue+valueAtNode
                        n.setProp(attributeName, newValue)
            #Divide by number of elements in the set that touch the node.
            preprocessor= elemSet.owner.getPreprocessor
            extrapolate_elem_attr.average_on_nodes(preprocessor= preprocessor, touchedNodes= touchedNodes, attributeName= attributeName)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; no strain loads in elements (dictionary empty).')

    def populateWithElementalStrainLoads(self, activeLoadPatterns):
        ''' Populate the strain load container with elemental loads 
            from the load pattern argument.

        :param activeLoadPatterns: list of active load patterns.
        '''
        elementalStrainLoads= self.sumElementalStrainLoads(activeLoadPatterns)
        self.extrapolateStrainLoadsToNodes(elementalStrainLoads)
    
    def dumpElementalStrainLoads(self, preprocessor, strainComponent):
        ''' Iterate over elemental strain loads dumping them into the graphic.

        :param preprocessor: preprocessor of the FE problem.
        :param strainComponent: strain component to display.
        '''
        self.attrComponent= strainComponent
        activeLoadPatterns= lvf.get_active_load_patterns(preprocessor)
        if(len(activeLoadPatterns)<1):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            message= className+'.'+methodName+'; no active load patterns.'
            lmsg.warning(message)
        else:
            numberOfLoads= self.populateWithElementalStrainLoads(activeLoadPatterns)

