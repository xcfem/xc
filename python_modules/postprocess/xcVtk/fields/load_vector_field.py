# -*- coding: utf-8 -*-
''' Loads represented as vectors. '''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
import xc
from misc_utils import log_messages as lmsg
from postprocess.xcVtk.fields import vector_field as vf
from vtk.vtkFiltersSources import vtkArrowSource

def get_active_load_patterns(preprocessor):
    ''' Return the load patterns currently active in the domain.

    :param preprocessor: preprocessor of the FE problem.
    '''
    retval= list()
    activeLoadPatterns= preprocessor.getDomain.getConstraints.getLoadPatterns
    if(len(activeLoadPatterns)>0):
        retval=[lp.data() for lp in activeLoadPatterns]
    return retval
    

class LoadOnPoints(vf.VectorField):
    '''Draws punctual loads.

    :ivar components: index of the components of the load. Default: [0,1,2]
    '''
    def __init__(self, name, fUnitConv= 1e-3,scaleFactor= 1.0,showPushing= True, components= [0,1,2], symType= vtkArrowSource()):
        '''
        Constructor.
        
        :param name: name of the vector field.
        :param fUnitConv: unit conversion factor.
        :param scaleFactor: scale factor for the size of the vectors.
        :param showPushing: if true the loads ares showed pushing the loaded point (as oppssed to pull). Default: True
        :param components: index of the components of the load. Default: [0,1,2]
        :param symType: shape of the symbol (defaults to an arrow).
        '''
        super(LoadOnPoints,self).__init__(name= name, fUnitConv= fUnitConv,scaleFactor= scaleFactor, showPushing= showPushing, symType= symType)
        self.components= components

    def getMaxLoad(self):
        ''' Calculate the maximum absolute value of the loads on the
            vector container.'''
        self.data.calculateLengths(self.fUnitConv)
        rgMaxMin=self.data.lengths.GetRange()
        return abs(max(rgMaxMin, key=abs))


