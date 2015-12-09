# -*- coding: utf-8 -*-
class FiberSet:
  '''This class constructs a set of all the  fibers made of the same material from a fiber section
  Attributes:
    scc:       name identifying the fiber section
    nmbSet:    name of the set of fibers to be generated
    tagDiag:   tag of the uniaxial material which forms the fibers
  '''
  fSet= None

  def __init__(self,scc,nmbSet,tagDiag):
    fiberSets= scc.getFiberSets()
    self.fSet= fiberSets.create(nmbSet)
    fibras= scc.getFibers()
    for f in fibras:
      if(f.getMaterial().tag==tagDiag):
        self.fSet.insert(f)
    self.fSet.updateCDG()
  def getFiberWithMinStrain(self):
    '''returns the fiber with the minimum straim from the set of fibers
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
    '''returns the fiber with the maximum straim from the set of fibers
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
    tagCDiag:  tag of the uniaxial material that makes up the concrete fibers of the section
    nmbSetC:   name of the set of fibers of concrete to be generated
    tagSdiag:  tag of the uniaxial material that makes up the reinforcing steel fibers of the section
    nmbSetS:   name of the set of fibers of reinforcing steel to be generated
  '''
  fibrasHormigon= None
  fibrasArmadura= None
  tractionFibers= None
  
  def __init__(self,scc,tagCdiag, nmbSetC,tagSdiag, nmbSetS):
    fiberSets= scc.getFiberSets()
    self.fibrasHormigon= FiberSet(scc,nmbSetC,tagCdiag)
    self.fibrasArmadura= FiberSet(scc,nmbSetS,tagSdiag)
    self.tractionFibers= None
  def reselTractionFibers(self,scc,tractionFibersSetName):
    '''returns a set with those fibers in tension from the total set 
    '''
    self.tractionFibers= scc.getFiberSets().create(tractionFibersSetName)
    for f in self.fibrasArmadura.fSet:
      if(f.getMaterial().getStress()>0.0):
        self.tractionFibers.insert(f)
    self.tractionFibers.updateCDG()
    return self.tractionFibers
  def getConcreteArea(self,factor):
    '''returns the cross section area of concrete in the set of fibers
    '''
    return self.fibrasHormigon.fSet.getArea(factor)
  def getMaxConcreteStrain(self):
    '''returns the maximum strain in the set of concrete fibers
    '''
    return self.fibrasHormigon.fSet.getStrainMax()
  def getConcreteInitialTangent(self):
    '''returns the initial tangent in the stress-strain diagram of the material that makes up the fibers of concrete
    '''
    return self.fibrasHormigon.fSet[0].getMaterial().getInitialTangent()
  def getConcreteCompression(self):
    '''returns the resultant of compressive stresses in the concrete fibers of the section
    '''
    return self.fibrasHormigon.fSet.ResultanteComp()
  def getNumBarrasTraccion(self):
    '''returns the number of reinforcing steel fibers in tension
    '''
    return self.tractionFibers.getNumFibers()

def fiberSectionSetupRCSets(scc,tagCdiag, nmbSetC,tagSdiag, nmbSetS):
  return RCSets(scc,tagCdiag, nmbSetC,tagSdiag, nmbSetS)

# Crea conjuntos de fibras en las secciones de los elementos de conjunto cuyo nombre.
def creaSetsFibrasHA(preprocessor, nmbSet, tagHA, tagAcero):
  '''Construct the sets of concrete fibers 'hormigon' and reinforcing steel fibers 'armadura' for all the elements included in a set of elements.
  Returns the following two sets of fibers:
    set named hormigon: include all the fibers of material tagHA from the set of elements nmbSet
    set named armadura: include all the fibers of material tagAcero from the set of elements nmbSet
  Parameters:
    mdlr:     preprocessor name
    nmbSet:   name identifying the set of element
    tagHA:    tag of the uniaxial material that makes up the concrete fibers
    tagAcero: tag of the uniaxial material that makes up the reinforcing steel fibers
  '''
  st= preprocessor.getSets.getSet(nmbSet)
  elements= st.getElements
  for e in elements:
    scc= e.getSection()
    sets= fiberSectionSetupRCSets(scc,tagHA,"hormigon",tagAcero,"armadura")
    scc.defProp("fiberSets",sets)

def reselTractionFibers(scc,fiberSetName,tractionFibersSetName):
  '''Returns the fibers under tension included in a set of fibers of a fiber section type
  Parameters:
  scc:          name identifying the fiber section
  fiberSetName: name identifying the set of fibers 
  '''
  fiberSet= scc.getFiberSets()[fiberSetName]
  tractionFibers= scc.getFiberSets().create(tractionFibersSetName)
  for f in fiberSet:
    if(f.getMaterial().getStress()>0.0):
      tractionFibers.insert(f)
  return tractionFibers

def fiberSectionSetupRC3Sets(scc,tagCdiag, nmbSetC,tagSdiag, nmbSetS):
  '''returns a set of tensioned fibers 'armaduraTraccion' of a fiber section of reinforced concrete.
  Parameters:
  scc:          name identifying the fiber section
  tagCdiag:     tag of the uniaxial material that makes up the concrete fibers
  nmbSetC:      name of the set of fibers of concrete to be generated
  tagSdiag:     tag of the uniaxial material that makes up the reinforcing steel fibers
  nmbSetS:      name of the set of fibers of reinforcing steel to be generated
  '''
  sets= fiberSectionSetupRCSets(scc,tagCdiag,nmbSetC,tagSdiag,nmbSetS)
  sets.reselTractionFibers(scc,"armaduraTraccion")
  return sets
  
