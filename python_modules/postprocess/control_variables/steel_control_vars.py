# -*- coding: utf-8 -*-
'''Control vars to store variables that control limit state checking of steel
   structures.
'''

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2026,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

from postprocess.control_variables import basic_control_variables as bcv


class UniaxialBendingStrengthControlVars(bcv.UniaxialBendingControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification on steel-shape elements.

    :ivar Ncrd:  design strength to axial compression
    :ivar McRdz: design moment strength about Z (strong) axis
    :ivar chiLT: reduction factor for lateral-torsional buckling (defaults to 1)
    :ivar chiN:  reduction factor for compressive strength (defaults to 1)
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0, N= 0.0, Mz= 0.0,Ncrd=0.0, McRdz=0.0, chiLT=1.0, chiN= 1.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Ncrd:     design strength to axial compression
        :param McRdz:    design moment strength about Z (strong) axis
        :param chiLT:    reduction factor for lateral-torsional buckling (defaults to 1)
        :param chiN:     reduction factor for compressive strength (defaults to 1)
        '''
        super(UniaxialBendingStrengthControlVars,self).__init__(idSection= idSection, combName= combName, CF= CF, N= N, My= Mz)
        self.Ncrd=Ncrd
        self.McRdz=McRdz
        self.chiLT=chiLT
        self.chiN= chiN
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(UniaxialBendingStrengthControlVars,self).getDict()
        retval['Mz']= retval.pop('My') # My <-> Mz
        retval.update({'Ncrd':self.Ncrd, 'McRdz':self.McRdz, 'chiLT':self.chiLT, 'chiN':self.chiN})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        dct['My']= dct.pop('Mz') # Mz <-> My
        super(UniaxialBendingStrengthControlVars,self).setFromDict(dct)
        self.Ncrd= dct['Ncrd']
        self.McRdz= dct['McRdz']
        self.chiLT= dct['chiLT']
        self.chiN= dct['chiN']

class BiaxialBendingStrengthControlVars(bcv.BiaxialBendingControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification on steel-shape elements.

    :ivar Ncrd:  design strength to axial compression
    :ivar McRdy: design moment strength about Y (weak) axis
    :ivar McRdz: design moment strength about Z (strong) axis
    :ivar chiLT: reduction factor for lateral-torsional buckling (defaults to 1)
    :ivar chiN:  reduction factor for compressive strength (defaults to 1)
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0,Ncrd=0.0,McRdy=0.0,McRdz=0.0,chiLT=1.0, chiN= 1.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Ncrd:     design strength to axial compression
        :param McRdy:    design moment strength about Y (weak) axis
        :param McRdz:    design moment strength about Z (strong) axis
        :param chiLT:    reduction factor for lateral-torsional buckling (defaults to 1)
        :param chiN:     reduction factor for compressive strength (defaults to 1)
        '''
        super(BiaxialBendingStrengthControlVars,self).__init__(idSection,combName,CF,N,My,Mz)
        self.Ncrd=Ncrd
        self.McRdy=McRdy
        self.McRdz=McRdz
        self.chiLT=chiLT
        self.chiN= chiN
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(BiaxialBendingStrengthControlVars,self).getDict()
        retval.update({'Ncrd':self.Ncrd, 'McRdy':self.McRdy, 'McRdz':self.McRdz, 'chiLT':self.chiLT, 'chiN':self.chiN})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(BiaxialBendingStrengthControlVars,self).setFromDict(dct)
        self.Ncrd= dct['Ncrd']
        self.McRdy= dct['McRdy']
        self.McRdz= dct['McRdz']
        self.chiLT= dct['chiLT']
        self.chiN= dct['chiN']
        
class SteelShapeUniaxialBendingControlVars(UniaxialBendingStrengthControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification en steel-shape elements.

    :ivar MvRdz:    reduced design moment strength about Z (strong) axis for shear interaction
    :ivar MbRdz:    reduced design moment strength about Z (strong) axis for lateral-torsional bucking
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0, Mz= 0.0, Ncrd=0.0, McRdz=0.0,MvRdz=0.0, MbRdz=0.0, chiLT=1.0, chiN=1.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Ncrd:     design strength to axial compression
        :param McRdz:    design moment strength about Z (strong) axis
        :param MvRdz:    reduced design moment strength about Z (strong) axis for shear interaction
        :param MbRdz:    reduced design moment strength about Z (strong) axis for lateral-torsional bucking
        :param chiLT:    reduction factor for lateral-torsional buckling (defaults to 1)
        :param chiN:     reduction factor for compressive strength (defaults to 1)
        '''
        super(SteelShapeUniaxialBendingControlVars,self).__init__(idSection= idSection,combName= combName, CF= CF, N= N, Mz= Mz, Ncrd= Ncrd, McRdz= McRdz, chiLT=chiLT, chiN= chiN)
        self.MvRdz=MvRdz
        self.MbRdz=MbRdz
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(SteelShapeUniaxialBendingControlVars,self).getDict()
        retval.update({'MvRdz':self.MvRdz, 'MbRdz':self.MbRdz})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(SteelShapeUniaxialBendingControlVars,self).setFromDict(dct)
        self.MvRdz= dct['MvRdz']
        self.MbRdz= dct['MbRdz']

class SteelShapeBiaxialBendingControlVars(BiaxialBendingStrengthControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification en steel-shape elements.

    :ivar MvRdz:    reduced design moment strength about Z (strong) axis for shear interaction
    :ivar MbRdz:    reduced design moment strength about Z (strong) axis for lateral-torsional bucking
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0,Ncrd=0.0,McRdy=0.0,McRdz=0.0,MvRdz=0.0,MbRdz=0.0, chiLT=1.0, chiN=1.0):
        '''
        Constructor.

        :param idSection: section identifier.
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
        super(SteelShapeBiaxialBendingControlVars,self).__init__(idSection= idSection,combName= combName,CF= CF,N= N, My= My, Mz= Mz, Ncrd= Ncrd, McRdy= McRdy, McRdz= McRdz, chiLT=chiLT, chiN= chiN)
        self.MvRdz=MvRdz
        self.MbRdz=MbRdz
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(SteelShapeBiaxialBendingControlVars,self).getDict()
        retval.update({'MvRdz':self.MvRdz, 'MbRdz':self.MbRdz})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(SteelShapeBiaxialBendingControlVars,self).setFromDict(dct)
        self.MvRdz= dct['MvRdz']
        self.MbRdz= dct['MbRdz']

class SteelBucklingControlVars(bcv.BucklingControlVarsBase):
    '''Control variables for buckling ultimate limit state verification on
       steel elements.

    '''
    def __init__(self, combName= 'nil', N= 0.0,My= 0.0,Mz= 0.0, effectiveLengths= None, mechLambda= None, strengthReductionFactors= None, bucklingResistance= None):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param N:  axial force (defaults to 0.0)
        :param My: bending moment about Y axis (defaults to 0.0)
        :param Mz: bending moment about Z axis (defaults to 0.0)
        :param effectiveLengths: buckling effective lengths for the elemement (first index: buckling mode, second index: axis).
        :param mechLambda: mechanical slenderness of the element (first index: buckling mode, second index: axis).
        :param strengthReductionFactors: buckling strength reduction factors of the element (first index: buckling mode, second index: axis).
        :param bucklingResistance: buckling resistance of the element (first index: buckling mode, second index: axis).
        '''
        super(SteelBucklingControlVars,self).__init__(combName= combName, N= N, My= My, Mz= Mz, effectiveLengths= effectiveLengths, mechLambda= mechLambda)
        self.strengthReductionFactors= list() if strengthReductionFactors is None else strengthReductionFactors
        self.bucklingResistance= list() if bucklingResistance is None else bucklingResistance
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(SteelBucklingControlVars,self).getDict()
        retval.update({'strengthReductionFactors':self.strengthReductionFactors})
        retval.update({'bucklingResistance':self.bucklingResistance})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(SteelBucklingControlVars,self).setFromDict(dct)
        self.strengthReductionFactors= dct['strengthReductionFactors']
        self.bucklingResistance= dct['bucklingResistance']
