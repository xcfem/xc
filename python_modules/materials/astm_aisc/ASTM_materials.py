# -*- coding: utf-8 -*-
''' Structural steel as specified in ASTM standard.

Predefined ASTM steel types: A36, A529, A572, A53, A992, A500, A307,
                             A325 (bolts M12 to M36)
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import inspect
import sys
import math
import scipy.interpolate
import geom
from geom_utils import closest_pair_of_points as cpp
from materials import steel_base
from misc_utils import log_messages as lmsg
from materials import member_base
from import_export import block_topology_entities as bte
from connections.steel_connections import bolts
from connections.steel_connections import square_plate_washer as swp
from connections.steel_connections import bolted_plate as bp
from connections.steel_connections import gusset_plate as gp
from connections.steel_connections import connected_members
from misc_utils import units_utils

class ASTMSteel(steel_base.BasicSteel):
    '''ASTM structural steel.

    :ivar name: steel identifier.
    :ivar Rt: Ratio of the expected tensile strength to the 
              specified minimum tensile strength, Fu.
              See table A3.1 of AISC 341 seismic provisions.
    :ivar Ry: Ratio of the expected yield stress to the specified
              minimum yield stress, Fy.
              See table A3.1 of AISC 341 seismic provisions.
    '''
    def __init__(self, name= None, fy= 250e6, fu= 400e6, gammaM= 1.0, Rt= None, Ry= None):
        ''' Constructor.

        :param name: steel identifier.
        :param fy: yield stress (defaults to 250e6 Pa)
        :param fu: ultimate tensile strength (defaults to 400e6 Pa)
        :param gammaM: partial factor (defaults to 1.0)
        :param Rt: Ratio of the expected tensile strength to the 
                  specified minimum tensile strength, Fu.
                  See table A3.1 of AISC 341 seismic provisions.
        :param Ry: Ratio of the expected yield stress to the specified
                  minimum yield stress, Fy.
                  See table A3.1 of AISC 341 seismic provisions.
        '''
        super(ASTMSteel,self).__init__(200e9,0.3,fy,fu,gammaM)
        if(name):
            self.name= name
        else:
            self.name= None
        self.Rt= Rt
        self.Ry= Ry

    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(ASTMSteel,self).getDict()
        name= None
        if(self.name):
            name= self.name
        retval.update({'name': name, 'Rt':self.Rt, 'Ry':self.Ry})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.name= None
        if('name' in dct):
            name= dct['name']
            if(name): self.name= name
        self.Rt= dct['Rt']
        self.Ry= dct['Ry']

    def getPeakConnectionStrengthFactor(self):
        ''' Return the factor to account for peak connection strength, 
            including strain hardening, local restraint, additional 
            reinforcement and other connection conditions according
            to clause 2.4.3 (equation 2.4-2) of AISC 358-16.'''
        retval= (self.fy+self.fu)/(2*self.fy)
        retval= min(retval, 1.2)
        return retval

    def getYt(self):
        ''' Return the Yt factor to use in clause F13.1 of AISC 360-16
            to obtain the strength reduction for members with holes
            in the tension flange.'''
        retval= 1.0
        tmp= self.fy/self.fu
        if(tmp>=0.8):
            retval= 1.1
        return retval
    
    def getEquation7_6_2Factor(self):
        ''' Return the the factor that multiplies the flange width
            in equation 7.6-2M of AISC 358-16.'''
        tmp= self.fy*self.Ry
        tmp/= (self.Rt*self.fu)
        return (1-tmp)

A36= ASTMSteel('A36', fy= 250e6, fu=400e6, gammaM= 1.0, Rt= 1.2, Ry= 1.5)
A53= ASTMSteel('A53', fy= 240e6, fu= 414e6, gammaM= 1.0, Rt= 1.2, Ry= 1.5)
# ASTM A193M steel mechanical properties
ksi= 6.89475908677537e6
A193B7le2_5= ASTMSteel('A193B7le2_5', fy= 105*ksi, fu= 125*ksi, gammaM= 1.0) # Diameter <=2.5 inches
A193B7gt2_5le4= ASTMSteel('A193B7gt2_5le4', fy= 95*ksi, fu= 115*ksi, gammaM= 1.0) # 2.5 < Diameter <= 4.0 inches
A193B7gt4le7= ASTMSteel('A193B7gt4le7', fy= 75*ksi, fu= 100*ksi, gammaM= 1.0) # 4.0 < Diameter <= 7.0 inches
A307= ASTMSteel('A307', fy= 245e6, fu= 390e6, gammaM= 1.0)
A325= ASTMSteel('A325', fy= 660e6, fu= 830e6, gammaM= 1.0)
A500= ASTMSteel('A500', fy= 315e6, fu= 400e6, gammaM= 1.0, Rt= 1.3, Ry= 1.4)
A529= ASTMSteel('A529', fy= 290e6, fu= 414e6, gammaM= 1.0, Rt= 1.2, Ry= 1.2)
A572= ASTMSteel('A572', fy= 345e6, fu= 450e6, gammaM= 1.0, Rt= 1.1, Ry= 1.1)
A992= ASTMSteel('A992', fy= 345e6, fu= 450e6, gammaM= 1.0, Rt= 1.1, Ry= 1.1)

A354BC= ASTMSteel('A354BC', fy=109*ksi, fu= 125*ksi, gammaM= 1.0)
A354BD= ASTMSteel('A354BD', fy=115*ksi, fu= 140*ksi, gammaM= 1.0) # Higher values for bolts under 2.5 in.
A490= ASTMSteel('A490', fy=940e6, fu= 1040e6, gammaM= 1.0)

F1554gr36=ASTMSteel('F1554gr36', fy=248e6, fu= 400e6, gammaM= 1.0)  #steel for anchor bolts
F1554gr55=ASTMSteel('F1554gr55', fy=380e6, fu= 517e6, gammaM= 1.0)  #steel for anchor bolts
F1554gr105=ASTMSteel('F1554gr105', fy=724e6, fu= 862e6, gammaM= 1.0)  #steel for anchor bolts

# Welding materials
E7018=ASTMSteel(name='E7018', fy= 400e6, fu= 490e6, gammaM= 1.0) #filler metal used for weldings (see properties in AWS D1.1/D1, (It's not an ASTM steel, change the definition in a next revision)
AWS_E308L_15=ASTMSteel(name='AWS-E308L-15',fy=430e6,fu=520e6, gammaM=1.0)  # filler metal used for weldings in stainless steel
AWS_E308L_15.E=200e9
AWS_E308L_15.nu=0.3


def getFilletWeldMinimumLeg(t):
    '''
    Return the minimum leg size for a fillet bead that welds a sheet 
    of thickness t according to table J2.4 of AISC 360.

    :param t: Sheet thickness.
    '''
    if(t<=6e-3):
        return 3e-3
    elif(t<=13e-3):
        return 5e-3
    elif(t<=19e-3):
        return 6e-3
    else:
        return 8e-3

def getFilletWeldMaximumLeg(t):
    '''
    Return the maximum leg size for a fillet bead that welds a sheet 
    of thickness t according to section J2.2b of AISC 360.

    :param t: Sheet thickness.
    '''
    if(t<=6e-3):
        return t
    else:
        return t-2e-3

def getFilletWeldMinimumLegSheets(t1, t2):
    '''
    Return the minimum leg size which can be used to weld two sheets
    according to table J2.4 of AISC 360.

    :param t1: Thickness of sheet 1.
    :param t2: Thickness of sheet 2.
    '''
    amin1= getFilletWeldMinimumLeg(t1)
    amin2= getFilletWeldMinimumLeg(t2)
    return max(amin1,amin2)

def getFilletWeldMaximumLegSheets(t1, t2):
    '''
    Return the maximum leg size which can be used to weld two sheets
    according to section J2.2b of AISC 360.

    :param t1: Thickness of sheet 1.
    :param t2: Thickness of sheet 2.
    '''
    amax1= getFilletWeldMaximumLeg(t1)
    amax2= getFilletWeldMaximumLeg(t2)
    return min(amax1,amax2)

class BoltFastener(bolts.BoltBase):
    ''' ASTM bolt according to chapter J of AISC 360-16.

    :ivar steelType: type of the bolt steel. 
    '''
    # See table J3.4 M of AISC 360-16.
    bf_diams= [12e-3,14e-3,16e-3, 20e-3, 22e-3, 24e-3, 27e-3, 30e-3, 36e-3]
    tabJ3_4M= [16e-3,18e-3,22e-3, 26e-3, 28e-3, 30e-3, 34e-3, 38e-3, 46e-3]
    fTabJ3_4M= scipy.interpolate.interp1d(bf_diams,tabJ3_4M)
    # See table J3.3 M of AISC 360-16.
    standardHoleDia= [14e-3,16e-3,18e-3, 22e-3, 24e-3, 27e-3, 30e-3, 33e-3]
    oversizeHoleDia= [16e-3,18e-3,20e-3, 24e-3, 28e-3, 30e-3, 35e-3, 38e-3]
    fStandardHoleDia= scipy.interpolate.interp1d(bf_diams[:-1], standardHoleDia)
    fOversizeHoleDia= scipy.interpolate.interp1d(bf_diams[:-1], oversizeHoleDia)
    groupA= ['A325', 'A325M', 'A354BC']
    groupB= ['A490', 'A490M', 'A354BD']
    groupC= ['F3043', 'F3111']
    diameterIncrement= 2e-3 # difference nominal and real hole diameter.
    
    def __init__(self, diameter, steelType= A307, pos3d= None):
       ''' Constructor.

       :param diameter: bolt diameter.
       :param steelType: bolt steel type.
       :param pos3d: bolt position.
       '''
       super(BoltFastener,self).__init__(diameter, pos3d)
       self.steelType= steelType

    def getGroup(self):
        ''' Return the bolt material strength group according to section
                 J3.1 of AISC 360-16.'''
        retval= None
        if(self.steelType.name in self.groupA):
            retval= 'A'
        elif(self.steelType.name in self.groupB):
            retval= 'B'
        elif(self.steelType.name in self.groupC):
            retval= 'C'
        return retval
            
    def getMinDistanceBetweenCenters(self):
        ''' Return the minimum distance between centers of standard, 
            oversized or slotted holes according to section J3.3 of
            AISC 360-16.'''
        return (2+2.0/3.0)*self.diameter
    
    def getRecommendedDistanceBetweenCenters(self):
        ''' Return the minimum distance between centers of standard, 
            oversized or slotted holes according to section J3.3 of
            AISC 360-16.'''
        return 3.0*self.diameter

    def getMinimumEdgeDistance(self, oversized= False):
        ''' Return the minimum edge distance from center of standard 
            hole to edge of connected part according to toble
            J3.4M of AISC 360-16.

        :param oversized: true if hole is oversized.
        '''
        if(oversized):
            this_function_name = inspect.currentframe().f_code.co_name
            lmsg.error(this_function_name+' for oversized holes not implemented yet.')        
        retval= 1.25*self.diameter
        if(self.diameter<=36e-3):
            if(self.diameter<BoltFastener.bf_diams[0]):
                lmsg.error('Bolt diameter = '+ str(round(self.diameter*1e3,1)) +' mm is less than minimum coded = '+ str(round(BoltFastener.bf_diams[0]*1e3,1)) + 'mm.')
            else:
                retval= self.fTabJ3_4M(self.diameter)
        return retval

    def getNominalHoleDiameter(self, oversized= False):
        ''' Return the minimum distance between centers of standard, 
            oversized or slotted holes according to table J3.3M of
            AISC 360-16.

        :param oversized: true if hole is oversized.
        '''
        if(oversized):
            if(self.diameter>=36e-3):
                return self.diameter+8e-3
            else:
                if self.diameter < BoltFastener.bf_diams[0]:
                    lmsg.error('Bolt diameter = '+ str(round(self.diameter*1e3,1)) +' mm is less than minimum coded = '+ str(round(BoltFastener.bf_diams[0]*1e3,1)) + 'mm.')
                else:
                    return float(self.fOversizeHoleDia(self.diameter))
        else:
            if(self.diameter>=36e-3):
                return self.diameter+3e-3
            else:
                if self.diameter<BoltFastener.bf_diams[0]:
                    lmsg.error('Bolt diameter = '+ str(round(self.diameter*1e3,1)) +' mm is less than minimum coded = '+ str(round(BoltFastener.bf_diams[0]*1e3,1)) + 'mm.')
                else:
                    return float(self.fStandardHoleDia(self.diameter))

    def getDesignHoleDiameter(self):
        ''' Return the hole diameter to use in net area calculations.'''
        return self.getNominalHoleDiameter()+self.diameterIncrement

    def getNominalTensileStrength(self):
        ''' Return the nominal strength of the fastener according
            to table J3.2 of AISC 360-16.
        '''
        retval= self.getArea()
        group= self.getGroup()
        if(group):
            if(group=='A'):
                retval*= 620e6
            elif(group=='B'):
                retval*= 780e6
            elif(group=='C'):
                retval*= 1040e6
        else:
            if(self.steelType.name=='A307'):
                retval*= 310e6
            else:
                retval*= 0.75*self.steelType.fu
        return retval
            
    def getDesignTensileStrength(self):
        ''' Return the design tensile strength of the bolt according
            to section J3.6 of of AISC 360-16.
        '''
        return 0.75*self.getNominalTensileStrength()
    
    def getNominalShearStrength(self, threadsExcluded= False):
        ''' Return the nominal strength of the fastener according
            to table J3.2 of AISC 360-16.

        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        '''
        retval= self.getArea()
        group= self.getGroup()
        if(group):
            if(group=='A'):
                if(threadsExcluded):
                    retval*= 469e6
                else:
                    retval*= 372e6
            elif(group=='B'):
                if(threadsExcluded):
                    retval*= 579e6
                else:
                    retval*= 469e6
            elif(group=='C'):
                if(threadsExcluded):
                    retval*= 779e6
                else:
                    retval*= 620e6
        else:
            if(self.steelType.name=='A307'):
                retval*= 186e6
            else:
                if(threadsExcluded):
                    retval*= 0.563*self.steelType.fu
                else:
                    retval*= 0.45*self.steelType.fu
        return retval

    def getDesignShearStrength(self, threadsExcluded= False):
        ''' Return the design shear strength of the bolt according
            to section J3.6 of of AISC 360-16.

        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        '''
        return 0.75*self.getNominalShearStrength(threadsExcluded)

    def getNominalCombinedStrength(self, V, threadsExcluded= False):
        ''' Return the nominal tensile stress modified to include the 
            effects of shear stress according to equations J3-3a 
            of AISC 360-16.

        :param V: shear force on the bolt.
        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        '''
        A= self.getArea()
        Fnt= self.getNominalTensileStrength()/A
        Fnv= self.getNominalShearStrength(threadsExcluded)/A
        frv= V/A
        retval= min(1.3*Fnt-Fnt/(0.75*Fnv)*frv, Fnt)*A
        return max(retval, 1e-6) # Don't return negative strength value.

    def getDesignCombinedStrength(self, V, threadsExcluded= False):
        ''' Return the design tensile stress modified to include the 
            effects of shear stress according to equations J3-3a 
            of AISC 360-16.

        :param V: shear force on the bolt.
        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        '''
        return 0.75*self.getNominalCombinedStrength(V, threadsExcluded)

    def getNumberOfBoltsForShear(self, shearForce, numberOfRows= 1, threadsExcluded= False):
        ''' Estimate the number of bolts required for resisting the
            shear force.

        :param shearForce: shear force to be resisted by the bolts.
        :param numberOfRows: number of rows of the bolt array.
        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        '''
        designShear= self.getDesignShearStrength(threadsExcluded)
        n= shearForce/designShear
        retval= math.ceil(n/numberOfRows)*numberOfRows
        return retval

    def __str__(self):
        return self.getName()
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(BoltFastener,self).getDict()
        steelTypeClassName= str(self.steelType.__class__)[8:-2]
        retval.update({'steelTypeClassName':steelTypeClassName, 'steelType':self.steelType.getDict()})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        super(BoltFastener,self).setFromDict(dct)
        steelTypeClassName= dct['steelTypeClassName']+'()'
        self.steelType= eval(steelTypeClassName)
        self.steelType.setFromDict(dct['steelType'])

    def report(self, outputFile):
        ''' Reports bolt design values.'''
        super(BoltFastener,self).report(outputFile)
        outputFile.write('      steel: '+self.steelType.name+'\n')

def getBoltForShear(shearForce, numberOfBolts, threadsExcluded= False):
    ''' Estimate the diameter of the bolts required for resisting the
        shear force.

    :param shearForce: shear force to be resisted by the bolts.
    :param numberOfBolts: number of bolts in the bolt array.
    :param threadsExcluded: true if threads and transition area of 
                            shank are excluded from the shear plane.
    '''
    retval= standardBolts[-1] # pessimistic assumption
    for sb in standardBolts:
        shearStrength= numberOfBolts*sb.getDesignShearStrength(threadsExcluded)
        if(shearStrength>=abs(shearForce)):
            retval= sb
            break
    if(shearStrength<shearForce):
        lmsg.warning('shear capacity exceeded; increase the number of bolts')
    return retval
        
M12= BoltFastener(12e-3)
M14= BoltFastener(14e-3)
M16= BoltFastener(16e-3)
M20= BoltFastener(20e-3)
M22= BoltFastener(22e-3)
M24= BoltFastener(24e-3)
M27= BoltFastener(27e-3)
M30= BoltFastener(30e-3)
M36= BoltFastener(36e-3)
M42= BoltFastener(42e-3)

# Standard bolt diameters
standardBolts= [M16, M20, M22, M24, M27, M30, M36]

def getBoltForHole(holeDiameter, boltSteelType= A307, tol= 0.5e-3):
    ''' Return the bolt that fits in the hole diameter
        argument.

    :param holeDiameter: diameter of the hole.
    :param tol: tolerance (defaults to 0.5 mm).
    '''
    tmp= None
    threshold= holeDiameter+tol
    for b in standardBolts[::-1]:
        boltHoleDiameter= b.getNominalHoleDiameter()
        if(boltHoleDiameter<threshold):
            tmp= b
            break
    if(tmp):
        retval= BoltFastener(diameter= tmp.diameter, steelType= boltSteelType)
    else:
        funcName= sys._getframe(0).f_code.co_name
        lmsg.error(funcName+': no standard bolt found for '+str(holeDiameter*1e3)+' mm hole.')
        retval= None
    return retval

class BoltArray(bp.BoltArrayBase):
    ''' Bolt array the AISC/ASTM way.'''
    def __init__(self, bolt= M16, nRows= 1, nCols= 1, dist= None):
        ''' Constructor.

        :param bolt: bolt type.
        :param nRows: row number.
        :param nCols: column number.
        :param dist: distance between rows and columns
                     (defaults to three diameters).
        '''
        super(BoltArray, self).__init__(bolt, nRows, nCols, dist)

class SquarePlateWasher(swp.SquarePlateWasher):
    ''' Class for AISC plate washers.'''
    def __init__(self, bolt, side= None, thickness= None, steelType= A36):
        ''' Constructor.

        :param bolt: bolt.
        :param side: plate side (if None compute automatically).
        :param thickness: plate thickness (defaults to nut height).
        :param steelType: steel type.
        '''
        super(SquarePlateWasher, self).__init__(bolt, side, thickness, steelType)
        
class BoltedPlate(bp.BoltedPlateBase):
    ''' Bolted plate the AISC/ASTM way.

    :ivar Cbearing: 2.4 when deformation at the bolt hole at service load is 
                    a design consideration (J3-6a) 3.0 when is not
                    (see J3-6a and J3-6b).
    :ivar longSlottedHoles: true if the holes are of the log slotted type.
    '''
    def __init__(self, boltArray= BoltArray(), width= None, length= None, thickness= 10e-3, steelType= A36, notched= False, eccentricity= geom.Vector2d(0.0,0.0), doublePlate= False):
        ''' Constructor.

        :param boltArray: bolt array.
        :param width: plate width (if None it will be computed from the bolt arrangement.)
        :param length: plate length (if None it will be computed from the bolt arrangement.)
        :param thickness: plate thickness.
        :param steelType: steel type.
        :param notched: if true use notchs when appropriate.
        :param eccentricity: eccentricity of the plate with respect the center
                             of the bolt array.
        :param doublePlate: if true there is one plate on each side
                            of the main member.
        '''
        super(BoltedPlate, self).__init__(boltArray= boltArray, width= width, length= length, thickness= thickness, steelType= steelType, notched= notched, eccentricity= eccentricity, doublePlate= doublePlate)
        self.Cbearing= 2.4
        self.longSlottedHoles= False

    def getGrossSectionYieldingLoad(self):
        ''' Return the load that determines the yielding of the gross
            section.'''
        return self.steelType.getYt()*self.steelType.fy*self.getGrossArea()

    def getNetSectionFractureLoad(self):
        ''' Return the load that determines the fracture of the net
            section.'''
        return self.steelType.fu*self.getNetArea()
    
    def getFilletMinimumLeg(self, otherThickness):
        '''
        Return the minimum leg size for a fillet bead 
        according to table J2.4 of AISC 360.

        :param otherThickness: thickness of the other part to weld.
        '''
        return getFilletWeldMinimumLegSheets(self.thickness, otherThickness)
        
    def getFilletMaximumLeg(self, otherThickness):
        '''
        Return the minimum leg size for a fillet bead 
        according to table J2.4 of AISC 360.

        :param otherThickness: thickness of the other part to weld.
        '''
        return getFilletWeldMaximumLegSheets(self.thickness, otherThickness)        
        
    def getNetWidth(self):
        ''' Return the net width of the base plate according to clause
        B.4.3b of AISC 360-16.
        '''
        retval= super(BoltedPlate,self).getNetWidth()
        return retval
    
    def getNetArea(self):
        ''' Return the net area of the base plate according to clause
        B.4.3b of AISC 360-16.
        '''
        return self.getNetWidth()*self.thickness
    
    def getMinThickness(self, Pd):
        ''' Return the minimum thickness of the plate
            to resist the force argument.

        :param Pd: design value of the force to resist.
        '''
        # Yielding in the gross section.
        minThicknessFy= Pd/0.9/self.steelType.getYt()/self.steelType.fy/self.width
        # Tension fracture in the net section.
        minThicknessFu= Pd/0.75/self.steelType.fu/self.getNetWidth()
        return max(minThicknessFy,minThicknessFu)

    def getNominalBearingStrength(self):
        ''' Return the nominal bearing strength at bolt holes according to
            clause J3.10.(a).(1) of AISC 360-16.
        '''
        if(self.longSlottedHoles):
            lmsg.error('Long slotted holes not implemented yet.')
        return self.boltArray.getNumberOfBolts()*self.Cbearing*self.boltArray.bolt.diameter*self.thickness*self.steelType.fu

    def getNominalTearoutStrength(self, loadDir):
        ''' Return the nominal tearout strength at bolt holes according to
            clause J3.10.(a).(2) of AISC 360-16.

        :param loadDir: direction of the load.
        '''
        if(self.longSlottedHoles):
            lmsg.error('Long slotted holes not implemented yet.')
        clearDistances= self.getClearDistances(loadDir)
        retval= 0.0
        C= self.Cbearing/2.0 # Half the value of C bearing (see J3-6c and J3-6d)
        for lc in clearDistances:
            retval+= C*lc*self.thickness*self.steelType.fu
        return retval

    def getDesignBearingStrength(self):
        ''' Return the design bearing strength at bolt holes according to
            clause J3.10.1 of AISC 360-16.
        '''
        if(self.longSlottedHoles):
            lmsg.error('Long slotted holes not implemented yet.')
        return 0.75*self.getNominalBearingStrength()

    def getDesignTearoutStrength(self, loadDir):
        ''' Return the design tearout strength at bolt holes according to
            clause J3.10.2 of AISC 360-16.

        :param loadDir: direction of the load.
        '''
        return 0.75*self.getNominalTearoutStrength(loadDir)

    def getDesignBearingEfficiency(self, load):
        ''' Return the efficiency of the plate with respect to the 
            bolt bearing under the load argument. '''
        return load/self.getDesignBearingStrength()
    
    def getDesignTearoutEfficiency(self, loadVector):
        ''' Return the efficiency of the plate with respect to the 
            bolt tearout under the load argument. '''
        load= loadVector.getModulus()
        return load/self.getDesignTearoutStrength(loadVector)
        
class GussetPlate(gp.GussetPlate):
    ''' Gusset plates according to AISC 360-16.'''
    def __init__(self, boltedPlateTemplate, gussetTip, halfChamfer, ijkVectors):
        ''' Constructor.

        :param boltedPlateTemplate: bolted plate dimensions and bolt type and 
                                    arrangement.
        :param gussetTip: intersection of the gusset plate with the
                          axis of its attached member.
        :param halfChamfer: half portion of the chamfer.
        :param ijkVectors: unary vectors of the local reference system.
        '''
        super(GussetPlate, self).__init__(boltedPlateTemplate= boltedPlateTemplate, gussetTip= gussetTip, halfChamfer= halfChamfer, ijkVectors= ijkVectors)
        
    def getFilletMinimumLeg(self, otherThickness):
        '''
        Return the minimum leg size for a fillet bead 
        according to table J2.4 of AISC 360.

        :param otherThickness: thickness of the other part to weld.
        '''
        return getFilletWeldMinimumLegSheets(self.thickness, otherThickness)
        
    def getFilletMaximumLeg(self, otherThickness):
        '''
        Return the minimum leg size for a fillet bead 
        according to table J2.4 of AISC 360.

        :param otherThickness: thickness of the other part to weld.
        '''
        return getFilletWeldMaximumLegSheets(self.thickness, otherThickness)
    
class FinPlate(BoltedPlate):
    ''' Fin plate the AISC/ASTM way.'''
    def __init__(self, boltArray, width= None, length= None, thickness= 10e-3, steelType= None, notched= False, eccentricity= geom.Vector2d(0.0,0.0)):
        ''' Constructor.

        :param boltArray: bolt array.
        :param width: plate width (if None it will be computed from the bolt arrangement.)
        :param length: plate length (if None it will be computed from the bolt arrangement.)
        :param thickness: plate thickness.
        :param steelType: steel type.
        :param eccentricity: eccentricity of the plate with respect the center
                             of the bolt array.
        :param notched: if true use notchs when appropriate.
        '''
        super(FinPlate, self).__init__(boltArray= boltArray, width= width, length= length, thickness= thickness, steelType= steelType, eccentricity= eccentricity, doublePlate= False)
        
    def getFilletWeldLegSize(self):
        ''' Return the conventional leg size for two fillet
            welds attaching the plate to the main member.

            Based on the document: Single-Plate Shear Connection Design 
            to Meet Structural Integrity Requirements. LOUIS F. GESCHWINDNER 
            and KURT D. GUSTAFSON. 2010
        '''
        return 5.0/8.0*self.thickness

    def getGrossAreaSubjectedToShear(self, shearVector):
        ''' Return the gross area of the plate subjected to shear.

        :param shearVector: 2D shear load vector.
        '''
        contour=  geom.Polygon2d(self.getCoreContour2d())
        centroid= contour.getCenterOfMass()
        line= geom.Line2d(centroid, centroid+shearVector)
        intersection= contour.clip(line)
        return intersection.getLength()*self.thickness

    def getNetAreaSubjectedToShear(self, Agv, shearVector):
        ''' Return the net area of the plate subjected to shear.

        :param Agv: gross area of the plate subjected to shear.
        :param shearVector: 2D shear load vector.
        '''
        holeDiameter= self.boltArray.bolt.getDesignHoleDiameter()
        return Agv-self.boltArray.getNumberOfBolts()*holeDiameter*self.thickness
    
    def getNominalShearYieldingStrength(self, shearVector):
        ''' Return the design shear strength of the plate according to
            clause J.4.2 of AISC 360-16.

        :param shearVector: 2D shear load vector.
        '''
        Agv= self.getGrossAreaSubjectedToShear(shearVector) # gross area subject to shear.
        return 0.6*self.steelType.fy*Agv # (J4-3)
    
    def getDesignShearYieldingStrength(self, shearVector):
        ''' Return the design shear strength of the plate according to
            clause J.4.2 of AISC 360-16.

        :param shearVector: 2D shear load vector.
        '''
        return self.getNominalShearYieldingStrength(shearVector) # (J4-3)

    def getNominalShearRuptureStrength(self, shearVector):
        ''' Return the design shear strength of the plate according to
            clause J.4.2 of AISC 360-16.

        :param shearVector: 2D shear load vector.
        '''
        Agv= self.getGrossAreaSubjectedToShear(shearVector) # gross area subject to shear.
        Anv= self.getNetAreaSubjectedToShear(Agv, shearVector) # net area subject to shear.
        return 0.6*self.steelType.fu*Anv # (J4-4) 
 
    def getDesignShearRuptureStrength(self, shearVector):
        ''' Return the design shear strength of the plate according to
            clause J.4.2 of AISC 360-16.

        :param shearVector: 2D shear load vector.
        '''
        return 0.75*self.getNominalShearRuptureStrength(shearVector)
    
    def getDesignShearStrength(self, shearVector):
        ''' Return the design shear strength of the plate according to
            clause J.4.2 of AISC 360-16.

        :param shearVector: 2D shear load vector.
        '''
        shearYielding= self.getDesignShearYieldingStrength(shearVector)
        shearRupture= self.getDesignShearRuptureStrength(shearVector)
        return min(shearYielding, shearRupture)

    def getDesignShearEfficiency(self, shearVector):
        ''' Return the design shear strength of the plate according to
            clause J.4.2 of AISC 360-16.

        :param shearVector: 2D shear load vector.
        '''
        shearStrength= self.getDesignShearStrength(shearVector)
        return shearVector.getModulus()/shearStrength
    
    def getNetAreaSubjectedToBlockTension(self, shearVector):
        ''' Return the net area the net area subjected to tension used
            in the calculation of the block shear strength.

        :param shearVector: 2D shear load vector.
        '''
        retval= self.thickness
        cover= self.getMinimumCoverInDir(shearVector.getNormalVector())
        holeDiameter= self.boltArray.bolt.getDesignHoleDiameter()
        if(self.boltArray.nCols==1): # single vertical line of bolts
            retval*= (cover-holeDiameter/2.0)
        elif(self.boltArray.nCols==2): # two vertical lines of bolts
            dist= self.boltArray.dist
            retval*= (cover+dist-3*holeDiameter/2.0)
        return retval

    def getGrossAreaSubjectedToBlockShear(self, loadDir):
        ''' Return the net area the gross area subjected to tension used
            in the calculation of the block shear strength.

        :param loadDir: direction of the load.
        '''
        Agv= self.getGrossAreaSubjectedToShear(loadDir) # gross area subject to shear.
        cover= self.getMinimumCoverInDir(-loadDir) # opposite direction
        return Agv-self.thickness*cover
    
    def getNetAreaSubjectedToBlockShear(self, loadDir):
        ''' Return the net area the net area subjected to tension used
            in the calculation of the block shear strength.

        :param loadDir: direction of the load.
        '''
        Agv= self.getGrossAreaSubjectedToBlockShear(loadDir) # gross area subject to shear.
        holeDiameter= self.boltArray.bolt.getDesignHoleDiameter()
        retval= Agv-self.thickness*(self.boltArray.nRows-0.5)*holeDiameter
        return retval
                    
    def getNominalBlockShearStrength(self, shearVector, Ubs= 0.5):
        ''' Return the nominal shear strength of the plate according to
            clause J.4.3 of AISC 360-16.

        :param shearVector: 2D shear load vector.
        :param Ubs: Reduction coefficient, used in calculating 
                    block shear rupture strength.
        '''
        # (J4-5)
        Ant= self.getNetAreaSubjectedToBlockTension(shearVector)
        tensionStrength= self.steelType.fu*Ubs*Ant
        Anv= self.getNetAreaSubjectedToBlockShear(shearVector)
        Agv= self.getGrossAreaSubjectedToBlockShear(shearVector)
        shearStrength= 0.6*min(self.steelType.fu*Anv,self.steelType.fy*Agv)
        return tensionStrength+shearStrength

    def getDesignBlockShearStrength(self, shearVector, Ubs= 0.5):
        ''' Return the design shear strength of the plate according to
            clause J.4.3 of AISC 360-16.

        :param shearVector: 2D shear load vector.
        :param Ubs: Reduction coefficient, used in calculating 
                    block shear rupture strength.
        '''
        return 0.75*self.getNominalBlockShearStrength(shearVector, Ubs)
    
    def getDesignBlockShearEfficiency(self, shearVector, Ubs= 0.5):
        ''' Return the design shear strength of the plate according to
            clause J.4.2 of AISC 360-16.

        :param shearVector: 2D shear load vector.
        :param Ubs: Reduction coefficient, used in calculating 
                    block shear rupture strength.
        '''
        blockShearStrength= self.getDesignBlockShearStrength(shearVector, Ubs)
        return shearVector.getModulus()/blockShearStrength
    
    def getDesignEfficiency(self, loadVector, Ubs= 0.5):
        ''' Return the efficiency of the fin plate under the load
            argument. Checks bolt shear, bolt bearing and tearout.
            Don't checks weld shear, shear rupture an block shear
            rupture (those will be checked using the CBFEM model).

        :param loadVector: 2D load vector.
        :param Ubs: Reduction coefficient, used in calculating 
                    block shear rupture strength.
        '''
        Rd= loadVector.getModulus()
        retval= self.boltArray.getDesignShearEfficiency(Rd, doubleShear= self.doublePlate)
        retval= max(retval, self.getDesignBearingEfficiency(Rd))
        retval= max(retval, self.getDesignTearoutEfficiency(loadVector))
        retval= max(retval, self.getDesignShearEfficiency(loadVector))
        retval= max(retval, self.getDesignBlockShearEfficiency(loadVector, Ubs))
        return retval
        

def readBoltedPlateFromJSONFile(inputFileName):
    ''' Read bolted plate object from a JSON file.'''
    retval= BoltedPlate()
    retval.jsonRead(inputFileName)
    return retval
        
class AnchorBolt(bolts.AnchorBase):
    ''' ASTM anchor bolt according to table 2.2 from the document
    Base Plate and Anchor Rod Design Second Edition
    American Institute of Steel Construction, Inc.

    :ivar name: bolt identifier
    :ivar steel: steel material.
    '''

    # See table 3.2 of the design guide:
    ab_diams= [0.015875, 0.01905, 0.022225, 0.0254, 0.028575, 0.03175, 0.0381, 0.04445, 0.0508, 0.05715, 0.0635, 0.06985, 0.0762, 0.08255, 0.0889, 0.09525, 0.1016]
    bearingArea=[0.00044451524, 0.00058451496, 0.0007870952, 0.00096774, 0.0011677396, 0.0014451584, 0.0020193508, 0.0026903172, 0.003451606, 0.0043161204, 0.0052709572, 0.006322568, 0.007354824, 0.008580628, 0.009870948, 0.0112903, 0.012838684]
    fBearingArea= scipy.interpolate.interp1d(ab_diams,bearingArea)
    # Threads per inch
    diam_threads_per_inch= [12.7e-3, 14.2875e-3, 15.875e-3, 19.05e-3, 22.225e-3, 25.4e-3, 28.575e-3, 31.75e-3, 25.4e-3, 38.1e-3, 44.45e-3, 50.8e-3, 57.15e-3, 63.5e-3, 69.85e-3, 76.2e-3, 82.55e-3, 88.9e-3, 95.25e-3, 101.6e-3] # Diameters (m)
    threads_per_inch= [13, 12, 11, 10, 9, 8, 7, 7, 6, 6, 5, 4.5, 4.5, 4, 4, 4, 4, 4, 4, 4]
    fThreadsPerInch= scipy.interpolate.interp1d(diam_threads_per_inch, threads_per_inch)
    
    # Hole diameters; see table 2.3 of the design guide:
    rDiams= [0.01905, 0.022225, 0.0254, 0.03175, 0.0381, 0.04445, 0.0508, 0.0635] # rod diameters.
    hDiams= [0.0333375, 0.0396875, 0.0460375, 0.0523875, 0.0587375, 0.06985, 0.08255, 0.08255] # hole diameters.
    fHoleDiameter= scipy.interpolate.interp1d(rDiams,hDiams)

    def __init__(self, name, steel, diameter, pos3d= None, plateWasher= None):
       ''' Constructor.

       :param name: bolt identifier
       :param steel: steel material.
       :param diameter: bolt diameter.
       :param pos3d: bolt position.
       :param plateWasher: plate washer (if any).
       '''
       super(AnchorBolt,self).__init__(diameter, pos3d, plateWasher)
       self.name= name
       self.steelType= steel

    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(AnchorBolt, self).getDict()
        steelTypeClassName= str(self.steelType.__class__)[8:-2]
        retval.update({'name':self.name, 'steelTypeClassName':steelTypeClassName, 'steelType':self.steelType.getDict()})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        super(AnchorBolt, self).setFromDict(dct)
        self.name= dct['name']
        steelTypeClassName= dct['steelTypeClassName']+'()'
        self.steelType= eval(steelTypeClassName)
        self.steelType.setFromDict(dct['steelType'])

    def getNumOfThreadsPerInch(self):
        ''' Return the number of threads per inch.'''
        return self.fThreadsPerInch(self.diameter)
        

    def getTensionEffectiveCrossSectionalArea(self):
        ''' Return the effective cross-sectional area of the
            anchor according to clause R17.4.1.2
            of ACI 318R-14.
        '''
        d_in= self.diameter/units_utils.inchToMeter # diameter in inches.
        nt= self.getNumOfThreadsPerInch()
        deff= (d_in-0.9743/nt)*units_utils.inchToMeter # effective diameter in m.
        return math.pi/4.0*deff**2
    
    def getShearEffectiveCrossSectionalArea(self):
        ''' Return the effective cross-sectional area of the
            anchor according to clause R17.5.1.2
            of ACI 318R-14.
        '''
        return self.getTensionEffectiveCrossSectionalArea()
    
    # Tension
    def getTensileStrength(self):
        ''' Return the tensile strength of the anchor rod.
        '''
        Ag= self.getTensionEffectiveCrossSectionalArea()
        return self.steelType.fu*Ag
    
    def getNominalTensileStrength(self):
        ''' Return the tensile strength of the anchor rod.
        '''
        return self.getTensileStrength()
    
    def getNominalShearStrength(self, typ= 'N'):
        ''' Return the shear strength of the anchor rod.
        '''
        factor= 0.4
        if(typ=='X'):
            factor= 0.5
        return factor*self.getTensileStrength()
    
    def getDesignTensileStrength(self, phi= 0.75):
        ''' Return the tensile strength of the anchor rod.

        :param phi: resistance factor for anchor steel  
                in tension (defaults to 0.75).
        '''
        return phi*self.getNominalTensileStrength()

    def getDesignShearStrength(self, typ= 'N', phi= 0.55):
        ''' Return the shear strength of the anchor rod.

        :param phi: resistance factor for anchor steel  
                in tension (defaults to 0.55).
        '''
        return phi*self.getNominalShearStrength(typ)
    
    def getBearingArea(self):
        ''' Return the bearing area of the anchor according
            to the table 3.2 of the design guide.'''
        return self.fBearingArea(self.diameter)
    
    def getNominalHoleDiameter(self, rounded= False, prec=3, base=.005):
        ''' Return the hole diameter for the anchor according
            to the table 2.3 of the design guide.

        :param rounded: if true return the rounded value.
        '''
        retval= self.fHoleDiameter(self.diameter)
        if(rounded):
            retval= round(base * round(float(retval)/base),prec)
        return retval
    
    def getNominalPulloutStrength(self, fc, psi4= 1.0):
        ''' Return the nominal pullout strength of the anchor 
        rod based on the ACI Appendix D provisions (Section D5.3)

        :param fc: concrete strength.
        :param psi4: 1.4 if the anchor is located in a region of a
                     concrete member where analysis indicates no
                     cracking at service levels, otherwise 1.0
        '''
        Abrg= self.getBearingArea() # the bearing area of the anchor
                                    # rod head or nut.
        if(self.plateWasher):
            Abrg= self.plateWasher.getBearingArea()
        return psi4*Abrg*8.0*fc
    
    def getDesignPulloutStrength(self, fc, psi4= 1.0, phi= 0.7):
        ''' Return the design pullout strength of the anchor 
        rod based on the ACI Appendix D provisions (Section D5.3)

        :param fc: concrete strength.
        :param psi4: 1.4 if the anchor is located in a region of a
                     concrete member where analysis indicates no
                     cracking (f t – f r ) at service levels, 
                     otherwise 1.0
        '''
        return phi*self.getNominalPulloutStrength(fc,psi4)

    def getPulloutEfficiency(self, Pu, fc):
        ''' Return the pullout efficiency of the bolt.

        :param Pu: axial load (LRFD).
        :param fc: concrete strength.
        '''
        Np= self.getNominalPulloutStrength(fc)
        retval= Pu/Np
        return retval

    def getConcreteBreakoutConePolygon(self, h_ef):
        ''' Return the full breakout cone in tension as per ACI 318-02.

        :param h_ef: depth of embedment.
        '''
        retval= geom.Polygon2d()
        delta= 1.5*h_ef
        if(self.plateWasher):
            delta+= math.sqrt(self.plateWasher.getBearingArea())
        if(self.pos3d):
            origin= geom.Pos2d(self.pos3d.x, self.pos3d.y)
            retval.appendVertex(origin+geom.Vector2d(delta,delta))
            retval.appendVertex(origin+geom.Vector2d(-delta,delta))
            retval.appendVertex(origin+geom.Vector2d(-delta,-delta))
            retval.appendVertex(origin+geom.Vector2d(delta,-delta))
        else:
            lmsg.error('Anchor position not specified.')        
        return retval
    
    def getNominalConcreteBreakoutStrength(self, h_ef, fc, psi3= 1.0):
        ''' Return the nominal value of the concrete breakout strength
            for the anchor group as per ACI 318-02, Appendix D.

        :param h_ef: depth of embedment.
        :param fc: concrete strength.
        :param psi3: 1.25 if the anchor is located in a region of a
                     concrete member where analysis indicates no
                     cracking at service levels, otherwise 1.0
        '''
        fc_psi= fc*145.038e-6
        retval= psi3*math.sqrt(fc_psi)
        h_ef_in= h_ef/units_utils.inchToMeter
        if(h_ef_in>25): # embedment length outside CCD method range (0.0 to 0.635 m)
            h_ef_in= 25
        if(h_ef_in<11): 
            retval*= 24*math.pow(h_ef_in,1.5) # Eq. (D-7)
        else:
            retval*= 16*math.pow(h_ef_in,5.0/3.0) # Eq. (D-8)
        retval*= 4.4482216 # pounds to Newtons
        return retval
    
    def getDesignConcreteBreakoutStrength(self, h_ef, fc, psi3= 1.0, phi= 0.7):
        ''' Return the design value of the concrete breakout strength 
            for the anchor group as per ACI 318-02, Appendix D.

        :param h_ef: depth of embedment.
        :param fc: concrete strength.
        :param psi3: 1.25 if the anchor is located in a region of a
                     concrete member where analysis indicates no
                     cracking at service levels, otherwise 1.0
        :param phi: resistance factor for concrete breakout  
                    (defaults to 0.70).
        '''
        nominalValue= self.getNominalConcreteBreakoutStrength(h_ef, fc, psi3= 1.0)
        return phi*nominalValue

    def checkDuctility(self, h_ef, fc):
        ''' Check the ductility of the anchor. If returned value
        is less or equal than 1.0 then ductility OK.

        :param h_ef: depth of embedment.
        :param fc: concrete strength.
        '''
        Np_lim= 0.85*self.getNominalPulloutStrength(fc)
        Nsa= self.getNominalTensileStrength()
        Ncb_lim= 0.85*self.getNominalConcreteBreakoutStrength(h_ef, fc)
        return max(Nsa/Np_lim, Nsa/Ncb_lim)
    
    def report(self, outputFile):
        ''' Write the anchor specification.'''
        super(AnchorBolt,self).report(outputFile)
        outputFile.write('     steel type: '+self.steelType.name+'\n')
    
class AnchorGroup(object):
    ''' Anchor group.

    :ivar anchors: anchor list.
    '''
    
    def __init__(self, steel, diameter, positions, plateWasher= None, postInstalled= False, torqued= False):
        ''' Creates an anchor group in the positions argument.

        :param steel: steel material.
        :param diameter: bolt diameter.
        :param positions: bolt positions.
        :param plateWasher: washer plate for the bolts (if any)
        :param postInstalled: true for post-installed anchors.
        :param torqued: true if anchors are put in tension to be sure
                        that they will never go above that tensile stress, 
                        and the baseplate always has pressure on the concrete. 
                        No stress reversal, therefore, less fatigue.
        '''
        self.anchors= list()
        count= 0
        for p in positions:
            self.anchors.append(AnchorBolt(name= str(count), steel= steel, diameter= diameter, pos3d= p, plateWasher= plateWasher))
            count+= 1
        self.postInstalled= postInstalled
        self.torqued= torqued

    def setPositions(self, positions):
        ''' Set the anchors positions.

        :param positions: list of positions.
        '''
        for anchor, pos in zip(self.anchors, positions):
            anchor.pos3d= pos

    def getSpacing(self):
        ''' Return the spacing between anchors.'''
        points= list()
        for anchor in self.anchors:
            pos= anchor.pos3d
            points.append(geom.Pos2d(pos.x, pos.y))
        return cpp.closest_pair_of_points(points, len(points))

    def getMinimumSpacing(self, rodEfficiency= 1.0):
        ''' Return the minimum spacing according to clause 17.7.1
            and 17.7.4 of ACI 318-14.
        '''
        diameter= self.anchors[0].diameter*math.sqrt(rodEfficiency)
        retval= 4.0*diameter
        if(self.torqued or self.postInstalled):
            retval= 6.0*diameter
        return retval
        
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= dict()
        for anchor in self.anchors:
            retval[anchor.name]= anchor.getDict()
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.anchors= list()
        for key in dct:
            bolt= AnchorBolt(name= '', steel= None, diameter= 0.0, pos3d= None)
            bolt.setFromDict(dct[key])
            self.anchors.append(bolt)
            
    def getNumberOfBolts(self):
        ''' Return the number of anchors in the group.'''
        return len(self.anchors)
    
    def getConcreteBreakoutConePolygon(self, h_ef):        
        ''' Return breakout cone in tension for the group
            as the union of individual cones.

        :param h_ef: depth of embedment.
        '''
        polygons= list()
        for anchor in self.anchors:
            polygons.append(anchor.getConcreteBreakoutConePolygon(h_ef))
        retval= polygons[0]
        for plg in polygons[1:]:
            retval.unePolygon2d(plg)
        return retval

    def getNominalConcreteBreakoutStrength(self, h_ef, fc, psi3= 1.25):
        ''' Return the concrete breakout strength for the anchor
            group as per ACI 318-02, Appendix D.

        :param h_ef: depth of embedment.
        :param fc: concrete strength.
        :param psi3: 1.25 if the anchor is located in a region of a
                     concrete member where analysis indicates no
                     cracking at service levels, otherwise 1.0
        '''
        AN= self.getConcreteBreakoutConePolygon(h_ef).getArea()
        ANo= self.anchors[0].getConcreteBreakoutConePolygon(h_ef).getArea()
        surfRatio= AN/ANo
        # fc_psi= fc*145.038e-6 # remove 20220607
        retval= self.anchors[0].getNominalConcreteBreakoutStrength(h_ef, fc,psi3)*surfRatio
        return retval
    
    def getDesignConcreteBreakoutStrength(self, h_ef, fc, psi3= 1.25, phi= 0.7):
        ''' Return the design value of the concrete breakout strength
            for the anchor group as per ACI 318-02, Appendix D.

        :param h_ef: depth of embedment.
        :param fc: concrete strength.
        :param psi3: 1.25 if the anchor is located in a region of a
                     concrete member where analysis indicates no
                     cracking at service levels, otherwise 1.0
        :param phi: resistance factor for concrete breakout  
                    (defaults to 0.70).
        '''
        nominalValue= self.getNominalConcreteBreakoutStrength(h_ef, fc, psi3)
        return phi*nominalValue
    
    def getHoleBlocks(self, refSys, blockProperties, ownerId):
        ''' Return octagons inscribed in the holes.

        :param refSys: coordinate reference system.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        :param ownerId: identifier of the holed surface.
        '''
        retval= bte.BlockData()
        holeProperties= bte.BlockProperties.copyFrom(blockProperties)
        holeProperties.appendAttribute('objType', 'hole')
        holeProperties.appendAttribute('ownerId', ownerId)
        for anchor in self.anchors:
            retval.extend(anchor.getAnchorBlock(refSys, holeProperties))
        return retval

    def report(self, outputFile):
        ''' Writes anchor group specification.'''
        outputFile.write('   anchors: \n')
        outputFile.write('     post-installed: '+str(self.postInstalled)+'\n')
        outputFile.write('     torqued: '+str(self.postInstalled)+'\n')
        outputFile.write('     number of anchors: '+ str(len(self.anchors))+ ' x '+self.anchors[0].getName()+'\n')
        outputFile.write('     minimum spacing: '+str(self.getSpacing())+' m\n')
        self.anchors[0].report(outputFile)
        
def getSquareAnchorGroup(steel, diameter, squareSide, xCenter= 0.0, yCenter= 0.0):
    ''' Return four anchors in the corners of the square.

    :param steel: steel material.
    :param diameter: bolt diameter.
    :param squareSide: side of the square.
    :param xCenter: x coordinate of the square center.
    :param yCenter: y coordinate of the square center.
    '''
    delta= squareSide/2.0
    origin= geom.Pos3d(xCenter, yCenter, 0.0)
    positions= list()
    positions.append(origin+geom.Vector3d(delta,delta,0.0))
    positions.append(origin+geom.Vector3d(-delta,delta,0.0))
    positions.append(origin+geom.Vector3d(-delta,-delta,0.0))
    positions.append(origin+geom.Vector3d(delta,-delta,0.0))
    return AnchorGroup(steel, diameter, positions)

class ASTMShape(object):
    """Steel shape with ASTM/AISC verification routines.

    :ivar name: shape name (i.e. W40X431)
    """
    def __init__(self, name= ''):
       '''Constructor.

       :param name: name of the shape.
       '''
       self.name= name
       
    def getDict(self):
        ''' Put member values in a dictionary.'''
        return {'name':self.name}

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.name= dct['name']

    def getShearArea(self, majorAxis= True):
        ''' Return area for shear strength calculation (added for compatibility
            with EC3 steel sections).

        :param majorAxis: if true major axis bending; so shear parallel to
                          minor axis.
        '''
        return self.getAw(majorAxis)    

    # Tension
    def getDesignTensileStrength(self, Ae= None):
        ''' Return the tensile strength of the section according
            to section D2 fo AISC-360-16.

        :param Ae: effective net area according to section D3.
        '''
        Ag= self.get('A')
        retval= 0.9*self.steelType.fy*Ag
        if(not Ae):
            Ae= Ag
        retval= min(retval,0.75*self.steelType.fu*Ae)
        return retval

    # Compression
    def getFlexuralSlendernessRatio(self, effectiveLengthY, effectiveLengthZ):
        ''' Return the flexural buckling slenderness ratio of the member.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        '''
        # sc= self.compressionSlendernessCheck()
        # if(sc>1.01):
        #     className= type(self).__name__
        #     methodName= sys._getframe(0).f_code.co_name
        #     lmsg.error(className+'.'+methodName+': member section has slender members. Results are not valid.')
        retval= effectiveLengthZ/self.get('iz')
        return max(retval,effectiveLengthY/self.get('iy'))
    
    def getFlexuralCriticalSlendernessRatio(self, effectiveLengthY, effectiveLengthZ):
        ''' Return the critical value of the flexural buckling 
            slenderness ratio of the member.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
         '''
        sr= self.getFlexuralSlendernessRatio(effectiveLengthY, effectiveLengthZ)
        E= self.get('E')
        Fy= self.steelType.fy
        return sr/math.pi*math.sqrt(Fy/E)
    
    def getFlexuralElasticBucklingStress(self, effectiveLengthY, effectiveLengthZ):
        ''' Return the flexural elastic buckling stress of the member according
            to equation E3-4 of AISC-360-16.
 
        :param effectiveLengthY: effective length of member (minor axis).
        :parem effectiveLengthZ: effective length of member (major axis).
        '''
        sr= self.getFlexuralSlendernessRatio(effectiveLengthY, effectiveLengthZ)
        E= self.get('E')
        return math.pi**2*E/sr**2
    
    def getFlexuralElasticBucklingStressOnAxis(self, effectiveLengthY, effectiveLengthZ, majorAxis= True):
        ''' Return the flexural elastic buckling stress of the member according
            to equations E4-5 or E4-6 of AISC-360-16.
  
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        :param majorAxis: true if flexure about the major axis.
       '''
        if(majorAxis):
            sr= effectiveLengthZ/self.shape.get('iz') # Slenderness ratio for eq. E4-5
        else:
            sr= effectiveLengthY/self.shape.get('iy') # Slenderness ratio for eq. E4-6
        E= self.get('E')
        return math.pi**2*E/sr**2
    
    def getCriticalStressE(self, effectiveLengthY, effectiveLengthZ, Fe):
        ''' Return the critical stress of the member according
            to equations E3-2 and E3-3 of AISC-360-16.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        :param Fe: flexural or torsional elastic buckling stress.
        '''
        sr= self.getFlexuralSlendernessRatio(effectiveLengthY, effectiveLengthZ)
        E= self.get('E')
        Fy= self.steelType.fy
        Fratio= Fy/Fe
        thresholdA= 4.71*math.sqrt(E/Fy)
        if((sr<=thresholdA) or (Fratio<=2.25)):
            retval= math.pow(0.658,Fratio)*Fy # (E3-2)
        else:
            retval= 0.877*Fe
        return retval
    
    def getFlexuralBucklingCriticalStress(self, effectiveLengthY, effectiveLengthZ):
        ''' Return the flexural critical stress of the member according
            to equations E3-2 and E3-3 of AISC-360-16.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        '''
        Fe_flexural= self.getFlexuralElasticBucklingStress(effectiveLengthY, effectiveLengthZ)
        retval= self.getCriticalStressE(effectiveLengthY, effectiveLengthZ, Fe= Fe_flexural)
        return retval
    
    def getTorsionalElasticBucklingStress(self, effectiveLengthX):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2, E4-3 and E4-4 of 
            AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        '''
        return self.getShapeTorsionalElasticBucklingStress(self, effectiveLengthX)
    
    def getTorsionalBucklingCriticalStress(self, effectiveLengthX, effectiveLengthY, effectiveLengthZ):
        ''' Return the torsional critical stress of the member according
            to equations E4-2, E4-3 and E4-4 of AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        '''
        Fe_torsional= self.getTorsionalElasticBucklingStress(effectiveLengthX)
        return self.getCriticalStressE(effectiveLengthY= effectiveLengthY, effectiveLengthZ= effectiveLengthZ, Fe= Fe_torsional)

    def getCompressiveCriticalStress(self, effectiveLengthX, effectiveLengthY, effectiveLengthZ):
        ''' Return the compressive critical stress of the member
            according to equations E3-2 or E3-3 of AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        '''
        retval= self.getFlexuralBucklingCriticalStress(effectiveLengthY, effectiveLengthZ)
        if(effectiveLengthX>max(effectiveLengthY,effectiveLengthZ)):
            # When the torsional effective length is larger than the lateral
            # effective length, Section E4 may control the design of
            # wide-flange and similarly shaped members.
            retval= min(retval, self.getTorsionalBucklingCriticalStress(effectiveLengthX, effectiveLengthY, effectiveLengthZ))
        return retval

    def getNominalCompressiveStrength(self, effectiveLengthX, effectiveLengthY, effectiveLengthZ):
        ''' Return the nominal compressive strength of the member
            according to equation E3-1 of AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        '''
        Ae= self.shape.get('A') # Gross area of member
        Fcr= self.getCompressiveCriticalStress(effectiveLengthX, effectiveLengthY, effectiveLengthZ)
        slendernessCheck= self.compressionSlendernessCheck()
        if(slendernessCheck>1.0):
            Ae= self.getCompressionEffectiveArea(Fcr)
        return Fcr*Ae
    
    def getDesignCompressiveStrength(self, effectiveLengthX, effectiveLengthY, effectiveLengthZ):
        ''' Return the design compressive strength of the member
            according to section E1 of AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        '''
        return 0.9*self.getNominalCompressiveStrength(effectiveLengthX, effectiveLengthY, effectiveLengthZ)
    
    def getReferenceCompressiveStrength(self):
        ''' Return the flexural strength of the section without
            take in to account the lateral buckling effect.

        '''
        # Store result to avoid repeating the same calculation many times.
        if(not hasattr(self, 'referenceCompressiveStrength')):
            self.referenceCompressiveStrength= self.getDesignCompressiveStrength(effectiveLengthX= 0.1, effectiveLengthY= 0.1, effectiveLengthZ= 0.1)
        return self.referenceCompressiveStrength
    
    # Bending 
    def getReferenceFlexuralStrength(self):
        ''' Return the flexural strength of the section without
            take in to account the lateral buckling effect.'''
        # Store result to avoid repeating the same calculation many times.
        if(not hasattr(self, 'referenceFlexuralStrength')):
            self.referenceFlexuralStrength= self.getDesignFlexuralStrength(lateralUnbracedLength= 0.1, Cb= 1.0, majorAxis= True)
        return self.referenceFlexuralStrength

    # Shear
    def getYShearEfficiency(self, Vyd):
        '''Return major axis shear efficiency according to AISC-360-16.

        :param Vyd: required shear strength (major axis)
        '''
        return abs(Vyd)/self.getDesignShearStrength(majorAxis= True)
    
    def getZShearEfficiency(self, Vzd):
        '''Return major axis shear efficiency according to AISC-360-16.

        :param Vz: required shear strength (minor axis)
        '''
        return Vzd/self.getDesignShearStrength(majorAxis= False)
        

    def getZBendingEfficiency(self, Nd, Mzd, Vyd= 0.0, chiN=1.0, chiLT= 1.0):
        '''Return major axis bending efficiency according to section H1
           of AISC-360-16.

        :param Nd: required axial strength.
        :param Mzd: required bending strength (major axis).
        :param Vyd: required shear strength (major axis)
        :param chiN: axial load reduction reduction factor (default= 1.0).
        :param chiLT: lateral buckling reduction factor (default= 1.0).
        '''
        if(Nd<0): # compression
            NcRd= chiN*self.getReferenceCompressiveStrength() # available compressive strength.
            ratioN=  abs(Nd)/NcRd
        else:
            NcRd= self.getDesignTensileStrength() # available axial strength.
            ratioN= Nd/NcRd
        McRdz= self.getReferenceFlexuralStrength() # reference flexural strength major axis.
        # MvRdz= self.getMvRdz(Vyd)
        MvRdz= McRdz
        MbRdz= chiLT*MvRdz # available flexural strength major axis.
        ratioMz= abs(Mzd)/MbRdz
        if(ratioN>=0.2):
            CF= ratioN+8.0/9.0*(ratioMz) # equation H1-1a
        else:
            CF= ratioN/2.0+(ratioMz) # equation H1-1b
        return (CF,NcRd,McRdz,MvRdz,MbRdz)

    
    # Combined internal forces
    def getBiaxialBendingEfficiency(self,Nd,Myd,Mzd,Vyd= 0.0, chiN=1.0, chiLT=1.0):
        '''Return biaxial bending efficiency according to section H1
           of AISC-360-16.

        :param Nd: required axial strength.
        :param Mzd: required bending strength (major axis).
        :param Vyd: required shear strength (major axis)
        :param chiN: axial load reduction reduction factor (default= 1.0).
        :param chiLT: lateral buckling reduction factor (default= 1.0).
        '''
        if(Nd<0): # compression
            NcRd= chiN*self.getReferenceCompressiveStrength() # available axial strength.
            ratioN=  abs(Nd)/NcRd
        else:
            NcRd= self.getDesignTensileStrength() # available axial strength.
            ratioN= Nd/NcRd
        McRdy= self.getDesignFlexuralStrength(lateralUnbracedLength= None, Cb= None, majorAxis= False) # available flexural strength minor axis.
        McRdz= self.getReferenceFlexuralStrength() # reference flexural strength major axis.
        # MvRdz= self.getMvRdz(Vyd)
        MvRdz= McRdz # available flexural strength due to shear interaction.
        MbRdz= chiLT*MvRdz # available flexural strength major axis.
        ratioMz= abs(Mzd)/MbRdz
        ratioMy= abs(Myd)/McRdy
        if(ratioN>=0.2):
            CF= ratioN+8.0/9.0*(ratioMz+ratioMy) # equation H1-1a
        else:
            CF= ratioN/2.0+(ratioMz+ratioMy) # equation H1-1b
        return (CF,NcRd,McRdy,McRdz,MvRdz,MbRdz)

    def setupULSControlVars(self, elems, chiN=1.0, chiLT=1.0):
        '''For each element creates the variables
           needed to check ultimate limit state criterion to be satisfied.

        :param elems: elements to define properties on.
        :param chiN: axial load reduction reduction factor (default= 1.0).
        :param chiLT: lateral buckling reduction factor (default= 1.0).
        '''
        super(ASTMShape,self).setupULSControlVars(elems)
        for e in elems:
            e.setProp('chiLT',chiLT) #Lateral torsional buckling reduction factor.
            e.setProp('chiN',chiN) # Axial strength reduction factor.
            e.setProp('crossSection',self)

    def getProbableMaxMomentAtPlasticHinge(self, majorAxis):
        ''' Return the probable maximum moment at plastic hinge according
            to clause 2.4.3 of AISC-358.'''
        Mp= self.getPlasticMoment(majorAxis)
        Cpr= self.steelType.getPeakConnectionStrengthFactor()
        Ry= self.steelType.Ry
        return Cpr*Ry*Mp
        
    def getFlangeMaximumBoltDiameter(self):
        ''' Return the maximum bolt diameter to prevent beam flange 
            tensile rupture according to equation 7.6-2M
            of AISC 358-16.'''
        bf_2= self.getFlangeWidth()/2.0
        factor= self.steelType.getEquation7_6_2Factor()
        return bf_2*factor-3e-3

    def getFlangeMaximumHoleDiameter(self):
        ''' Return the maximum hole diameter to prevent beam flange 
            tensile rupture according to equation 7.6-2M
            of AISC 358-16.'''
        bd= self.getFlangeMaximumBoltDiameter()
        return bd+BoltFastener.diameterIncrement

    def getFlangeMaximumBolt(self, boltSteelType= A307):
        ''' Return the maximum bolt to prevent beam flange 
            tensile rupture according to equation 7.6-2M
            of AISC 358-16.

        :param boltSteelType: bolt steel type. 
        '''
        return getBoltForHole(self.getFlangeMaximumBoltDiameter(), boltSteelType)

    def getWebSuitableBolt(self, ratio= 2.5, boltSteelType= A307):
        ''' Return the a suitable bolt to be used in the profile web
            based on the web thickness.

        :param ratio: bolt diameter / web thickness ratio. 
        :param boltSteelType: bolt steel type. 
        '''
        return getBoltForHole(ratio*self.getWebThickness(), boltSteelType)

    def getFlangeGrossArea(self):
        ''' Return the gross area of the flange.'''
        return self.getFlangeWidth()*self.getFlangeThickness()
        
    def getFlangeYieldStrength(self):
        ''' Return the yield strength of the flange.'''
        stress= self.steelType.getYt()*self.steelType.fy
        return stress*self.getFlangeGrossArea()

            
