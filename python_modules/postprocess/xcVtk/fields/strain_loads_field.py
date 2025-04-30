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
from vtk.vtkCommonCore import vtkDoubleArray

class StrainLoadsField(fields.ScalarField):
    ''' Represent strain loads components as scalar fields.

    :ivar name: name of the strain loads field.
    :ivar setToDisplay: set over which to display the loads.
    '''
    def __init__(self, name, setToDisplay, get_strain_component_index_from_name, fUnitConv= 1e3, rgMinMax= None):
        '''
        Constructor.

        :param name: name of the vector field.
        :param setToDisplay: set over which to display the loads.
        :param get_strain_component_index_from_name: function that returns the
                                                     component index from the
                                                     generalized strain name.
        :param fUnitConv: unit conversion factor.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax  in red (defaults to None)
        '''
        super().__init__(name= name, functionName= 'getProp', component= None, fUnitConv= fUnitConv)
        self.setToDisplay= setToDisplay
        self.get_strain_component_index_from_name= get_strain_component_index_from_name

    def getPropertyName(self):
        ''' Return the name of the property that will be used to store
            the values of the strain loads in the nodes.'''
        return self.name+'_strain_load_values'

    def _compute_loaded_elements_tags(self, activeLoadPatterns):
        ''' Compute the tags of the loaded elements.

        :param activeLoadPatterns: list of active load patterns.
        '''
        self.elementsToDisplayTags= set()
        eTagsSet= self.setToDisplay.getElements.getTags()
        for lp in activeLoadPatterns:
            lIter= lp.loads.getElementalLoadIter
            elementLoad= lIter.next()
            while(elementLoad):
                tags= elementLoad.elementTags
                for i, eTag in enumerate(tags):
                    if eTag in eTagsSet:
                        self.elementsToDisplayTags.add(eTag)
                elementLoad= lIter.next()

    def _get_elements_to_display(self):
        ''' Return the elements that are loaded and belong to the set of
            elements to be displayed (self.setToDisplay).
        '''
        retval= list()
        elemSet= self.setToDisplay.elements
        for e in elemSet:
            if(e.tag in self.elementsToDisplayTags):
                retval.append(e)
        return retval
        
    def _compute_nodes_strain_component_indexes(self, strainComponentName):
        ''' Compute the indexes of the strain component corresponding to the 
            given name in the strain matrices of each element gauss points
            and store them in a dictionary whose key is the element node tags.
            This dictionary will be used later to populate the VTK arrays
            used to represent the field values.

        :param strainComponentName: strain component to display.
        '''
        self.nodes_strain_component_indexes= dict()
        elemSet= self.setToDisplay.elements
        for e in elemSet:
            responseId= None
            if(hasattr(e,'getSection')): # is a beam or shell element.
                responseId= e.getSection(0).getResponseType
            tmp= self.get_strain_component_index_from_name(compName= strainComponentName, responseId= responseId)
            elemNodes= e.getNodes
            for n in elemNodes:
                nTag= n.tag
                if(nTag in self.nodes_strain_component_indexes):
                    previous= self.nodes_strain_component_indexes[nTag]
                    if(tmp!=previous):
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        warningMsg= '; the node '+str(nTag)
                        warningMsg+= ' belongs to two or more elements of different'
                        warningMsg+= ' type. The results in this node will'
                        warningMsg+= ' correspond to the last element only.'
                        lmsg.warning(className+'.'+methodName+warningMsg)
                self.nodes_strain_component_indexes[nTag]= tmp
    
    def _fill_vtk_double_array(self, nodeSet):
        '''Creates an vtkDoubleArray filled with the proper values.

        :param nodeSet: set of nodes that will provide the values
                        to populate the array.
        '''
        # Scalar values.
        self.arr= vtkDoubleArray()
        self.arr.SetName(self.name)
        self.arr.SetNumberOfTuples(len(nodeSet))
        self.arr.SetNumberOfComponents(1)
        propertyName= self.getPropertyName()
        for n in nodeSet:
            attr= getattr(n,self.attrName)
            nTag= n.tag
            self.attrComponent= self.nodes_strain_component_indexes[nTag]
            tmp= None
            if(self.attrComponent is not None):
                if(callable(attr)):
                    if(attr.__name__!='getProp'):
                        tmp= attr(propertyName)
                    elif(n.hasProp(propertyName)):
                        tmp= attr(propertyName)
                    else:
                        tmp= 0.0
                else:
                    tmp= attr
                if(hasattr(tmp,"__getitem__")):
                    tmp= tmp[self.attrComponent]
                tmp*= self.fUnitConv
                if not(self.rgMinMax):
                    self.updateMinMax(tmp)
                else:
                    self.updateMinMaxWithinRange(tmp,self.rgMinMax)
                    lmsg.warning('Displayed values have been clipped whitin the range: ('+str(self.rgMinMax[0])+', '+str(self.rgMinMax[1])+"), so they don't correspond to the computed ones.")
            else: # No component index found.
                tmp= 0.0
            self.arr.SetTuple1(n.getIdx,tmp)
        return self.arr
    
    def sumElementalStrainLoads(self, activeLoadPatterns):
        ''' Iterate over active load patterns and cumulate on elements 
            their elemental uniform loads.

        :param activeLoadPatterns: list of active load patterns.
        '''
        retval= dict()
        self._compute_loaded_elements_tags(activeLoadPatterns)
        for lp in activeLoadPatterns:
            lIter= lp.loads.getElementalLoadIter
            preprocessor= lp.getDomain.getPreprocessor
            spaceDimension= preprocessor.getNodeHandler.dimSpace
            elementLoad= lIter.next()
            while(elementLoad):
                category= elementLoad.category
                if('strain' in category):
                    if hasattr(elementLoad,'getElementStrainsMatrix'):
                        tags= elementLoad.elementTags
                        for i, eTag in enumerate(tags):
                            if eTag in self.elementsToDisplayTags:
                                element= self.setToDisplay.getElements.findTag(eTag)
                                dim= element.getDimension
                                nDOFperNode= element.getNumDOF()/element.numNodes
                                # Strain loads on beam elements are displayed
                                # as diagrams.
                                beamElement= (dim==1) and (nDOFperNode>spaceDimension)
                                if(not beamElement):
                                    if eTag in retval:
                                        retval[eTag]+= elementLoad.getElementStrainsMatrix(element)
                                    else:
                                        retval[eTag]= elementLoad.getElementStrainsMatrix(element)
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
        elemSet= self._get_elements_to_display()
        firstValue= next(iter(elementalStrainLoads.values()), None)
        retval= 0
        if(firstValue is not None):
            attributeName= self.getPropertyName()
            initialValue= xc.Vector(firstValue.noCols, 0.0)
            touchedNodes= extrapolate_elem_attr.create_attribute_at_nodes(elemSet, attributeName, initialValue)
            #Calculate totals.
            for e in elemSet:
                if(e.tag in elementalStrainLoads):
                    retval+= 1
                    values= elementalStrainLoads[e.tag]
                    extrapolatedValues= e.getExtrapolatedValues(values)
                    szV= extrapolatedValues.noRows
                    elemNodes= e.getNodes
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
            preprocessor= self.setToDisplay.getPreprocessor
            extrapolate_elem_attr.average_on_nodes(preprocessor= preprocessor, touchedNodes= touchedNodes, attributeName= attributeName)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; no strain loads in elements (dictionary empty).')
        return retval

    def populateWithElementalStrainLoads(self, activeLoadPatterns):
        ''' Populate the strain load container with elemental loads 
            from the load pattern argument.

        :param activeLoadPatterns: list of active load patterns.
        '''
        retval= 0
        elementalStrainLoads= self.sumElementalStrainLoads(activeLoadPatterns)
        if(len(elementalStrainLoads)>0):
            retval= self.extrapolateStrainLoadsToNodes(elementalStrainLoads)
        return retval

    
    def dumpElementalStrainLoads(self, preprocessor, strainComponentName):
        ''' Iterate over elemental strain loads dumping them into the graphic.

        :param preprocessor: preprocessor of the FE problem.
        :param strainComponentName: strain component to display.
        '''
        retval= 0
        self._compute_nodes_strain_component_indexes(strainComponentName= strainComponentName)
        #self.attrComponent= self.getStrainComponentIndexFromName(strainComponentName)
        activeLoadPatterns= lvf.get_active_load_patterns(preprocessor)
        if(len(activeLoadPatterns)<1):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            message= className+'.'+methodName+'; no active load patterns.'
            lmsg.warning(message)
        else:
            retval= self.populateWithElementalStrainLoads(activeLoadPatterns)
        return retval

 
