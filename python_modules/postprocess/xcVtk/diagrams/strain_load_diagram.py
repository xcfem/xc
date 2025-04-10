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

class StrainLoadDiagram(ld.LoadDiagram):
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
        super(StrainLoadDiagram,self).__init__(setToDisp= setToDisp, scale= scale, lRefModSize= lRefModSize, fUnitConv= fUnitConv, component= component, rgMinMax= rgMinMax)

    def autoScale(self, preprocessor):
        ''' Autoscale the diagram.

        :param preprocessor: pre-processor of the finite element problem.
        '''
        maxAbsComp= self.getMaxAbsComp(preprocessor= preprocessor)
        if(maxAbsComp>0):
            self.scaleFactor*= self.lRefModSize/maxAbsComp/5e3
        return maxAbsComp
    
    def sumElementalStrainLoads(self, actLP):
        ''' Iterate over active load patterns and cumulate on elements their 
        elemental unifirm loads. Returns a dictionary that stores for each 
        linear loaded element the sum of active loads on it

        :param actLP: list of active load patterns.
        '''
        retval= dict()
        if(len(actLP)>0):
            preprocessor= actLP[0].getDomain.getPreprocessor     
            spaceDimension= preprocessor.getNodeHandler.dimSpace
            for lp in actLP:
                lIter= lp.loads.getElementalLoadIter
                eLoad= lIter.next()
                eTagsSet= self.setToDisp.getElements.getTags()
                while(eLoad):
                    category= eLoad.category
                    if('strain' in category):
                        if(hasattr(eLoad,'getStrainsMatrix')):
                            tags= (set(eLoad.elementTags) & set(eTagsSet))
                            for eTag in tags:
                                elem= preprocessor.getElementHandler.getElement(eTag)
                                dim= elem.getDimension
                                nDOFperNode= elem.getNumDOF()/elem.numNodes
                                # Strain loads on elements other than beams
                                # elements are displayed as fields on the
                                # element itself (not as diagrams).
                                beamElement= (dim==1) and (nDOFperNode>spaceDimension)
                                if(beamElement):
                                    if eTag in retval:
                                        retval[eTag]+= eLoad.getStrainsMatrix()
                                    else:
                                        retval[eTag]= eLoad.getStrainsMatrix() # use eLoad method to make sure that
                                                                               # retval[eTag] stores a copy of the
                                                                               # strains matrix (instead of a pointer
                                                                               # to a local variable).
                        else:
                            className= type(self).__name__
                            methodName= sys._getframe(0).f_code.co_name
                            warningMsg= '; display of loads of type '+str(type(eLoad))
                            warningMsg+= ' not implemented yet.'
                            lmsg.warning(className+'.'+methodName+warningMsg)
                    elif(category in ['uniform', 'raw', 'punctual']):
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        errorMsg= "; component:'"+str(self.component)+"' unknown."
                        errorMsg= "; use LinearLoadDiagram to draw loads of type: '"
                        errorMsg+= str(type(eLoad))+ "'."
                        lmsg.error(className+'.'+methodName+errorMsg)
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
        :param defFScale: factor to apply to current displacement of nodes 
                   so that the display position of each node equals to
                   the initial position plus its displacement multiplied
                   by this factor. (Defaults to 0.0, i.e. display of 
                   initial/undeformed shape).
        '''
        preprocessor= actLP[0].getDomain.getPreprocessor
        if not self.dictActLoadVectors:
            self.dictActLoadVectors= self.sumElementalStrainLoads(actLP)
        valueCouples= list()
        elements= list()
        directions= list()
        for eTag in self.dictActLoadVectors.keys():
            elem= preprocessor.getElementHandler.getElement(eTag)
            dim= elem.getDimension
            strainMatrix= self.dictActLoadVectors[eTag]
            if(self.component==0):
                directions.append(elem.getJVector3d(True)) # diagram direction.
                elements.append(elem)
                valueCouples.append((strainMatrix(0, self.component), strainMatrix(1, self.component)))      
            elif(self.component==1):
                directions.append(elem.getJVector3d(True))
                elements.append(elem)
                valueCouples.append((strainMatrix(0, self.component), strainMatrix(1, self.component)))      
            elif(self.component==2):  
                directions.append(elem.getKVector3d(True))
                elements.append(elem)
                valueCouples.append((strainMatrix(0, self.component), strainMatrix(1, self.component)))      
            elif(self.component==3):
                directions.append(elem.getJVector3d(True)) # diagram direction.
                elements.append(elem)
                valueCouples.append((strainMatrix(0, self.component), strainMatrix(1, self.component)))      
            elif(self.component==4):
                directions.append(elem.getJVector3d(True))
                elements.append(elem)
                valueCouples.append((strainMatrix(0, self.component), strainMatrix(1, self.component)))      
            elif(self.component==5):  
                directions.append(elem.getKVector3d(True))
                elements.append(elem)
                valueCouples.append((strainMatrix(0, self.component), strainMatrix(1, self.component)))      
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                errorMsg= "; component:'"+str(self.component)+"' unknown."
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
            self.dictActLoadVectors= self.sumElementalStrainLoads(actLP)
        eTags=[tag for tag in self.dictActLoadVectors.keys()]
        retval= 0.0
        if(len(eTags)>0):
            retval= 0.0
            for eTag in eTags:
                strainMatrix= self.dictActLoadVectors[eTag]
                retval= max(retval, abs(strainMatrix(0, self.component)), abs(strainMatrix(1, self.component)))
        return retval

