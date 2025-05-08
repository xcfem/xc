# -*- coding: utf-8 -*-
'''Base class for load diagrams over a set of 1D elements.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2025, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
from postprocess.xcVtk.diagrams import colored_diagram as cd
from misc_utils import log_messages as lmsg

class LoadDiagram(cd.ColoredDiagram):
    '''Base class for load diagrams over a set of 1D elements.

    :ivar setToDisp: set to display.
    :ivar component: component to display.
    :ivar dictActLoadVectors: dictionary that stores for each 
          linear loaded element the sum of active loads on it
          (defaults to None)
    :ivar lRefModSize: reference length of the model (how big the model is).
    '''
    forceComponentLabels= ['axialComponent', 'transComponent', 'transYComponent', 'transZComponent', 'xyzComponents']
    strainComponentLabels= ['epsilon', 'kappa', 'kappa_z', 'kappa_y', 'gamma', 'gamma_y', 'gamma_z']
    
    def __init__(self, setToDisp, scale, lRefModSize, fUnitConv, component, rgMinMax= None):
        ''' Constructor.

        :param setToDisp: set to display.
        :param scale: scale factor for the diagram (can be negative too).
        :param lRefModSize: reference length of the model (how big the model is).
        :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :param component: component to display.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax in red (defaults to None)
        '''
        super(LoadDiagram,self).__init__(scaleFactor= scale, fUnitConv= fUnitConv, rgMinMax= rgMinMax)
        self.setToDisp= setToDisp
        self.component= component
        self.dictActLoadVectors= None
        self.lRefModSize= lRefModSize        

    def dumpLoads(self, preprocessor, diagramIndex, defFScale= 0.0):
        ''' Dump loads over elements.

        :param preprocessor: pre-processor of the finite element problem.
        :param diagramIndex: index-counter for the values to insert.
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        activeLoadPatterns= preprocessor.getDomain.getConstraints.getLoadPatterns
        if(len(activeLoadPatterns)<1):
            lmsg.warning('No active load patterns.')
        retval= 0
        actLP=[lp.data() for lp in activeLoadPatterns]
        retval= self.dumpElementalLoads(actLP, diagramIndex, defFScale= defFScale)
        return retval

    def addDiagram(self,preprocessor, defFScale= 0.0):
        ''' Create the diagram actor.
       
        :param preprocessor: pre-processor of the finite element problem.
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        self.createDiagramDataStructure()

        diagramIndex= 0
        diagramIndex= self.dumpLoads(preprocessor= preprocessor, diagramIndex= diagramIndex, defFScale= defFScale)

        if(diagramIndex>0):
            self.createLookUpTable()
            self.createDiagramActor()
            # self.updateLookUpTable()
            # self.updateDiagramActor()