class LoadVectorField(LoadOnPoints):
    '''Draws an arrow representing a punctual load on nodes and on elements.

    :ivar multiplyByElementSize: if true multiply the load value by the element
                                 size (length or area or volume).
    :ivar setToDisp: set to display the loads on.
    '''
    def __init__(self, name, setToDisp, fUnitConv= 1e-3, scaleFactor= 1.0, showPushing= True, components= [0,1,2], multiplyByElementSize= True, symType= vtkArrowSource()):
        '''
        Constructor.

        :param name: name of the vector field.
        :param setToDisp: set over which to display the loads.
        :param fUnitConv: unit conversion factor.
        :param scaleFactor: scale factor for the size of the vectors.
        :param showPushing: true if the loads push the loaded point (as oppssed to pull). Default: True
        :param components: index of the components of the load. Default: [0,1,2] 
        :param multiplyByElementSize: for loads over elements (default= True).
        :param symType: shape of the symbol (defaults to an arrow).
        '''
        super(LoadVectorField,self).__init__(name= name, fUnitConv= fUnitConv, scaleFactor= scaleFactor, showPushing= showPushing, components= components, symType= symType)
        self.multiplyByElementSize= multiplyByElementSize
        self.setToDisp= setToDisp

    def sumElementalUniformLoads(self, actLP):
        ''' Iterate over active load patterns and cumulate on elements 
            their elemental uniform loads.

        :param actLP: list of active load patterns.
        '''
        retval= dict()
        dim= len(self.components)
        if(dim>1):
            comp_i= self.components[0]; comp_j= self.components[1];
        if(dim>2):
            comp_k= self.components[2]
        for lp in actLP:
            lIter= lp.loads.getElementalLoadIter
            preprocessor= lp.getDomain.getPreprocessor
            elementLoad= lIter.next()
            eTagsSet=self.setToDisp.getElements.getTags()
            while(elementLoad):
                category= elementLoad.category
                if(category=='uniform' or category=='raw'):
                    if hasattr(elementLoad,'getGlobalForces'):
                        tags= elementLoad.elementTags
                        globalForces= elementLoad.getGlobalForces()
                        for i, eTag in enumerate(tags):
                          if eTag in eTagsSet:
                              elem= preprocessor.getElementHandler.getElement(eTag)
                              elemDimension= elem.getDimension
                              vLoad= globalForces.getRow(i)
                              if(elemDimension==2):
                                  if((category=='raw') and (not self.multiplyByElementSize)): # Those loads return the total load over the element.
                                      vLoad*= (1.0/elem.getArea(True))
                                  if(self.multiplyByElementSize and (category!='raw')):
                                      vLoad*= elem.getArea(True)
                                  if(dim>2):
                                      v= xc.Vector([vLoad[comp_i],vLoad[comp_j],vLoad[comp_k]])
                                  elif(dim>1):
                                      v= xc.Vector([vLoad[comp_i],vLoad[comp_j], 0.0])
                                  if eTag in retval:
                                      retval[eTag]+= v
                                  else:
                                      retval[eTag]= v
                              elif(elemDimension==3):
                                  if((category=='raw') and (not self.multiplyByElementSize)): # Those loads return the total load over the element.
                                      vLoad*= (1.0/elem.getVolume(True))
                                  if(self.multiplyByElementSize and (category!='raw')):
                                      vLoad*= elem.getVolume(True)
                                  v= xc.Vector([vLoad[comp_i],vLoad[comp_j],vLoad[comp_k]])
                                  if eTag in retval:
                                      retval[eTag]+= v
                                  else:
                                      retval[eTag]= v
                              elif(elemDimension==1):
                                  if(category!='uniform'): # Uniform loads are represented with diagrams.
                                      className= type(self).__name__
                                      methodName= sys._getframe(0).f_code.co_name
                                      warningMsg= '; display of '+str(category)+' loads over '+str(elemDimension)+'-dimensional'
                                      warningMsg+= ' elements not implemented yet.'
                                      lmsg.warning(className+'.'+methodName+warningMsg)
                              else:
                                  className= type(self).__name__
                                  methodName= sys._getframe(0).f_code.co_name
                                  warningMsg= '; display of '+str(category)+' loads over '+str(elemDimension)+'-dimensional'
                                  warningMsg+= ' elements not implemented yet.'
                                  lmsg.warning(className+'.'+methodName+warningMsg)
                    else:
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        warningMsg= '; display of loads of type '+str(type(elementLoad))
                        warningMsg+= ' not implemented yet.'
                        lmsg.warning(className+'.'+methodName+warningMsg)
                elif(category=='punctual'):
                    # Concentrated load must be treated elsewhere
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.warning(className+'.'+methodName+'; display of concentrated loads not implemented yet.')
                elif('strain' in category):
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.warning(className+'.'+methodName+'; display of strain loads not implemented yet.')
                else:
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.warning(className+'.'+methodName+'; category: '+str(category)+' unknown.')
                elementLoad= lIter.next()
        return retval

    def populateWithElementalLoads(self, actLP):
        ''' Populate the vector container with elemental loads 
            from the load pattern argument.

        :param actLP: list of active load patterns.
        '''
        self.elementalLoadVectors= self.sumElementalUniformLoads(actLP)
        preprocessor= actLP[0].getDomain.getPreprocessor
        for eTag in self.elementalLoadVectors.keys():
            elem= preprocessor.getElementHandler.getElement(eTag)
            if(elem.getDimension>=2):
                vLoad= self.elementalLoadVectors[eTag]
                self.data.insertNextVector(vLoad[0],vLoad[1],vLoad[2])
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; displaying of loads over 1D elements not yet implemented')
        return len(self.elementalLoadVectors)

    def dumpElementalPositions(self, preprocessor):
        ''' Iterate over cumulated loads dumping them into the graphic.

        :param preprocessor: preprocessor of the FE problem.
        '''
        for eTag in self.elementalLoadVectors.keys():
            elem= preprocessor.getElementHandler.getElement(eTag)
            if(elem.getDimension>=2):
                vLoad= self.elementalLoadVectors[eTag]
                p= elem.getPosCentroid(True)
                self.data.insertNextPair(p.x,p.y,p.z,vLoad[0],vLoad[1],vLoad[2],self.fUnitConv,self.showPushing)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; displaying of loads over 1D elements not yet implemented')
        return len(self.elementalLoadVectors)

    def sumNodalLoads(self,actLP):
        ''' Iterate over loaded nodes to cumulate their loads.

        :param actLP: list of active load patterns
        '''
        retval= dict()
        dim= len(self.components)
        comp_i= None; comp_j= None; comp_k= None
        if(dim>1):
            comp_i= self.components[0];
            comp_j= self.components[1];
            if(dim>2):
                comp_k= self.components[2]
        else:
            comp_k= self.components[0]
        for lp in actLP:
            lIter= lp.loads.getNodalLoadIter
            nl= lIter.next()
            nTagsSet=self.setToDisp.getNodes.getTags()
            preprocessor= lp.getDomain.getPreprocessor
            while(nl):
                nTag= nl.getNodeTag
                if nTag in nTagsSet:
                    node= preprocessor.getNodeHandler.getNode(nTag)
                    if(__debug__):
                        if(not node):
                            AssertionError('Can\'t create the node.')
                    vLoad= nl.getLoadVector
                    if(dim>2):
                        v= xc.Vector([vLoad[comp_i], vLoad[comp_j], vLoad[comp_k]])
                    elif(dim>1):
                        v= xc.Vector([vLoad[comp_i], vLoad[comp_j], 0.0])
                    else:
                        v= xc.Vector([0.0, 0.0, vLoad[comp_k]])
                    if(v.Norm()>1e-6):
                        if nTag in retval:
                            retval[nTag]+= v
                        else:
                            retval[nTag]= v
                nl= lIter.next()
        return retval

    def populateWithNodalLoads(self, actLP):
        ''' Populate the vector container with nodal loads 
            from the load pattern argument.

        :param actLP: list of active load patterns.
        '''
        self.nodalLoadVectors= self.sumNodalLoads(actLP)
        for nTag in self.nodalLoadVectors.keys():
            vLoad= self.nodalLoadVectors[nTag]
            self.data.insertNextVector(vLoad[0],vLoad[1],vLoad[2])
        return len(self.nodalLoadVectors)

    def dumpNodalPositions(self, preprocessor, defFScale):
        ''' Iterate over loaded nodes dumping its loads into the graphic.

	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        for nTag in self.nodalLoadVectors.keys():
            node= preprocessor.getNodeHandler.getNode(nTag)
            p= node.getCurrentPos3d(defFScale)
            vLoad= self.nodalLoadVectors[nTag]
            self.data.insertNextPair(p.x,p.y,p.z,vLoad[0],vLoad[1],vLoad[2],self.fUnitConv,self.showPushing)
        return len(self.nodalLoadVectors)

    def populateLoads(self, actLP, showElementalLoads= True, showNodalLoads= True):
        ''' Populate the vector container with loads from the active load patterns.

        :param actLP: list of active load patterns.
        :param showElementalLoads: True (default) to add element loads to vector container.
        :param showNodalLoads: True (default) to add nodal loads to vector container. 
        '''
        numberOfLoads= 0
        if(showElementalLoads):
            numberOfLoads+= self.populateWithElementalLoads(actLP)
        if(showNodalLoads):
            numberOfLoads+= self.populateWithNodalLoads(actLP)
        return numberOfLoads

    def dumpVectors(self, preprocessor, defFScale, showElementalLoads= True, showNodalLoads= True):
        ''' Iterate over loads dumping them into the graphic.

        :param preprocessor: preprocessor of the FE problem.
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        :param showElementalLoads: if true show loads over elements.
        :param showNodalLoads: if true show loads over nodes.
        '''
        count= 0
        activeLoadPatterns= get_active_load_patterns(preprocessor)
        if(len(activeLoadPatterns)<1):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            message= className+'.'+methodName+'; no active load patterns.'
            lmsg.warning(message)
        else:
            numberOfLoads= self.populateLoads(activeLoadPatterns, showElementalLoads, showNodalLoads)
            if(numberOfLoads>0):
                maxLoad= self.getMaxLoad()
                if(maxLoad!= 0):
                    self.data.scaleFactor/= self.getMaxLoad()
                #Iterate over loaded elements.
                if showElementalLoads:
                    count+= self.dumpElementalPositions(preprocessor)
                #Iterate over loaded nodes.
                if showNodalLoads:
                    count+= self.dumpNodalPositions(preprocessor, defFScale)
                if(count==0):
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    message= className+'.'+methodName+'; no loads defined.'
                    lmsg.warning(message)
        return count

    def dumpNodalLoads(self, preprocessor, defFScale):
        ''' Iterate over nodal loads dumping them into the graphic.

        :param preprocessor: preprocessor of the FE problem.
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        return self.dumpVectors(preprocessor, defFScale,showElementalLoads=False,showNodalLoads=True)
    
    def dumpElementalLoads(self, preprocessor, defFScale):
        ''' Iterate over elemental loads dumping them into the graphic.

        :param preprocessor: preprocessor of the FE problem.
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        return self.dumpVectors(preprocessor, defFScale, showElementalLoads=True, showNodalLoads=False)
    

class TorqueVectorField(LoadVectorField):
    '''Draws an torque-arrow representing a torque moment on nodes.

    '''
    def __init__(self, name, setToDisp, fUnitConv= 1e-3, scaleFactor= 1.0, showPushing= True, components= [3,4,5], multiplyByElementSize= False, symType= vf.get_double_headed_arrow()):
        '''
        Constructor.

        :param name: name of the vector field.
        :param setToDisp: set over which to display the loads.
        :param fUnitConv: unit conversion factor.
        :param scaleFactor: scale factor for the size of the vectors.
        :param showPushing: true if the loads push the loaded point (as oppssed to pull). Default: True
        :param components: index of the components of the load. Default: [0,1,2] 
        :param multiplyByElementSize: for loads over elements (default= False).
        :param symType: shape of the symbol (defaults to a double-headed arrow).
        '''
        super(TorqueVectorField,self).__init__(name= name, setToDisp= setToDisp, fUnitConv= fUnitConv,scaleFactor= scaleFactor, showPushing= showPushing, components= components, multiplyByElementSize= multiplyByElementSize, symType= symType)
