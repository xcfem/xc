# -*- coding: utf-8 -*-
''' Display of loads over linear elements. '''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
from postprocess.xcVtk.diagrams import load_diagram as ld
from misc_utils import log_messages as lmsg

class LinearLoadDiagram(ld.LoadDiagram):
    '''Draw a load over a set of linear elements (qx,qy,qz,...)

    '''
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
        super(LinearLoadDiagram,self).__init__(setToDisp= setToDisp, scale= scale, lRefModSize= lRefModSize, fUnitConv= fUnitConv, component= component, rgMinMax= rgMinMax)

    def autoScale(self, preprocessor):
        ''' Autoscale the diagram.

        :param preprocessor: pre-processor of the finite element problem.
        '''
        maxAbsComp= self.getMaxAbsComp(preprocessor= preprocessor)
        if(maxAbsComp>0):
            self.scaleFactor*= self.lRefModSize/maxAbsComp*100.0
        return maxAbsComp
    
    def sumElementalUniformLoads(self, actLP):
        ''' Iterate over active load patterns and cumulate on elements their 
        elemental unifirm loads. Returns a dictionary that stores for each 
        linear loaded element the sum of active loads on it

        :param actLP: list of active load patterns.
        '''
        retval= dict()
        if(len(actLP)>0):
            preprocessor= actLP[0].getDomain.getPreprocessor     
            for lp in actLP:
                lIter= lp.loads.getElementalLoadIter
                eLoad= lIter.next()
                eTagsSet= self.setToDisp.getElements.getTags()
                while(eLoad):
                    category= eLoad.category
                    if(category=='uniform' or category=='raw'):
                        if(hasattr(eLoad,'getVector3dLocalForce')):
                            tags= (set(eLoad.elementTags) & set(eTagsSet))
                            for eTag in tags:
                                elem= preprocessor.getElementHandler.getElement(eTag)
                                dim= elem.getDimension
                                if(dim==1):
                                    if eTag in retval:
                                        retval[eTag]+= eLoad.getVector3dLocalForce()
                                    else:
                                        retval[eTag]= eLoad.getVector3dLocalForce() # use eLoad method to make sure
                                                                                    # that retval[eTag] stores a 
                                                                                    # copy of the strains matrix
                                                                                    # (instead of a pointer to a local
                                                                                    # variable).
                        else:
                            className= type(self).__name__
                            methodName= sys._getframe(0).f_code.co_name
                            warningMsg= '; load of type: '+str(type(eLoad))
                            warningMsg+= " has no getVector3dLocalForce method. Can't display the load over this element."
                            lmsg.warning(className+'.'+methodName+warningMsg)
                    elif('strain' in category):
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        errorMsg= "; component:'"+str(self.component)+"' unknown."
                        errorMsg= "; use StrainLoadDiagram to draw loads of type: '"
                        errorMsg+= str(type(eLoad))+ "'."
                        lmsg.error(className+'.'+methodName+errorMsg)
                    elif(category=='punctual'):
                        # Concentrated load must be treated elsewhere
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        lmsg.warning(className+'.'+methodName+'; display of concentrated loads not implemented yet.')
                    else:
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        lmsg.warning(className+'.'+methodName+'; category: '+str(category)+' unknown.')
                    eLoad= lIter.next()
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+': no active load patterns; doing nothing.')
        return retval
       
    def dumpElementalLoads(self, actLP, diagramIndex, defFScale= 0.0):
        ''' Iterate over loaded elements dumping its loads into the graphic.

        :param actLP: list of active load patterns.
        :param diagramIndex: index-counter for the values to insert.
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        preprocessor= actLP[0].getDomain.getPreprocessor
        if not self.dictActLoadVectors:
            self.dictActLoadVectors= self.sumElementalUniformLoads(actLP)
        valueCouples= list()
        elements= list()
        directions= list()
        if(self.component=='axialComponent'):
            for eTag in self.dictActLoadVectors.keys():
                elem= preprocessor.getElementHandler.getElement(eTag)
                directions.append(elem.getJVector3d(True))
                axialLoad=self.dictActLoadVectors[eTag].x
                elements.append(elem)
                valueCouples.append((axialLoad, axialLoad))      
        elif(self.component in ['transComponent','transYComponent']):  # transComponent only for 2D models
            for eTag in self.dictActLoadVectors.keys():
                elem= preprocessor.getElementHandler.getElement(eTag)
                directions.append(elem.getJVector3d(True))
                transLoad=self.dictActLoadVectors[eTag].y
                elements.append(elem)
                valueCouples.append((transLoad,transLoad))
        elif(self.component=='transZComponent'):  
            for eTag in self.dictActLoadVectors.keys():
                elem= preprocessor.getElementHandler.getElement(eTag)
                directions.append(elem.getKVector3d(True))
                transZLoad=self.dictActLoadVectors[eTag].z
                elements.append(elem)
                valueCouples.append((transZLoad,transZLoad))
        elif(self.component=='xyzComponents'):
            for eTag in self.dictActLoadVectors.keys():
                elem= preprocessor.getElementHandler.getElement(eTag)
                vI= elem.getIVector3d(True)
                vJ= elem.getJVector3d(True)
                vK= elem.getKVector3d(True)
                localForce= self.dictActLoadVectors[eTag]
                v= localForce.x*vI+localForce.y*vJ+localForce.z*vK
                directions.append(v.normalized())
                totLoad= v.getModulus()
                elements.append(elem)
                valueCouples.append((totLoad, totLoad))
        elif(self.component in self.strainComponentLabels):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= "; component:'"+str(self.component)+"' unknown."
            errorMsg+= " Use StrainLoadDiagram to draw that kind of load."
            lmsg.error(className+'.'+methodName+errorMsg)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= "; component:'"+self.component+"' unknown."
            lmsg.error(className+'.'+methodName+errorMsg)
        if(valueCouples):
            diagramIndex= self.appendDataToDiagram(elements= elements, diagramIndex= diagramIndex, valueCouples= valueCouples, directions= directions, defFScale= defFScale)      
        return diagramIndex
 
    def getMaxAbsComp(self, preprocessor):
        '''Return the maximum absolute value of the component.
        It is used for calculating auto-scale parameter.

        :param preprocessor: pre-processor of the finite element problem.
        '''
        activeLoadPatterns= preprocessor.getDomain.getConstraints.getLoadPatterns
        if(len(activeLoadPatterns)<1):
            lmsg.warning('No active load patterns.')
        actLP=[lp.data() for lp in activeLoadPatterns]
        if not self.dictActLoadVectors:
            self.dictActLoadVectors= self.sumElementalUniformLoads(actLP)
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
            elif(self.component in self.strainComponentLabels):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                errorMsg= "; component:'"+str(self.component)+"' unknown."
                errorMsg+= " Use StrainLoadDiagram to draw that kind of load."
                lmsg.error(className+'.'+methodName+errorMsg)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                errorMsg= "; component:'"+str(self.component)+"' unknown."
                lmsg.error(className+'.'+methodName+errorMsg)
        return retval