from materials.sections.structural_shapes import aisc_metric_shapes

class WShape(ASTMShape,aisc_metric_shapes.WShape):
    """W shape with ASTM/AISC verification routines."""
    def __init__(self,steel= None, name= ''):
        ''' Constructor.

         :param steel: steel material (i.e. A36).
         :param name: shape name (i.e. W40X431)
        '''
        super(WShape, self).__init__(name= name)
        aisc_metric_shapes.WShape.__init__(self, steel= steel, name= name)
        
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= ASTMShape.getDict(self)
        retval.update(aisc_metric_shapes.WShape.getDict(self))
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        ASTMShape.setFromDict(self, dct)
        aisc_metric_shapes.WShape.setFromDict(self, dct)

    def getTorsionalElasticBucklingStress(self, effectiveLengthX):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2, E4-3 and E4-4 of 
            AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        '''
        return super(WShape, self).getTorsionalElasticBucklingStress(effectiveLengthX)

class IShape(ASTMShape, aisc_metric_shapes.IShape):
    """I shape with ASTM/AISC verification routines."""

    def __init__(self, bf, tf, tw, hw, steel=None, name=''):
        ''' Constructor.

        :param bf: Flange width
        :param tf: Flange thickness
        :param tw: Web thickess
        :param hw: Web height
        :param steel: steel material (i.e. A36).
        :param name: shape name (i.e. W40X431)
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.IShape.__init__(self, bf, tf, tw, hw, steel, name)

    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval = ASTMShape.getDict(self)
        retval.update(aisc_metric_shapes.IShape.getDict(self))
        return retval

    def setFromDict(self, dct):
        ''' Read member values from a dictionary.'''
        ASTMShape.setFromDict(self, dct)
        aisc_metric_shapes.IShape.setFromDict(self, dct)

    def getTorsionalElasticBucklingStress(self, effectiveLengthX):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2, E4-3 and E4-4 of 
            AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        '''
        return super(IShape, self).getTorsionalElasticBucklingStress(effectiveLengthX)

class CShape(ASTMShape,aisc_metric_shapes.CShape):
    """C shape with ASTM/AISC verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material (i.e. A36).
        :param name: shape name  (i.e. C380X74).
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.CShape.__init__(self,steel,name)
        
    def getFlangeYieldStrength(self):
        ''' Return the yield strength of the flange.'''
        grossArea= self.getFlangeWidth()*self.getFlangeThickness()
        return self.steelType.getYt()*self.steelType.fy*grossArea
    
    def getFlangeMaximumBoltDiameter(self):
        ''' Return the maximum bolt diameter to prevent beam flange 
            tensile rupture according to equation 7.6-2M
            of AISC 358-16.'''
        bf= self.getFlangeWidth() # Only one bolt per row.
        factor= self.steelType.getEquation7_6_2Factor()
        return bf*factor-3e-3
    
    def getTorsionalElasticBucklingStress(self, effectiveLengthX):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2, E4-3 and E4-4 of 
            AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        '''
        return super(CShape, self).getTorsionalElasticBucklingStress(effectiveLengthX)
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(CShape, self).getDict()
        retval.update(aisc_metric_shapes.CShape.getDict(self))
        return retval

    def setFromDict(self, dct):
        ''' Read member values from a dictionary.'''
        super(CShape, self).setFromDict(dct)
        aisc_metric_shapes.CShape.setFromDict(self, dct)

