# -*- coding: utf-8 -*-
from __future__ import division

''' Loads represented as vectors. '''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import geom
import xc
import vtk
from miscUtils import LogMessages as lmsg
from postprocess.xcVtk import vector_field as vf

class LoadOnPoints(vf.VectorField):
    '''Draws punctual loads.'''
    def __init__(self,loadPatternName,fUnitConv= 1e-3,scaleFactor= 1.0,showPushing= True,components= [0,1,2]):
        '''
        Parameters:
          loadPatternName: name of the load pattern to display.
          fUnitConv: unit conversion factor.
          scaleFactor: scale factor for the size of the vectors.
          showPushing: true if the loads push the loaded point (as oppssed to pull). Default: True
          components: index of the components of the load. Default: [0,1,2] 
        '''
        super(LoadOnPoints,self).__init__(loadPatternName,fUnitConv,scaleFactor,showPushing)
        self.lpName= loadPatternName 
        self.components= components

    def getMaxLoad(self):
        ''' Calculate the maximum absolute value of the loads on the
            vector container.'''
        self.data.calculateLengths(self.fUnitConv)
        rgMaxMin=self.data.lengths.GetRange()
        return abs(max(rgMaxMin, key=abs))


class LoadVectorField(LoadOnPoints):
    '''Draws a load over a point on nodes and on elements.'''
    def __init__(self,loadPatternName,setToDisp,fUnitConv= 1e-3,scaleFactor= 1.0,showPushing= True,components= [0,1,2],multiplyByElementArea= True):
        '''
        Parameters:
          loadPatternName: name of the load pattern to display.
          setToDisp: set over which to display the loads.
          fUnitConv: unit conversion factor.
          scaleFactor: scale factor for the size of the vectors.
          showPushing: true if the loads push the loaded point (as oppssed to pull). Default: True
          components: index of the components of the load. Default: [0,1,2] 
          multiplyByElementArea: for loads over elements (default= True).
        '''
        super(LoadVectorField,self).__init__(loadPatternName,fUnitConv,scaleFactor,showPushing)
        self.multiplyByElementArea= multiplyByElementArea
        self.setToDisp=setToDisp

    def sumElementalLoads(self,preprocessor,lp):
        ''' Iterate over loaded elements and cumulate their loads.'''
        lIter= lp.loads.getElementalLoadIter
        elementLoad= lIter.next()
        comp_i= self.components[0]; comp_j= self.components[1]; comp_k= self.components[2]
        retval= dict()
        eTagsSet=self.setToDisp.getElements.getTags()
        while(elementLoad):
          tags= elementLoad.elementTags
          for i in range(0,len(tags)):
            eTag= tags[i]
            if eTag in eTagsSet:
                elem= preprocessor.getElementHandler.getElement(eTag)
                if(elem.getDimension==2):
                  vLoad= elem.getCoordTransf.getVectorGlobalCoordFromLocal(elementLoad.getLocalForce())
                  if(self.multiplyByElementArea):
                    vLoad*=elem.getArea(True)
                  v= xc.Vector([vLoad[comp_i],vLoad[comp_j],vLoad[comp_k]])
                  if eTag in retval:
                    retval[eTag]+= v
                  else:
                    retval[eTag]= v
        #        else:
        #          lmsg.warning('displaying of loads over 1D elements not yet implemented')
          elementLoad= lIter.next()
        return retval

    def populateWithElementalLoads(self,preprocessor,lp):
        ''' Populate the vector container with elemental loads 
            from the load pattern argument.'''
        self.elementalLoadVectors= self.sumElementalLoads(preprocessor,lp)
        for eTag in self.elementalLoadVectors.keys():
          elem= preprocessor.getElementHandler.getElement(eTag)
          if(elem.getDimension==2):
            vLoad= self.elementalLoadVectors[eTag]
            self.data.insertNextVector(vLoad[0],vLoad[1],vLoad[2])
          else:
            lmsg.warning('displaying of loads over 1D elements not yet implemented')
        return len(self.elementalLoadVectors)

    def dumpElementalPositions(self,preprocessor,lp):
        ''' Iterate over cumulated loads dumping them into the graphic.'''
        for eTag in self.elementalLoadVectors.keys():
          elem= preprocessor.getElementHandler.getElement(eTag)
          if(elem.getDimension==2):
            vLoad= self.elementalLoadVectors[eTag]
            p= elem.getPosCentroid(True)
            self.data.insertNextPair(p.x,p.y,p.z,vLoad[0],vLoad[1],vLoad[2],self.fUnitConv,self.showPushing)
          else:
            lmsg.warning('displaying of loads over 1D elements not yet implemented')
        return len(self.elementalLoadVectors)

    def sumNodalLoads(self,preprocessor,lp):
        ''' Iterate over loaded nodes to cumulate their loads.

        :param lp: load pattern
        '''
        lIter= lp.loads.getNodalLoadIter
        nl= lIter.next()
        retval= dict()
        nTagsSet=self.setToDisp.getNodes.getTags()
        while(nl):
          nTag= nl.getNodeTag
          if nTag in nTagsSet:
              node= preprocessor.getNodeHandler.getNode(nTag)
              vLoad= nl.getForce
              v= xc.Vector([vLoad[0], vLoad[1], vLoad[2]])
              if nTag in retval:
                retval[nTag]+= v
              else:
                retval[nTag]= v
          nl= lIter.next()
        return retval

    def populateWithNodalLoads(self,preprocessor,lp):
        ''' Populate the vector container with nodal loads 
            from the load pattern argument.'''
        self.nodalLoadVectors= self.sumNodalLoads(preprocessor,lp)
        for nTag in self.nodalLoadVectors.keys():
          vLoad= self.nodalLoadVectors[nTag]
          self.data.insertNextVector(vLoad[0],vLoad[1],vLoad[2])
        return len(self.nodalLoadVectors)

    def dumpNodalPositions(self,preprocessor,lp,defFScale):
        ''' Iterate over loaded nodes dumping its loads into the graphic.

        :param lp: load pattern
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor.    
        '''
        for nTag in self.nodalLoadVectors.keys():
          node= preprocessor.getNodeHandler.getNode(nTag)
          p= node.getCurrentPos3d(defFScale)
          vLoad= self.nodalLoadVectors[nTag]
          self.data.insertNextPair(p.x,p.y,p.z,vLoad[0],vLoad[1],vLoad[2],self.fUnitConv,self.showPushing)
        return len(self.nodalLoadVectors)

    def populateLoads(self,preprocessor,lp, showElementalLoads= True, showNodalLoads= True):
        ''' Populate the vector container with loads 
            from the load pattern argument.'''
        numberOfLoads= 0
        if(showElementalLoads):
          numberOfLoads+= self.populateWithElementalLoads(preprocessor,lp)
        if(showNodalLoads):
          numberOfLoads+= self.populateWithNodalLoads(preprocessor,lp)
        return numberOfLoads

    def dumpVectors(self, preprocessor,defFScale, showElementalLoads= True, showNodalLoads= True):
        ''' Iterate over loads dumping them into the graphic.

        :param lp: load pattern
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor.
        :param showElementalLoads: if true show loads over elements.
        :param showNodalLoads: if true show loads over nodes.
        '''
        preprocessor.resetLoadCase()
        loadPatterns= preprocessor.getLoadHandler.getLoadPatterns
        loadPatterns.addToDomain(self.lpName)
        lp= loadPatterns[self.lpName]
        count= 0
        if(lp):
          numberOfLoads= self.populateLoads(preprocessor,lp)
          if(numberOfLoads>0):
            self.data.scaleFactor/= self.getMaxLoad()
            #Iterate over loaded elements.
            count+= self.dumpElementalPositions(preprocessor,lp)
            #Iterate over loaded nodes.
            count+= self.dumpNodalPositions(preprocessor,lp,defFScale)
            if(count==0):
              lmsg.warning('LoadVectorField.dumpVectors: no loads defined.')
            loadPatterns.removeFromDomain(self.lpName)
        else:
          lmsg.error('Load pattern: '+ self.lpName + ' not found.')
        return count

    def dumpNodalLoads(self, preprocessor,defFScale):
        ''' Iterate over nodal loads dumping them into the graphic.

        :param lp: load pattern
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor.
        '''
        return self.dumpVectors(preprocessor,defFScale,False,True)
    

