# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to Eurocode 3. '''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O) Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@gmail.com"

import sys
import enum
import math
from misc_utils import log_messages as lmsg
from materials import steel_member_base
from materials import limit_state_checking_base as lsc
from postprocess import control_vars as cv
from postprocess import limit_state_data as lsd
import xc

class SectionClassif(enum.IntEnum):
    '''Classification of sections for local buckling.'''
    compact= 0
    noncompact= 1
    slender= 2
    too_slender= 3

class MemberModel(object):
    ''' Representation of an AISC steel member (beam, column,...) in
        the XC model.

    :ivar name: object name.
    :ivar memberType: type of the member (beam, column, diagonal,...)
    :ivar lineList: list of lines (xc.Line) objects that represent
                    the member in the finite element model.
    '''
    def __init__(self,name, memberType, lineList, overrideUnbracedLength= None):
        ''' Constructor.

        :param name: object name.
        :param memberType: type of the member (beam, column, diagonal,...)
        :param lineList: list of lines (xc.Line) objects that represent
                        the member in the finite element model.
        :param overrideUnbracedLength: length to replace the true length of the
                                       member.
        '''
        self.name= name
        self.memberType= memberType
        self.lineList= lineList
        self.overrideUnbracedLength= overrideUnbracedLength

    def getMemberLength(self):
        ''' Return the length of the member.'''
        retval= 0.0
        if(self.overrideUnbracedLength):
            retval= overrideUnbracedLength
        else:
            for l in self.lineList:
                retval+= l.getLength()
        return retval

    def getCrossSection(self):
        ''' Return the member cross section.'''
        retval= None
        element= self.lineList[0].getElements[0]
        if(element.hasProp('crossSection')):
            retval= element.getProp('crossSection')
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+': you need to define "crossSection" property for the elements. Returning None')
        return retval

    def createMember(self):
        ''' Create the AISC member object corresponding to this member model.
        '''
        return Member(name= self.name, section= self.getCrossSection(),unbracedLengthX= self.getMemberLength(), lstLines= self.lineList)


class MemberModels(list):
    ''' List of MemberModel objects with some convenience methods.'''

    def createMembers(self, aiscMemberSet: xc.Set):
        ''' Create the AISC member objects corresponding to the objects 
        in the collection.

        :param aiscMemberSet: set to populate with the finite elements that
                              belong to the AISC members for later code
                              checking.
        '''
        self.members= list()
        self.beamMembers= list()
        self.columnMembers= list()
        # Create AISC member objects.
        for mm in self: # for each member model.
            memberLength= mm.getMemberLength()
            member= mm.createMember()
            if(mm.memberType=='beam'):
                self.beamMembers.append(member)
            elif(mm.memberType=='column'):
                self.columnMembers.append(member)
            self.members.append(member)
        # Install recorder and populate AISC member set.
        for am in self.members:
            am.installULSControlRecorder(recorderType="element_prop_recorder")
            aiscMemberSet.extend(am.elemSet)
            
    def populateColumnSet(self, columnSet):
        ''' Populate the argument set with the finite elements belonging to
            the columns.

        :param columnSet: set to put the finite elements in.
        '''
        for mm in self:
            if(mm.memberType=='column'):
                for l in mm.lineList:
                    columnSet.lines.append(l)
        columnSet.fillDownwards()



# Unbraced segment ascii art:
#
#         A      B      C
#  x------+------+------+------x 
#  ^                           ^
#  0      1      2      3      4
#
# x: brace for lateral buckling
# +: control point.

