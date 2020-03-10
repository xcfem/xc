 # -*- coding: utf-8 -*-

''' Display of loads over linear elements. '''

import geom
import vtk
from postprocess.xcVtk.diagrams import colored_diagram as cd
from misc_utils import log_messages as lmsg

class LinearLoadDiagram(cd.ColoredDiagram):
    '''Draw a load over a set of linear element (qx,qy,qz,...)'''
    def __init__(self,setToDisp,scale,fUnitConv,component):
        super(LinearLoadDiagram,self).__init__(scale,fUnitConv)
        self.setToDisp=setToDisp
        self.component= component

    def dumpElementalLoads(self,preprocessor,lp,indxDiagram):
        ''' Iterate over loaded elements dumping its loads into the graphic.'''
        lIter= lp.loads.getElementalLoadIter
        eLoad= lIter.next()
        eTagsSet=self.setToDisp.getElements.getTags()
        while(eLoad):
            tags= eLoad.elementTags
            for i in range(0,len(tags)):
                eTag= tags[i]
                if eTag in eTagsSet:
                    elem= preprocessor.getElementHandler.getElement(eTag)
                    if(self.component=='axialComponent'):
                        self.vDir= elem.getJVector3d(True)
                        indxDiagram= self.appendDataToDiagram(elem,indxDiagram,eLoad.axialComponent,eLoad.axialComponent)
                    elif(self.component=='transComponent'):
                        self.vDir= elem.getJVector3d(True) # initialGeometry= True  
                        indxDiagram= self.appendDataToDiagram(elem,indxDiagram,eLoad.transComponent,eLoad.transComponent)
                    elif(self.component=='transYComponent'):
                        self.vDir= elem.getJVector3d(True) # initialGeometry= True  
                        indxDiagram= self.appendDataToDiagram(elem,indxDiagram,eLoad.transYComponent,eLoad.transYComponent)
                    elif(self.component=='transZComponent'):
                        self.vDir= elem.getKVector3d(True) # initialGeometry= True  
                        indxDiagram= self.appendDataToDiagram(elem,indxDiagram,eLoad.transZComponent,eLoad.transZComponent)
                    else:
                        lmsg.error("LinearLoadDiagram :'"+self.component+"' unknown.")        
                eLoad= lIter.next()

    def getMaxAbsComp(self,preprocessor):
        '''Return the maximum absolute value of the component.
        It is used for calculating auto-scale parameter
        '''
        maxV=0
        activeLoadPatterns= preprocessor.getDomain.getConstraints.getLoadPatterns
        if(len(activeLoadPatterns)<1):
            lmsg.warning('No active load patterns.')
        for lp in activeLoadPatterns: #Iterate over loaded elements.
            lIter= lp.data().loads.getElementalLoadIter
            eLoad= lIter.next()
            while(eLoad):
                tags= eLoad.elementTags
                for i in range(0,len(tags)):
                    if(self.component=='axialComponent'):
                        vComp= eLoad.axialComponent
                    elif(self.component=='transComponent'):
                        vComp= eLoad.transComponent
                    elif(self.component=='transYComponent'):
                        vComp=eLoad.transYComponent
                    elif(self.component=='transZComponent'):
                        vComp=eLoad.transZComponent
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
        for lp in activeLoadPatterns: #Iterate over loaded elements.
            self.dumpElementalLoads(preprocessor,lp.data(),indxDiagram)

    def addDiagram(self,preprocessor):
        self.creaEstrucDatosDiagrama()
        self.creaLookUpTable()
        self.creaActorDiagrama()

        indxDiagram= 0
        self.dumpLoads(preprocessor,indxDiagram)

        self.updateLookUpTable()
        self.updateActorDiagrama()
