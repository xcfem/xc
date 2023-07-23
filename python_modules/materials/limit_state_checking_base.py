# -*- coding: utf-8 -*-
''' Base classes for limit state control.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) & Ana Ortega (AO_O)"
__copyright__= "Copyright 2017,LCPT AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import sys
import math
from misc_utils import log_messages as lmsg
from materials.sections.fiber_section import fiber_sets
from solution import predefined_solutions
from postprocess import control_vars as cv
from postprocess import limit_state_data as lsd
import geom

class RebarController(object):
    '''Base class for rebar controllers (control of some parameters 
       as anchorage length minimum reinforcement and so on.

       :ivar concreteCover: the distance from center of a bar or wire to 
                            nearest concrete surface.
       :ivar spacing: center-to-center spacing of bars or wires being 
                      developed, in.
    '''    
    def __init__(self, concreteCover= 35e-3, spacing= 150e-3):
        '''Constructor.

        :param concreteCover: the distance from center of a bar or wire to 
                             nearest concrete surface.
        :param spacing: center-to-center spacing of bars or wires being 
                       developed, in.
        '''
        self.concreteCover= concreteCover
        self.spacing= spacing

class EURebarController(RebarController):
    '''Base class for Eurocode based rebar controllers.

    '''    
    def __init__(self, concreteCover, spacing, compression, alpha_1= 1.0, alpha_3= 1.0, alpha_4= 1.0, alpha_5= 1.0):
        '''Constructor.

        :param concreteCover: the distance from center of a bar or wire to 
                             nearest concrete surface.
        :param spacing: center-to-center spacing of bars or wires being 
                       developed, in.
        :param eta1: coefficient related to the quality of the bond condition 
                     and the position of the bar during concreting.
                     eta1= 1,0 when 'good' conditions are obtained and
                     eta1= 0,7 for all other cases.
        :param compression: true if reinforcement is compressed.
        :param alpha_1: effect of the form of the bars assuming adequate cover.
        :param alpha_3: effect of confinement by transverse reinforcement.
        :param alpha_4: influence of one or more welded transverse bars along 
                        the design anchorage length.
        :param alpha_5: effect of the pressure transverse to the plane of 
                        splitting along the design anchorage length.
        '''
        super(EURebarController,self).__init__(concreteCover= concreteCover, spacing= spacing)
        self.compression= compression # true if reinforcement is compressed.
        self.alpha_1= alpha_1 #effect of the form of the bars assuming adequate cover.
        self.alpha_3= alpha_3 # effect of confinement by transverse reinforcement.
        self.alpha_4= alpha_4 # influence of one or more welded transverse bars along the design anchorage length.
        self.alpha_5= alpha_5 # effect of the pressure transverse to the plane of splitting along the design anchorage length.

    def getConcreteMinimumCoverEffect(self, rebarDiameter, barShape= 'bent', lateralConcreteCover= None):
        ''' Return the value of the alpha_2 factor that introduces the effect
            of concrete minimum cover according to figure 8.3 and table 8.2
            of EC2:2004.

        :param rebarDiameter: nominal diameter of the bar.
        :param barShape: 'straight' or 'bent' or 'looped'.
        :param lateralConcreteCover: lateral concrete cover (c1 in figure 8.3
                                     of EC2:2004). If None make it equal to
                                     the regular concrete cover.
        '''
        retval= 1.0
        if(not self.compression): # bar in tension.
            if(lateralConcreteCover is None):
                lateralConcreteCover= self.concreteCover
            if(barShape=='straight'):
                cd= min(self.spacing/2.0, lateralConcreteCover, self.concreteCover)
                retval-= 0.15*(cd-rebarDiameter)/rebarDiameter
                retval= max(retval, 0.7)
            elif(barShape=='bent'):
                cd= min(self.spacing/2.0, lateralConcreteCover)
                retval-= 0.15*(cd-3*rebarDiameter)/rebarDiameter
                retval= max(retval, 0.7)
            elif(barShape=='looped'):
                cd= min(self.spacing/2.0, self.concreteCover)
                retval-= 0.15*(cd-3*rebarDiameter)/rebarDiameter
                retval= max(retval, 0.7)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_nameS
                lmsg.error(className+'.'+methodName+'; unknown bar shape: '+str(barShape)+'.')
        retval= min(retval, 1.0)
        return retval
        

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
        self.solutionProcedureType=  predefined_solutions.SimpleStaticLinear
        self.preprocessor=None
        self.verbose= True # display log messages by default
        
    def expectsTensionStiffeningModel(self):
        ''' Return true if a tension-stiffening concrete fiber model must be
            used for this limit state.'''
        return False
        
    def initControlVars(self,elements):
        ''' Initialize control variables over elements.

          :param elements: elements to define control variables in.
        '''
        for e in elements:
            e.setProp(self.limitStateLabel, self.ControlVars())

    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the element material
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose section will be checked.
        :param elementInternalForces: internal forces acting on the steel shape.
        '''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+': not implemented yet.')

    def readInternalForces(self, intForcCombFileName, setCalc=None):
        '''Launch checking.

        :param intForcCombFileName: Name of the file containing the internal 
                                    forces on the element sections.
        '''
        # Read internal forces results.
        intForcItems= lsd.readIntForcesFile(intForcCombFileName,setCalc)
        return intForcItems[2]

    def updateEfficiencyForSet(self,intForcCombFileName, setCalc=None):
        '''Update the efficiency value (and the corresponding control vars)
           for the elements in the argument set, under the internal forces
           stored in the file argument.

        :param intForcCombFileName: Name of the file containing the internal 
                                    forces on the element sections.
        :param setCalc: set of elements to check
        '''
        # Read internal forces results.
        internalForcesValues= self.readInternalForces(intForcCombFileName, setCalc)
        # Check elements on setCalc.
        for e in setCalc.elements:
            # Get element internal forces.
            elIntForc= internalForcesValues[e.tag]
            if(len(elIntForc)==0):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+': no internal forces for element: '+str(e.tag)+' of type: '+e.type())
            # Update efficiency.
            self.updateEfficiency(e, elIntForc)

    def check(self,elements,nmbComb):
        '''Limit state control.'''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+': limit state check not implemented.')