class LateralTorsionalBucklingModificationFactor(object):
    ''' Calculation of the Cb lateral-torsional buckling modification factor
    as defined in section F1 (c) of ANSI AISC 360-16.'''
    def __init__(self,Mi):
        ''' Constructor.

        :param Mi: bending moment list as follows:
                   *Mi[0]: Value of moment at the back end of the unbraced segment.
                   *Mi[1]: Value of moment at quarter point of the unbraced segment (MA in equation F1-1).
                   *Mi[2]: Value of moment at centerline of the unbraced segment (MB in equation F1-1).
                   *Mi[3]: Value of moment at three-quarter point of the unbraced segment (MC in equation F1-1).
                   *Mi[4]: Value of moment at the front end of the unbraced segment.
        '''
        self.Mi= list()
        for m in Mi:
            self.Mi.append(abs(m)) # absolute value of moments.

    def getLateralTorsionalBucklingModificationFactor(self):
        ''' Return the lateral-torsional buckling modification factor
            according to equation F1-1 of ANSI AISC 360-16.'''
        retval= 1.0 # Conservative value.
        mMax= max(self.Mi)
        if(mMax>0.0):
            denom= 2.5*mMax+3.0*self.Mi[1]+4.0*self.Mi[2]+3.0*self.Mi[3]
            retval= 12.5*mMax/denom
        return retval

