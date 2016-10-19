# -*- coding: utf-8 -*-
# Model made of zero_lenght_section elements that is used only
# for checking limit states i.e. for reinforced concrete sections.
#
# This kind of phantom model is needed when the model that we use
# to check limit states is different from the model that we use for
# the analysis. A typical example:
#
# - The internal forces under different load combinations are obtained using a linear elastic model for the materials.
# - A "phantom model" with a more realistic representation of the cross sections
#   (fiber models,...) is used for limit state checking at cross section 
#   level (crack control, shear,...).

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import csv
import numpy
import xc_base
import geom
import xc
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials
from materials import paramRectangularSection
from postprocess import ControlVars as cv
from solution import predefined_solutions
from solution import resuelve_combinacion
from miscUtils import LogMessages as lmsg

# Fake section (elements must have a stiffness)
sccFICT= paramRectangularSection.RectangularSection("rectang",b=.40,h=40)
matSccFICT= typical_materials.MaterialData("mrectang",E=2.1e6,nu=0.3,rho=2500)

class PhantomModel(object):
  def __init__(self,preprocessor, sectionNamesForEveryElement, mapSectionsDefinition, mapInteractionDiagrams):
    ''' Extrae los identificadores de elementos de un archivo de salida con resultados
    de combinaciones generado en XC 
    
    :ivar preprocessor:        preprocessor name
    :ivar    sectionsNamesForEveryElement:  file containing a dictionary  such that
                                       for each shell element of the model stores 
                                       two names (for the sections 1 and 2) to 
                                       be employed in verifications
     :ivar   mapSectionsDefinition:      file containing a dictionary with the two 
                                    sections associated with each elements to be
                                    used in the verification
     :ivar   mapInteractionDiagrams:     file containing a dictionary such that
                                    associates each element with the two interactions
                                    diagrams of materials to be used in the verification  
    '''
    self.preprocessor= preprocessor
    self.sectionsNamesForEveryElement= sectionNamesForEveryElement
    self.mapSectionsDefinition= mapSectionsDefinition
    self.mapInteractionDiagrams= mapInteractionDiagrams

  def createElements(self,intForcCombFileName,controller,fakeSection= True):
    '''Creates the phantom model elements from the data read on the file.
    
    :param   intForcCombFileName: name of the file containing the forces and bending moments 
                           obtained for each element for the combinations analyzed
    :param   controller:     object that takes the results and checks the limit state.
    :param   fakeSection:  true if a fiber section model of the section is not needed for control.
    '''
    idElements= set()
    self.idCombs= set()
    retval= []
    f= open(intForcCombFileName,"r")
    listado= csv.reader(f)
    for lst in listado:    #lst: list of internal forces for a given combination and element 
      if(len(lst)>0):
        idComb= lst[0]
        self.idCombs.add(idComb)
        idElem= eval(lst[1])
        idElements.add(idElem)
    f.close()
    nodes= self.preprocessor.getNodeLoader
    predefined_spaces.gdls_resist_materiales3D(nodes)
    elements= self.preprocessor.getElementLoader
    coacciones= self.preprocessor.getConstraintLoader
    # Definimos materiales
    fkSection= sccFICT.defSeccShElastica3d(self.preprocessor,matSccFICT) # The problem is isostatic, so the section is not a matter
    elements.dimElem= 1
    self.tagsNodesToLoad1= {}
    self.tagsNodesToLoad2= {}
    if(fakeSection):
      elements.defaultMaterial= sccFICT.nmb
    for tagElem in idElements:
      nmbScc1= self.sectionsNamesForEveryElement[tagElem][0]
      n1= nodes.newNodeXYZ(0,0,0)
      n3= nodes.newNodeXYZ(0,0,0)
      fix_node_6dof.fixNode6DOF(coacciones,n1.tag)
      self.tagsNodesToLoad1[tagElem]= n3.tag
      if(not fakeSection):
        elements.defaultMaterial= nmbScc1
      e1= elements.newElement("zero_length_section",xc.ID([n1.tag,n3.tag]))
      retval.append(e1)
      e1.setProp("idElem", tagElem) #Elemento que se comprueba
      e1.setProp("dir",1)
      e1.setProp("idSection", nmbScc1) #Section to verify
      scc= e1.getSection()
      scc.setProp("datosSecc", self.mapSectionsDefinition[nmbScc1]) #Section definition (XXX duplicated)
      if(self.mapInteractionDiagrams != None):
        diagIntScc1= self.mapInteractionDiagrams[nmbScc1]
        e1.setProp("diagInt",diagIntScc1) 
      nmbScc2= self.sectionsNamesForEveryElement[tagElem][1]
      n2= nodes.newNodeIDXYZ(tagElem*10+2,0,0,0)
      n4= nodes.newNodeIDXYZ(tagElem*10+4,0,0,0)
      fix_node_6dof.fixNode6DOF(coacciones,n2.tag)
      self.tagsNodesToLoad2[tagElem]= n4.tag
      if(not fakeSection):
        elements.defaultMaterial= nmbScc2
      e2= elements.newElement("zero_length_section",xc.ID([n2.tag,n4.tag])) 
      retval.append(e2)
      e2.setProp("idElem", tagElem) #Elemento que se comprueba
      e2.setProp("dir",2)
      e2.setProp("idSection", nmbScc2) #Section to verify
      scc= e2.getSection()
      scc.setProp("datosSecc",  self.mapSectionsDefinition[nmbScc2]) #Section definition.
      if(self.mapInteractionDiagrams != None):
        diagIntScc2= self.mapInteractionDiagrams[nmbScc2]
        e2.setProp("diagInt",diagIntScc2)
    controller.initControlVars(retval)
    return retval

  def createLoads(self,intForcCombFileName,controller):
    '''Creates the loads from the data readed on the file.
    Parameters:
       intForcCombFileName: name of the file containing the forces and bending moments 
                           obtained for each element for the combinations analyzed
       controller:     object that takes the results and checks the limit state.
    '''
    cargas= self.preprocessor.getLoadLoader
    casos= cargas.getLoadPatterns
    #Load modulation.
    ts= casos.newTimeSeries("constant_ts","ts")
    casos.currentTimeSeries= "ts"
    #Load case definition
    mapCombs= {}
    for comb in self.idCombs:
      mapCombs[comb]= casos.newLoadPattern("default",comb)

    f= open(intForcCombFileName,"r")
    listado= csv.reader(f)
    for lst in listado:
      if(len(lst)>0):
        nmbComb= lst[0]
        idElem= eval(lst[1])
        lp= mapCombs[nmbComb]
        tagNode3= self.tagsNodesToLoad1[idElem]
        '''Adoptamos el método de Wood para evaluar los momentos para dimensionar a flexión'''
        # AXIL_X,Q_X,RASANTE,TORSOR=0,MOM_X*,0.0
        #      2,  8,      4,     7,    5,0.0
        N= eval(lst[2])
        Qx= eval(lst[8])
        R= eval(lst[4])
        T= 0.0
        Mx= eval(lst[5])
        s= float(numpy.sign(Mx))
        Mx+= s*abs(eval(lst[7]))
        My= 0.0
        lp.newNodalLoad(tagNode3,xc.Vector([N,Qx,R,T,Mx,My]))
   
        tagNode4= self.tagsNodesToLoad2[idElem]
        # AXIL_Y,Q_Y,RASANTE,TORSOR=0,MOM_Y*,0.0
        #      3,  9,      4,     7,    6,0.0
        N= eval(lst[3])
        Qx= eval(lst[9])
        R= eval(lst[4])
        T= 0.0
        Mx= eval(lst[6])
        s= float(numpy.sign(Mx))
        Mx+= s*abs(eval(lst[7]))
        My= 0.0
        lp.newNodalLoad(tagNode4,xc.Vector([N,Qx,R,T,Mx,My]))

    f.close()

  def build(self,intForcCombFileName,controller,fakeSection= True):
    '''Builds the phantom model from the data readed from the file.
    Parameters:
       intForcCombFileName: name of the file containing the forces and bending moments 
                           obtained for each element for the combinations analyzed
       controller:     object that takes the results and checks the limit state.
       fakeSection:  true if a fiber section model of the section is not needed for control.
    '''
    retval= self.createElements(intForcCombFileName,controller)
    self.createLoads(intForcCombFileName,controller)
    return retval

  def check(self, analysis, controller):
    '''
    Lanza el análisis (lineal) y la comprobación en las combinaciones que se pasan como parámetros
    Parameters:
      elements: elements to check
      analysis: type of analysis
      controller: object that controls limit state in elements.
    '''
    combs= self.preprocessor.getLoadLoader.getLoadPatterns #Here each load pattern represents a combination.
    elements= self.preprocessor.getSets.getSet("total").getElements
    for key in combs.getKeys():
      comb= combs[key]
      #print "Resolviendo para acción: ",key
      resuelve_combinacion.resuelveComb(self.preprocessor,key,analysis,1)
      controller.check(elements,key)

  def write(self,controller,outputFileName):
    '''
    Writes results into the file
    Parameters:
      controller: object that controls limit state in elements.
      outputFileName: base name of output file (extensions .py and .tex)
    '''
    return cv.writeControlVarsFromElements(controller.limitStateLabel,self.preprocessor,outputFileName)

  def runChecking(self,intForcCombFileName,analysis,controller,outputFileName):
    '''Run the analysis, check the results and write them into a file)
    Parameters:
      intForcCombFileName: name of the file containing the forces and bending moments 
                           obtained for each element for the combinations analyzed
      analysis: type of analysis
      controller: object that controls limit state in elements.
      outputFileName: base name of output file (extensions .py and .tex)
    '''
    meanCFs= -1.0 
    if(controller):
      self.build(intForcCombFileName,controller,False)
      self.check(analysis,controller)
      meanCFs= self.write(controller,outputFileName)
    else:
      lmsg.error('PhantomModel::runChecking controller not defined.')
    return meanCFs
