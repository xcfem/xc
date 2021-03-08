 # -*- coding: utf-8 -*-
''' Display of loads over linear elements. '''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import geom
import vtk
import math
from postprocess.xcVtk.diagrams import colored_diagram as cd
from misc_utils import log_messages as lmsg

class LinearLoadDiagram(cd.ColoredDiagram):
    '''Draw a load over a set of linear element (qx,qy,qz,...)

    :ivar setToDisp: set to display.
    :ivar component: component to display.
    :ivar dictActLoadVectors: dictionary that stores for each 
          linear loaded element the sum of active loads on it
          (defaults to None)
    '''
    def __init__(self,setToDisp,scale,fUnitConv,component):
        ''' Constructor.

        :param setToDisp: set to display.
        :param component: component to display.
        '''
        super(LinearLoadDiagram,self).__init__(scale,fUnitConv)
        self.setToDisp=setToDisp
        self.component= component
        self.dictActLoadVectors=None

    def sumElementalLoads(self,actLP):
        ''' Iterate over active load patterns and cumulate on elements their elemental loads.
        Returns a dictionary that stores for each linear loaded element the sum of active 
        loads on it

        :param actLP: list of active load patterns.
        '''
        retval= dict()
        preprocessor=actLP[0].getDomain.getPreprocessor     
        for lp in actLP:
            lIter= lp.loads.getElementalLoadIter
            eLoad= lIter.next()
            eTagsSet= self.setToDisp.getElements.getTags()
            while(eLoad):
                if(hasattr(eLoad,'getVector3dLocalForce')):
                    tags= eLoad.elementTags
                    for i in range(0,len(tags)):
                        eTag= tags[i]
                        if eTag in eTagsSet:
                            elem= preprocessor.getElementHandler.getElement(eTag)
                            dim= elem.getDimension
                            if(dim==1):
                                if eTag in retval:
                                    retval[eTag]+= eLoad.getVector3dLocalForce()
                                else:
                                    retval[eTag]= eLoad.getVector3dLocalForce()
                eLoad= lIter.next()
        return retval
       

    def dumpElementalLoads(self,actLP,indxDiagram):
        ''' Iterate over loaded elements dumping its loads into the graphic.

        :param actLP: list of active load patterns.
        '''
        preprocessor=actLP[0].getDomain.getPreprocessor
        if not self.dictActLoadVectors:
            self.dictActLoadVectors=self.sumElementalLoads(actLP)
        if(self.component=='axialComponent'):
            for eTag in self.dictActLoadVectors.keys():
                elem= preprocessor.getElementHandler.getElement(eTag)
                self.vDir= elem.getJVector3d(True)
                axialLoad=self.dictActLoadVectors[eTag].x
                indxDiagram= self.appendDataToDiagram(elem,indxDiagram,axialLoad,axialLoad)
        elif(self.component in ['transComponent','transYComponent']):  # transComponent only for 2D models
            for eTag in self.dictActLoadVectors.keys():
                elem= preprocessor.getElementHandler.getElement(eTag)
                self.vDir= elem.getJVector3d(True)
                transLoad=self.dictActLoadVectors[eTag].y
                indxDiagram= self.appendDataToDiagram(elem,indxDiagram,transLoad,transLoad)
        elif(self.component=='transZComponent'):  
            for eTag in self.dictActLoadVectors.keys():
                elem= preprocessor.getElementHandler.getElement(eTag)
                self.vDir= elem.getKVector3d(True)
                transZLoad=self.dictActLoadVectors[eTag].z
                indxDiagram= self.appendDataToDiagram(elem,indxDiagram,transZLoad,transZLoad)
        elif(self.component=='xyzComponents'):
            for eTag in self.dictActLoadVectors.keys():
                elem= preprocessor.getElementHandler.getElement(eTag)
                vI= elem.getIVector3d(True)
                vJ= elem.getJVector3d(True)
                vK= elem.getKVector3d(True)
                localForce=self.dictActLoadVectors[eTag]
                v= localForce.x*vI+localForce.y*vJ+localForce.z*vK 
                self.vDir= v.normalized()
                totLoad= v.getModulus()
                indxDiagram= self.appendDataToDiagram(elem,indxDiagram,totLoad,totLoad)
        else:
            lmsg.error("LinearLoadDiagram :'"+self.component+"' unknown.")
        return indxDiagram
 
    def getMaxAbsComp(self,preprocessor):
        '''Return the maximum absolute value of the component.
        It is used for calculating auto-scale parameter
        '''
        activeLoadPatterns= preprocessor.getDomain.getConstraints.getLoadPatterns
        if(len(activeLoadPatterns)<1):
            lmsg.warning('No active load patterns.')
        actLP=[lp.data() for lp in activeLoadPatterns]
        if not self.dictActLoadVectors:
            self.dictActLoadVectors= self.sumElementalLoads(actLP)
        eTags=[tag for tag in self.dictActLoadVectors.keys()]
        retval= 0.0
        if(len(eTags)>0):
            if(self.component=='axialComponent'):
                retval=max([abs(self.dictActLoadVectors[tag].x) for tag in eTags])
            elif(self.component in ['transComponent','transYComponent']):  # transComponent only for 2D models
                retval=max([abs(self.dictActLoadVectors[tag].y) for tag in eTags])
            elif(self.component=='transZComponent'):  
                retval=max([abs(self.dictActLoadVectors[tag].z) for tag in eTags])
            elif(self.component=='xyzComponents'):
                retval=max([abs(self.dictActLoadVectors[tag].getModulus()) for tag in eTags])
            else:
                lmsg.error("LinearLoadDiagram :'"+self.component+"' unknown.")
        return retval

    def dumpLoads(self, preprocessor, indxDiagram):
        ''' Dump loads over elements.'''
        activeLoadPatterns= preprocessor.getDomain.getConstraints.getLoadPatterns
        if(len(activeLoadPatterns)<1):
            lmsg.warning('No active load patterns.')
        retval= 0
        actLP=[lp.data() for lp in activeLoadPatterns]
        retval= self.dumpElementalLoads(actLP,indxDiagram)
        return retval

    def addDiagram(self,preprocessor):
        self.creaEstrucDatosDiagrama()
        self.creaLookUpTable()
        self.creaActorDiagrama()

        indxDiagram= 0
        indxDiagram= self.dumpLoads(preprocessor,indxDiagram)

        if(indxDiagram>0):
            self.updateLookUpTable()
            self.updateActorDiagrama()
