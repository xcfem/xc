# -*- coding: utf-8 -*-
''' Diagram to display a feature defined at nodes over linear elements. '''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import sys
from postprocess.xcVtk.diagrams import colored_diagram as cd
from misc_utils import log_messages as lmsg

class FeatureDiagram(cd.ColoredDiagram):
    '''Diagram to display a feature (attribute or property) defined at nodes over linear elements.
    '''
    def __init__(self, scaleFactor, lRefModSize, fUnitConv, sets, featureName, rgMinMax= None):
        ''' Constructor.

        :param scaleFactor: scale factor for the diagram (can be negative too).
        :param lRefModSize: reference length of the model (how big the model is).
        :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :param sets:      list of element sets for which the diagram will be displayed.
        :param featureName: feature (attribute or property) to display.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax in red (defaults to None)
        '''
        super(FeatureDiagram,self).__init__(scaleFactor= scaleFactor, fUnitConv= fUnitConv, rgMinMax= rgMinMax)
        self.elemSets= sets
        self.featureName= featureName
        self.lRefModSize= lRefModSize

    def getMaxAbsComp(self):
        '''Return the maximum absolute value of the component.
        It is used only for calculating auto-scale parameter, so 
        we compare only values of the component in first node of the elements
        '''
        maxV= 0.0
        for (v0, v1) in self.valueCouples:
            maxV= max(abs(v0), maxV)
            maxV= max(abs(v1), maxV)
        return maxV

    def autoScale(self):
        ''' Autoscale the diagram. '''
        maxAbsComp= self.getMaxAbsComp()
        if(maxAbsComp>0):
            self.scaleFactor*= 0.15*self.lRefModSize/(maxAbsComp*self.fUnitConv)
            
    def appendDataSetToDiagram(self, eSet, diagramIndex, defFScale=0.0):
        ''' Append feature values to diagram.

        :param eSet: Element set.
        :param diagramIndex: index-counter for the values to insert.
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        ''' 
        self.computeDiagramValues();
        return self.appendDataToDiagram(elements= self.elements, diagramIndex= diagramIndex, valueCouples= self.valueCouples, directions= self.directions, defFScale= defFScale)

    def clear(self):
        ''' Clear the diagram data.'''
        self.elements.clear()
        self.valueCouples.clear()

    def addDiagram(self, defFScale= 0.0):
        ''' Add diagram to the scene.

	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        numberOfComputedValues= self.computeDiagramValues()
        
        if(numberOfComputedValues>0):
            self.createDiagramDataStructure()
            diagramIndex= 0
            diagramIndex= self.appendDataFromElementEnds(dirVectors= self.directions, elements= self.elements, diagramIndex= diagramIndex, valueCouples= self.valueCouples)
            # for s in self.elemSets:
            #     diagramIndex= self.appendDataSetToDiagram(eSet= s, diagramIndex= diagramIndex, defFScale= defFScale)
            self.createLookUpTable()
            self.createDiagramActor()
            self.clear()
            retval= True
        else:
            retval= False
        return retval

