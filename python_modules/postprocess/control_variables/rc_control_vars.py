# -*- coding: utf-8 -*-
'''Control vars to store variables that control the reinforced concrete
   limit state checking.
'''

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2026,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

from postprocess.control_variables import basic_control_variables as bcv

UniaxialBendingControlVars= bcv.UniaxialBendingControlVars
BiaxialBendingControlVars= bcv.BiaxialBendingControlVars


class BiaxialBucklingControlVars(bcv.BiaxialBendingControlVars):
    '''Control variables for buckling limit state verification 
       verification.

    :ivar Leff: effective length.
    :ivar mechLambda: mechanical slenderness.
    :ivar efY: additional eccentricity for buckling around Y axis.
    :ivar efZ: additional eccentricity for buckling around Z axis.
    :ivar mode: buckling mode corresponding to the previous parameters.
    :ivar alpha_cr: factor by which the design loading would have to be increased to cause elastic instability (see Eurocode 4:2004 cl.5.2.1(2)).
    :ivar strongAxisBucklingPercent: dot product (projection) of the buckling eigenvector over the strong axis.
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0, Leff= -1.0, mechLambda= -1.0, efY= 0.0, efZ= 0.0, mode= None, alpha_cr= None, strongAxisBucklingPercent= 0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Leff: effective length.
        :param mechLambda: mechanical slenderness.
        :param efY: additional eccentricity around Y axis.
        :param efZ: additional eccentricity around Z axis.
        :param mode: buckling mode that corresponds to the previous parameters.
        :param alpha_cr: factor by which the design loading would have to be increased to cause elastic instability (see Eurocode 4:2004 cl.5.2.1(2)).
        :param strongAxisBucklingPercent: dot product (projection) of the buckling eigenvector over the strong axis.
        '''
        super(BiaxialBucklingControlVars,self).__init__(idSection= idSection, combName= combName, CF= CF, N= N, My= My, Mz= Mz)
        self.setBucklingParameters(Leff= Leff, mechLambda= mechLambda, efZ= efZ, efY= efY, mode= mode, alpha_cr= alpha_cr, strongAxisBucklingPercent= strongAxisBucklingPercent)

    def setBucklingParameters(self, Leff, mechLambda, efZ, efY, mode, alpha_cr, strongAxisBucklingPercent):
        ''' Assigns values to the buckling parameters.

        :param Leff: effective length.
        :param mechLambda: mechanical slenderness.
        :param efZ: additional eccentricity around Z axis.
        :param efY: additional eccentricity around Y axis.
        :param mode: buckling mode that corresponds to the previous parameters.
        :param alpha_cr: factor by which the design loading would have to be increased to cause elastic instability (see Eurocode 4:2004 cl.5.2.1(2)).
        :param strongAxisBucklingPercent: dot product (projection) of the buckling eigenvector over the strong axis.
        '''
        self.Leff= Leff # effective length.
        self.mechLambda= mechLambda # mechanical slenderness.
        self.efY= efY # additional eccentricity around Y axis.
        self.efZ= efZ # additional eccentricity around Z axis.
        self.mode= mode # buckling mode that corresponds to the previous parameters.
        self.alpha_cr= alpha_cr
        self.strongAxisBucklingPercent= strongAxisBucklingPercent # dot product (projection) of the buckling eigenvector over the strong axis.
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(BiaxialBucklingControlVars, self).getDict()
        retval.update({'Leff': self.Leff, 'mechLambda': self.mechLambda, 'efY': self.efY, 'efZ': self.efZ, 'mode': self.mode, 'alpha_cr': self.alpha_cr, 'strongAxisBucklingPercent':self.strongAxisBucklingPercent})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(BiaxialBendingStrengthControlVars,self).setFromDict(dct)
        self.Leff= dct['Leff'] # effective length.
        self.mechLambda= dct['mechLambda'] # mechanical slenderness.
        self.efY= dct['efY'] # additional eccentricity around Y axis.
        self.efZ= dct['efZ'] # additional eccentricity around Z axis.
        self.mode= dct['mode'] # buckling mode that corresponds to the previous parameters.
        self.alpha_cr= dct['alpha_cr'] # axial load divided by the critical axial load.
        self.strongAxisBucklingPercent= dct['strongAxisBucklingPercent'] # dot product (projection) of the buckling eigenvector over the strong axis.