class Member(steel_member_base.BucklingMember):
    ''' Beam and column members according to ANSI AISC 360-16.

    :ivar unbracedLengthX: unbraced length for torsional buckling 
                           about the longitudinal axis.
    :ivar unbracedLengthY: unbraced length for flexural buckling 
                           about y-axis.
    :ivar unbracedLengthZ: unbraced length for flexural buckling 
                           about z-axis.
    :ivar Kx: effective length factor for torsional buckling 
              about the longitudinal axis.
    :ivar Ky: effective length factor for flexural buckling 
              about y-axis.
    :ivar Kz: effective length factor for flexural buckling 
              about z-axis.
    '''
    def __init__(self, name, section, unbracedLengthX, unbracedLengthY= None, unbracedLengthZ= None, kx= 1.0, ky= 1.0, kz= 1.0, Cb= None, lstLines=None, lstPoints=None):
        ''' Constructor. 

        :param name: object name.
        :param ec3Shape: cross-section shape (e.g. IPNShape, IPEShape, ...)
        :param Cb: lateral-torsional buckling modification factor
                   (defaults to None).
        :param lstLines: ordered list of lines that make up the beam. 
                         Ignored if Cb is given (defaults to None).
        :param lstPoints: ordered list of points that make up the beam. 
                          Ignored if Cb or lstLines is given (defaults to None)
        '''
        super(Member,self).__init__(name, section, lstLines, lstPoints)
        self.unbracedLengthX= unbracedLengthX
        if(unbracedLengthY):
            self.unbracedLengthY= unbracedLengthY
        else:
            self.unbracedLengthY= unbracedLengthX
        if(unbracedLengthZ):
            self.unbracedLengthZ= unbracedLengthZ
        else:
            self.unbracedLengthZ= unbracedLengthX            
        self.Kx= kx
        self.Ky= ky
        self.Kz= kz
        self.Cb= Cb
        if(self.Cb):
            if(lstLines or lstPoints):
                lmsg.error('lateral-torsional buckling modification factor was specified, computation from finite element model will not be performed.')

    def getEffectiveLengthX(self):
        ''' Return the effective length of member for torsional buckling 
            about longitudinal axis: L_cx.'''
        return self.Kx*self.unbracedLengthX
    
    def getEffectiveLengthY(self):
        ''' Return the effective length of member for buckling 
            about y axis: L_cy.'''
        return self.Ky*self.unbracedLengthY

    def getEffectiveLengthZ(self):
        ''' Return the effective length of member for buckling 
            about y axis: L_cy.'''
        return self.Kz*self.unbracedLengthZ
    
    def getNominalCompressiveStrength(self):
        ''' Return the nominal compressive strength of the member
            according to equation E3-1 of AISC-360-16.
        '''
        return self.shape.getNominalCompressiveStrength(effectiveLengthX= self.getEffectiveLengthX(), effectiveLengthY= self.getEffectiveLengthY(), effectiveLengthZ= self.getEffectiveLengthZ())

    def getDesignCompressiveStrength(self):
        ''' Return the design compressive strength of the member
            according to section E1 of AISC-360-16.
        '''
        return self.shape.getDesignCompressiveStrength(effectiveLengthX= self.getEffectiveLengthX(), effectiveLengthY= self.getEffectiveLengthY(), effectiveLengthZ= self.getEffectiveLengthZ())

    def getFlexuralStrengthReductionFactor(self):
        ''' Return the reduction factor of the flexural strength 
            of the member with respect to the reference flexural strength
            of its section.
        '''
        return self.getDesignFlexuralStrength()/self.shape.getReferenceFlexuralStrength()
    
    def getCompressiveStrengthReductionFactor(self):
        ''' Return the reduction factor of the compressive strength 
            of the member with respect to the plastic axial load of
            its section.
        '''
        return self.getDesignCompressiveStrength()/self.shape.getReferenceCompressiveStrength()

    def getLateralTorsionalBucklingModificationFactor(self):
        ''' Return the lateral-torsional buckling modification factor
            according to equation F1-1 of ANSI AISC 360-16.'''
        Mi= self.getBendingMomentsAtControlPoints()
        mf= LateralTorsionalBucklingModificationFactor(Mi)
        retval= mf.getLateralTorsionalBucklingModificationFactor()
        return retval

    def getNominalFlexuralStrength(self, majorAxis= True):
        ''' Return the nominal compressive strength of the member
            according to chapter F of AISC-360-16.
        '''
        lateralUnbracedLength= self.getEffectiveLengthX()
        Cb= 1.0 # conservative value
        if(self.Cb):
            Cb= self.Cb
        else:
            if(self.lstLines or self.lstPoints):
                Cb= self.getLateralTorsionalBucklingModificationFactor()
            else:
                lmsg.error('Can\'t compute lateral-torsional buckling modification factor, taken as: '+str(Cb))
        return self.shape.getNominalFlexuralStrength(lateralUnbracedLength, Cb, majorAxis)

    def getDesignFlexuralStrength(self, majorAxis= True):
        ''' Return the design flexural strength of the member
            according to section F of AISC-360-16.
        '''
        nominalValue= self.getNominalFlexuralStrength(majorAxis)
        if(isinstance(nominalValue, tuple)):
            retval_MWpositive= 0.9*nominalValue[0]
            retval_MWnegative= 0.9*nominalValue[1]
            retval= (retval_MWpositive, retval_MWnegative)
        else:
            retval= 0.9*nominalValue
        return retval
        
    def getFlexuralStrengthReductionFactor(self):
        ''' Return the reduction factor of the flexural strength 
            of the member with respect to the reference flexural strength 
            of its section.
        '''
        num= self.getDesignFlexuralStrength()
        denom= self.shape.getReferenceFlexuralStrength()
        if(isinstance(num, tuple)):
            retval_MWpositive= num[0]/denom[0]
            retval_MWnegative= num[1]/denom[1]
            retval= min(retval_MWpositive, retval_MWnegative)
        else:
            retval= num/denom
        return retval

    def getDesignShearStrength(self, majorAxis= True):
        ''' Return the design shear strength of the member
            according to section G of AISC-360-16.
        '''
        return self.shape.getDesignShearStrength(majorAxis= majorAxis)
    
    def getBiaxialBendingEfficiency(self,Nd,Myd,Mzd):
        '''Return biaxial bending efficiency according to section H1
           of AISC-360-16.

           :param Nd: axial design load (required axial strength).
           :param Myd: bending moment about weak axis (required flexural strength).
           :param Mzd: bending moment about strong axis (required flexural strength).
        '''
        # Compute axial load reduction factor.
        lrfN= self.getCompressiveStrengthReductionFactor()

        # Compute lateral buckling reduction factor.
        lrfLT= self.getFlexuralStrengthReductionFactor()

        return self.shape.getBiaxialBendingEfficiency(Nd= Nd, Myd= Myd, Mzd= Mzd, Vyd= 0.0, chiN= lrfN, chiLT= lrfLT)
    
    def updateReductionFactors(self):
        '''Update the value of the appropriate reduction factors.'''
        chiN= self.getCompressiveStrengthReductionFactor()
        chiLT= self.getFlexuralStrengthReductionFactor()
        for e in self.elemSet:
             e.setProp('chiLT',chiLT) # flexural strength reduction factor.
             e.setProp('chiN',chiN) # compressive strength reduction factor.

    def installULSControlRecorder(self,recorderType, chiN: float= 1.0, chiLT: float= 1.0):
        '''Install recorder for verification of ULS criterion.

        :param recorderType: type of the recorder to install.
        :param chiN: compressive strength reduction factor.
        :param chiLT: flexural strength reduction factor.
        '''
        prep= self.getPreprocessor()
        nodes= prep.getNodeHandler
        domain= prep.getDomain
        recorder= domain.newRecorder(recorderType,None)
        if(not self.elemSet):
            self.createElementSet()
        eleTags= list()
        for e in self.elemSet:
            eleTags.append(e.tag)
            e.setProp('ULSControlRecorder',recorder)
        idEleTags= xc.ID(eleTags)
        recorder.setElements(idEleTags)
        sectionClassif= 1 # dummy argument usef for compatibility
        self.shape.setupULSControlVars(self.elemSet, sectionClassif, chiN= chiN, chiLT= chiLT)
        if(nodes.numDOFs==3):
            recorder.callbackRecord= controlULSCriterion2D()
        else:
            recorder.callbackRecord= controlULSCriterion()
