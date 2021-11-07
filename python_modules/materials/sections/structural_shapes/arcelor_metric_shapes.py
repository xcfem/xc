# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
from materials.sections.structural_shapes import arcelor_shapes_dictionaries

''' ARCELOR's structural steel shapes (metric units).'''

# European I beams

# Section axis:

#    ARCELOR          XC

#                     ^ Y                    
#                     |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----

#       |
#       v Z

# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

# XXX Check shear distortion constants:
#    alpha-> alphaZ,alphaY

'''
    b: width  [m]
    h: height [m]
    e: thickness [m]
    P: mass per metre [kg/m]
    A: area of section [m2]
    Iz: second moment of area about strong axis [m4]
    Wzel: elastic modulus about strong axis [m4]
    Wzpl: plastic modulus about strong axis [m4]
    iz: radius of gyration about strong axis [m] 
    Iy: second moment of area about weak axis [m4]
    Wyel: elastic modulus about weak axis [m4]
    Wypl: plastic modulus about weak axis [m4]
    iy: radius of gyration about weak axis [m] 
    It: torsional constant J [m4]
    E: elastic modulus [N/m2]
    nu: Poisson's modulus
'''

IPE= arcelor_shapes_dictionaries.IPE
for item in IPE:
    shape= IPE[item]
    Avy= shape['Avy']
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    b= shape['b']
    d= shape['d']
    h= shape['h']
    hi= shape['hi']
    tf= shape['tf']
    tw= shape['tw']
    r= shape['r']
    shape['Iw']= tf*((h+hi)/2.0)**2*b**3/24.0
    shape['alpha']= Avy/A
    shape['G']= E/(2*(1+nu))
    shape['AreaQy']= A-2*b*tf+(tw+2*r)*tf
    shape['AreaQz']= A-hi*tw
    
# Arcelor IPN steel shapes.

# Section axis:


#    ARCELOR          XC

#                     ^ Y                    
#                     |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----

#       |
#       v Z

# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

# XXX Check shear distortion constants:
#   alpha-> alphaY,alphaZ}
IPN= arcelor_shapes_dictionaries.IPN
for item in IPN:
    shape= IPN[item]
    Avy= shape['Avy']
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    b= shape['b']
    h= shape['h']
    tf= shape['tf']
    tw= shape['tw']
    r1=shape['r1']
    r= shape['r1']
    shape['d']= h-2*tf-2*r
    shape['hi']= h-2*tf
    shape['alpha']= Avy/A
    shape['G']= E/(2*(1+nu))
    shape['AreaQy']= 2*b*tf+(tw+2*r1)*tf
    shape['AreaQz']= A-d*tw
    
# Arcelor HE steel shapes.

# Axis of the section:

#    ARCELOR          XC

#                     ^ Y                    
#                     |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----

#       |
#       v Z

# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

# XXX Check shear distortion constants:
#    alpha-> alphaY,alphaZ

HE= arcelor_shapes_dictionaries.HE
for item in HE:
    shape= HE[item]
    Avy= shape['Avy']
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    b= shape['b']
    d= shape['d']
    h= shape['h']
    hi= shape['hi']
    tf= shape['tf']
    tw= shape['tw']
    r= shape['r']
    shape['Iw']= tf*((h+hi)/2.0)**2*b**3/24.0
    shape['alpha']= Avy/A
    shape['G']= E/(2*(1+nu))
    shape['AreaQy']= A-2*b*tf+(tw+2*r)*tf
    shape['AreaQz']= A-hi*tw

# Arcelor UPN steel shapes.

# Cross section axis:

#    ARCELOR          XC

#                     ^ Y                    
#                     |

#       ---           ---
#       |             | 
#       | -> Y        | -> Z
#       |             |
#       ---           ---

#       |
#       v Z

# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

UPN= arcelor_shapes_dictionaries.UPN
for item in UPN:
    shape= UPN[item]
    Avy= shape['Avy']
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    b= shape['b']
    d= shape['d']
    tf= shape['tf']
    tw= shape['tw']
    r1= shape['r1']
    shape['alpha']= Avy/A
    shape['G']= E/(2*(1+nu))
    shape['AreaQy']= A-2*b*tf+(tw+r1)*tf
    shape['AreaQz']= A-d*tw

# Arcelor L steel shapes.

L= arcelor_shapes_dictionaries.L
for item in L:
    shape= L[item]
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    b= shape['b']
    t= shape['t']
    r2= shape['r2']
    shape['G']= E/(2*(1+nu))
    shape['AreaQy']= t*(b-t-r2)
    shape['AreaQz']= t*(b-t-r2)
    shape['It']= 2/3.0*b*t*t*t
    shape['alpha']= shape['AreaQy']/A

# Arcelor U shaped sheet pile.

# Cross section axis:

#    ARCELOR                   XC

#                              ^ Y                    
#                              |

#       -----                -----
#      /     \              /     \
#     /       \ -> Y       /       \ | -> Z
#    /         \          /         \
# ---           ---    ---           ---

#         |
#         v Z

# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

# Warning: area in m2/m, inertia in m4/m, Wz in m3/m and so on

AU= arcelor_shapes_dictionaries.AU
for item in AU:
    shape= AU[item]
    h= shape['h'] #Height
    tf= shape['t'] #Flange thickness
    tw= shape['s'] #Web thickness
    b= shape['b']
    Avy= tw*(h-tf)/b #Projected shear area.
    shape['Avy']= Avy
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    shape['alpha']= Avy/A
    shape['G']= E/(2*(1+nu))

# Arcelor square hollow tubes.
# Cross section axis:

#    ARCELOR          XC

#                     ^ Y                    
#                     |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----

#       |
#       v Z


# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

# In this case the inertia tensor is cylindrical so, in its plane, both
# values are identical: no need to exchange.

SHS= arcelor_shapes_dictionaries.SHS
for item in SHS:
    shape= SHS[item]
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    b= shape['b']
    h= shape['h']
    e= shape['e']
    shape['alpha']= 0.5*5/6.0
    shape['G']= E/(2*(1+nu))
    shape['AreaQy']= 2*0.7*h*e
    shape['AreaQz']= 2*0.7*b*e
    shape['Wyel']= shape['Wzel']
    shape['Wypl']= shape['Wzpl']

# Arcelor rectangular hollow tubes.

RHS= arcelor_shapes_dictionaries.RHS
for item in RHS:
    shape= RHS[item]
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    b= shape['b']
    h= shape['h']
    e= shape['e']
    shape['alpha']= 0.5*5/6.0
    shape['G']= E/(2*(1+nu))
    shape['AreaQy']= 2*0.7*h*e
    shape['AreaQz']= 2*0.7*b*e

# Tata steel circular hollow tubes.

CHS= arcelor_shapes_dictionaries.CHS
for item in CHS:
    shape= CHS[item]
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    shape['alpha']= 1.0/2.0
    shape['G']= E/(2*(1+nu))
    shape['AreaQy']= 0.5*A
    shape['AreaQz']= 0.5*A
  
# ARCELOR's hot rolled round steel bars.

R= arcelor_shapes_dictionaries.R
for item in R:
    shape= R[item]
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    r= shape['r']
    Iz= math.pi*r**4/4.0
    Iy= Iz
    shape['alpha']= 0.5*5/6.0
    shape['G']= E/(2*(1+nu))
    shape['AreaQy']=  9/10*A
    shape['AreaQz']=  9/10*A
    shape['Iz']= Iz
    shape['Iy']= Iy
    shape['iz']= Iz/r
    shape['iy']= Iy/r
    shape['Wzel']= Iz/r
    shape['Wyel']= Iy/r
    shape['Wypl']= 4.0*r**3/3.0
    shape['Wzpl']= 4.0*r**3/3.0
    shape['J']= math.pi*r**4/2.0

  
import xc_base
import geom
from materials.sections import structural_steel

def common_prefix(a:str, b:str):
    ''' Return the common prefix of both strings.

    :param a: first string.
    :param b: second string.
    '''
    retval= ''
    for ch1, ch2 in zip(a,b):
        if(ch1==ch2):
            retval+=ch1
        else:
            break
    return retval

def findNearestSteelShapeByDepth(namePattern:str, depth:float):
    ''' Return the steel shape which name matches the pattern argument
        and whose depth is the nearest to the argument.

    :param namePattern: regular expression to match.
    :param depth: depth of the steel shape.
    '''
    # Find the dictionary to search in.
    retval= None
    arcelorDict= arcelor_shapes_dictionaries.arcelor_shapes
    for tableKey in arcelorDict:
        c_prefix= common_prefix(namePattern, tableKey)
        if(c_prefix==tableKey):
            shapeDict= arcelorDict[tableKey]
            # initial values.
            shapeKey= next(iter(shapeDict))
            retval= shapeDict[shapeKey]
            diff= abs(depth-retval['h'])
            # iterate through dictionary
            for shapeKey in shapeDict:
                shape= shapeDict[shapeKey]
                tmp= abs(depth-shape['h'])
                if(tmp<diff):
                    diff= tmp
                    retval= shape
                    if(diff==0.0): break;
        if(retval): break;
    return retval
            

class IPNShape(structural_steel.IShape):
    def __init__(self,steel,name):
        super(IPNShape,self).__init__(steel,name,IPN)
        
    def d(self):
        '''Return internal web height'''
        return self.get('d')

    def r(self):
        '''Return radius web-flange'''
        return (self.h()-2*self.tf()-self.d())/2.0

class IPEShape(structural_steel.IShape):
    def __init__(self,steel,name):
        super(IPEShape,self).__init__(steel,name,IPE)
  

class HEShape(structural_steel.IShape):
    def __init__(self,steel,name):
        super(HEShape,self).__init__(steel,name,HE)

        
class RHSShape(structural_steel.QHShape):
    def __init__(self,steel,name):
        super(RHSShape,self).__init__(steel,name,RHS)

    def t(self):
        '''Return the thickness'''
        return self.get('e')

        
class SHSShape(structural_steel.QHShape):
    def __init__(self,steel,name):
        super(SHSShape,self).__init__(steel,name,SHS)
        
    def t(self):
        '''Return the thickness'''
        return self.get('e')

    

class UPNShape(structural_steel.UShape):
    def __init__(self,steel,name):
        super(UPNShape,self).__init__(steel,name,UPN)

class AUShape(structural_steel.SteelShape):
    def __init__(self,steel,name):
        super(AUShape,self).__init__(steel,name,AU)
    def getRho(self):
        ''' Returns mass per unit length. '''
        return self.get('P')

class CHSShape(structural_steel.SteelShape):
    def __init__(self,steel,name):
        super(CHSShape,self).__init__(steel,name,CHS)
    def getRho(self):
        ''' Returns mass per unit length. '''
        return self.get('P')
    def alphaY(self):
        '''Return shear shape factor with respect to local y-axis'''
        return 1.0/2.0 #Shear distortion constant. See E. Oñate book page 122.
    def alphaZ(self):
        '''Return shear shape factor with respect to local z-axis'''
        return self.alphaY()
        