class UniaxialBucklingControlVars(bcv.UniaxialBendingControlVars):
    '''Control variables for buckling limit state verification 
       verification.

    :ivar Leff: effective length for buckling.
    :ivar mechLambda: mechanical slenderness for buckling.
    :ivar ef: additional eccentricity for buckling.
    :ivar mode: buckling mode corresponding to the previous parameters.
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,M= 0.0, Leff= -1.0, mechLambda= -1.0, ef= 0.0, mode= None):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param M:       bending moment (defaults to 0.0)
        :param Leff: effective length for buckling.
        :param mechLambda: mechanical slenderness for buckling.
        :param ef: additional eccentricity for buckling.
        :param mode: buckling mode corresponding to the previous parameters.
        '''
        super(UniaxialBucklingControlVars,self).__init__(idSection= idSection, combName= combName, CF= CF, N= N, M= M)
        self.setBucklingParameters(Leff= Leff, mechLambda= mechLambda, ef= ef, mode= mode)
        
    def setBucklingParameters(self, Leff, mechLambda, ef, mode):
        ''' Assigns values to the buckling parameters.

        :param Leff: effective length for buckling.
        :param mechLambda: mechanical slenderness for buckling.
        :param ef: additional eccentricity for buckling.
        :param mode: buckling mode corresponding to the previous parameters.
        '''
        self.Leff= Leff # effective length for buckling.
        self.mechLambda= mechLambda # mechanical slenderness for buckling.
        self.ef= ef # additional eccentricity for buckling.
        self.mode= mode # buckling mode that corresponds to the previous parameters.
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(UniaxialBucklingControlVars, self).getDict()
        retval.update({'Leff': self.Leff, 'mechLambda': self.mechLambda, 'ef': self.ef, 'mode':self.mode})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(BiaxialBendingStrengthControlVars,self).setFromDict(dct)
        self.Leff= dct['Leff'] #effective length for buckling.
        self.mechLambda= dct['mechLambda'] #mechanical slenderness for buckling.
        self.ef= dct['ef'] #additional eccentricity for buckling.
        self.mode= dct['mode'] # buckling mode that corresponds to the previous parameters.

#       ___ _                 
#      / __| |_  ___ __ _ _ _ 
#      \__ \ ' \/ -_) _` | '_|
#      |___/_||_\___\__,_|_|  
# Shear control vars.
        
