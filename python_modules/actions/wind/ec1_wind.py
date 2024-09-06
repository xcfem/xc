# -*- coding: utf-8 -*-
'''Functions to compute wind loads according to EN 1991-1-4:2005.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2024, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
import math
import scipy.interpolate
from misc_utils import log_messages as lmsg
from actions.wind import base_wind
from actions.wind import IAP_wind

# Probability factor

def get_probability_factor(T:float, K= 0.2, n= 0.5):
    ''' Return the probability factor according to NOTE 4 to clause 4.2 (2) 
        of EN 1991-1-4:2005.

    :param T: return period expressed in years.
    :param K: parameter (defaults to 0.2).
    :param n: parameter (defaults to 0.5)
    '''
    return math.pow((1.0-K*math.log(-math.log(1-1/T)))/(1-K*math.log(-math.log(0.98))),n)

def get_basic_velocity_pressure(vb, rho= 1.25):
    ''' Return the basic velocity pressure according to expression (4.10)
        of EN 1991-1-4:2005.

    :param vb: basic wind velocity.
    :param rho: air density.
    '''
    return 0.5*rho*vb**2

def get_z0(terrainCategory:str):
    ''' Return the rugosity length according to table 4.1 of EN 1991-1-4:2005.

    :param terrainCategory: terrain category.
    '''
    return IAP_wind.getZ0(terrainCategory= terrainCategory)


def get_zmin(terrainCategory:str):
    ''' Return the minimum height according to table 4.1 of EN 1991-1-4:2005.

    :param terrainCategory: terrain category.
    '''
    return IAP_wind.getZmin(terrainCategory= terrainCategory)

def get_kr(terrainCategory:str):
    ''' Return the terrain factor according to expression (4.5) of EN 1991-1-4:2005.

    :param terrainCategory: terrain category.
    '''
    z0= get_z0(terrainCategory= terrainCategory)
    z0II= get_z0(terrainCategory= 'II')
    return 0.19*pow(z0/z0II, 0.07)

def get_roughness_factor(terrainCategory:str, z, zMax= 200.0):
    ''' Return the ground roughness factor according to expression (4.4) of EN 1991-1-4:2005.

    :param terrainCategory: terrain category.
    :param z: height above ground.
    :param zMax: maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
    '''
    zMin= get_zmin(terrainCategory= terrainCategory)
    z0= get_z0(terrainCategory= terrainCategory)
    kr= get_kr(terrainCategory= terrainCategory)
    retval= None
    if((z>=zMin) and (z<zMax)):
        retval= kr*math.log(z/z0)
    elif(z<zMin):
        retval= kr*math.log(zMin/z0)
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; value of z= '+str(z)+' out of range (0,200] m.')
    return retval

def get_turbulence_intensity(terrainCategory:str, z, zMax= 200.0, k1= 1.0, c0= 1.0):
    ''' Return the turbulence intensity according to expression (4.7)
        of EN 1991-1-4:2005.

    :param terrainCategory: terrain category.
    :param z: height above ground.
    :param zMax: maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
    :param k1: turbulence factor.
    :param c0: orography factor.
    '''
    zMin= get_zmin(terrainCategory= terrainCategory)
    z0= get_z0(terrainCategory= terrainCategory)
    retval= None
    if((z>=zMin) and (z<zMax)):
        retval= k1/(c0*math.log(z/z0))
    elif(z<zMin):
        retval= k1/(c0*math.log(zMin/z0))
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; value of z= '+str(z)+' out of range (0,200] m.')
    return retval

def get_peak_velocity_pressure(terrainCategory:str, vb, z, zMax= 200.0, rho= 1.25, k1= 1.0, c0= 1.0):
    ''' Return the peak velocity pressure according to expression (4.8)
        of EN 1991-1-4:2005.

    :param terrainCategory: terrain category.
    :param vb: basic wind velocity.
    :param z: height above ground.
    :param zMax: maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
    :param rho: air density.
    :param k1: turbulence factor.
    :param c0: orography factor.
    '''
    Cr= get_roughness_factor(terrainCategory= terrainCategory, z= z)
    vm= Cr*c0*vb
    Iv= get_turbulence_intensity(terrainCategory= terrainCategory, z= z, k1= k1, c0= c0)
    return (1+7*Iv)*0.5*rho*vm**2
    
def get_rectangular_wall_peak_velocity_pressure_distribution(b, h, terrainCategory:str, vb, zMax= 200.0, rho= 1.25, k1= 1.0, c0= 1.0, factor= 1.0):
    ''' Return the peak velocity pressure distribution of a rectangular wall
        of a rectangular plan building according to figure 7.4 of 
        EN 1991-1-4:2005.

    :param b: wall width.
    :param h: wall height.
    :param terrainCategory: terrain category.
    :param vb: basic wind velocity.
    :param zMax: maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
    :param rho: air density.
    :param k1: turbulence factor.
    :param c0: orography factor.
    :param factor: factor multiplying the returned pressures.
    '''
    qph= get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= h, zMax= zMax, rho= rho, k1= k1, c0= c0)
    if(h<b):
        zi= [0, h]
        pi= [qph*factor, qph*factor]
    elif(h<2*b):
        qpb= get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= b, zMax= zMax, rho= rho, k1= k1, c0= c0)
        epsZ= h/1e2
        zi= [0, b-epsZ, b+epsZ, h]
        pi= [qpb*factor, qpb*factor, qph*factor, qph*factor]
    else:
        qpb= get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= b, zMax= zMax, rho= rho, k1= k1, c0= c0)
        zi= [0, b, h-b, h]
        pi= [qpb*factor, qpb*factor, qph*factor, qph*factor]
    return scipy.interpolate.interp1d(zi, pi, kind='linear'), zi

def get_exposure_factor(terrainCategory:str, vb, z, zMax= 200.0, rho= 1.25, k1= 1.0, c0= 1.0):
    ''' Return the exposure factor pressure according to expression (4.9),
        clause 45 of EN 1991-1-4:2005.

    :param terrainCategory: terrain category.
    :param vb: height above ground.
    :param z: height above ground.
    :param zMax: maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
    :param rho: air density.
    :param k1: turbulence factor.
    :param c0: orography factor.
    '''    
    retval= get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0)
    retval/= get_basic_velocity_pressure(vb= vb, rho= rho)
    return retval

cfx0_a= scipy.interpolate.interp1d([0.0, 0.3627, 4.0, 12.0], [2.4, 2.4, 1.3, 1.3], kind='linear')
cfx0_b= scipy.interpolate.interp1d([0.0, 0.3627, 5.0, 12.0], [2.4, 2.4, 1.0, 1.0], kind='linear')

def get_bridge_deck_transverse_force_coefficient(b, dtot, solidParapets= True):
    ''' Return the value of the transverse force coefficient for bridge decks 
        according to figure 8.3 of EN 1991-1-4:2005.

    :param b: deck width.
    :param dtot: sum of the projected (windward) depth of the structure
    '''
    b_dtot= b/dtot
    if(solidParapets):
        return cfx0_b(b_dtot)
    else:
        return cfx0_a(b_dtot)
    
cfz_10= scipy.interpolate.interp1d([0.0, 4, 22.0, 1e3], [0.75, 0.9, 0.9, 0.9], kind='linear')
cfz_6= scipy.interpolate.interp1d([0.0, 22.0, 1e3], [0.75, 0.9, 0.9], kind='linear')
cfz_0= scipy.interpolate.interp1d([0.0, 14.0, 22.0, 1e3], [0.75, 0.15, 0.15, 0.15], kind='linear')

def get_bridge_deck_vertical_force_coefficient(b, dtotVP, alpha= math.radians(10), beta= 0.0):
    ''' Return the value of the vertical force coefficient for bridge decks 
        according to figure 8.6 of EN 1991-1-4:2005.

    :param b: deck width.
    :param dtotVP: sum of the projected (windward) depth of the structure. Here
                 d_tot must be computed disregarding the traffic and any 
                 bridge equipment (see NOTE 1 in clause 8.3.3 of 
                 EN 1991-1-4:2005.
    :parma alpha: angle of the wind with the horizontal (see figure 8.6 of EN 1991-1-4:2005).
    :param beta: superelevation of the bridge deck (see figure 8.6 of EN 1991-1-4:2005).
    '''
    angle10= math.radians(10)
    angle6= math.radians(6)
    b_dtot= b/dtotVP
    theta= abs(beta+alpha)
    if(theta>=angle10):
        retval= cfz_10(b_dtot)
    elif(theta>=angle6):
        topValue= cfz_10(b_dtot)
        bottomValue= cfz_6(b_dtot)
        retval= (topValue-bottomValue)/(angle10-angle6)*(theta-angle6)+bottomValue
    else:
        topValue= cfz_6(b_dtot)
        bottomValue= cfz_0(b_dtot)
        retval= (topValue-bottomValue)/angle6*theta+bottomValue
    return retval

def get_vertical_pressure(terrainCategory:str, b:float, dtotVP:float, z:float, vb:float, zMax= 200.0, rho= 1.25, k1:float= 1.0, c0:float= 1.0, alpha= math.radians(10), beta= 0.0):
    ''' Return the value of the average vertical pressure according to clause
        4.2.5.1.2 of IAP-11.

    :param terrainCategory: terrain category according to clause 4.2.2 
                            of IAP-11.
    :param b: deck width.
    :param dtotVP: sum of the projected (windward) depth of the structure. Here
                 d_tot must be computed disregarding the traffic and any 
                 bridge equipment (see NOTE 1 in clause 8.3.3 of 
                 EN 1991-1-4:2005.
    :param z: Height above ground surface at the site of the building or 
              other structure.
    :param v_b: basic wind velocity according to clause 4.2.1 of IAP-11.
    :param zMax: maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
    :param rho: density of air (defaults to 1.25 kg/m3).
    :param k1: turbulence factor.
    :param c0: orography factor.
    :parma alpha: angle of the wind with the horizontal (see figure 8.6 of EN 1991-1-4:2005).
    :param beta: superelevation of the bridge deck (see figure 8.6 of EN 1991-1-4:2005).
    '''
    # Compute peak velocity pressure (qp= qb*ce).
    qp= get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0)
    # Compute vertical force coefficient
    cfz= get_bridge_deck_vertical_force_coefficient(b= b, dtotVP= dtotVP, alpha= alpha, beta= beta)
    return (qp*-cfz, qp*cfz)

def get_vertical_pressure_distribution(terrainCategory:str, x0: float, x1: float, dtotVP:float, z:float, vb:float, zMax= 200.0, rho= 1.25, k1:float= 1.0, c0:float= 1.0, alpha= math.radians(10), beta= 0.0):
    ''' Return the vertical pressure distribution over a bridge deck, so 
        the resultant force has a horizontal eccentricity of a quarter 
        of the deck width.

    :param terrainCategory: terrain category according to clause 4.2.2 
                            of IAP-11.
    :param x0: leftmost point of the deck.
    :param x1: rigthmost point of the deck.
    :param dtotVP: sum of the projected (windward) depth of the structure. Here
                 d_tot must be computed disregarding the traffic and any 
                 bridge equipment (see NOTE 1 in clause 8.3.3 of 
                 EN 1991-1-4:2005.
    :param z: Height above ground surface.
    :param vb: basic wind velocity.
    :param zMax: maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
    :param rho: density of air (defaults to 1.25 kg/m3).
    :param k1: turbulence factor.
    :param c0: orography factor.
    :parma alpha: angle of the wind with the horizontal (see figure 8.6 of EN 1991-1-4:2005).
    :param beta: superelevation of the bridge deck (see figure 8.6 of EN 1991-1-4:2005).
    '''
    b= x1-x0 # Deck width
    # Compute vertical pressure.
    averagePressure= get_vertical_pressure(terrainCategory= terrainCategory, b= b, dtotVP= dtotVP, z= z, vb= vb, zMax= zMax, rho= rho, k1= k1, c0= c0, alpha= alpha, beta= beta)
    (a,b)= base_wind.getLinearDistribution(h= b, hR= 0.75*b)
    xi= [x0, x1]
    y0i= [averagePressure[0]*a, averagePressure[0]*b]
    pressureDistrib0= scipy.interpolate.interp1d(xi, y0i, kind='linear')
    y1i= [averagePressure[1]*a, averagePressure[1]*b]
    pressureDistrib1= scipy.interpolate.interp1d(xi, y1i, kind='linear')
    return (pressureDistrib0, pressureDistrib1)

# Rectangular sections.
cf0_rs_xi= [.1, .2, .6, .7, 1, 2, 5, 10, 20, 50, 1000]
cf0_rs_yi= [2.0, 2.0, 2.35, 2.4, 2.1, 1.65, 1.0, 0.9, 0.9, 0.9, 0.9]
cf0_rs= scipy.interpolate.interp1d(cf0_rs_xi, cf0_rs_yi, kind='linear')
def get_rectangular_section_force_coefficient_without_free_end_flow(d, b):
    ''' Return the force coefficient of the flow around a rectangular section
        according to the figure 7.23 of EN 1991-1-4:2005.
 
    :param d: length of the section (parallel to wind direction).
    :param b: width of the section (perpendicular to wind direction).
    '''
    return float(cf0_rs(d/b))

psi_r_ss_xi= [0, 0.2, 0.4, 1e3]
psi_r_ss_yi= [1.0, 0.5, 0.5, 0.5]
psi_r_ss= scipy.interpolate.interp1d(psi_r_ss_xi, psi_r_ss_yi, kind='linear')
def get_square_section_round_corners_reduction_factor(b, r):
    ''' Return the reduction factor for a square cross-section with rounded
        corners according to figurea 7.24 of EN 1991-1-4:2005.
 
     :param b: width of the section (perpendicular to wind direction).
     :param r: radius of the corners.
    '''
    return float(psi_r_ss(r/b))

def get_polygonal_section_lambda(b, l):
    ''' Return the value of lambdar for a cylinder according to table 7.16
        of EN 1991-1-4:2005.

    :param b: width of the section (perpendicular to wind direction).
    :param l: length of the prism.
    '''
    if(l>=50):
        retval= min(1.4*l/b, 70)
    elif(l<15):
        retval= min(2*l/b, 70)
    else:
        v50= min(1.4*l/b, 70)
        v15= min(2*l/b, 70)
        retval= (v50-v15)/35*(l-15)+v15
    return retval

def logarithmic_expression(x0, F0, x1, F1, x):
    ''' Computes the value of a function defined by a linear-log plot.

    See: https://en.wikipedia.org/wiki/Semi-log_plot

    :param (x0, F0): first point of a straight line in the graph.
    :param (x1, F1): second point of a straight line in the graph.
    '''
    return (F1-F0)/math.log10(x1/x0)*math.log10(x/x0)+F0

def get_end_effect_factor(lmbd:float,  solidityRatio= 1.0):
    ''' Return the end effect factor according to figure 7.36
        of EN 1991-1-4:2005.

    :param lmbd: effective slenderness of the structure.
    :param solidityRatio: ratio between the projected areas of the member and the area of the evelope.
    '''
    retval= None
    if(solidityRatio == 1.0):
        if(lmbd<=10):
            retval= logarithmic_expression(x0= 1, F0= 0.6, x1= 10, F1=  0.698, x= lmbd)
        elif(lmbd<=70.0):
            retval= logarithmic_expression(x0= 10, F0= 0.698, x1= 70, F1=  0.9182, x= lmbd)
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(methodName+'; not yet implementd for lambda greater than 70.')
            

    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; not yet implementd for solidity ratios different from 1.0.')
    return retval  

def get_polygonal_section_end_effect_factor(b, l, solidityRatio= 1.0):
    ''' Return the end effect factor for a prism according to figure 7.36
        of EN 1991-1-4:2005.

    :param b: width of the section (perpendicular to wind direction).
    :param l: length of the prism.
    :param solidityRatio: ratio between the projected areas of the member and the area of the evelope.
    '''
    retval= None
    l= get_polygonal_section_lambda(b= b, l= l)
    return get_end_effect_factor(l)

def get_rectangular_section_force_coefficient(d, b, l, r= 0.0, solidityRatio= 1.0):
    ''' Return the force coefficient of the flow around a rectangular prism
        according to expression (7.9) of EN 1991-1-4:2005.
 
    :param d: length of the section (parallel to wind direction).
    :param b: width of the section (perpendicular to wind direction).
    :param l: length of the prism.
    :param r: radius of the corners.
    :param solidityRatio: ratio between the projected areas of the member and the area of the evelope.
    '''
    cf0= get_rectangular_section_force_coefficient_without_free_end_flow(d= d, b= b)
    psi_r= get_square_section_round_corners_reduction_factor(b= b, r= r)
    psi_l= get_polygonal_section_end_effect_factor(b= b, l=10.0, solidityRatio= 1.0)

    return cf0*psi_r*psi_l

def get_rectangular_prism_pressure_distribution(d, b, h, terrainCategory:str, vb, zMax= 200.0, rho= 1.25, k1= 1.0, c0= 1.0, cscd= 1.0, r= 0.0):
    ''' Return the wind pressure distribution of a rectangular prism
        according to EN 1991-1-4:2005.

    :param d: length of the section (parallel to wind direction).
    :param b: width of the prism (perpendicular to wind direction).
    :param h: prism height.
    :param terrainCategory: terrain category.
    :param vb: basic wind velocity.
    :param zMax: maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
    :param rho: air density.
    :param k1: turbulence factor.
    :param c0: orography factor.
    :param cscd: structural factor.
    :param r: radius of the corners.
    '''
    cf= get_rectangular_section_force_coefficient(d= d, b= b, l= h, r= r, solidityRatio= 1.0)
    factor= cscd*cf
    p, zi= get_rectangular_wall_peak_velocity_pressure_distribution(b= b, h= h, terrainCategory= terrainCategory, vb= vb, zMax= zMax, rho= rho, k1= k1, c0= c0, factor= factor)
    return p, zi


# Circular cylindrical sections.
def get_cylinder_reynolds_number(b, v, nu= 15e-6):
    ''' Return the reynolds number of the flow around a cylinder based on the
        equation 7.15 of EN 1991-1-4:2005.
 
    :param b: diameter of the cylinder.
    :param v: wind velocity.
    :param nu: kinematic viscosity of the air (ν=15*10-6 m2/s).
    '''
    return b*v/nu

def get_cylinder_lambda(b, l):
    ''' Return the value of lambdar for a cylinder according to table 7.16
        of EN 1991-1-4:2005.

    :param b: diameter of the cylinder.
    :param l: length of the cylinder.
    '''
    if(l>=50):
        retval= min(0.7*l/b, 70)
    elif(l<15):
        retval= min(l/b, 70)
    else:
        v50= min(0.7*l/b, 70)
        v15= min(l/b, 70)
        retval= (v50-v15)/35*(l-15)+v15
    return retval

def get_cylinder_end_effect_factor(b, l, solidityRatio= 1.0):
    ''' Return the end effect factor for a cylinder according to figure 7.36
        of EN 1991-1-4:2005.

    :param b: diameter of the cylinder.
    :param l: length of the cylinder.
    :param solidityRatio: ratio between the projected areas of the member and the area of the evelope.
    '''
    retval= None
    l= get_cylinder_lambda(b= b, l= l)
    return get_end_effect_factor(l)

def get_cylinder_force_coefficient_without_free_end_flow(b, k, terrainCategory:str, vb, z, zMax= 200.0, rho= 1.25, k1= 1.0, c0= 1.0, nu= 15e-6):
    ''' Return the force coefficient of the flow around a cylinder based on the
        expressions in the figure 7.28 of EN 1991-1-4:2005.
 
    :param b: diameter of the cylinder.
    :param k: equivalent surface roughness (see table 7.13 of EN 1991-1-4:2005).
    :param terrainCategory: terrain category.
    :param vb: basic wind velocity.
    :param z: height above ground.
    :param zMax: maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
    :param rho: air density.
    :param k1: turbulence factor.
    :param c0: orography factor.
    :param nu: kinematic viscosity of the air (ν=15*10-6 m2/s).
    '''
    retval= 0.18*math.log10(10*k/b)
    qp= get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0)
    v= math.sqrt(2*qp/rho) # See NOTE 2 on figure 7.28
    Re= get_cylinder_reynolds_number(b= b, v= v, nu= nu)
    retval/= 1+0.4*math.log10(Re/1e6)
    retval+= 1.2
    return retval

def get_cylinder_force_coefficient(b, l, k, terrainCategory:str, vb, z, zMax= 200.0, rho= 1.25, k1= 1.0, c0= 1.0, nu= 15e-6, solidityRatio= 1.0):
    ''' Return the force coefficient of the flow around a cylinder according to 
        expression (7.19) of EN 1991-1-4:2005.
 
    :param b: diameter of the cylinder.
    :param l: length of the cylinder.
    :param k: equivalent surface roughness (see table 7.13 of EN 1991-1-4:2005).
    :param terrainCategory: terrain category.
    :param vb: basic wind velocity.
    :param z: height above ground.
    :param zMax: maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
    :param rho: air density.
    :param k1: turbulence factor.
    :param c0: orography factor.
    :param nu: kinematic viscosity of the air (ν=15*10-6 m2/s).
    :param solidityRatio: ratio between the projected areas of the member and the area of the evelope.
    '''
    cf0= get_cylinder_force_coefficient_without_free_end_flow(b=b, k= k, terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0, nu= nu)
    endEffectFactor= get_cylinder_end_effect_factor(b= b, l= l, solidityRatio= solidityRatio)
    return cf0*endEffectFactor

def get_cylinder_effective_wind_pressure(terrainCategory:str, vb, z, b, l, k, zMax= 200.0, rho= 1.25, k1= 1.0, c0= 1.0, nu= 15e-6, solidityRatio= 1.0, structuralFactor= 1.0):
    ''' Return the effective wind pressure on a circular cylinder according to 
        expression (7.19) of EN 1991-1-4:2005.
 
    :param terrainCategory: terrain category.
    :param vb: basic wind velocity.
    :param z: height above ground.
    :param b: diameter of the cylinder.
    :param l: length of the cylinder.
    :param k: equivalent surface roughness (see table 7.13 of EN 1991-1-4:2005).
    :param zMax: maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
    :param rho: air density.
    :param k1: turbulence factor.
    :param c0: orography factor.
    :param nu: kinematic viscosity of the air (ν=15*10-6 m2/s).
    :param solidityRatio: ratio between the projected areas of the member and the area of the evelope.
    :param structuralFactor: structural factor as defined in Section 6 of EN 1991-1-4:2005.
    '''
    qp= get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0)
    v= math.sqrt(2.0*qp/rho)
    cf= get_cylinder_force_coefficient(b=b, l= l, k= k, terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0, nu= nu, solidityRatio= solidityRatio)
    return cf*qp*structuralFactor

def get_turbulent_length_scale(terrainCategory:str, z:float, zt= 200.0, Lt= 300):
    ''' Compute the turbulent length scale according to expression (B.1) 
        of EN 1991-1-4:2005. 

    :param terrainCategory: terrain category.
    :param z: height above ground (m).
    '''
    zmin= get_zmin(terrainCategory)
    z0= get_z0(terrainCategory)
    a= 0.67+0.05*math.log(z0)
    z_calc= max(z, zmin)
    return Lt*math.pow(z_calc/zt, a)

def get_phi_long_wind_reduction_factor(terrainCategory:str, z:float, L:float, zt= 200.0, Lt= 300):
    ''' Compute the value of the phi function according to clause 8.3.4(1) of
        the Spanish annex for EN 1991-1-4:2005. 

    :param terrainCategory: terrain category.
    :param z: height above ground (m).
    :param L: bridge length.
    '''
    Lz= get_turbulent_length_scale(terrainCategory= terrainCategory, z= z, zt= zt, Lt= Lt)
    retval= 0.23+ 0.182*math.log(L/Lz)
    return max(min(retval,1.0), 0.0)

def get_spanish_annex_long_wind_reduction_factor(terrainCategory:str, z:float, L:float, c0= 1.0, zt= 200.0, Lt= 300):
    ''' Compute the value of the longitudinal wind reduction factor according
        to clause 8.3.4(1) of the Spanish annex for EN 1991-1-4:2005. 

    :param terrainCategory: terrain category.
    :param z: height above ground (m).
    :param L: bridge length.
    :param c0: orography factor.
    '''
    z0= get_z0(terrainCategory)
    denom= c0*math.log(z/z0)+7
    phi= get_phi_long_wind_reduction_factor(terrainCategory= terrainCategory, z= z, L= L, zt= zt, Lt= Lt)
    retval= 1-7/denom*phi
    return retval

# Values from the table 7.9 of EN 1991-1-4:2005.
fsw_l_h= [3.0, 5.0, 10.0]
fsw_cpnet_a= scipy.interpolate.interp1d(fsw_l_h, [2.3, 2.9, 3.4], kind='linear')
fsw_cpnet_b= scipy.interpolate.interp1d(fsw_l_h, [1.4, 1.8, 2.1], kind='linear')
fsw_cpnet_c= scipy.interpolate.interp1d(fsw_l_h, [1.2, 1.4, 1.7], kind='linear')
# fsw_cpnet_d= [1.2, 1.2, 1.2] no need to interpolate.

def get_free_standing_wall_abcd_cp(length:float, height:float, returnCornersLength= 0.0, solidityRatio= 1.0):
    ''' Compute the value of the pressure coefficients $c_{p,net}$ for 
        free-standing walls and parapets according to table 7.9 of 
        clause 7.4.1 of EN 1991-1-4:2005. 

    :param length: length of the free-standing wall.
    :param height: height of the free-standing wall.
    :param returnCornersLength: length of the return corner (see figure 7.19).
    :param solidityRatio: solidity ratio.
    '''
    # Compute values of c_p,net for zones A, B, C and D
    abcd= 4*[1.2]
    l_h= length/height
    if(solidityRatio==1.0):
        if(returnCornersLength>=height):
            abcd= [2.1, 1.8, 1.4, 1.2]
        else:
            factor= (1.0-returnCornersLength/height)
            a= factor*float(fsw_cpnet_a(l_h))
            b= factor*float(fsw_cpnet_b(l_h))
            c= factor*float(fsw_cpnet_c(l_h))
            d= factor*1.2
            if(returnCornersLength==0.0):
                abcd= [a, b, c, d]
    elif(solidityRatio==0.8):
        abcd= 4*[1.2]
    else:
        if(solidityRatio<0.8):
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(methodName+'; value of solidity ratio phi= '+str(phi)+' out of range [0.8, 1.0].')
        else: # 0.8 < solidityRatio < 1.0
            abcd1= get_free_standing_wall_abcd_cp(length= length, height= height, returnCornersLength= returnCornersLength, solidityRatio= 1.0)
            abcd= list()
            for v in abcd1:
                abcd.append((v-1.2)/0.2*(solidityRatio-0.8)+1.2)
    return abcd
                
def get_free_standing_wall_pressure_coefficients(length:float, height:float, returnCornersLength= 0.0, solidityRatio= 1.0):
    ''' Compute the value of the pressure coefficients $c_{p,net}$ for 
        free-standing walls and parapets according to clause 7.4.1 
        of EN 1991-1-4:2005. 

    :param length: length of the free-standing wall.
    :param height: height of the free-standing wall.
    :param returnCornersLength: length of the return corner (see figure 7.19).
    :param solidityRatio: solidity ratio.
    '''
    # Compute values of c_p,net for zones A, B, C and D
    abcd= get_free_standing_wall_abcd_cp(length= length, height= height, returnCornersLength= returnCornersLength, solidityRatio= solidityRatio)
    # Compute interpolation function
    l_h= length/height
    dx= .01/2.0
    xi= [0.0, 0.3*height-dx, .3*height+dx, 2*height-dx, 2*height+dx, 4*height-dx, 4*height+dx, length]
    cp_i= [abcd[0], abcd[0], abcd[1], abcd[1], abcd[2], abcd[2], abcd[3], abcd[3]]
    if(l_h<=4.0):
        cp_i[7]= 0.0
        cp_i[6]= 0.0
    elif(l_h<=2.0):
        cp_i[5]= 0.0
        cp_i[4]= 0.0
    elif(l_h<=0.3):
        cp_i[3]= 0.0
        cp_i[2]= 0.0
    return xi, cp_i
    
def get_free_standing_wall_net_pressure_distribution(terrainCategory:str, vb:float, length:float, height:float, hBase:float, zMax= 200.0, rho= 1.25, k1= 1.0, c0= 1.0, returnCornersLength= 0.0, solidityRatio= 1.0):
    ''' Compute the net pressure distribution for free-standing walls and 
        parapets according to clause 7.4.1 of EN 1991-1-4:2005.
        The net pressure corresponds to the overall wind effect on the 
        front face and the back face of the wall.

    :param terrainCategory: terrain category.
    :param vb: basic wind velocity.
    :param length: length of the free-standing wall.
    :param height: height of the free-standing wall.
    :param hBase: height of wall base above ground.
    :param zMax: maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
    :param rho: air density.
    :param k1: turbulence factor.
    :param c0: orography factor.
    :param returnCornersLength: length of the return corner (see figure 7.19).
    :param solidityRatio: solidity ratio.
    '''
    # Compute peak velocity pressure.
    qp= get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= hBase+height, zMax= zMax, rho= rho, k1= k1, c0= c0)

    # Compute values of c_p,net for zones A, B, C and D
    xi, cp_i= get_free_standing_wall_pressure_coefficients(length= length, height= height, returnCornersLength= returnCornersLength, solidityRatio= solidityRatio)
    wnet_i= list()
    for cp in cp_i:
        wnet_i.append(cp*qp)
    return scipy.interpolate.interp1d(xi, wnet_i, kind='linear')