class LimitStateControllerBase2Sections(LimitStateControllerBase):
    '''
    Limit state controller 2 sections for each element..
    '''
    def __init__(self,limitStateLabel, fakeSection= True, elementSections= ['Sect1', 'Sect2']):
        '''
        :param limitStateLabel: property name in the check results file 
               (something like 'ULS_shear' or 'SLS_crack_freq' or ...).
        :param fakeSection: true if a fiber section model of the section is 
               not needed to perform control. In that case a fake section 
               of type 'xc.ElasticShearSection3d' is generated for each 
               element of the phantom model. Otherwise, when fakeSection 
               is set to False (shear and cracking LS checking), a true fiber 
               section of type 'xc.FiberSectionShear3d' is generated. 
        '''
        super(LimitStateControllerBase2Sections,self).__init__(limitStateLabel, fakeSection)
        self.elementSections= elementSections

    def getSectionLabel(self, idSection):
        ''' Return the label that corresponds to the index argument.'''
        return self.elementSections[idSection]
    
    def initControlVars(self,setCalc):
        '''Initialize control variables over elements.

        :param setCalc: set of elements to which define control variables.
        :param elementSections: element sections to initialize.
        '''
        for e in setCalc.elements:
            for s in self.elementSections:
                e.setProp(self.limitStateLabel+s,self.ControlVars(idSection= s))