class RCShearControlVars(bcv.BiaxialBendingControlVars):
    '''Control variables for shear limit state verification in reinforced 
       concrete elements.

    :ivar Mu:       ultimate bending moment
    :ivar Vy:       shear force parallel to the y axis
    :ivar Vz:       shear force parallel to the z axis
    :ivar theta:    angle between the concrete compression struts and the beam axis
    :ivar Vu:       shear strength

    '''
    def __init__(self,idSection=-1,combName= 'nil',CF= -1.0,N= 0.0, My= 0.0, Mz= 0.0, Mu= 0.0, Vy= 0.0, Vz= 0.0, theta= 0.0, Vu= 0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Mu:       ultimate bending moment
        :param Vy:       shear force parallel to the y axis
        :param Vz:       shear force parallel to the z axis
        :param theta:    angle between the concrete compression struts and the beam axis
        :param Vu:       shear strength
        '''
        super(RCShearControlVars,self).__init__(idSection,combName,CF,N,My,Mz)
        self.Mu= Mu #Ultimate bending moment.
        self.Vy= Vy #Shear parallel to the y axis.
        self.Vz= Vz #Shear parallel to the z axis.
        self.theta= theta #Strut angle.
        self.Vu= Vu # Shear strength.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(RCShearControlVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Mu" +axis+","+str(self.Mu*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vy" +axis+","+str(self.Vy*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vz" +axis+","+str(self.Vz*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vu" +axis+","+str(self.Vu*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(RCShearControlVars,self).getDict()
        retval.update({'Mu':self.Mu, 'Vy':self.Vy, 'Vz':self.Vz, 'theta':self.theta, 'Vu':self.Vu})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(RCShearControlVars,self).setFromDict(dct)
        self.Mu= dct['Mu']
        self.Vy= dct['Vy']
        self.Vz= dct['Vz']
        self.theta= dct['theta']
        self.Vu= dct['Vu']

class SIATypeRCShearControlVars(RCShearControlVars):
    '''Control variables for shear limit state verification in EHE-08.

    :ivar Vcu: Vcu component of the shear strength.
    :ivar Vsu: Vsu component of the shear strength.
    '''
    def __init__(self,idSection=-1,combName= 'nil',CF= -1.0, N= 0.0, My= 0.0, Mz= 0.0, Mu= 0.0, Vy= 0.0, Vz= 0.0, theta= 0.0, Vcu= 0.0, Vsu= 0.0, Vu= 0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Mu:       ultimate bending moment
        :param Vy:       shear force parallel to the y axis
        :param Vz:       shear force parallel to the z axis
        :param theta:    angle between the concrete compression struts and the beam axis
        :param Vcu:      Vcu component of the shear strength (defined in the codes)
        :param Vsu:      Vsu component of the shear strength (defined in the codes)
        :param Vu:       shear strength
        '''
        super(SIATypeRCShearControlVars,self).__init__(idSection= idSection, combName= combName, CF= CF, N= N, My= My, Mz= Mz, Mu= Mu, Vy= Vy, Vz= Vz, theta= theta, Vu= Vu)
        self.Vcu= Vcu #Vcu component of the shear strength.
        self.Vsu= Vsu #Vsu component of the shear strength.
    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)
        '''
        retval= super(SIATypeRCShearControlVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Vcu" +axis+","+str(self.Vcu*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vsu" +axis+","+str(self.Vsu*factor)+"\n")
        return retval
    
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(SIATypeRCShearControlVars,self).getDict()
        retval.update({'Vcu':self.Vcu, 'Vsu':self.Vsu})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(SIATypeRCShearControlVars,self).setFromDict(dct)
        self.Vcu= dct['Vcu']
        self.Vsu= dct['Vsu']

#        ___             _                _           _ 
#       / __|_ _ __ _ __| |__  __ ___ _ _| |_ _ _ ___| |
#      | (__| '_/ _` / _| / / / _/ _ \ ' \  _| '_/ _ \ |
#       \___|_| \__,_\__|_\_\ \__\___/_||_\__|_| \___/_|
# Crack control control vars.     

class CrackControlBaseVars(bcv.CFNMyMz):
    '''Biaxial bending. Cracking serviceability limit state variables.

    :ivar steelStress: maximum stress in the reinforcement bars
    '''
    def __init__(self,combName= 'nil',CF= -1.0,N= 0.0, My= 0.0, Mz= 0.0, steelStress= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency)
        :param N:        axial force
        :param My:       bending moment about Y axis
        :param Mz:       bending moment about Z axis
        :param steelStress: maximum stress in the reinforcement bars
        '''
    #    super(CrackControlBaseVars,self).__init__(combName,CF,N,My) #Jan 26th 2017
        super(CrackControlBaseVars,self).__init__(combName,CF,N,My,Mz)
        self.steelStress= steelStress #Stress in rebars.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(CrackControlBaseVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",steelStress" +axis+","+str(self.steelStress*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(CrackControlBaseVars,self).getDict()
        retval.update({'steelStress':self.steelStress})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(CrackControlBaseVars,self).setFromDict(dct)
        self.steelStress= dct['steelStress']

class CrackControlVars(bcv.ControlVarsBase):
    '''Cracking serviceability limit state control variables.

    :ivar idSection: section identifier
    :ivar crackControlVarsPos:     Crack control in + face.
    :ivar crackControlVarsNeg:     Crack control in - face.
    '''
    def __init__(self, idSection= 'nil', crackControlBaseVarsPos= None ,crackControlBaseVarsNeg= None):
        '''
        Constructor.

        :param idSection: section identifier
        :param crackControlVarsPos:     Crack control in + face.
        :param crackControlVarsNeg:     Crack control in - face.
        '''
        super(CrackControlVars, self).__init__() # combName?
        self.idSection= idSection # Section identifier.
        if(crackControlBaseVarsPos):
            self.crackControlVarsPos= crackControlBaseVarsPos #Cracks in + face.
        else:
            self.crackControlVarsPos= CrackControlBaseVars()
        if(crackControlBaseVarsNeg):
            self.crackControlVarsNeg= crackControlBaseVarsNeg #Cracks in - face.
        else:
            self.crackControlVarsNeg= CrackControlBaseVars()

    def getCF(self):
        ''' Return the capacity factor.'''
        return max(self.crackControlVarsPos.getCF(),self.crackControlVarsNeg.getCF())

    def getMaxSteelStress(self):
        '''Maximum value for rebar stresses.'''
        return max(self.crackControlVarsPos.steelStress,self.crackControlVarsNeg.steelStress)

    def getMaxN(self):
        '''Maximum internal axial force.'''
        return max(self.crackControlVarsPos.N,self.crackControlVarsNeg.N)

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= 'idSection= "' + self.idSection + '"' 
        retval+= ', crackControlBaseVarsPos= ' + self.crackControlVarsPos.getStrConstructor()
        retval+= ', crackControlBaseVarsNeg= ' + self.crackControlVarsNeg.getStrConstructor()
        return retval
    
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= dict()
        retval.update({'idSection':self.idSection})
        retval.update({'crackControlBaseVarsPos':self.crackControlVarsPos.getStrConstructor()})
        retval.update({'crackControlBaseVarsNeg':self.crackControlVarsNeg.getStrConstructor()})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.idSection= dct['idSection']
        self.crackControlVarsPos= eval(dct['crackControlBaseVarsPos'])
        self.crackControlVarsNeg= eval(dct['crackControlBaseVarsNeg'])