#        recorder.callbackRestart= "print(\"Restart method called.\")" #20181121
        return recorder
    
class AISCBiaxialBendingControlVars(cv.SteelShapeBiaxialBendingControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification in steel-shape elements according to AISC.

    :ivar chiN:    reduction factor for compressive strength (defaults to 1)
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0,Ncrd=0.0,McRdy=0.0,McRdz=0.0,MvRdz=0.0,MbRdz=0.0, chiLT=1.0, chiN= 1.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Ncrd:     design strength to axial compression
        :param McRdy:    design moment strength about Y (weak) axis
        :param McRdz:    design moment strength about Z (strong) axis
        :param MvRdz:    reduced design moment strength about Z (strong) axis for shear interaction
        :param MbRdz:    reduced design moment strength about Z (strong) axis for lateral-torsional bucking
        :param chiLT:    reduction factor for lateral-torsional buckling (defaults to 1)
        :param chiN:     reduction factor for compressive strength (defaults to 1)
        '''
        super(AISCBiaxialBendingControlVars,self).__init__(idSection,combName,CF,N,My,Mz,Ncrd=Ncrd,McRdy=McRdy,McRdz=McRdz,MvRdz=MvRdz,MbRdz=MbRdz, chiLT=chiLT)
        self.chiN= chiN # reduction factor for compressive strength
        
    
class BiaxialBendingNormalStressController(lsc.LimitStateControllerBase2Sections):
    '''Object that controls normal stresses limit state.'''

    ControlVars= AISCBiaxialBendingControlVars
    def __init__(self,limitStateLabel):
        super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel)

    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the element steel shape
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose section will be checked.
        :param elementInternalForces: internal forces acting on the steel shape.
        '''
        # Get element section properties.
        steelShape= elem.getProp('crossSection') # steel shape to check.
        for sectionIForces in elementInternalForces: # Check each element section.
            # Compute efficiency.
            CFtmp,NcRdtmp,McRdytmp,McRdztmp,MvRdztmp,MbRdztmp= steelShape.getBiaxialBendingEfficiency(Nd= sectionIForces.N,Myd= sectionIForces.My,Mzd= sectionIForces.Mz,Vyd= sectionIForces.Vy,chiN= sectionIForces.chiN, chiLT= sectionIForces.chiLT)
            sectionLabel= self.getSectionLabel(sectionIForces.idSection)
            label= self.limitStateLabel+sectionLabel
            # Update efficiency.
            if(CFtmp>elem.getProp(label).CF):
                elem.setProp(label,self.ControlVars(sectionLabel,sectionIForces.idComb,CFtmp,sectionIForces.N,sectionIForces.My,sectionIForces.Mz,NcRdtmp,McRdytmp,McRdztmp,MvRdztmp,MbRdztmp,sectionIForces.chiLT, sectionIForces.chiN))
        

class ShearController(lsc.LimitStateControllerBase2Sections):
    '''Object that controls shear limit state.'''

    ControlVars= cv.ShearYControlVars
    def __init__(self,limitStateLabel):
        super(ShearController,self).__init__(limitStateLabel)

    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the element steel shape
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose section will be checked.
        :param elementInternalForces: internal forces acting on the steel shape.
        '''
        # Get section properties.
        steelShape= elem.getProp('crossSection')
        sectionClass= elem.getProp('sectionClass')
        # Check each element section.
        for sectionIForces in elementInternalForces:
            # Compute efficiency.
            CFtmp= steelShape.getYShearEfficiency(sectionClass,sectionIForces.Vy)
            sectionLabel= self.getSectionLabel(sectionIForces.idSection)
            label= self.limitStateLabel+sectionLabel
            # Update efficiency.
            if(CFtmp>elem.getProp(label).CF):
                elem.setProp(label,self.ControlVars(sectionLabel+'s',sectionIForces.idComb,CFtmp,sectionIForces.Vy))
        
                
class VonMisesStressController(lsc.LimitStateControllerBase):
    '''Object that controls Von Mises stress limit state.'''

    ControlVars= cv.VonMisesControlVars
    def __init__(self,limitStateLabel, vonMisesStressId= 'max_von_mises_stress'):
        ''' Constructor.

        :param limitStateLabel: limit state identifier.
        :param vonMisesStressId: identifier of the Von Mises stress to read
                                (see NDMaterial and MembranePlateFiberSection).
        '''
        super(VonMisesStressController,self).__init__(limitStateLabel)

    def initControlVars(self,setCalc):
        '''Initialize control variables over elements.

        :param setCalc: set of elements to which define control variables
        '''
        for e in setCalc.elements:
            e.setProp(self.limitStateLabel,self.ControlVars())
            
    def readInternalForces(self,intForcCombFileName, setCalc= None):
        '''Launch checking.

        :param intForcCombFileName: Name of the file containing the internal 
                                    forces on the element sections.
        '''
        # Read internal forces results.
        intForcItems= lsd.readIntForcesFile(intForcCombFileName, setCalc, self.vonMisesStressId)
        return intForcItems[2]
            
    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the element material
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose section will be checked.
        :param elementInternalForces: internal forces acting on the steel shape.
        '''
        # Get factored yield stress.
        factoredYieldStress= 0.9*elem.getProp('yieldStress')
        # Check each element section.
        for lf in elementInternalForces:
            # Compute efficiency (both sections will have the
            # same Von Mises stress so this is redundant)
            CFtmp= lf.vonMisesStress/factoredYieldStress
            # Update efficiency.
            if(CFtmp>elem.getProp(self.limitStateLabel).CF):
                elem.setProp(self.limitStateLabel,self.ControlVars(lf.idComb,CFtmp,lf.vonMisesStress))


                        
def controlULSCriterion():
    return '''recorder= self.getProp('ULSControlRecorder')
nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkBiaxialBendingForElement(self,nmbComb)
crossSection.checkYShearForElement(self,nmbComb)
crossSection.checkZShearForElement(self,nmbComb)'''

def controlULSCriterion2D():
    return '''recorder= self.getProp('ULSControlRecorder')
nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkUniaxialBendingForElement(self,nmbComb)
crossSection.checkYShearForElement(self,nmbComb)'''

# Model softening

def backupStiffness(elementSet):
    ''' Stores the original value of the Young modulus in a property
        to retrieve it in subsequent calls to softenElements.

    :param elementSet: elements to process.
    '''
    for e in elementSet.elements:
        Ebackup= 0.0
        if(hasattr(e,"getMaterial")): # Trusses
            Ebackup= e.getMaterial().E
        else: # Beam elements.
            mat= e.getPhysicalProperties.getVectorMaterials[0]
            Ebackup= mat.sectionProperties.E
            if(mat.sectionProperties.dimension==2): # 2D section
                e.setProp('IzBackup', mat.sectionProperties.I)
            else:
                e.setProp('IyBackup', mat.sectionProperties.Iy)
                e.setProp('IzBackup', mat.sectionProperties.Iz)
        e.setProp('Ebackup',Ebackup)
        
def softenElements(elementSet):
    ''' Adjust the stiffnes of the elements according to 
        clause C2.3 of AISC 360-16.

    :param elementSet: elements to soften.
    '''
    for e in elementSet.elements:
        Ebackup= e.getProp('Ebackup')
        if(e.isAlive):
            Pr= e.getN()
            if(hasattr(e,"getMaterial")): # Trusses
                mat= e.getMaterial()
                mat.E= 0.8*Ebackup
            else: # Beam elements.
                mat= e.getPhysicalProperties.getVectorMaterials[0]
                # print('mat= ',mat.name)
                mat.sectionProperties.E= .8*Ebackup # Axial stiffness
                #print('properties: ', e.getPropNames())
                crossSection= e.getProp('crossSection')
                # C2.3 (b) clause (flexural stiffness): 
                Pns= crossSection.getReferenceCompressiveStrength()
                ratio= abs(Pr)/Pns
                tau= 1.0
                if(ratio>0.5):
                    lmsg.log('Pr= '+ str(Pr/1e3)+' kN, Pns= '+str(Pns/1e3)+' kN, ratio= '+str(ratio))
                    tau= 4*ratio*(1-ratio)
                    if(e.hasProp('IyBackup')):
                        mat.sectionProperties.Iy= tau*e.getProp('IyBackup')
                    mat.sectionProperties.Iz= tau*e.getProp('IzBackup')

def restoreStiffness(elementSet):
    ''' Restore the original Young modulus value in a property
        to retrieve it in subsequent calls to softenElements.

    :param elementSet: elements to process.
    '''
    for e in elementSet.elements:
        if(e.hasProp('Ebackup')):
            Ebackup= e.getProp('Ebackup')
            if(hasattr(e,"getMaterial")): # Trusses
                e.getMaterial().E= Ebackup
            else: # Beam elements.
                mat= e.getPhysicalProperties.getVectorMaterials[0]
                mat.sectionProperties.E= Ebackup
                IzBackup= e.getProp('IzBackup')
                if(mat.sectionProperties.dimension==2): # 2D section
                    mat.sectionProperties.I= IzBackup
                else:
                    mat.sectionProperties.Iz= IzBackup
                    IyBackup= e.getProp('IyBackup')
                    mat.sectionProperties.Iy= IyBackup
        else:
            funcName= sys._getframe(0).f_code.co_name
            lmsg.error(funcName+': Ebackup property not found. You need to call backupStiffness prior to analysis run.')

def getBeamColumnGroups(xcSet):
    ''' Return the lines of the set arguments grouped according to its
        'belongsTo' property (groups to wich the line belongs). If the
        line doesn't belong to any group it will be returned as a group
        by itself (with one item only).

    :param xcSet: set of the entities that will be classified according to
                  its group.
    '''
    retval= dict() # group name: key/group member: value
    for ln in xcSet.getLines:
        attributes= ln.getProp('attributes')
        groups= list()
        if('belongsTo' in attributes):
            groups= attributes['belongsTo']
        if(len(groups)>0): # line belongs to a group
            for grp in groups:
                key= str(grp) # group name.
                if(key in retval): # group already exists.
                    retval[key].append(ln)
                else:
                    retval[key]= [ln] # new group.
        else: # line stands by itself.
            retval[ln.name]= [ln]
    return retval
