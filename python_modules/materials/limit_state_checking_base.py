# -*- coding: utf-8 -*-
''' Base classes for limit state control.'''

__author__= "Luis C. Pérez Tato (LCPT) & Ana Ortega (AO_O)"
__copyright__= "Copyright 2017,LCPT AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
from miscUtils import LogMessages as lmsg
from materials.sections.fiber_section import fiber_sets
from solution import predefined_solutions
from postprocess import control_vars as cv

class LimitStateControllerBase(object):
    '''
    Basic parameters for limit state control (normal stresses, shear, crack,...)    .'''
    tensionedRebarsFiberSetName= "tensionedReinforcement" #Name of the tensiones rebar fibers set.

    def __init__(self,limitStateLabel, fakeSection= True):
        '''
        :param limitStateLabel: property name in the check results file 
               (something like 'ULS_shear' or 'SLS_crack_freq' or ...).
        :param fakeSection: true if a fiber section model of the section is 
               not needed to perform control. In that case a fake section 
               of type 'xc.ElasticShearSection3d' is generated for each 
               element of the phantom model. Otherwise, when fakeSection 
               is set to False (shear and cracking LS checking), a true fiber 
               section of type 'xc.FiberSectionShear3d' is generated. 
        :param preprocessor: only used to perform the crack straight control.
        '''
        self.limitStateLabel= limitStateLabel
        self.fakeSection= fakeSection
        #Linear analysis by default.
        self.analysisToPerform= predefined_solutions.simple_static_linear
        self.preprocessor=None   


    def check(self,elements,nmbComb):
        '''Crack control.'''
        lmsg.error('limit state check not implemented.')



class fibSectLSProperties(object):
    '''Class that sets up basic properties associatted to a fiber section
    in order to be used in the checking of different limit states.
    '''
    def __init__(self,sct):
        self.sct=sct
        self.sctProp=sct.getProp("datosSecc")
        self.concrTagK=self.sctProp.concrType.matTagK
        self.rsteelTagK=self.sctProp.reinfSteelType.matTagK
        self.concrName=str(self.sctProp.concrType)
        self.rsteelName=str(self.sctProp.reinfSteelType)
        self.cover=0 #init  concrete cover
        self.fiEqu=0 #init equivalent diameter
        
    def setupSets(self):
        self.setsRC= fiber_sets.fiberSectionSetupRCSets(scc=self.sct,concrMatTag=self.concrTagK,concrSetName="concrSetFb",reinfMatTag=self.rsteelTagK,reinfSetName="reinfSetFb")
        self.tensSetFb=self.setsRC.reselTensionFibers(scc=self.sct,tensionFibersSetName='tensSetFb')
        
    def setupStrghCrackDist(self):
        '''Set some parameters that are going to be used to calculate the 
        tension stiffening properties of concrete and the cracking distance. 
        '''
        self.setupSets()
        self.x=self.sct.getNeutralAxisDepth()
        self.d=self.sct.getEffectiveDepth()
        self.h=self.sct.getLeverArm()
        self.As=self.setsRC.tensionFibers.getArea(1.0)
        self.eps1=self.setsRC.concrFibers.fSet.getStrainMax()
        self.eps2= max(self.setsRC.concrFibers.fSet.getStrainMin(),0.0)
        self.sct.computeSpacement('tensSetFb')
        self.spacing= self.tensSetFb.getAverageDistanceBetweenFibers()
        nmbFi=self.tensSetFb.getNumFibers()
        '''
        print 'x= ',self.x
        print 'd= ',self.d
        print 'h= ',self.h
        print 'As= ',self.As
        print 'eps1= ', self.eps1
        print 'eps2= ',self.eps2
        '''
        self.sct.computeSpacement('tensSetFb')
        self.spacing= self.tensSetFb.getAverageDistanceBetweenFibers()
        nmbFi=self.tensSetFb.getNumFibers()
        '''
        print 'nmbFI= ', nmbFi
        '''
        if nmbFi>0:
            self.fiEqu=math.sqrt(4/math.pi*self.As/nmbFi)
        if len(self.tensSetFb) > 0:
            self.cover=self.calcCover(self.tensSetFb,'tensSetFb')

    def calcCover(self,setSteelFibers,setSteelFibersName):
        '''Return the mean concrete cover in the set of reinforcing steel 
        fibers setSteelFibers
        '''
        self.sct.computeCovers(setSteelFibersName)
        covers = [setSteelFibers.getFiberCover(i) for i in range(len(setSteelFibers))]
        return sum(covers) / float(len(covers))
        