class RCCrackControlVars(bcv.CFNMyMz):
    '''Control variables for cracking serviacebility limit state verification.

    :ivar idSection: section identifier.
    :ivar s_rmax: maximum distance between cracks (otherwise a new crack could occur in-between)
    :ivar wk: crack width
    '''
    def __init__(self, idSection=-1,combName= 'nil', CF=-1, N= 0.0, My= 0.0, Mz= 0.0, s_rmax= 0.0, sigma_s= 0.0, sigma_c= 0.0, wk= 0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor.
        :param N:        axial force
        :param My:       bending moment about Y axis
        :param Mz:       bending moment about Z axis
        :param s_rmax:   maximum distance between cracks (otherwise a new crack could occur in-between
        :param sigma_s: maximum tensile stress in the reinforcement.
        :param sigma_c: compressive stress in the most compressed concrete fibre.
        :param wk:       crack width
        '''
        super(RCCrackControlVars,self).__init__(combName= combName, CF= CF, N= N,My= My, Mz= Mz)
        self.idSection=idSection
        self.s_rmax=s_rmax
        self.sigma_s= sigma_s
        self.sigma_c= sigma_c
        self.wk=wk
    
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(RCCrackControlVars,self).getDict()
        retval.update({'s_rmax':self.s_rmax, 'sigma_s':self.sigma_s, 'sigma_c':self.sigma_c, 'wk': self.wk})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(RCCrackControlVars,self).setFromDict(dct)
        self.s_rmax= dct['s_rmax']
        self.sigma_s= dct['sigma_s']
        self.sigma_c= dct['sigma_c']
        self.wk= dct['wk']

class RCCrackStraightControlVars(RCCrackControlVars):
    '''Control variables for cracking serviacebility limit state verification
    when when considering a concrete stress-strain diagram that takes account
    of the effects of tension stiffening.

    :ivar eps_sm:   mean strain in the reinforcement when taking into account the effects of tension stiffening
    '''
    def __init__(self, idSection=-1, combName= 'nil', CF=-1, N= 0.0, My= 0.0, Mz= 0.0, s_rmax=0.0, eps_sm=0.0, wk=0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param N:        axial force
        :param My:       bending moment about Y axis
        :param Mz:       bending moment about Z axis
        :param s_rmax:   maximum distance between cracks (otherwise a new crack could occur in-between
        :param eps_sm:   mean strain in the reinforcement when taking into account the effects of tension stiffening
        :param wk:       crack width
        '''
        super(RCCrackStraightControlVars,self).__init__(idSection= idSection, combName= combName, CF= CF, N= N, My= My, Mz= Mz, s_rmax= s_rmax, wk= wk)
        self.eps_sm= eps_sm

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(RCCrackStraightControlVars,self).getDict()
        retval.update({'eps_sm':self.eps_sm})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(RCCrackStraightControlVars,self).setFromDict(dct)
        self.eps_sm= dct['eps_sm']

#       ___     _   _               
#      | __|_ _| |_(_)__ _ _  _ ___ 
#      | _/ _` |  _| / _` | || / -_)
#      |_|\__,_|\__|_\__, |\_,_\___|
#                    |___/          
# Fatigue control vars
  
class FatigueControlBaseVars(bcv.NMyMz):
    '''Biaxial bending. Fatigue limit state variables.

    :ivar Vy:       shear force parallel to Y axis.
    :ivar posSteelStress: traction stress in rebars.
    :ivar negSteelStress: compression stress in rebars.
    :ivar concreteStress: compression stress in concrete.
    '''
    def __init__(self,combName= 'nil',CF=-1.0,N= 0.0, My= 0.0, Mz= 0.0, Vy= 0.0, posSteelStress= 0.0, negSteelStress= 0.0, concreteStress= 0.0):
        '''
        Constructor.

        :param combName: name of the load combination to deal with
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Vy:       shear force parallel to Y axis.
        :param posSteelStress: traction stress in rebars.
        :param negSteelStress: compression stress in rebars.
        :param concreteStress: compression stress in concrete.
        '''
        #Note: Currently, CF attribute  has no sense in fatigue verification.
        # Perhaps, in the future, the maximum value
        #of the CF calculated can be represented by CF. For now, this
        # attribute remains in this class only for a purpose
        #of compatibility with the parent classes
        super(FatigueControlBaseVars,self).__init__(combName,N,My,Mz)
        self.Vy= Vy #Shear.
        self.posSteelStress= posSteelStress #traction stress in rebars.
        self.negSteelStress= negSteelStress #compression stress in rebars.
        self.concreteStress= concreteStress #compression stress in concrete.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(FatigueControlBaseVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Vy" +axis+","+str(self.Vy*factor)+"\n")
        retval.append("detab,"+str(eTag)+",posSteelStress" +axis+","+str(self.posSteelStress*factor*factor)+"\n")
        retval.append("detab,"+str(eTag)+",negSteelStress" +axis+","+str(self.negSteelStress*factor*factor)+"\n")
        retval.append("detab,"+str(eTag)+",concreteStress" +axis+","+str(self.concreteStress*factor*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(FatigueControlBaseVars,self).getDict()
        retval.update({'Vy':self.Vy, 'negSteelStress':self.negSteelStress, 'posSteelStress':self.posSteelStress, 'concreteStress': self.concreteStress})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(FatigueControlBaseVars,self).setFromDict(dct)
        self.Vy= dct['Vy']
        self.negSteelStress= dct['negSteelStress']
        self.posSteelStress= dct['posSteelStress']
        self.concreteStress= dct['concreteStress']

class FatigueControlVars(bcv.ControlVarsBase):
    '''Fatigue limit state control variables.

    :ivar idSection: section identifier
    :ivar combName:  name of the load combinations to deal with
    :ivar state0:    Fatigue values (FatigueControlBaseVars) under permanent load.
    :ivar state1:    Fatigue values (FatigueControlBaseVars) under fatigue load.
    :ivar concreteLimitStress: limit for the concrete stress as specified in SIA 262(2013)  4.3.8.3.1
    :ivar concreteBendingCF:   concrete capacity factor under fatigue due to normal stresses.
    :ivar shearLimit:        limit for the shear force as sepecified in SIA 262(2013)  4.3.8.3.2
    :ivar concreteShearCF:   concrete capacity factor under fatigue due to shear forces.
    :ivar Mu:        ultimate bending moment
    :ivar Vu:        ultimate shear force  
    '''
    def __init__(self, idSection= 'nil', controlBaseVars0= None, controlBaseVars1= None, concreteLimitStress= 0.0,concreteBendingCF=-1.0,shearLimit=0.0,concreteShearCF=-1.0,Mu=0.0,Vu=0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName:  name of the load combinations to deal with
        :param state0:    Fatigue values (FatigueControlBaseVars) under permanent load.
        :param state1:    Fatigue values (FatigueControlBaseVars) under fatigue load.
        :param concreteLimitStress: limit for the concrete stress as specified in SIA 262(2013)  4.3.8.3.1
        :param concreteBendingCF:   concrete capacity factor under fatigue due to normal stresses.
        :param shearLimit:        limit for the shear force as sepecified in SIA 262(2013)  4.3.8.3.2
        :param concreteShearCF:   concrete capacity factor under fatigue due to shear forces.
        :param Mu:        ultimate bending moment
        :param Vu:        ultimate shear force  
        '''
        super(FatigueControlVars, self).__init__() # combName?
        self.idSection= idSection # Section identifier.
        if(controlBaseVars0):
            self.state0=  controlBaseVars0 #Under permanent loads.
        else:
            self.state0= FatigueControlBaseVars()
        if(controlBaseVars1):
            self.state1= controlBaseVars1 #Under fatigue loads.
        else:
            self.state1= FatigueControlBaseVars()
        self.concreteLimitStress= concreteLimitStress #SIA 262(2013)  4.3.8.3.1
        self.concreteBendingCF= concreteBendingCF #Concrete capacity factor.
        self.shearLimit=shearLimit #SIA 262(2013)  4.3.8.3.2
        self.concreteShearCF= concreteShearCF #Concrete shear factor.
        self.Mu= Mu #Ultimate bending moment.
        self.Vu= Vu #Ultimate shear.

    def getSteelPosStressIncrement(self):
        '''Returns positive stress increment in rebars.'''
        return self.state1.posSteelStress-self.state0.posSteelStress

    def getSteelNegStressIncrement(self):
        '''Returns negative stress increment in rebars.'''
        return self.state1.negSteelStress-self.state0.negSteelStress
      
    def getAbsSteelStressIncrement(self):
        '''Returns maximun stress increment in rebars (absolute value).'''
        return max(self.getSteelPosStressIncrement(),-self.getSteelNegStressIncrement())

    def getConcreteStressIncrement(self):
        '''Returns stress increment in concrete.'''
        return self.state1.concreteStress-self.state0.concreteStress

    def getConcreteMaxMinStresses(self):
        '''Used in FatigueController.concreteLimitStress.''' 
        retval= [0.0,0.0]
        retval[0]= min(min(self.state0.concreteStress,self.state1.concreteStress),0.0) # No positive stresses.
        retval[1]= min(max(self.state0.concreteStress,self.state1.concreteStress),0.0) # No positive stresses.
        return retval

    def getConcreteMinStress(self):
        '''Returns minimum (max. compressive) concrete stress between loaded and unloaded states.'''
        sgc0= abs(min(self.state0.concreteStress,0.0))
        sgc1= abs(min(self.state1.concreteStress,0.0))
        return max(sgc0,sgc1)

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= dict()
        retval.update({'idSection':self.idSection})
        retval.update({'controlBaseVars0':self.state0.getStrConstructor()})
        retval.update({'controlBaseVars1':self.state1.getStrConstructor()})
        retval.update({'concreteLimitStress':self.concreteLimitStress, 'concreteBendingCF':self.concreteBendingCF, 'shearLimit':self.shearLimit, 'concreteShearCF':self.concreteShearCF, 'Mu':self.Mu, 'Vu':self.Vu})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.idSection= dct['idSection']
        self.controlBaseVars0= eval(dct['controlBaseVars0'])
        self.controlBaseVars1= eval(dct['controlBaseVars1'])
        self.concreteLimitStress= dct['concreteLimitStress']
        self.concreteBendingCF= dct['concreteBendingCF']
        self.shearLimit= dct['shearLimit']
        self.concreteShearCF= dct['concreteShearCF']
        self.Mu= dct['Mu']
        self.Vu= dct['Vu']

class RCBucklingControlVars(bcv.BucklingControlVarsBase):
    '''Control variables for buckling ultimate limit state verification on 
       reinforced concrete elements.

    '''
    def __init__(self, combName= 'nil', N= 0.0,My= 0.0,Mz= 0.0, effectiveLengths= None, mechLambda= None, fictitiousEccentricities= None):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param N:  axial force (defaults to 0.0)
        :param My: bending moment about Y axis (defaults to 0.0)
        :param Mz: bending moment about Z axis (defaults to 0.0)
        :param effectiveLengths: buckling effective lengths for the elemement (first index: buckling mode, second index: axis).
        :param mechLambda: mechanical slenderness of the element (first index: buckling mode, second index: axis).
        :param fictitiousEccenctricities: fictitious eccentricities for the element (first index: buckling mode, second index: axis).
        '''
        super(RCBucklingControlVars,self).__init__(combName= combName, N= N, My= My, Mz= Mz, effectiveLengths= effectiveLengths, mechLambda= mechLambda)
        self.fictitiousEccenctricities= list() if fictitiousEccenctricities is None else fictitiousEccenctricities
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(RCBucklingControlVars,self).getDict()
        retval.update({'fictitiousEccenctricities':self.fictitiousEccenctricities})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(RCBucklingControlVars,self).setFromDict(dct)
        self.fictitiousEccenctricities= dct['fictitiousEccenctricities']
