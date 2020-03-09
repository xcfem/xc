# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) & Ana Ortega (AO_O)"
__copyright__= "Copyright 2016,LCPT & AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com  ana.ortega@ciccp.es"

import numpy
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from materials.sections import section_properties
from postprocess import control_vars as cv
from solution import predefined_solutions
from miscUtils import log_messages as lmsg
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

    def setupForElementsAndCombinations(self,intForcCombFileName,setCalc=None):
        '''Extracts element and combination identifiers from the internal
           forces listing file.

        :param   intForcCombFileName: name of the file containing the internal
                                      forces obtained for each element for 
                                      the combinations analyzed
        :param setCalc: set of elements to be analyzed (defaults to None which 
                        means that all the elements in the file of internal forces
                        results are analyzed) 
        '''
        intForcItems=lsd.readIntForcesFile(intForcCombFileName,setCalc)
        self.elementTags= intForcItems[0]
        self.idCombs=intForcItems[1]
        self.internalForcesValues=intForcItems[2]    

    def createPhantomElement(self,idElem,sectionName,sectionDefinition,sectionIndex,interactionDiagram,fakeSection):
        '''Creates a phantom element (that represents a section to check) 

        :param idElem: identifier of the element in the "true" model associated
               with the phantom element to be created.
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
        phantomElement.setProp("idElem", idElem) #Element to check
        phantomElement.setProp("idSection", sectionName) #Section to check
        phantomElement.setProp("dir",sectionIndex) #Section index in the element.
        scc= phantomElement.getSection()
        scc.setProp("datosSecc", sectionDefinition) #Section definition
        phantomElement.setProp("diagInt",interactionDiagram)
        return phantomElement


    def createElements(self,intForcCombFileName,controller,setCalc=None):
        '''Creates the phantom model elements from the data read on the file.

        :param intForcCombFileName: name of the file containing the internal
                                    forces obtained for each element for 
                                    the combinations analyzed
        :param controller: object that takes the internal forces and the
                           section definition and checks the limit state.
        :param setCalc: set of elements to be analyzed (defaults to None which 
                        means that all the elements in the file of internal forces
                        results are analyzed) 
        '''
        self.setupForElementsAndCombinations(intForcCombFileName,setCalc)

        retval= []
        nodes= self.preprocessor.getNodeHandler
        self.modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
        elements= self.preprocessor.getElementHandler
        constraints= self.preprocessor.getBoundaryCondHandler
        # Define materials
        fkSection= sccFICT.defElasticShearSection3d(self.preprocessor,matSccFICT) # The problem is isostatic, so the section is not a matter
        elements.dimElem= 1
        self.tagsNodesToLoad= defaultdict(list)
        if(controller.fakeSection):
            elements.defaultMaterial= sccFICT.sectionName
        for tagElem in self.elementTags:
            elementSectionNames= self.sectionsDistribution.getSectionNamesForElement(tagElem)
            if(elementSectionNames):
                elementSectionDefinitions= self.sectionsDistribution.getSectionDefinitionsForElement(tagElem)
                mapInteractionDiagrams= self.sectionsDistribution.sectionDefinition.mapInteractionDiagrams
                sz= len(elementSectionNames)
                for i in range(0,sz):
                    sectionName= elementSectionNames[i]
                    diagInt= None
                    if(mapInteractionDiagrams != None):
                        diagInt= mapInteractionDiagrams[sectionName]
          #         print 'tagElem =',tagElem,' sectionName=',sectionName,' elSecDef=',elementSectionDefinitions[i],' sectIndex=', i+1,' diagInt=', diagInt
                    phantomElem= self.createPhantomElement(tagElem,sectionName,elementSectionDefinitions[i],i+1,diagInt,controller.fakeSection)
                    retval.append(phantomElem)
                    self.tagsNodesToLoad[tagElem].append(phantomElem.getNodes[1].tag) #Node to load
                                                                                      #for this element
            else:
              lmsg.error("Element section names not found for element with tag: "+str(tagElem))
        controller.initControlVars(retval)
        return retval

    def createLoads(self,intForcCombFileName,controller):
        '''Creates the loads from the data read from the file.

           :param intForcCombFileName: name of the file containing the forces and 
                               bending moments obtained for each element for all 
                               the combinations analyzed
           :param controller:  object that takes the internal forces and the
                               section definition and checks the limit state.

        '''
        cargas= self.preprocessor.getLoadHandler
        casos= cargas.getLoadPatterns
        #Load modulation.
        ts= casos.newTimeSeries("constant_ts","ts")
        casos.currentTimeSeries= "ts"
        #Load case definition
        mapCombs= {}
        for comb in self.idCombs:
          mapCombs[comb]= casos.newLoadPattern("default",comb)

        for key in self.internalForcesValues:
          internalForcesElem= self.internalForcesValues[key]
          for iforce in internalForcesElem:
            lp= mapCombs[iforce.idComb]
            nodeTag= self.tagsNodesToLoad[iforce.tagElem][iforce.idSection]
            lp.newNodalLoad(nodeTag,xc.Vector(iforce.getComponents()))

    def build(self,intForcCombFileName,controller,setCalc=None):
        '''Builds the phantom model from the data read from the file.

        :param intForcCombFileName: name of the file containing the forces and 
                               bending moments obtained for each element for all 
                               the combinations analyzed
        :param controller:     object that takes the internal forces and the
                               section definition and checks the limit state.
        :param setCalc: set of elements to be analyzed (defaults to None which 
                        means that all the elements in the file of internal forces
                        results are analyzed) 
        '''
        retval= self.createElements(intForcCombFileName,controller,setCalc)
        self.createLoads(intForcCombFileName,controller)
        return retval

    def check(self, controller):
        '''Runs the analysis (linear) and checking of combinations passed as
        parameters

        :param elements: elements to check
        :param analysis: type of analysis
        :param controller: object that controls limit state in elements.
        '''
        combs= self.preprocessor.getLoadHandler.getLoadPatterns #Here each load pattern represents a combination.
        elements= self.preprocessor.getSets.getSet("total").elements
        for key in combs.getKeys():
            comb= combs[key]
            #print "Resolving load combination: ",key
            predefined_solutions.resuelveComb(self.preprocessor,key,controller.analysis,1)
            controller.preprocessor=self.preprocessor
            controller.check(elements,key)

    def write(self,controller,outputFileName,outputCfg):
        '''Writes results into the output file

        :param controller:     object that controls limit state in elements
        :param outputFileName: base name of output file (extensions .py and .tex)
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (append or not
               the results to a file, generation or not of lists, ...)
        '''
        return cv.writeControlVarsFromPhantomElements(controller.limitStateLabel,self.preprocessor,outputFileName,outputCfg)

    def runChecking(self,limitStateData,outputCfg):
        '''Run the analysis, check the results and write them into a file

        :param limitStateData: object that contains the name of the file
                               containing the internal forces 
                               obtained for each element 
                               for the combinations analyzed and the
                               controller to use for the checking.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
                   variables that control the output of the checking (set of 
                   elements to be analyzed, append or not the results to a file,
                   generation or not of lists, ...)
         '''
        retval=None
        intForcCombFileName= limitStateData.getInternalForcesFileName()
        controller= limitStateData.controller
        if(controller):
            self.build(intForcCombFileName,controller,outputCfg.setCalc)
            self.check(controller)
            retval=self.write(controller,limitStateData.getOutputDataBaseFileName(),outputCfg)
        else:
            lmsg.error('PhantomModel::runChecking controller not defined.')
        return retval