# *************************************************************************
# AISC single angle profiles.
# *************************************************************************
#         Section axis:         *                *
#      AISC            XC       *    Points:     *    Principal axes:
#                               *                *
#         ^ Y           ^ y     *                *   
#         |             |       *                *    Z-axis
#                               *     A          *       \  |       W-axis
#       |             |         *      |         *        \ |     /
#       | -> X        | -> z    *      |         *         \|   /
#       |             |         *      |         *          | +
#       -------       -------   *     B-------C  *          ----------
#
# *************************************************************************
class SimpleLShape(ASTMShape, aisc_metric_shapes.SimpleLShape):
    '''Single angle shape with simplified ASTM/AISC verification routines.'''
    
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material (i.e. A36).
        :param name: shape name.
        '''
        super(SimpleLShape, self).__init__(name= name)
        aisc_metric_shapes.SimpleLShape.__init__(self, steel= steel, name= name)
        
    def getCriticalStressE(self, effectiveLengthY, effectiveLengthZ, Fe):
        ''' Return the critical stress of the member according
            to equations E3-2 and E3-3 of AISC-360-16.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        :param Fe: flexural or torsional elastic buckling stress.
        '''
        return aisc_metric_shapes.SimpleLShape.getCriticalStressE(self, effectiveLengthY)
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(SimpleLShape, self).getDict()
        retval.update(aisc_metric_shapes.SimpleLShape.getDict(self))
        return retval

    def setFromDict(self, dct):
        ''' Read member values from a dictionary.'''
        super(SimpleLShape, self).setFromDict(dct)
        aisc_metric_shapes.SimpleLShape.setFromDict(self, dct)

class LShape(ASTMShape, aisc_metric_shapes.LShape):
    """Single angle shape with ASTM/AISC verification routines."""
    
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material (i.e. A36).
        :param name: shape name  (i.e. L6x6x1/2).
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.LShape.__init__(self,steel,name)
        
    def getCriticalStressE(self, effectiveLengthY, effectiveLengthZ, Fe):
        ''' Return the critical stress of the member according
            to equations E3-2 and E3-3 of AISC-360-16.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        :param Fe: flexural or torsional elastic buckling stress.
        '''
        return aisc_metric_shapes.LShape.getCriticalStressE(self, effectiveLengthY)
        
    def getBiaxialBendingEfficiencyPrincipalAxes(self,Nd, MZ, MW, chiN=1.0, chiLT=1.0):
        '''Return biaxial bending efficiency according to section H2
           of AISC-360-16.

        :param Nd: required axial strength.
        :param MZ: required shear strength (minor axis)
        :param MW: required bending strength (major axis).
        :param chiN: axial load reduction reduction factor (default= 1.0).
        :param chiLT: lateral buckling reduction factor (default= 1.0).
        '''
        if(Nd<0):
            Pn= chiN*0.9*self.getReferenceCompressiveStrength()
            ratioN= -Nd/Pn
        elif(Nd==0):
            ratioN= 0.0
        else:
            NcRd= self.getDesignTensileStrength() # available axial strength.
            ratioN= Nd/NcRd
        # Flexural strength.
        MnZ= 0.9*self.getZAxisNominalFlexuralStrength(MZ= MZ) # available flexural strength minor axis.
        MnW= chiLT*0.9*self.getWAxisNominalFlexuralStrength(lateralUnbracedLength= 0.1, Cb= 1.0, MW= MW) # available flexural strength major axis.
        ratioZ= MZ/MnZ
        ratioW= MW/MnW
        # Positions relative to principal axes.
        pA, pB, pC= self.getPrincipalAxesABCPositions()
        # At point B
        cfb_value= ratioN
        if(abs(pB.x)>1e-3):
            if(pB.x<0):
                cfb_value-= ratioZ
            elif(pB.x>0):
                cfb_value+= ratioZ
        if(abs(pB.y)>1e-3):
            if(pB.y<0):
                cfb_value-= ratioW
            elif(pB.y>0):
                cfb_value+= ratioW
        CFB= abs(cfb_value)
        # At point C
        cfc_value= ratioN
        if(abs(pC.x)>1e-3):
            if(pC.x<0):
                cfc_value-= ratioZ
            elif(pC.x>0):
                cfc_value+= ratioZ
        if(abs(pC.y)>1e-3):
            if(pC.y<0):
                cfc_value+= ratioW
            elif(pC.y>0):
                cfc_value-= ratioW
        CFC= abs(cfc_value)
        # At point A
        cfa_value= ratioN
        if(abs(pA.x)>1e-3):
            if(pA.x<0):
                cfa_value-= ratioZ
            elif(pA.x>0):
                cfa_value+= ratioZ
        if(abs(pA.y)>1e-3):
            if(pA.y<0):
                cfa_value-= ratioW
            elif(pA.y>0):
                cfa_value+= ratioW
        CFA= abs(cfa_value)
        CF= max(CFA,CFB,CFC)
        MbRdz, McRdy= self.getGeometricAxesMoments(MW= MnW, MZ= MnZ)
        McRdz= MbRdz/chiLT
        NcRd= 0.0 # available compressive strength.
        MvRdz= McRdz # available flexural strength due to shear interaction.
        return (CF,NcRd,McRdy,McRdz,MvRdz,MbRdz)
    
    def getBiaxialBendingEfficiency(self,Nd,Myd,Mzd,Vyd= 0.0, chiN=1.0, chiLT=1.0):
        '''Return biaxial bending efficiency according to section H2
           of AISC-360-16.

        :param Nd: required axial strength.
        :param Mzd: required bending strength (major axis).
        :param Vyd: required shear strength (major axis)
        :param chiN: axial load reduction reduction factor (default= 1.0).
        :param chiLT: lateral buckling reduction factor (default= 1.0).
        '''
        # Moments about principal axes (signs inverted).
        MW, MZ= self.getPrincipalAxesMoments(Mz= -Mzd, My= -Myd)
        return self.getBiaxialBendingEfficiencyPrincipalAxes(Nd= Nd, MZ= MZ, MW= MW, chiN= chiN, chiLT= chiLT)
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(LShape, self).getDict()
        retval.update(aisc_metric_shapes.LShape.getDict(self))
        return retval

    def setFromDict(self, dct):
        ''' Read member values from a dictionary.'''
        super(LShape, self).setFromDict(dct)
        aisc_metric_shapes.LShape.setFromDict(self, dct)

class HSSShape(ASTMShape,aisc_metric_shapes.HSSShape):
    """Rectangular HSS shape with ASTM/AISC verification routines."""
    
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material (i.e. A36).
        :param name: shape name.
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.HSSShape.__init__(self,steel,name)
        
    def getTorsionalElasticBucklingStress(self, effectiveLengthX):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2, E4-3 and E4-4 of 
            AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        '''
        return super(HSSShape, self).getTorsionalElasticBucklingStress(effectiveLengthX)
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(HSSShape, self).getDict()
        retval.update(aisc_metric_shapes.HSSShape.getDict(self))
        return retval

    def setFromDict(self, dct):
        ''' Read member values from a dictionary.'''
        super(HSSShape, self).setFromDict(dct)
        aisc_metric_shapes.HSSShape.setFromDict(self, dct)
 