class TensionedRebarsBasicProperties(object):
    '''Basic properties of tensioned rebars (used in shear checking).

    '''

    def __init__(self):
        self.number= 0 #Number of tensioned bars.
        self.area= 0.0 #Tensioned rebars area.

    
class TensionedRebarsProperties(TensionedRebarsBasicProperties):
    '''Properties of tensioned rebars that are useful for crack control 
     analysis.
    '''

    def __init__(self):
        super(TensionedRebarsProperties,self).__init__()
        self.averageStress= 0.0 #Tensioned rebars average stress.
        self.iAreaMaxima= None #Tensioned rebars maximum area.
        self.maxDiameter= 0.0 #Tensioned rebars maximum diameter.
        self.E= 0.0 #Tensioned rebars young modulus.
        self.spacing= None #Tensioned rebars spacing.
        self.yCentroid= None
        self.zCentroid= None
        self.cover= None #Cover of tensioned rebars.
        self.effectiveArea= None #Area of concrete of the cover zone, (see figure 49.2.4.b of EHE-08)
                                 # in which the tension bars effectively influence the crack opening.
    def setup(self,tensionedReinforcement):
        '''Get the parameter values from the fiber set.

         :param tensionedReinforcement: fibers that represent tensioned rebars.
        '''
        self.spacing= tensionedReinforcement.getAverageDistanceBetweenFibers()
        self.area= tensionedReinforcement.getArea(1)
        self.yCentroid= tensionedReinforcement.getCenterOfMassY()
        self.zCentroid= tensionedReinforcement.getCenterOfMassZ()
        self.averageStress= tensionedReinforcement.getStressMed()
        self.iAreaMaxima= fiber_sets.getIMaxPropFiber(tensionedReinforcement,"getArea")
        self.maxDiameter= 2*math.sqrt(tensionedReinforcement[self.iAreaMaxima].getArea()/math.pi) 
        self.E= tensionedReinforcement[0].getMaterial().getInitialTangent()

 
    def printParams(self):
        print "Number of tensioned bars: ",self.number,"\n"
        print "Spacement of tensioned bars; s= ",self.spacing," m\n"
        print "Area of tensioned bars; As= ",self.area*1e4," cm2\n"
        print "Centroid of tensioned bars; COG= (",self.yCentroid,",",self.zCentroid,") m\n"
        print "Tensioned rebars average stress: ",self.averageStress/1e6," MPa\n"
        print "Effective area; AcEf= ",self.effectiveArea*1e4," cm2\n"

class CrackControlBaseParameters(LimitStateControllerBase):
    '''
    Basic parameters for crack control.'''
    def __init__(self,limitStateLabel):
        super(CrackControlBaseParameters,self).__init__(limitStateLabel)
        self.concreteFibersSetName= "concrete" #Name of the concrete fibers set.
        self.rebarFibersSetName= "reinforcement" #Name of the rebar fibers set.
        self.tensionedRebars= TensionedRebarsProperties()
        self.claseEsfuerzo= "" #Clase de esfuerzo al que está sometida la sección.
        self.rcSets= None

    def printParams(self):
        # Prints the section crack control parameters.
        print "Clase esfuerzo: ",self.claseEsfuerzo,"\n"
        self.tensionedRebars.printParams()

        
class ShearControllerBase(LimitStateControllerBase):
    '''Base class for shear controller classes.'''
    def initControlVars(self,elements):
        ''' Initialize control variables over elements.

          :param elements: elements to define control variables in
        '''
        for e in elements:
          e.setProp(self.limitStateLabel,cv.RCShearControlVars())
