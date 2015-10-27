# -*- coding: utf-8 -*-
class FiberSet:
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
    retval= None
    epsMin= 1e9
    for f in self.fSet:
      eps= f.getMaterial().getStrain()
      if(eps<epsMin):
        retval= f
        epsMin= eps
    return retval
  def getFiberWithMaxStrain(self):
    retval= None
    epsMax= -1e9
    for f in self.fSet:
      eps= f.getMaterial().getStrain()
      if(eps>epsMax):
        retval= f
        epsMax= eps
    return retval     

class RCSets(object):
  fibrasHormigon= None
  fibrasArmadura= None
  tractionFibers= None
  
  def __init__(self,scc,tagCdiag, nmbSetC,tagSdiag, nmbSetS):
    fiberSets= scc.getFiberSets()
    self.fibrasHormigon= FiberSet(scc,nmbSetC,tagCdiag)
    self.fibrasArmadura= FiberSet(scc,nmbSetS,tagSdiag)
    self.tractionFibers= None
  def reselTractionFibers(self,scc,tractionFibersSetName):
    self.tractionFibers= scc.getFiberSets().create(tractionFibersSetName)
    for f in self.fibrasArmadura.fSet:
      if(f.getMaterial().getStress()>0.0):
        self.tractionFibers.insert(f)
    self.tractionFibers.updateCDG()
    return self.tractionFibers
  def getConcreteArea(self,factor):
    return self.fibrasHormigon.fSet.getArea(factor)
  def getMaxConcreteStrain(self):
    return self.fibrasHormigon.fSet.getStrainMax()
  def getConcreteInitialTangent(self):
    return self.fibrasHormigon.fSet[0].getMaterial().getInitialTangent()
  def getConcreteCompression(self):
    return self.fibrasHormigon.fSet.ResultanteComp()
  def getNumBarrasTraccion(self):
    return self.tractionFibers.getNumFibers()

def fiberSectionSetupRCSets(scc,tagCdiag, nmbSetC,tagSdiag, nmbSetS):
  return RCSets(scc,tagCdiag, nmbSetC,tagSdiag, nmbSetS)

# Crea conjuntos de fibras en las secciones de los elementos de conjunto cuyo nombre.
def creaSetsFibrasHA(preprocessor, nmbSet, tagHA, tagAcero):
  st= preprocessor.getSets.getSet(nmbSet)
  elements= st.getElements
  for e in elements:
    scc= e.getSection()
    sets= fiberSectionSetupRCSets(scc,tagHA,"hormigon",tagAcero,"armadura")
    scc.defProp("fiberSets",sets)

def reselTractionFibers(scc,fiberSetName,tractionFibersSetName):
  fiberSet= scc.getFiberSets()[fiberSetName]
  tractionFibers= scc.getFiberSets().create(tractionFibersSetName)
  for f in fiberSet:
    if(f.getMaterial().getStress()>0.0):
      tractionFibers.insert(f)
  return tractionFibers

def fiberSectionSetupRC3Sets(scc,tagCdiag, nmbSetC,tagSdiag, nmbSetS):
  sets= fiberSectionSetupRCSets(scc,tagCdiag,nmbSetC,tagSdiag,nmbSetS)
  sets.reselTractionFibers(scc,"armaduraTraccion")
  return sets
  
