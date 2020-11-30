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
    '''
    def __init__(self,setToDisp,scale,fUnitConv,component):
        ''' Constructor.

        :param setToDisp: set to display.
        :param component: component to display.
        '''
        super(LinearLoadDiagram,self).__init__(scale,fUnitConv)
        self.setToDisp=setToDisp
        self.component= component

    def dumpElementalLoads(self,preprocessor,lp,indxDiagram):
        ''' Iterate over loaded elements dumping its loads into the graphic.'''
        lIter= lp.loads.getElementalLoadIter
        eLoad= lIter.next()
        eTagsSet= self.setToDisp.getElements.getTags()
        while(eLoad):
            tags= eLoad.elementTags
            for i in range(0,len(tags)):
                eTag= tags[i]
                if eTag in eTagsSet:
                    elem= preprocessor.getElementHandler.getElement(eTag)
                    dim= elem.getDimension
                    if(dim==1):
                        vJ= elem.getJVector3d(True)
                        vK= elem.getKVector3d(True)
                        if(self.component=='axialComponent'):
                            self.vDir= vJ
                            indxDiagram= self.appendDataToDiagram(elem,indxDiagram,eLoad.axialComponent,eLoad.axialComponent)
                        elif(self.component=='transComponent'):
                            self.vDir= vJ
                            indxDiagram= self.appendDataToDiagram(elem,indxDiagram,eLoad.transComponent,eLoad.transComponent)
                        elif(self.component=='transYComponent'):
                            self.vDir= vJ
                            indxDiagram= self.appendDataToDiagram(elem,indxDiagram,eLoad.transYComponent,eLoad.transYComponent)
                        elif(self.component=='transZComponent'):
                            self.vDir= vK
                            indxDiagram= self.appendDataToDiagram(elem,indxDiagram,eLoad.transZComponent,eLoad.transZComponent)
                        elif(self.component=='xyzComponents'):
                            vI= elem.getIVector3d(True)
                            localForce= eLoad.getVector3dLocalForce() # Local components of the force.
                            v= localForce.x*vI+localForce.y*vJ+localForce.z*vK # Global force vector.
                            self.vDir= v.normalized() 
                            value= v.getModulus()
                            indxDiagram= self.appendDataToDiagram(elem,indxDiagram,value,value)
                        else:    
                            lmsg.error("LinearLoadDiagram :'"+self.component+"' unknown.")        
            eLoad= lIter.next()
        return indxDiagram
        

    def getMaxAbsComp(self,preprocessor):
        '''Return the maximum absolute value of the component.
        It is used for calculating auto-scale parameter
        '''
        maxV= 0.0
        activeLoadPatterns= preprocessor.getDomain.getConstraints.getLoadPatterns
        if(len(activeLoadPatterns)<1):
            lmsg.warning('No active load patterns.')
        eTagsSet= self.setToDisp.getElements.getTags()
        for lp in activeLoadPatterns: #Iterate over load patterns.
            lIter= lp.data().loads.getElementalLoadIter
            eLoad= lIter.next()
            while(eLoad):
                tags= eLoad.elementTags
                for i in range(0,len(tags)):
                    eTag= tags[i]
                    if eTag in eTagsSet: # if element in set.
                        if(self.component=='axialComponent'):
                            vComp= eLoad.axialComponent
                        elif(self.component=='transComponent'):
                            vComp= eLoad.transComponent
                        elif(self.component=='transYComponent'):
                            vComp=eLoad.transYComponent
                        elif(self.component=='transZComponent'):
                            vComp=eLoad.transZComponent
                        elif(self.component=='xyzComponents'):
                            hasLocalForce= getattr(eLoad, "getVector3dLocalForce", None)
                            if(hasLocalForce):
                                vComp= eLoad.getVector3dLocalForce().getModulus()
                            else: # strain load or alike.
                                vComp= 0.0
                        else:
                            lmsg.error("LinearLoadDiagram :'"+self.component+"' unknown.")
                        maxV=max(abs(vComp),maxV)
                eLoad= lIter.next()
        return maxV

    def dumpLoads(self, preprocessor, indxDiagram):
        ''' Dump loads over elements.'''
        activeLoadPatterns= preprocessor.getDomain.getConstraints.getLoadPatterns
        if(len(activeLoadPatterns)<1):
            lmsg.warning('No active load patterns.')
        retval= 0
        for lp in activeLoadPatterns: #Iterate over loaded elements.
            retval+= self.dumpElementalLoads(preprocessor,lp.data(),indxDiagram)
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
