# -*- coding: utf-8 -*-
''' Torsional function curves according to the document: "Technical Note:
    Torsional Analysis of Steel Sections". William E. Moore II and Keith
    M. Mueller. American Institute of Steel Construction. 2003.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2020, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math

def theta(case, steelShape, z, L, T, zT):
    ''' Return the value of the angle twist.

    :param case: loading and end condition case according to "AISC Design Guide number 9".
    :param steelShape: steel shape of the rolled steel section.
    :param z: length from left support to cross-section analyzed.
    :param L: span length
    :param T: concentrated torque.
    :param zT: length from left support to cross-section where torque
               is applied.
    '''
    GJ= steelShape.get('G')*steelShape.J()
    Cw= steelShape.get('Cw') # Warping constant.
    E= steelShape.get('E')
    a= math.sqrt(E*Cw/GJ)
    if(case==1): # Concentrated Torques with Free Ends
        return T*z/GJ
    elif(case==2):  # Concentrated Torques with Fixed Ends
        c1= Ta/GJ
        c2= math.tanh(L/2/a)
        z_a= z/a
        return c1*(c2*math.cosh(z_a)-c2+z_a-math.sinh(z_a))
    elif(case==3):  # Concentrated Torque with Pinned Ends
        alph= zT/L
        alphL= zT
        alphL_a= alphL/a
        c1= T/GJ
        c2= math.sinh(alphL_a)/math.tanh(L/a)-math.cosh(alphL_a)
        z_a= z/a
        if(z<zT):
            return c1*((1-alph)*z-c2*a*math.sinh(z_a))
        else:
            return c1*((L-z)*alph+a*(math.sinh(alphL_a)/math.tanh(L/a)*math.sinh(z_a)-math.sinh(alphL_a)*math.cosh(z_a)))
    else:
        lmsg.error('case: '+str(case)+' not implemented yet.')

def theta_dot(case, steelShape, z, L, T, zT):
    ''' Return the first derivative of the angle twist.

    :param case: loading and end condition case according to "AISC Design Guide number 9".
    :param steelShape: steel shape of the rolled steel section.
    :param z: length from left support to cross-section analyzed.
    :param L: span length
    :param T: concentrated torque.
    :param zT: length from left support to cross-section where torque
               is applied.
    '''
    GJ= steelShape.get('G')*steelShape.J()
    if(case==1): #Concentrated Torques with Free Ends
        return T/GJ
    elif(case==2):  # Concentrated Torques with Fixed Ends
        c1= Ta/GJ
        c2= math.tanh(L/2/a)
        z_a= z/a
        return c1/a*(1-math.cosh(z_a)+c2*math.sinh(z_a))
    elif(case==3):  # Concentrated Torque with Pinned Ends
        alph= zT/L
        alphL= zT
        alphL_a= alphL/a
        c1= T/GJ
        c2= math.sinh(alphL_a)/math.tanh(L/a)-math.cosh(alphL_a)
        z_a= z/a
        if(z<zT):
            return c1*((1-alph)+c2*math.cosh(z_a))
        else:
            return c1*(math.sinh(alphL_a)/math.tanh(L/a)*math.cosh(z_a)-math.sinh(alphL_a)*math.sinh(z_a)-alph)
    else:
        lmsg.error('case: '+str(case)+' not implemented yet.')
    
def theta_dot_dot(case, steelShape, z, L, T, zT):
    ''' Return the second derivative of the angle twist.

    :param case: loading and end condition case according to "AISC Design Guide number 9".
    :param steelShape: steel shape of the rolled steel section.
    :param z: length from left support to cross-section analyzed.
    :param L: span length
    :param T: concentrated torque.
    :param zT: length from left support to cross-section where torque
               is applied.
    '''
    GJ= steelShape.get('G')*steelShape.J()
    if(case==1): #Concentrated Torques with Free Ends
        return 0.0
    elif(case==2):  # Concentrated Torques with Fixed Ends
        c1= Ta/GJ
        c2= math.tanh(L/2/a)
        z_a= z/a
        return c1/(a**2)*(c2*math.cosh(z_a)-math.sinh(z_a))
    elif(case==3):  # Concentrated Torque with Pinned Ends
        alph= zT/L
        alphL= zT
        alphL_a= alphL/a
        c1= T/GJ
        c2= math.sinh(alphL_a)/math.tanh(L/a)-math.cosh(alphL_a)
        z_a= z/a
        if(z<zT):
            return c1*c2/a*math.sinh(z_a)
        else:
            return c1/a*(math.sinh(alphL_a)/math.tanh(L/a)*math.sinh(z_a)-math.sinh(alphL_a)*math.cosh(z_a))
    else:
        lmsg.error('case: '+str(case)+' not implemented yet.')
    
def theta_dot_dot_dot(case, steelShape, z, L, T, zT):
    ''' Return the third derivative of the angle twist.

    :param case: loading and end condition case according to "AISC Design Guide number 9".
    :param steelShape: steel shape of the rolled steel section.
    :param z: length from left support to cross-section analyzed.
    :param L: span length
    :param T: concentrated torque.
    :param zT: length from left support to cross-section where torque
               is applied.
    '''
    GJ= steelShape.get('G')*steelShape.J()
    if(case==1): #Concentrated Torques with Free Ends
        return 0.0
    elif(case==2):  # Concentrated Torques with Fixed Ends
        c1= Ta/GJ
        c2= math.tanh(L/2/a)
        z_a= z/a
        return c1/(a**3)*(-math.cosh(z_a)+c2*math.sinh(z_a))
    elif(case==3):  # Concentrated Torque with Pinned Ends
        alph= zT/L
        alphL= zT
        alphL_a= alphL/a
        c1= T/GJ
        c2= math.sinh(alphL_a)/math.tanh(L/a)-math.cosh(alphL_a)
        z_a= z/a
        if(z<zT):
            return c1*c2/a**2*math.cosh(z_a)
        else:
            return c1/a**2*(math.sinh(alphL_a)/math.tanh(L/a)*math.cosh(z_a)-math.sinh(alphL_a)*math.sinh(z_a))
    else:
        lmsg.error('case: '+str(case)+' not implemented yet.')

def theta_factor(case, steelShape, z, L, zT):
    ''' Return the ratio between the basic torsional stiffness factor (L/GJ)
        and the stiffness calculated with the torsinal function curve.

    :param case: loading and end condition case according to "AISC Design Guide number 9".
    :param steelShape: steel shape of the rolled steel section.
    :param z: length from left support to cross-section analyzed.
    :param L: span length
    :param zT: length from left support to cross-section where torque
               is applied.
    '''
    th= theta(case, steelShape, z, L, 1.0, zT)
    GJ= steelShape.get('G')*steelShape.J()
    return GJ*th/L

