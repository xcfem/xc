# -*- coding: utf-8 -*-
'''Material internal forces (generalized stresses).'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2016, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from misc_utils import log_messages as lmsg

class CrossSectionInternalForces:
    ''' Internal forces on a 3D section (6 degrees of freedom).

    :ivar N: axial force.
    :ivar Vy: shear force parallel to axis y.
    :ivar Vz: shear force parallel to axis z.
    :ivar T: Torque.
    :ivar My: bending moment parallel to axis y.
    :ivar Mz: bending moment parallel to axis z.
    '''
    def __init__(self,N= 0.0,Vy= 0.0,Vz= 0.0,T= 0.0,My= 0.0,Mz= 0.0):
        '''Internal forces on a 3D section (6 degrees of freedom).

        :param N: axial force.
        :param Vy: shear force parallel to axis y.
        :param Vz: shear force parallel to axis z.
        :param T: Torque.
        :param My: bending moment parallel to axis y.
        :param Mz: bending moment parallel to axis z.
        '''
        self.N= N
        self.Vy= Vy
        self.Vz= Vz
        self.T= T
        self.My= My
        self.Mz= Mz

    def __iadd__(self,other):
        '''+= operator.'''
        self.N+= other.N
        self.Vy+= other.Vy
        self.Vz+= other.Vz
        self.T+= other.T
        self.My+= other.My
        self.Mz+= other.Mz
        return self

    def __add__(self,other):
        '''+ operator.'''
        return CrossSectionInternalForces(self.N+other.N,self.Vy+other.Vy,self.Vz+other.Vz,self.T+other.T,self.My+other.My,self.Mz+other.Mz)

    def __isub__(self,other):
        '''-= operator.'''
        self.N-= other.N
        self.Vy-= other.Vy
        self.Vz-= other.Vz
        self.T-= other.T
        self.My-= other.My
        self.Mz-= other.Mz
        return self

    def __sub__(self,other):
        '''- operator.'''
        return CrossSectionInternalForces(self.N-other.N,self.Vy-other.Vy,self.Vz-other.Vz,self.T-other.T,self.My-other.My,self.Mz-other.Mz)

    def __imul__(self,number):
        '''*= operator.'''
        self.N*= number
        self.Vy*= number
        self.Vz*= number
        self.T*= number
        self.My*= number
        self.Mz*= number
        return self

    def __mul__(self,number):
        '''* operator.'''
        return CrossSectionInternalForces(self.N*number,self.Vy*number,self.Vz*number,self.T*number,self.My*number,self.Mz*number)

    def getModulus(self):
        '''modulus of the corresponding [N,Vy,Vz,T,My,Mz] vector.'''
        retval= self.N*self.N
        retval+= self.Vy*self.Vy
        retval+= self.Vz*self.Vz
        retval+= self.T*self.T
        retval+= self.My*self.My
        retval+= self.Mz*self.Mz
        return retval

    def getCopy(self):
        ''' Return a copy of this object.'''
        retval= CrossSectionInternalForces()
        retval.setFromDict(self.getDict())
        return retval

    def getDict(self):
        '''returns a dictionary whith the values of the internal forces.'''
        retval= dict()
        retval['N']= self.N; retval['Vy']= self.Vy; retval['Vz']= self.Vz
        retval['T']= self.T; retval['My']= self.My; retval['Mz']= self.Mz
        # if(hasattr(self, 'chiLT')):
        #     retval['chiLT']= chiLT
        # if(hasattr(self, 'chiN')):
        #     retval['chiN']= chiN
        # if(hasattr(self, 'FcE')):
        #     retval['FcE']= FcE
        # if(hasattr(self, 'FbE')):
        #     retval['FbE']= FbE
        return retval
    
    def setForcesFromDict(self, dct):
        ''' Assing values to the internal forces extracting them from the
            given dictionary.

        :param dct: dictionary containing the values of the forces.
        '''
        self.N= dct['N']
        self.Vy= dct['Vy']
        self.Vz= dct['Vz']
        self.T= dct['T']
        self.My= dct['My']
        self.Mz= dct['Mz']
        # Lateral torsional buckling reduction factor.
        if('chiLT' in dct.keys()):
            self.chiLT= dct['chiLT']
        # Axial strength reduction factor.     
        if('chiN' in dct.keys()): 
            self.chiN= dct['chiN']
        # AWC NDS-2018 critical buckling design values for compression.
        if('FcE' in dct.keys()):
            self.FcE= dct['FcE']
        # AWC NDS-2018 critical buckling design value for bending.
        if('FbE' in dct.keys()): 
            self.FbE= dct['FbE']

    def setFromDict(self,dct):
        '''Sets the internal forces from the dictionary argument.'''
        self.setForcesFromDict(dct)

    def getCSVString(self):
        '''returns a comma separated values string that represents the
           internal forces.'''
        lmsg.log('DEPRECATED')
        retval= str(self.N)+", "+str(self.Vy)+", "+str(self.Vz)+", "
        retval+= str(self.T)+", "+str(self.My)+", "+str(self.Mz)
        return retval

    def setFromCSVString(self,csvStr,offset):
        '''Sets the internal forces from a CSV string.'''
        #lmsg.log('DEPRECATED')
        self.N= eval(csvStr[0+offset])
        self.Vy= eval(csvStr[1+offset])
        self.Vz= eval(csvStr[2+offset])
        self.T= eval(csvStr[3+offset])
        self.My= eval(csvStr[4+offset])
        self.Mz= eval(csvStr[5+offset])
        if len(csvStr) > (6+offset):   #steel beam
            self.chiLT=eval(csvStr[6+offset])

    def getComponents(self):
        '''Returns the internal forces in a list.'''
        return [self.N,self.Vy,self.Vz,self.T,self.My,self.Mz]

    def increaseEccentricities(self, ez, ey):
        ''' Increase the eccentricity of the axial force with the given values.

        :param ez: additional eccentricity around z axis.
        :param ey: additional eccentricity around y axis.
        '''
        if(abs(ez)>0): # additional eccentricity not zero.
            e0z= self.Mz/self.N
            incMz= self.N*ez
            if(abs(e0z/ez)>1e-3): # original eccentricity not zero.
                if(self.Mz>0): # Always increase the moment.
                    if(incMz>0): # both positive
                        self.Mz+= incMz
                    else: # increment negative.
                        self.Mz-= incMz
                else: # Always increase the moment.
                    if(incMz>0): # increment positive.
                        self.Mz-= incMz
                    else: # both negative.
                        self.Mz+= incMz
            else: # original eccentricity almost zero.
                self.Mz= incMz
        if(abs(ey)>0): # additional eccentricity not zero.
            e0y= self.My/self.N
            incMy= self.N*ey
            if(abs(e0y/ey)>1e-3): # original eccentricity not zero.
                if(self.My>0): # Always increase the moment.
                    if(incMy>0): # both positive
                        self.My+= incMy
                    else: # increment negative.
                        self.My-= incMy
                else: # Always increase the moment.
                    if(incMy>0): # increment positive.
                        self.My-= incMy
                    else: # both negative.
                        self.My+= incMy
            else: # original eccentricity almost zero.
                self.My= incMy

    def __str__(self):
        return str(self.getDict())

def transform_internal_forces(iForces,theta):
    '''Computes internal forces in a system rotated theta
       degrees with respect to the z(3) axis.'''
    cos2T= math.cos(2*theta)
    sin2T= math.sin(2*theta)
    tmpA= (iForces[0]+iForces[1])/2.0
    tmpB= (iForces[0]-iForces[1])/2.0*cos2T+iForces[2]*sin2T
    retval= [0.0,0.0,0.0]
    retval[0]= tmpA+tmpB 
    retval[2]= -(iForces[0]-iForces[1])/2.0*sin2T+iForces[2]*cos2T
    retval[1]= tmpA-tmpB
    return retval

class ShellMaterialInternalForces:
    '''Internal forces on a material point of a shell element

    :ivar n1: axial force parallel to axis 1.
    :ivar n2: axial force parallel to axis 2.
    :ivar n12: in plane shear force.
    :ivar m1: bending moment parallel to axis 1.
    :ivar m2: bending moment parallel to axis 2.
    :ivar m12: twisting stress resultant.
    :ivar q13: out of plane shear force normal to axis 1 and parallel to axis 3.
    :ivar q23: out of plane shear force normal to axis 1 and parallel to axis 3.
    '''
    def __init__(self,n1= 0.0,n2= 0.0,n12= 0.0,m1= 0.0,m2= 0.0,m12= 0.0,q13= 0.0,q23= 0.0):
        '''Internal forces on a material point of a shell element

        :param n1: axial force parallel to axis 1.
        :param n2: axial force parallel to axis 2.
        :param n12: in plane shear force.
        :param m1: bending moment parallel to axis 1.
        :param m2: bending moment parallel to axis 2.
        :param m12: twisting stress resultant.
        :param q13: out of plane shear force normal to axis 1 and parallel to axis 3.
        :param q23: out of plane shear force normal to axis 1 and parallel to axis 3.
        '''
        self.n1= n1
        self.n2= n2
        self.n12= n12
        self.m1= m1
        self.m2= m2
        self.m12= m12
        self.q13= q13
        self.q23= q23

    def setFromAverageInShellElement(self,element,fConv= 1.0):
        '''Extracts the average internal forces from the element.

        :param element: element to get the internal forces from.
        :param fConv: conversion factor.
        '''
        element.getResistingForce()
        physProp= element.physicalProperties
        self.n1= physProp.getMeanGeneralizedStressByName("n1")*fConv
        self.n2= physProp.getMeanGeneralizedStressByName("n2")*fConv
        self.n12= physProp.getMeanGeneralizedStressByName("n12")*fConv
        self.m1= physProp.getMeanGeneralizedStressByName("m1")*fConv
        self.m2= physProp.getMeanGeneralizedStressByName("m2")*fConv
        self.m12= physProp.getMeanGeneralizedStressByName("m12")*fConv
        self.q13= physProp.getMeanGeneralizedStressByName("q13")*fConv
        self.q23= physProp.getMeanGeneralizedStressByName("q23")*fConv
        # Change the internal forces reference system if needed.
        if(element.hasProp('theta')):
            theta= element.getProp('theta')
            self.transform(theta)

    def transform(self, theta):
        '''Calculates the components for a reference system rotated 
           the given angle.

        :param theta: angle of the new reference system with the original one.
        '''
        N= transform_internal_forces([self.n1,self.n2,self.n12],theta)
        self.n1= N[0]; self.n2= N[1]; self.n12= N[2]
        M= transform_internal_forces([self.m1,self.m2,self.m12],theta)
        self.m1= M[0]; self.m2= M[1]; self.m12= M[2]
        Q= transform_internal_forces([self.q13,self.q23,0.0],theta)
        self.q13= Q[0]; self.q23= Q[1]

    def getDict(self):
        '''returns a dictionary whith the values of the internal forces.'''
        retval= dict()
        retval['n1']= self.n1; retval['n2']= self.n2; retval['n12']= self.n12
        retval['m1']= self.m1; retval['m2']= self.m2; retval['m12']= self.m12
        retval['q13']= self.q13; retval['q23']= self.q23
        return retval

    def setFromDict(self,dct):
        '''Sets the internal forces from the dictionary argument.'''
        self.n1= dct['n1']
        self.n2= dct['n2']
        self.n12= dct['n12']
        self.m1= dct['m1']
        self.m2= dct['m2']
        self.m12= dct['m12']
        self.q13= dct['q13']
        self.q23= dct['q23']

    def getCSVString(self):
        '''returns a comma separated values string that represents the
           internal forces.'''
        lmsg.log('DEPRECATED')
        retval= str(self.n1)+", "+str(self.n2)+", "+str(self.n12)+", "
        retval+= str(self.m1)+", "+str(self.m2)+", "+str(self.m12)+", "
        retval+= str(self.q13)+", "+str(self.q23)
        return retval

    def setFromCSVString(self,csvStr,offset):
        '''Sets the internal forces from a CSV string.'''
        #lmsg.log('DEPRECATED')
        self.n1= eval(csvStr[0+offset])
        self.n2= eval(csvStr[1+offset])
        self.n12= eval(csvStr[2+offset])
        self.m1= eval(csvStr[3+offset])
        self.m2= eval(csvStr[4+offset])
        self.m12= eval(csvStr[5+offset])
        self.q13= eval(csvStr[6+offset])
        self.q23= eval(csvStr[7+offset])

    # Wood-Armer method for the assessment of reinforced concrete shells.
    def getWoodArmer1(self, alsoForAxialForces= False):
        '''returns Wood-Armer method internal forces for axis 1.

        :param alsoForAxialForces: if true use Wood-Armer method for both
                                   axial and bending internal forces 
                                   otherwise use it only for bending moments.
        '''
        N= self.n1
        M= self.m1+ math.copysign(self.m12,self.m1)
        Vz= self.n12
        if(alsoForAxialForces):
            N+= math.copysign(Vz, N)
            Vz= 0.0
        return CrossSectionInternalForces(N= N, Vy= self.q13, Vz= Vz, T= 0.0,My= M, Mz= 0.0)

    def getWoodArmer2(self, alsoForAxialForces= False):
        '''returns Wood-Armer method internal forces for axis 2.

        :param alsoForAxialForces: if true use Wood-Armer method for both
                                   axial and bending internal forces 
                                   otherwise use it only for bending moments.
        '''
        N= self.n2
        M= self.m2+ math.copysign(self.m12,self.m2)
        Vz= self.n12
        if(alsoForAxialForces):
            N+= math.copysign(Vz, N)
            Vz= 0.0        
        return CrossSectionInternalForces(N= N, Vy= self.q23, Vz= Vz,T= 0.0, My= M, Mz= 0.0)

    def getWoodArmer(self, alsoForAxialForces= False):
        '''returns Wood-Armer method internal forces.

        :param alsoForAxialForces: if true use Wood-Armer method for both
                                   axial and bending internal forces 
                                   otherwise use it only for bending moments.
        '''
        return [self.getWoodArmer1(alsoForAxialForces),self.getWoodArmer2(alsoForAxialForces)]
