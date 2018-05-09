# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.ortega@ciccp.es"


class FiberSet:
    '''This class constructs a set of all the  fibers made of the same material
        from a fiber section

    :ivar scc:       fiber section
    :ivar setName:   name of the set of fibers to be generated
    :ivar matTag:    integer tag indentifying the uniaxial material which 
          forms the fibers
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
    '''This class constructs both the concrete and reinforced steel fiber sets 
    from a reinforced concrete fiber section

    :ivar scc:       fiber section
    :ivar concrMatTag:  tag of the uniaxial material that makes up the concrete
          fibers of the section
    :ivar concrSetName:   name of the set of fibers of concrete to be generated
    :ivar reinfMatTag:  tag of the uniaxial material that makes up the 
          reinforcing steel fibers of the section
    :ivar reinfSetName:   name of the set of fibers of reinforcing steel to be 
          generated
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
        '''returns the initial tangent in the stress-strain diagram of the 
        material that makes up the fibers of concrete
        '''
        return self.concrFibers.fSet[0].getMaterial().getInitialTangent()
    def getConcreteCompression(self):
        '''returns the resultant of compressive stresses in the concrete fibers
        of the section
        '''
        return self.concrFibers.fSet.getCompressionResultant()
    def getNumTensionRebars(self):
        '''returns the number of reinforcing steel fibers in tension
        '''
        return self.tensionFibers.getNumFibers()

def fiberSectionSetupRCSets(scc,concrMatTag, concrSetName,reinfMatTag, reinfSetName):
    return RCSets(scc,concrMatTag, concrSetName,reinfMatTag, reinfSetName)


def createRCFiberSets(preprocessor, setName, concrMatTag, reinfMatTag):
    '''Constructs the sets of concrete fibers 'concrete' and reinforcing steel 
     fibers 'reinforcement' for all the elements included in a set of elements.

     Returns the following two sets of fibers:

     - set named concrete: include all the fibers of material concrMatTag from 
       the set of elements setName

     - set named reinforcement: include all the fibers of material reinfMatTag 
       from the set of elements setName

    :param preprocessor:     preprocessor name
    :param setName:          name identifying the set of element
    :param concrMatTag:      tag of the uniaxial material that makes up the concrete fibers
    :param reinfMatTag:      tag of the uniaxial material that makes up the reinforcing steel fibers
    '''
    st= preprocessor.getSets.getSet(setName)
    elements= st.getElements
    for e in elements:
        scc= e.getSection()
        sets= fiberSectionSetupRCSets(scc,concrMatTag,"concrete",reinfMatTag,"reinforcement")
        scc.defProp("fiberSets",sets)

def reselTensionFibers(scc,fiberSetName,tensionFibersSetName):
    '''Returns the fibers under tension included in a set of fibers of a fiber section type

    :param scc:          name identifying the fiber section
    :param fiberSetName:     name identifying the set of fibers 
    '''
    fiberSet= scc.getFiberSets()[fiberSetName]
    tensionFibers= scc.getFiberSets().create(tensionFibersSetName)
    for f in fiberSet:
        if(f.getMaterial().getStress()>0.0):
            tensionFibers.insert(f)
    return tensionFibers

def redefTensStiffConcr(setOfTenStffConcrFibSect,ft,Ets):
    '''Redefine the tension stiffening parameters of the concrete fibers in 
    set passed as parameter.

    :param fibSect: fiber section
    :param setOfTenStffConcrFibSect: set of fibers made of a concrete material with 
           tension stiffening  (constitutive model type concrete02) whose tension 
           properties we want to redefine.
    :param ft:  new value of the tensile strength (see concrete02 constitutive model)
    :param Ets: new value of the softening stiffness (see concrete02 constitutive model)
    '''
    fibers=setOfTenStffConcrFibSect.fSet
    for f in fibers:
        m=f.getMaterial()
        m.ft=ft
        m.Ets=Ets
    return

def fiberSectionSetupRC3Sets(scc,concrMatTag, concrSetName,reinfMatTag, reinfSetName):
    '''returns a set of tensioned fibers `tensionedReinforcement` of a fiber 
    section of reinforced concrete.

    :param scc:         name identifying the fiber section
    :param concrMatTag: tag of the uniaxial material that makes up the concrete
           fibers
    :param concrSetName: name of the set of fibers of concrete to be generated
    :param reinfMatTag: tag of the uniaxial material that makes up the 
           reinforcing steel fibers
    :param reinfSetName: name of the set of fibers of reinforcing steel to be 
           generated
    '''
    sets= fiberSectionSetupRCSets(scc,concrMatTag,concrSetName,reinfMatTag,reinfSetName)
    sets.reselTensionFibers(scc,"tensionedReinforcement")
    return sets
  
def getIMaxPropFiber(fibers,methodName):
    '''returns the fiber from a set of fibers where the maximum value of a 
    certain property is reached
    Parameters:
      fibers:     set of fibers
      methodName: name of the method that returns the fiber property searched
    '''
    retval= 0
    sz= len(fibers)
    f= fibers[0]
    methodToCall= getattr(f, methodName)
    maxValue= methodToCall()
    for i in range(1,sz):
        f= fibers[i]
        methodToCall= getattr(f, methodName)
        value= methodToCall()
        if(value>maxValue):
            maxValue= value
            retval= i
    return retval

def getIMinPropFiber(fibers,methodName):
    '''returns the fiber from a set of fibers where the minimum value of a 
    certain property is reached
    Parameters:
      fibers:     set of fibers
      methodName: name of the method that returns the fiber property searched
    '''
    retval= 0
    sz= len(fibers)
    f= fibers[0]
    methodToCall= getattr(f, methodName)
    minValue= methodToCall()
    for i in range(1,sz):
        f= fibers[i]
        methodToCall= getattr(f, methodName)
        value= methodToCall()
        if(value>minValue):
            minValue= value
            retval= i
    return retval