class CHSSShape(ASTMShape,aisc_metric_shapes.CHSSShape):
    """Circular HSS shape with ASTM/AISC verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material (i.e. A36).
        :param name: shape name.
         '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.CHSSShape.__init__(self,steel,name)
        
    def getTorsionalElasticBucklingStress(self, effectiveLengthX):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2, E4-3 and E4-4 of 
            AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        '''
        return super(CHSSShape, self).getTorsionalElasticBucklingStress(effectiveLengthX)
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(CHSSShape, self).getDict()
        retval.update(aisc_metric_shapes.CHSSShape.getDict(self))
        return retval

    def setFromDict(self, dct):
        ''' Read member values from a dictionary.'''
        super(CHSSShape, self).setFromDict(dct)
        aisc_metric_shapes.CHSSShape.setFromDict(self, dct)

class HFSHSShape(ASTMShape,aisc_metric_shapes.HFSHSShape):
    """BS EN 10210-2: 2006 steel shapes with ASTM/AISC
       verification routines.
    """
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. 'HFSHS300x300x10.0')
        '''
        ASTMShape.__init__(self,name)
        aisc_metric_shapes.HFSHSShape.__init__(self,steel,name)

    def getTorsionalElasticBucklingStress(self, effectiveLengthX):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2, E4-3 and E4-4 of 
            AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        '''
        return super(HFSHSShape, self).getTorsionalElasticBucklingStress(effectiveLengthX)
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(HFSHSShape, self).getDict()
        retval.update(aisc_metric_shapes.HFSHSShape.getDict(self))
        return retval

    def setFromDict(self, dct):
        ''' Read member values from a dictionary.'''
        super(HFSHSShape, self).setFromDict(dct)
        aisc_metric_shapes.HFSHSShape.setFromDict(self, dct)

def getCrossSections(steelShapes, steel_W= A992, steel_C= A36, steel_L= A36, steel_HSS= A500):
    ''' Return a dictionary containing the cross section definition for each
        of the structural shapes in the steelShapes arguments.

       :param steelShapes: string identifiers of the steel shapes.
       :param steel_W: steel for the wide flange steel shapes (defaults to A992).
       :param steel_C: steel for the channel steel shapes (defaults to A36).
       :param steel_L: steel for the single-angle steel shapes (defaults to A36).
       :param steel_HSS: steel for the hollow structural steel shapes (defaults to A500).
    '''
    retval= dict()
    crossSection= None
    for ss in steelShapes:
        if(ss.startswith('W')):
            crossSection= WShape(steel_W,ss)
        elif(ss.startswith('C')):
            crossSection= CShape(steel_C,ss)
        elif(ss.startswith('L')):
            crossSection= LShape(steel_L,ss)
        elif(ss.startswith('HSS')):
            crossSection= HSSShape(steel_HSS,ss)
        else:
            funcName= sys._getframe(0).f_code.co_name
            lmsg.error(funcName+': '+ss+'shape not implemented yet. Returning None.')
            crossSection= None
        retval[ss]= crossSection
    return retval
 
        
 
class BendingState(object):
    ''' Bending moments along the member.

        :ivar Mmax: absolute value of maximum moment in the unbraced 
                     segment.
        :ivar Ma: absolute value of moment at quarter point of the 
                   unbraced segment.
        :ivar Mb: absolute value of moment at centerline of the 
                   unbraced segment.
        :ivar Mc: absolute value of moment at three-quarter point of 
                   the unbraced segment.
    '''
    def __init__(self, Mmax, Ma, Mb, Mc):
        ''' Constructor.'''
        self.Mmax= Mmax
        self.Ma= Ma
        self.Mb= Mb
        self.Mc= Mc
        
    def getLateralTorsionalBucklingModificationFactor(self):
        ''' Return the lateral-torsional buckling modification factor Cb
        for non uniform moment diagrams when both ends of the segment
        are braced according to expression F1-1 of AISC 360-16.
        '''
        return 12.5*self.Mmax/(2.5*self.Mmax+3*self.Ma+4*self.Mb+3*self.Mc)

class MemberConnection(member_base.MemberConnection):
    '''Member length and connections

       :ivar L: member length.
       :ivar Lb: Length between points that are either braced 
                 against lateral displacement of compression 
                 flange or braced against twist of the cross section.
       :ivar rotI: fixity of the rotation at member start.
       :ivar transI: fixity of the translation at member start.
       :ivar rotJ: fixity of the rotation at member end.
       :ivar transJ: fixity of the translation at member end.
    '''
    def __init__(self,L,rotI='free',transI='fixed',rotJ= 'free',transJ= 'fixed'):
        '''Constructor.'''
        super(MemberConnection, self).__init__(rotI,transI,rotJ,transJ)
        self.L= L
        self.Lb= L
        
    def getLateralTorsionalBucklingModificationFactor(self,bendingState):
        ''' Return the lateral-torsional buckling modification factor Cb
            for non uniform moment diagrams when both ends of the segment
            are braced according to expression F1-1 of AISC 360-16.

            :param Mmax: absolute value of maximum moment in the unbraced 
                         segment.
            :param Ma: absolute value of moment at quarter point of the 
                       unbraced segment.
            :param Mb: absolute value of moment at centerline of the 
                       unbraced segment.
            :param Mc: absolute value of moment at three-quarter point of 
                       the unbraced segment.
        '''
        if(self.transJ=='free' and self.rotJ=='free'): # cantilever
            return 1.0
        else:
            return bendingState.getLateralTorsionalBucklingModificationFactor()

class MemberWithEndConnections(object):
    '''Steel member with end connections with ASTM/AISC verification routines.'''
    def __init__(self,shape,connection):
        ''' Constructor.

        :ivar shape: structural shape.
        :ivar connection: member length and connection information.
        '''
        self.shape= shape
        self.connection= connection
        
    def getEffectiveLength(self):
        '''Return the member effective length according to
           section E2 of AISC 360-16.'''
        K= self.connection.getEffectiveBucklingLengthCoefficientRecommended()
        return  K*self.connection.L #Effective length of member.
    
    def getSlendernessRatio(self):
        '''Return the slenderness ratio of the member.'''
        Lc= self.getEffectiveLength()
        r= min(self.shape.get('iy'),self.shape.get('iz'))
        return Lc/r
    
    def getElasticBucklingStress(self):
        '''Return the elastic buckling stress of the member.'''
        return math.pi**2*self.shape.steelType.E/(self.getSlendernessRatio())**2
    
    def getCriticalStress(self):
        '''Return the critical stress as definded in
           section E7 of AISC 360-16.
        '''
        treshold= 4.71*math.sqrt(self.shape.steelType.E/self.shape.steelType.fy)
        r= self.getSlendernessRatio()
        Fe= self.getElasticBucklingStress()
        fy_fe= self.shape.steelType.fy/Fe
        if((r<=treshold) or (fy_fe<=2.25)):
            return math.pow(0.658,fy_fe)*self.shape.steelType.fy
        else:
            return 0.877*Fe

    def getCompressiveCriticalStress(self):
        ''' Return the compressive critical stress of the member.'''
        effectiveLengthY= self.getEffectiveLength()
        effectiveLengthZ= effectiveLengthY
        effectiveLengthX= effectiveLengthY/100.0 # Torsional buckling neglected.
        return self.shape.getCompressiveCriticalStress(effectiveLengthX, effectiveLengthY, effectiveLengthZ)
    
        
    def getNominalCompressiveStrength(self):
        ''' Return the nominal compressive strength according to
            section E7 of AISC 360-16.
        '''
        effectiveLengthY= self.getEffectiveLength()
        effectiveLengthZ= effectiveLengthY
        effectiveLengthX= effectiveLengthY/100.0 # Torsional buckling neglected.
        return self.shape.getNominalCompressiveStrength(effectiveLengthX, effectiveLengthY, effectiveLengthZ)
    
    def getZLateralTorsionalBucklingFlexuralStrength(self, bendingState):
        ''' Return the maximum flexural strength
            due to web local buckling according to
            expressions F7-10 to F7-11 of AISC 360-16

            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getLateralTorsionalBucklingLimit(Lb= self.connection.Lb, Cb= cb, majorAxis= True)
    
    def getZNominalFlexuralStrength(self, bendingState, AfgAfnRatio= 1.0):
        ''' Return the nominal flexural strength
            around z axis.


        :param bendingState: bending moments along the member.
        :param AfgAfnRatio: value of gross area of tension flange calculated
                            in accordance with the provisions of Section
                            B4.3a of AISC 360-16 divided by the net area
                            of tension flange calculated in accordance with
                            the provisions of Section B4.3b. 
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        retval= self.shape.getNominalFlexuralStrength(lateralUnbracedLength= self.connection.Lb, Cb= cb, majorAxis= True)
        if(AfgAfnRatio>1.0): # Holes in flange
            Yt= self.shape.steelType.getYt()
            Fu= self.shape.steelType.fu
            if(Fu<self.steelType.fy*Yt*AfgAfnRatio):
                Sx= self.get('Wyel')
                Mn= Fu/AfgAfnRatio*Sx # Equation F13-1
                retval= min(retval,Mn) 
        return retval
        
    def getYLateralTorsionalBucklingFlexuralStrength(self, bendingState):
        ''' Return the maximum flexural strength
            due to web local buckling according to
            expressions F7-10 to F7-11 of AISC 360-16

            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getLateralTorsionalBucklingLimit(Lb= self.connection.Lb, Cb= cb, majorAxis= False)
    
    def getYNominalFlexuralStrength(self, bendingState):
        ''' Return the nominal flexural strength
            around z axis.

            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getNominalFlexuralStrength(lateralUnbracedLength= self.connection.Lb, Cb= cb, majorAxis= False)
    
    def getCapacityFactor(self,Nd,Myd,Mzd,gammaC,bendingStateY,bendingStateZ, AfgAfnRatio= 1.0):
        ''' Return the capacity factor according to section
            H1 of AISC 360-16.

        :param Lb: Length between points that are either braced 
                   against lateral displacement of compression 
                   flange or braced against twist of the cross section.
        :param Nd: design value for the axial load (negative for compression).
        :param Mzd: design value for the bending moment around z axis.
        :param Myd: design value for the bending moment around z axis.
        :param bendingStateY: y bending moments along the member.
        :param bendingStateZ: z bending moments along the member.
        :param AfgAfnRatio: value of gross area of tension flange calculated
                            in accordance with the provisions of Section
                            B4.3a of AISC 360-16 divided by the net area
                            of tension flange calculated in accordance with
                            the provisions of Section B4.3b. 
        '''
        if(Nd<=0.0):
            Pn= self.getNominalCompressiveStrength()/gammaC
            Mnz= self.getZNominalFlexuralStrength(bendingStateZ, AfgAfnRatio)/gammaC
            Mny= self.getYNominalFlexuralStrength(bendingStateY)/gammaC
            ratioN= abs(Nd)/Pn
            Msum= (abs(Mzd)/Mnz+abs(Myd)/Mny)
            if(ratioN>=0.2):
                return ratioN+8/9.0*Msum # (H1-1a)
            else:
                return ratioN/2.0+Msum # (H1-1b)

        else:
            lmsg.error('Capacity factor not implemented for tension.')

class ConnectedMember(connected_members.ConnectedMemberMetaData):
    ''' Connected member.'''

    def getFlangeBoltArray(self, boltSteel):
        ''' Return a suitable bolted array for the beam flange.

        :param boltSteel: steel type of the bolts that connect the plate with
                          the flange.
        '''
        flangeStrength= self.shape.getFlangeYieldStrength()
        bolt= self.shape.getFlangeMaximumBolt(boltSteelType= boltSteel)
        numberOfBolts= bolt.getNumberOfBoltsForShear(flangeStrength, numberOfRows= 2, threadsExcluded= True)
        spacing= self.shape.getFlangeWidth()/2.0
        return BoltArray(bolt, nRows= 2, nCols= int(numberOfBolts/2), dist= spacing)
    
    def getFlangeBoltedPlateCore(self, boltSteel, plateSteel):
        ''' Return a minimal bolted plate for the beam flange. The
        length of the plate is just enough to accomodate the bolts
        and its width is chosen as the maximum of the flange width
        and the bolted plate minimum width.

        :param boltSteel: steel type of the bolts that connect the plate with
                          the flange.
        :param plateSteel: steel type of the bolted plate.
        '''
        # Compute bolt arangement.
        boltArray= self.getFlangeBoltArray(boltSteel)
        # Compute bolted plate dimensions.
        thicknessRatio= max(self.shape.steelType.fy/plateSteel.fy, self.shape.steelType.fu/plateSteel.fu)
        plateThickness= round(thicknessRatio*self.shape.getFlangeThickness()*1000,0)/1000
        plateWidth= max(self.shape.getFlangeWidth(), boltArray.getMinPlateWidth())
        plateLength= boltArray.getStandardPlateLength()
        return BoltedPlate(boltArray, width= plateWidth, length= plateLength, thickness= plateThickness, steelType= plateSteel)

    def getShearTabBoltArray(self, shearEfficiency, boltSteelType= A307, threadsExcluded= False):
        ''' Return a suitable bolted array for the shear tab.

        :param shearEfficiency: ratio between the design shear and 
                                the shear strength.
        :param boltSteelType: bolt steel type. 
        '''
        # Compute the shear to resist.
        shear= shearEfficiency*self.shape.getDesignShearStrength(majorAxis= True)
        # Get a suitable bolt.
        bolt= self.shape.getWebSuitableBolt(boltSteelType= A307)
        # Compute the number of bolts needed.
        numberOfBolts= bolt.getNumberOfBoltsForShear(shear, numberOfRows= 1, threadsExcluded= threadsExcluded)
        # check number of bolts.
        if(numberOfBolts<2):
            numberOfBolts= 2
        elif(numberOfBolts>7):
            lmsg.warning('too many bolts: '+str(numberOfBolts))
        defaultSpacing= 75e-3 # about 3 inches. See General Requirements in Design of Single Plate Shear Connections
                              #                 Abolhassan Astaneh, Steven M. Call and Kurt M. McMullin
        spacing= defaultSpacing
        # check spacing.
        distBetweenToes= self.shape.getDistanceBetweenWebToes()
        # depth= self.shape.h() # remove 20220607
        if(distBetweenToes<numberOfBolts*spacing):
            fmt= '{:.2f}'
            oldSpacing= spacing
            newSpacing= distBetweenToes/numberOfBolts
            incrementFraction= (newSpacing-oldSpacing)/oldSpacing
            warningMessage= 'This steel shape has not enough distance between toes to accomodate '+str(numberOfBolts)+' bolts; '
            if(incrementFraction>-0.05): # reduce spacing
                warningMessage+= ' spacing will be reduced from: '+fmt.format(oldSpacing*1e3)+' mm to: '+fmt.format(newSpacing*1e3)+' mm ('+fmt.format(incrementFraction*100.0)+') %.'
                lmsg.warning(warningMessage)
                spacing= newSpacing
            else: # reduce number of bolts and increment bolt diameter
                oldBoltName= str(bolt)
                oldNumberOfBolts= numberOfBolts
                newNumberOfBolts= max(int(distBetweenToes/spacing), 2) # two bolts minimum
                newSpacing= min(distBetweenToes/newNumberOfBolts, defaultSpacing) # new spacing value.
                bolt= getBoltForShear(shearForce= shear, numberOfBolts= newNumberOfBolts)
                warningMessage+= 'bolt diameter will be augmented from '+oldBoltName+' to '+str(bolt)+' mm and number of bolts will be reduced from '+str(oldNumberOfBolts)+' to '+str(newNumberOfBolts)+'. '
                if(newSpacing<spacing): # this can be a problem.
                    warningMessage+= 'Even with less bolts ('+str(newNumberOfBolts)+') spacing will be reduced from '+str(spacing*1e3)+' mm to '+str(newSpacing*1e3)+' mm.'
                    lmsg.warning(warningMessage)
                numberOfBolts= newNumberOfBolts
                spacing= newSpacing
        return BoltArray(bolt, nRows= 1, nCols= numberOfBolts, dist= spacing)
    
    def getShearTabCore(self, boltSteel, plateSteel, shearEfficiency):
        ''' Return a minimal shear tab. The
        length of the plate is just enough to accomodate the bolts
        and its width is chosen as three inches.

        :param boltSteel: steel type of the bolts that connect the plate with
                          the flange.
        :param plateSteel: steel type of the bolted plate.
        :param shearEfficiency: ratio between the design shear and 
                                the shear strength.
        '''
        # Compute bolt arangement.
        boltArray= self.getShearTabBoltArray(shearEfficiency, boltSteel)
        # Compute bolted plate dimensions.
        boltDiameter= boltArray.bolt.diameter
        plateThickness= round((0.5*boltDiameter+1.6e-3)*1000,0)/1000 # See General Requirements in Design of Single Plate Shear Connections
                                                                     # Abolhassan Astaneh, Steven M. Call and Kurt M. McMullin
        plateWidth= max(75e-3, boltArray.getMinPlateWidth())
        plateLength= boltArray.getStandardPlateLength()
        return BoltedPlate(boltArray, width= plateWidth, length= plateLength, thickness= plateThickness, steelType= plateSteel, notched= True)
    
    def getTopFlangeBoltedPlate(self, connectionOrigin, column, boltSteel, plateSteel):
        ''' Return the top flange bolted plate.

        :param connectionOrigin: origin for the connection.
        :param column: column to which the beam is attached to. 
        :param boltSteel: steel type of the bolts that connect the plate with
                          the flange.
        :param plateSteel: steel type of the bolted plate.
        '''
        beamTopFlangePlate= self.getFlangeBoltedPlateCore(boltSteel= boltSteel, plateSteel= plateSteel)
        # Top plate
        ## Compute top plate reference system.
        beamTopFlangePlateRefSys= self.getTopFlangeBoltedPlateRefSys(connectionOrigin, beamTopFlangePlate)
        beamTopFlangePlate.setRefSys(beamTopFlangePlateRefSys)
        ## Compute the intersection of the column axis with the flange plate
        ## midplane.
        columnCenter= beamTopFlangePlateRefSys.getXYPlane().getIntersection(column.getAxis())
        beamTopFlangePlate.attachedMemberCenter= columnCenter 
        ## Set location properties of the plate.
        beamTopFlangePlate.connectedTo= self.connectedTo
        beamTopFlangePlate.location= 'beam_top_flange'
        beamTopFlangePlate.distBetweenPlates= (self.shape.getFlangeThickness()+beamTopFlangePlate.thickness)/2.0
        ## Compute connection lines
        self.computeWeldLines(column, beamTopFlangePlate)
        return beamTopFlangePlate
    
    def getBottomFlangeBoltedPlate(self, connectionOrigin, column, boltSteel, plateSteel):
        ''' Return the blocks corresponding to the bottom flange bolted plate.

        :param connectionOrigin: origin for the connection.
        :param column: column to which the beam is attached to. 
        :param boltSteel: steel type of the bolts that connect the plate with
                          the flange.
        :param plateSteel: steel type of the bolted plate.
        '''
        beamBottomFlangePlate= self.getFlangeBoltedPlateCore(boltSteel= boltSteel, plateSteel= plateSteel)
        # Bottom plate
        ## Compute bottom plate reference system.
        beamBottomFlangePlateRefSys= self.getBottomFlangeBoltedPlateRefSys(connectionOrigin, beamBottomFlangePlate)
        beamBottomFlangePlate.setRefSys(beamBottomFlangePlateRefSys)
        ## Compute the intersection of the column axis with the flange plate
        ## midplane.
        columnCenter= beamBottomFlangePlateRefSys.getXYPlane().getIntersection(column.getAxis())
        beamBottomFlangePlate.attachedMemberCenter= columnCenter        
        ## Set location properties of the plate.
        beamBottomFlangePlate.connectedTo= self.connectedTo
        beamBottomFlangePlate.location= 'beam_bottom_flange'
        beamBottomFlangePlate.distBetweenPlates= (self.shape.getFlangeThickness()+beamBottomFlangePlate.thickness)/2.0
        ## Compute connection lines
        self.computeWeldLines(column, beamBottomFlangePlate)
        return beamBottomFlangePlate
    
    def getShearTab(self, connectionOrigin, column, boltSteel, plateSteel, shearEfficiency):
        ''' Return the blocks corresponding to the shear tab.

        :param connectionOrigin: origin for the connection.
        :param column: column to which the shear tab is attached to. 
        :param boltSteel: steel type of the bolts that connect the plate with
                          the beam web.
        :param plateSteel: steel type of the shear tab.
        :param shearEfficiency: ratio between the design shear and 
                                the shear strength.
        '''
        shearTab= self.getShearTabCore(boltSteel= boltSteel, plateSteel= plateSteel, shearEfficiency= shearEfficiency)
        # Top plate
        ## Compute shear tab reference system.
        shearTabRefSys= self.getShearTabRefSys(connectionOrigin, shearTab, positiveSide= True)
        shearTab.setRefSys(shearTabRefSys)
        ## Compute the intersection of the column axis with the shear tab
        ## equatorial.
        columnCenter= shearTabRefSys.getYZPlane().getIntersection(column.getAxis())
        shearTab.attachedMemberCenter= columnCenter
        ## Set location properties of the plate.
        shearTab.connectedTo= self.connectedTo
        shearTab.location= 'beam_web'
        shearTab.distBetweenPlates= (self.shape.getWebThickness()+shearTab.thickness)/2.0
        ## Compute welded connection lines
        self.computeWeldLines(column, shearTab)
        return shearTab

class FilletWeld(object):
    '''Fillet weld according to chapter J, section J2, of AISC 360-16, 

    :ivar legSize: weld size (leg dimension or actual throat) (t)
    :ivar fillerMetal: weld metal (defaults to E7018)
    :ivar throat: weld throat thickness (a) (= shortest distance from the root
                   to the face of the diagrammatic weld)
                   (defaults to None, in which case it's calculated for 90º)
    :ivar endLoaded: True if the load is trasferred at the end of the weld 
                   (e.g. lap joints with longitudinal welds, as do bearing 
                    stiffeners. Welds subject to shear loading due to 
                    bending forces are not included in end-loaded applications)
                    Defaults to False
    :ivar lapJoint: True if lap joint (defaults to False)
    '''
    def __init__(self,legSize,length,fillerMetal=E7018,throat=None,endLoaded=False,lapJoint=False):
        self.legSize=legSize
        self.fillerMetal=fillerMetal
        self.length=length
        if not throat:
            self.throat=self.legSize/math.sqrt(2)
        else:
            self.throat=throat
        self.endLoaded=endLoaded
        self.lapJoint=lapJoint

    def getMinLength(self):
        '''Return the minimum length of the fillet weld (Sect. J2.2b.(c) of AISC-360)
        '''
        minL=4*self.legSize
        return minL

    def getMinimumSize(self,t1,t2):
        '''Return the minimum leg size of the fillet weld that can be used to weld 
        two sheets of thicknesses t1 and t2 (Sect. J2.4 of AISC-360)
        '''
        return getFilletWeldMinimumLegSheets(t1, t2)

    def getMaximumSize(self,t1,t2):
        '''Return the maximum leg size of the fillet weld that can be used to weld 
        two sheets of thicknesses t1 and t2 (Sect. J2.2b of AISC 360)
        '''
        return getFilletWeldMaximumLegSheets(t1, t2)
    

    def checkWeldDimensions(self,t1,t2):
        '''Perform the AISC verifications of the weld dimensions and 
        return its strength

        :param t1, t2: thicknesses of the two steel sheets to weld
        '''
        if self.length < self.getMinLength():
            lmsg.error('The weld length = '+str(round(self.length*1e3,0) ) +' mm is less than the minimum length = '+ str(round(self.getMinLength()*1e3,0))+' mm')
        if self.legSize < self.getMinimumSize(t1,t2):
            lmsg.error('The weld size = '+str(round(self.legSize*1e3,1) ) +' mm is less than the minimum size = '+ str(round(self.getMinimumSize(t1,t2)*1e3,1))+' mm')
        if self.legSize > self.getMaximumSize(t1,t2):
            lmsg.warning('The weld size = '+str(round(self.legSize*1e3,0) ) +' mm is greater than the maximum size = '+ str(round(self.getMaximumSize(t1,t2)*1e3,0))+' mm')
        if self.lapJoint and self.length <max(25e-3,5*min(t1,t2)):
            lmsg.error('The amount of lap= '+ str(round(self.length*1e3,0)) + ' mm is less than the minimum =' + str(max(25,5*min(t1,t2)*1e3)))
                                              

    def getEffectiveLength(self):
        '''Return the effective length of the weld (Sect J2.2.2b.(d) of AISC-360)
        '''
        effL=self.length
        if self.endLoaded and effL>100*self.legSize:
            beta=min(1.0,1.2-0.002*effL/self.legSize)
            effL=beta*effL
        if effL>300*self.legSize: effL=180*self.legSize
        return effL

    def getEffectiveArea(self):
        '''Return the effective area of the weld (Sect. J2.2.2a of AISC-360)
        '''
        effArea=self.throat*self.getEffectiveLength()
        return effArea
        

    def getNominalStrength(self,Fpar=None,Fperp=None,theta=0):
        '''Return the nominal strength of the weld (sect. J2.4 of AISC-360)
        If acting forces parallel and perpendicular to the weld longitudinal
        axis are defined, the angle between the the line of action of the 
        required force and the weld longitudinal axis is calculated based on
        those forces. Otherwise, angle theta (degrees) is used.

        :param Fpar: required force in direction parallel to the weld 
                      longitudinal axis
        :param Fperp: required force in direction perpendicular to the weld 
                      longitudinal axis
        :param theta: angle between the line of action of the required force and
                      the weld longitudinal axis (in degrees) (defaults to 0º)
        '''
        if Fpar and Fperp:
            theta=math.atan(abs(Fperp)/abs(Fpar))
        factor=1.5 if theta==90 else (1.0+0.5*math.sin(math.radians(theta))**1.5)
        Fnw=0.6*self.fillerMetal.fu*factor
        Rn=Fnw*self.getEffectiveArea()
        return Rn

    def getDesignStrength(self,Fpar=None,Fperp=None,theta=0):
        '''Return the design strength of the weld (sect. J2.4 of AISC-360)
        If acting forces parallel and perpendicular to the weld longitudinal
        axis are defined, the angle between the the line of action of the 
        required force and the weld longitudinal axis is calculated based on
        those forces. Otherwise, angle theta (degrees) is used.

        :param Fpar: required force in direction parallel to the weld 
                      longitudinal axis
        :param Fperp: required force in direction perpendicular to the weld 
                      longitudinal axis
        :param theta: angle between the line of action of the required force and
                      the weld longitudinal axis (in degrees) (defaults to 0º)
        '''
        fi= 0.75
        Rn=self.getNominalStrength(Fpar,Fperp,theta)
        Rnd=fi*Rn
        return Rnd

    def getCapacityFactor(self,Fpar_d,Fperp_d,t1=None,t2=None):
        '''Return the weld capacity factor. If the thicknesses of the
        plates to be welded are provided, the dimensions of the weld are
        also checked.

        :param Fpar_d: design required force in direction parallel to the weld 
                      longitudinal axis
        :param Fperp_d: design required force in direction perpendicular to the weld 
                      longitudinal axis
        :param t1, t2: thickness of the two steel sheets to be welded
      
        '''
        if (t1 and t2): self.checkWeldDimensions(t1,t2)
        F=math.sqrt(Fpar_d**2+Fperp_d**2)
        R=self.getDesignStrength(Fpar_d,Fperp_d)
        CF=F/R
        return CF
    
        
    

    
    
