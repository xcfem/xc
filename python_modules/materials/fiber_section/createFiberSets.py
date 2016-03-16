# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__cppyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.Ortega.Ort@gmail.com"


class FiberSet:
  '''This class constructs a set of all the  fibers made of the same material from a fiber section
  Attributes:
    scc:       name identifying the fiber section
    setName:   name of the set of fibers to be generated
    matTag:    integer tag indentifying the uniaxial material which forms the fibers
  '''
  fSet= None

  def __init__(self,scc,setName,matTag):
    fiberSets= scc.getFiberSets()
    self.fSet= fiberSets.create(setName)
    fibras= scc.getFibers()
    for f in fibras:
      if(f.getMaterial().tag==matTag):
        self.fSet.insert(f)
    self.fSet.updateCDG()
  def getFiberWithMinStrain(self):
    '''returns the fiber with the minimum strain from the set of fibers
    '''
    retval= None
    epsMin= 1e9
    for f in self.fSet:
      eps= f.getMaterial().getStrain()
      if(eps<epsMin):
        retval= f
        epsMin= eps
    return retval
  def getFiberWithMaxStrain(self):
    '''returns the fiber with the maximum strain from the set of fibers
    '''
    retval= None
    epsMax= -1e9
    for f in self.fSet:
      eps= f.getMaterial().getStrain()
      if(eps>epsMax):
        retval= f
        epsMax= eps
    return retval     

class RCSets(object):
  '''This class constructs both the concrete and reinforced steel fiber sets from a reinforced concrete fiber section
  Attributes:
    scc:       name identifying the fiber section
    concrMatTag:  tag of the uniaxial material that makes up the concrete fibers of the section
    concrSetName:   name of the set of fibers of concrete to be generated
    reinfMatTag:  tag of the uniaxial material that makes up the reinforcing steel fibers of the section
    reinfSetName:   name of the set of fibers of reinforcing steel to be generated
  '''
  concrFibers= None
  reinfFibers= None
  tensionFibers= None
  
  def __init__(self,scc,concrMatTag, concrSetName,reinfMatTag, reinfSetName):
    fiberSets= scc.getFiberSets()
    self.concrFibers= FiberSet(scc,concrSetName,concrMatTag)
    self.reinfFibers= FiberSet(scc,reinfSetName,reinfMatTag)
    self.tensionFibers= None
  def reselTensionFibers(self,scc,tensionFibersSetName):
    '''returns a set with those fibers in tension from the total set 
    '''
    self.tensionFibers= scc.getFiberSets().create(tensionFibersSetName)
    for f in self.reinfFibers.fSet:
      if(f.getMaterial().getStress()>0.0):
        self.tensionFibers.insert(f)
    self.tensionFibers.updateCDG()
    return self.tensionFibers
  def getConcreteArea(self,factor):
    '''returns the cross section area of concrete in the set of fibers
    '''
    return self.concrFibers.fSet.getArea(factor)
  def getMaxConcreteStrain(self):
    '''returns the maximum strain in the set of concrete fibers
    '''
    return self.concrFibers.fSet.getStrainMax()
  def getConcreteInitialTangent(self):
    '''returns the initial tangent in the stress-strain diagram of the material that makes up the fibers of concrete
    '''
    return self.concrFibers.fSet[0].getMaterial().getInitialTangent()
  def getConcreteCompression(self):
    '''returns the resultant of compressive stresses in the concrete fibers of the section
    '''
    return self.concrFibers.fSet.ResultanteComp()
  def getNumTensionRebars(self):
    '''returns the number of reinforcing steel fibers in tension
    '''
    return self.tensionFibers.getNumFibers()

def fiberSectionSetupRCSets(scc,concrMatTag, concrSetName,reinfMatTag, reinfSetName):
  return RCSets(scc,concrMatTag, concrSetName,reinfMatTag, reinfSetName)

# Crea conjuntos de fibras en las secciones de los elementos de conjunto cuyo nombre.
def createRCFiberSets(preprocessor, setName, concrMatTag, reinfMatTag):
  '''Constructs the sets of concrete fibers 'hormigon' and reinforcing steel fibers 'reinforcement' for all the elements included in a set of elements.
  Returns the following two sets of fibers:
    set named hormigon: include all the fibers of material concrMatTag from the set of elements setName
    set named reinforcement: include all the fibers of material reinfMatTag from the set of elements setName
  Parameters:
    preprocessor:     preprocessor name
    setName:          name identifying the set of element
    concrMatTag:      tag of the uniaxial material that makes up the concrete fibers
    reinfMatTag:      tag of the uniaxial material that makes up the reinforcing steel fibers
  '''
  st= preprocessor.getSets.getSet(setName)
  elements= st.getElements
  for e in elements:
    scc= e.getSection()
    sets= fiberSectionSetupRCSets(scc,concrMatTag,"hormigon",reinfMatTag,"reinforcement")
    scc.defProp("fiberSets",sets)

def reselTensionFibers(scc,fiberSetName,tensionFibersSetName):
  '''Returns the fibers under tension included in a set of fibers of a fiber section type
  Parameters:
  scc:          name identifying the fiber section
  fiberSetName:     name identifying the set of fibers 
  '''
  fiberSet= scc.getFiberSets()[fiberSetName]
  tensionFibers= scc.getFiberSets().create(tensionFibersSetName)
  for f in fiberSet:
    if(f.getMaterial().getStress()>0.0):
      tensionFibers.insert(f)
  return tensionFibers

def fiberSectionSetupRC3Sets(scc,concrMatTag, concrSetName,reinfMatTag, reinfSetName):
  '''returns a set of tensioned fibers 'reinforcementTraccion' of a fiber section of reinforced concrete.
  Parameters:
  scc:         name identifying the fiber section
  concrMatTag:     tag of the uniaxial material that makes up the concrete fibers
  concrSetName:    name of the set of fibers of concrete to be generated
  reinfMatTag:     tag of the uniaxial material that makes up the reinforcing steel fibers
  reinfSetName:    name of the set of fibers of reinforcing steel to be generated
  '''
  sets= fiberSectionSetupRCSets(scc,concrMatTag,concrSetName,reinfMatTag,reinfSetName)
  sets.reselTensionFibers(scc,"reinforcementTraccion")
  return sets
  