class fibSectLSProperties(object):
    '''Class that sets up basic properties associatted to a fiber section
    in order to be used in the checking of different limit states.
    '''
    def __init__(self,sct):
        self.sct=sct
        self.sctProp=sct.getProp('sectionData')
        self.concrTagK= self.sctProp.fiberSectionParameters.concrType.matTagK
        self.rsteelTagK= self.sctProp.fiberSectionParameters.reinfSteelType.matTagK
        self.concrName= str(self.sctProp.fiberSectionParameters.concrType)
        self.rsteelName= str(self.sctProp.fiberSectionParameters.reinfSteelType)
        self.cover=0 #init concrete cover
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
        '''
        print('x= ',self.x)
        print('d= ',self.d)
        print('h= ',self.h)
        print('As= ',self.As)
        print('eps1= ', self.eps1)
        print('eps2= ',self.eps2)
        '''
        self.sct.computeSpacement('tensSetFb')
        self.spacing= self.tensSetFb.getAverageDistanceBetweenFibers()
        nmbFi=self.tensSetFb.getNumFibers()
        '''
        print('nmbFI= ', nmbFi)
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
        #self.effectiveArea= None #Area of concrete of the cover zone, (see figure 49.2.4.b of EHE-08)
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
        print("Number of tensioned bars: ",self.number,"")
        print("Spacement of tensioned bars; s= ",self.spacing," m")
        print("Area of tensioned bars; As= ",self.area*1e4," cm2")
        print("Centroid of tensioned bars; COG= (",self.yCentroid,",",self.zCentroid,") m")
        print("Tensioned rebars average stress: ",self.averageStress/1e6," MPa")
        #print("Effective area; AcEf= ",self.effectiveArea*1e4," cm2")

class CrackControlBaseParameters(LimitStateControllerBase):
    '''
    Basic parameters for crack control.'''
    def __init__(self,limitStateLabel):
        ''' Constructor.

        :param limitStateLabel: label that identifies the limit state.
        '''        
        super(CrackControlBaseParameters,self).__init__(limitStateLabel)
        self.concreteFibersSetName= "concrete" #Name of the concrete fibers set.
        self.rebarFibersSetName= "reinforcement" #Name of the rebar fibers set.
        self.tensionedRebars= TensionedRebarsProperties()
        self.claseEsfuerzo= "" #Clase de esfuerzo al que está sometida la sección.
        self.rcSets= None

    def printParams(self):
        # Prints the section crack control parameters.
        print("Clase esfuerzo: ",self.claseEsfuerzo)
        self.tensionedRebars.printParams()

class BiaxialBendingNormalStressControllerBase(LimitStateControllerBase):
    '''Base class for object that controls normal stresses
       limit state.
    '''
    ControlVars= cv.BiaxialBendingControlVars
    def __init__(self,limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''
        super(BiaxialBendingNormalStressControllerBase,self).__init__(limitStateLabel)

    def check(self, elements, nmbComb):
        '''Launch checking.

          :param elements: elements to check.
          :param nmbComb: load case name.
        '''
        if(self.verbose):
            lmsg.log("Postprocessing combination: "+nmbComb)
        for e in elements:
            e.getResistingForce()
            scc= e.getSection() # Element section in the phantom model.
            idSection= e.getProp("idSection") # Element section in the real model.
            Ntmp= scc.getStressResultantComponent("N")
            MyTmp= scc.getStressResultantComponent("My")
            MzTmp= scc.getStressResultantComponent("Mz")
            posEsf= geom.Pos3d(Ntmp,MyTmp,MzTmp)
            diagInt= e.getProp("diagInt")
            CFtmp= diagInt.getCapacityFactor(posEsf)
            if(CFtmp>e.getProp(self.limitStateLabel).CF):
                e.setProp(self.limitStateLabel, self.ControlVars(idSection,nmbComb,CFtmp,Ntmp,MyTmp,MzTmp)) # Worst case.


class UniaxialBendingNormalStressControllerBase(LimitStateControllerBase):
    '''Base class for object that controls normal stresses 
       limit state (uniaxial bending).
    '''
    ControlVars= cv.UniaxialBendingControlVars
    def __init__(self,limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''
        super(UniaxialBendingNormalStressControllerBase,self).__init__(limitStateLabel)

    def check(self,elements, combName):
        '''
        Check the normal stresses

        :param  elements: elements to check
        :param  combName: name of the load combination being treated.
        '''
        if(self.verbose):
            lmsg.log("Postprocessing combination: "+combName)
        if(len(elements)>0):
            # Check dimension of the interaction diagrams.
            firstElement= elements[0]
            diagInt= firstElement.getProp("diagInt")
            dimension= diagInt.getDimension()
            if(dimension!= 2):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; a two-dimensional interaction diagram was expected, got a: '+str(dimension)+'-dimensional one instead.')
                exit(1)
            # Compute efficiencies.
            for e in elements:
                e.getResistingForce()
                scc= e.getSection() # Element section in the phantom model.
                idSection= e.getProp("idSection") # Element section in the "real" model.
                Ntmp= scc.getStressResultantComponent("N")
                MzTmp= scc.getStressResultantComponent("Mz")
                posEsf= geom.Pos2d(Ntmp,MzTmp)
                diagInt= e.getProp("diagInt")
                CFtmp= diagInt.getCapacityFactor(posEsf)
                if(CFtmp>e.getProp(self.limitStateLabel).CF):
                    e.setProp(self.limitStateLabel, self.ControlVars(idSection,combName,CFtmp,Ntmp,MzTmp)) # Worst case.

class ShearControllerBase(LimitStateControllerBase):
    '''Base class for shear controller classes.'''

    ControlVars= cv.RCShearControlVars
          
    def check(self, elements, combName):
        '''Shear control.'''
        lmsg.error('shear limit state check not implemented.')
        
    def extractFiberData(self, scc, concrete, reinfSteel):
        ''' Extract basic parameters from the fiber model of the section

         :param scc: fiber model of the section.
         :param concrete: parameters to modelize concrete.
         :param reinfSteel: parameters to modelize reinforcement steel.
        '''
        self.concreteMatTag= concrete.matTagD
        self.fckH= abs(concrete.fck)
        self.fcdH= abs(concrete.fcd())
        self.fctdH= concrete.fctd()
        self.gammaC= concrete.gmmC
        self.reinfSteelMaterialTag= reinfSteel.matTagD
        self.fydS= reinfSteel.fyd()
        if(not scc.hasProp("rcSets")):
            scc.setProp("rcSets", fiber_sets.fiberSectionSetupRC3Sets(scc,self.concreteMatTag,self.concreteFibersSetName,self.reinfSteelMaterialTag,self.rebarFibersSetName))
        return scc.getProp("rcSets")

    def getShearForce(self, Vy, Vz, elementDimension):
        ''' Return the shear internal force to use when checking shear 
            strenght.

        :param Vy: shear force on "Y" axis.
        :param Vz: shear force on "Z" axis.
        :param elementDimension: dimension of the element (1, 2 or 3).
        '''
        retval= None
        if((elementDimension==0) or (elementDimension==1)):
            # 0D elements (pure sections).
            # 1D elements (beam-column, ...).
            retval= math.sqrt((Vy)**2+(Vz)**2)
        elif(elementDimension==2):
            # 2D elements (shell,...)
            retval= math.fabs(Vy)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for elements of dimension: '+str(elementDimension))
            exit(1)
        return retval
          
