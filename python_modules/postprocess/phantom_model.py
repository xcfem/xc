# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) & Ana Ortega (AO_O)"
__copyright__= "Copyright 2016,LCPT & AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com  ana.ortega@ciccp.es"

import sys
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from materials.sections import section_properties
from misc_utils import log_messages as lmsg
from collections import defaultdict
from postprocess import limit_state_data as lsd

# Fake section (elements must have a stiffness)
sccFICT= section_properties.RectangularSection("rectang",b=.40,h=40)
matSccFICT= typical_materials.MaterialData("mrectang",E=2.1e6,nu=0.3,rho=2500)

class PhantomModel(object):
    '''
       Model made of ZeroLengthSection elements that is used only
       for checking limit states i.e. for reinforced concrete sections.

       This kind of phantom model is needed when the model that we use
       to check limit states is different from the model that we use for
       the analysis. A typical example:

       - The internal forces under different load combinations are obtained using a linear elastic model for the materials.
       - A "phantom model" with a more realistic representation of the cross sections
         (fiber models,...) is used for limit state checking at cross section 
         level (crack control, shear,...).
    '''
    def __init__(self,preprocessor, sectionDistribution):
        '''Extracts the element identifiers from a XC output file generated
        with the results for each conbination analyzed 

        :ivar preprocessor: preprocessor object (access to FE model -nodes, 
                            elements, loads,...)
        :ivar sectionsDistribution:  file containing the section definition for
                                     each element (this section will be 
                                     be employed in verifications).
        '''
        self.preprocessor= preprocessor
        self.sectionsDistribution= sectionDistribution

    def setupForElementsAndCombinations(self, intForcItems):
        '''Extracts element and combination identifiers from the internal
           forces listing file.

        :param intForcItems: tuple containing the element tags, the identifiers
                             of the load combinations and the values of the
                             internal forces.
        '''
        numberOfElements= len(intForcItems[0])
        if(numberOfElements>0):
            self.elementTags= intForcItems[0]
            self.idCombs= intForcItems[1]
            self.internalForcesValues= intForcItems[2]
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; no load combinations (so no elements) to check in internal forces file.')
            exit(1)

    def createPhantomElement(self, masterElementId, masterElementDimension, sectionName, sectionDefinition, sectionIndex, interactionDiagram, fakeSection):
        '''Creates a phantom element (that represents a section to check) 

        :param masterElementId: identifier of the master element (element in the "true" model associated
               with the phantom element to be created).
        :param masterElementDimension: dimension (1, 2 or 3) of the master element.
        :param sectionName: name of the 3D fiber section to create the 
               zero-length phantom element (default material)  
        :param idSection: name of the section assigned to the phantom element
                      (the section to check) -sectionName-.
        :param sectionIndex: index of the section in the "true" model element 
                     -sectionIndex-. To be renamed as sectionIndex.
        :param interactionDiagram: interaction diagram that corresponds to 
                                      the section to check.
        :param fakeSection: if True (default value) generates a fake section 
               of type 'xc.ElasticShearSection3d', if False, generates a true 
               fiber model of the section (xc.FiberSectionShear3d)
        '''
        nA= self.preprocessor.getNodeHandler.newNodeXYZ(0,0,0)
        nB= self.preprocessor.getNodeHandler.newNodeXYZ(0,0,0)
        self.modelSpace.fixNode000_000(nA.tag)
        if(not fakeSection):
            self.preprocessor.getElementHandler.defaultMaterial= sectionName
        phantomElement= self.preprocessor.getElementHandler.newElement("ZeroLengthSection",xc.ID([nA.tag,nB.tag]))
        phantomElement.setProp("idElem", masterElementId) # Element to check
        phantomElement.setProp("idSection", sectionName) # Section to check
        phantomElement.setProp("masterElementDimension", masterElementDimension) # dimension (1, 2 or 3) of the master element.
        phantomElement.setProp("dir",sectionIndex) # Section index in the element.
        scc= phantomElement.getSection()
        scc.setProp('sectionData', sectionDefinition) #Section definition
        phantomElement.setProp("diagInt",interactionDiagram)
        return phantomElement

    def computeExhaustedSections(self, intForcItems, outputCfg, thresholdCF= 1.0):
        ''' Compute the elements for which the capacity factor is greater than 1.0,
            so they will cause the solver crash when performing a non-linear analysis.

        :param intForcItems: tuple containing the element tags, the identifiers
                             of the load combinations and the values of the
                             internal forces.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
                   variables that control the output of the checking (set of 
                   elements to be analyzed, append or not the results to a file,
                   generation or not of lists, ...)
        :param thresholdCF: value of the capacity factor above which the 
                            section will be considered exhausted.
        '''
        retval= defaultdict(list)
        # If fakeSection is true, there is no need to compute exhausted elements
        # because a linear elastic analysis will be performed.
        if(not outputCfg.controller.fakeSection):
            elementsWithoutSection= set()
            sectionNames= self.sectionsDistribution.getSectionNames(self.elementTags)
            for tagElem, elementSectionNames in zip(self.elementTags, sectionNames):
                if(elementSectionNames):
                    mapInteractionDiagrams= self.sectionsDistribution.sectionDefinition.mapInteractionDiagrams
                    for i, sectionName in enumerate(elementSectionNames):
                        diagInt= None
                        if(mapInteractionDiagrams is not None):
                            diagInt= mapInteractionDiagrams[sectionName]
                        internalForcesElem= self.internalForcesValues[tagElem]
                        internalForcesSection= list()
                        for iForce in internalForcesElem:
                            if(iForce.idSection== i):
                                internalForcesSection.append(iForce)
                        # Check capacity factor if the analysis is performed on
                        # fiber sections to avoid solver crashes due to internal
                        # forces beyond the section capacity.
                        if(diagInt and not outputCfg.controller.fakeSection):
                            for iForces in internalForcesSection:
                                N= iForces.N
                                My= iForces.My
                                Mz= iForces.Mz
                                idComb= iForces.idComb
                                idSection= iForces.idSection
                                diagDimension= diagInt.getDimension()
                                if(diagDimension==2):
                                    posEsf= geom.Pos2d(N,My) # assume Mz is zero.
                                    if(abs(Mz)>1e-3):
                                        className= type(self).__name__
                                        methodName= sys._getframe(0).f_code.co_name
                                        warningMsg= className+'.'+methodName+' element: '+str(tagElem)+'; using a two-dimensional interaction with biaxial bending internal forces (Mz!=0) use a three-dimensional one instead.'
                                        lmsg.warning(warningMsg)
                                else:
                                    posEsf= geom.Pos3d(N,My,Mz)                    
                                CF= diagInt.getCapacityFactor(posEsf)
                                if(CF>thresholdCF):
                                    retval[tagElem].append(idSection)
                                    className= type(self).__name__
                                    methodName= sys._getframe(0).f_code.co_name
                                    msg= className+'.'+methodName+'; section: '+str(idSection)
                                    msg+= ' of element: '+str(tagElem)
                                    msg+= ' is exhausted (CF= '+str(CF)+'>1)'
                                    msg+= ' under internal forces for load combination: '+str(idComb)
                                    msg+= '. The section will be excluded from the analysis to avoid the solver to crash.'
                                    lmsg.warning(msg)
                else:
                    elementsWithoutSection.add(tagElem)
            if(elementsWithoutSection):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; element section names not found for element with tags: '+str(elementsWithoutSection))
        return retval
            

    def createElements(self, intForcItems, outputCfg, exhaustedSections):
        '''Creates the phantom model elements from the data read on the file.

        :param intForcItems: tuple containing the element tags, the identifiers
                             of the load combinations and the values of the
                             internal forces.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
                   variables that control the output of the checking (set of 
                   elements to be analyzed, append or not the results to a file,
                   generation or not of lists, ...)
        :param exhaustedSections: dictionary containing the sections that will
                                  be exhausted (and will make the solver to crash)
                                  if a non-linear analysis is performed.
        '''
        retval= list()
        nodes= self.preprocessor.getNodeHandler
        self.modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
        elements= self.preprocessor.getElementHandler
        elements.dimElem= 1
        self.tagsNodesToLoad= defaultdict(list)
        # If fakeSection is true, then define default material.
        if(outputCfg.controller.fakeSection):
            fkSection= sccFICT.defElasticShearSection3d(self.preprocessor,matSccFICT) # The problem is isostatic, so the section is not a matter
            if __debug__:
                if(not fkSection):
                    AssertionError('Can\'t define material.')
            elements.defaultMaterial= sccFICT.name
        elementsWithoutSection= set()
        sectionNames= self.sectionsDistribution.getSectionNames(self.elementTags)
        for tagElem, elementSectionNames in zip(self.elementTags, sectionNames):
            if(elementSectionNames):
                elementSectionDefinitions= self.sectionsDistribution.getSectionDefinitionsForElement(tagElem)
                masterElementDimension= self.sectionsDistribution.getMasterElementDimension(tagElem)
                mapInteractionDiagrams= self.sectionsDistribution.sectionDefinition.mapInteractionDiagrams
                for i, sectionName in enumerate(elementSectionNames):
                    diagInt= None
                    if(mapInteractionDiagrams is not None):
                        diagInt= mapInteractionDiagrams[sectionName]
                    createPhantomElement= True
                    if(tagElem in exhaustedSections):
                        if(i in exhaustedSections[tagElem]):
                            createPhantomElement= False # section exhausted.
                    if(createPhantomElement):
                        phantomElem= self.createPhantomElement(masterElementId= tagElem, masterElementDimension= masterElementDimension, sectionName= sectionName, sectionDefinition= elementSectionDefinitions[i], sectionIndex= i+1, interactionDiagram= diagInt, fakeSection= outputCfg.controller.fakeSection)
                        retval.append(phantomElem)
                        self.tagsNodesToLoad[tagElem].append(phantomElem.getNodes[1].tag) # Node to load
                                                                                          # for this element
            else:
                elementsWithoutSection.add(tagElem)
        if(elementsWithoutSection):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; element section names not found for element with tags: '+str(elementsWithoutSection))
        outputCfg.controller.initControlVars(retval)
        return retval

    def createLoads(self, exhaustedSections):
        '''Creates the loads from the data read from the file.

        :param exhaustedSections: dictionary containing the sections that
                                  will be exhausted (and will make the 
                                  solver to crash) if a non-linear analysis
                                  is performed.
        '''
        cargas= self.preprocessor.getLoadHandler
        casos= cargas.getLoadPatterns
        #Load modulation.
        ts= casos.newTimeSeries("constant_ts","ts")
        casos.currentTimeSeries= ts.name
        #Load case definition
        mapCombs= {}
        for comb in self.idCombs:
            mapCombs[comb]= casos.newLoadPattern("default",str(comb))

        elementsWithoutLoadedNodes= set()
        for key in self.internalForcesValues:
            internalForcesElem= self.internalForcesValues[key]
            for iforce in internalForcesElem:
                lp= mapCombs[iforce.idComb]
                tagElem= iforce.tagElem
                idSection= iforce.idSection
                createLoad= True
                if(tagElem in exhaustedSections):
                    if(idSection in exhaustedSections[tagElem]):
                        createLoad= False # section exhausted.
                if(createLoad):
                    tagsNodesToLoad= self.tagsNodesToLoad[iforce.tagElem]
                    if(tagsNodesToLoad):
                        nodeTag= tagsNodesToLoad[iforce.idSection]
                        lp.newNodalLoad(nodeTag,xc.Vector(iforce.getComponents()))
                    else:
                        elementsWithoutLoadedNodes.add(iforce.tagElem)
        if(elementsWithoutLoadedNodes):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; no loaded nodes for elements with tags: '+str(elementsWithoutLoadedNodes))                    

    def build(self, intForcItems, outputCfg, thresholdCF= 1.0):
        '''Builds the phantom model from the data read from the file.

        :param intForcItems: tuple containing the element tags, the identifiers
                             of the load combinations and the values of the
                             internal forces.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
                   variables that control the output of the checking (set of 
                   elements to be analyzed, append or not the results to a file,
                   generation or not of lists, ...)
        :param thresholdCF: value of the capacity factor above which the 
                            section will be considered exhausted.
        '''
        self.setupForElementsAndCombinations(intForcItems= intForcItems)
        exhaustedSections= self.computeExhaustedSections(intForcItems= intForcItems, outputCfg= outputCfg, thresholdCF= thresholdCF)
        retval= self.createElements(intForcItems= intForcItems, outputCfg= outputCfg, exhaustedSections= exhaustedSections)
        if(retval):
            self.createLoads(exhaustedSections= exhaustedSections)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; no elements to check.')
            exit(1)
        return retval

    def check(self, controller):
        '''Runs the analysis (linear) and checking of combinations passed as
        parameters

        :param controller: object that controls limit state in elements.
        '''
        combs= self.preprocessor.getLoadHandler.getLoadPatterns #Here each load pattern represents a combination.
        elements= self.preprocessor.getSets.getSet("total").elements
        controller.checkSolverAdequacy()
        for key in combs.getKeys():
            #comb= combs[key]
            controller.solutionProcedure.solveComb(key)
            controller.preprocessor= self.preprocessor
            controller.check(elements, key)

    def getControlVarsDict(self, outputCfg):
        '''Return a dictionary with the values of the control variables for each element

        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (append or not
               the results to a file, generation or not of lists, ...)
        '''
        controlVarName= outputCfg.controller.limitStateLabel
        elements= self.preprocessor.getSets['total'].elements # all the elements in the phantom model.
        retval= dict()
        for e in elements:
            eTag= e.getProp("idElem")
            index= e.getProp('dir')
            controlVar= e.getProp(controlVarName)
            if not eTag in retval:
                retval[eTag]= dict()
            retval[eTag][index]= controlVar
        return retval

    def runChecking(self, intForcItems, outputCfg):
        '''Run the analysis, check the results and write them into a file

        :param intForcItems: tuple containing the element tags, the identifiers
                             of the load combinations and the values of the
                             internal forces.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
                   variables that control the output of the checking (set of 
                   elements to be analyzed, append or not the results to a file,
                   generation or not of lists, ...)
        '''
        retval=None
        controller= outputCfg.controller
        if(controller):
            thresholdCF= controller.exhaustedSectionsThresholdCF
            self.build(intForcItems= intForcItems, outputCfg= outputCfg, thresholdCF= thresholdCF)
            self.check(controller)
            # Return the values of the control variables in a dictionary.
            retval= self.getControlVarsDict(outputCfg)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; controller not defined.')
        return retval
