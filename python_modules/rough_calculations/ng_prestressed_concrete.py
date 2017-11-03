# -*- coding: utf-8 -*-

'''Rough calculations related to prestressed concrete
(geometry, losses of prestress, ...) 
'''

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import xc_base
import geom
from geom_utils import parabola as pb
import math
from scipy import integrate


#Rough calculation of prestress loss due to elastic shortening of concrete
def loss_elastic_shortening_concr(Ac,Ic,Ec,Ep,Ap,ec_p,sigma_p_0,M_sw):
    '''return a rough estimation of the loss of prestress in a section due 
    to elastic shortening of concrete.

    :param Ac: cross-section area of concrete
    :param Ic: moment of inertia of the concrete cross-section
    :param Ec: modulus of elasticity of concrete
    :param Ep: modulus of elasticity of prestressing steel
    :param Ap: area of prestressing steel
    :param ec_p: eccentricity of tendon (with respect to COG of concrete 
                 section)
    :param sigma_p_0: initial stress in tendon
    :param M_sw: bending moment in the section due to self weight
    '''
    sigma_c=sigma_concr_tendon_lev(Ac,Ic,Ec,Ep,Ap,ec_p,sigma_p_0,M_sw)
    loss_prestr=Ep/Ec*sigma_c
    return loss_prestr
  
def sigma_concr_tendon_lev(Ac,Ic,Ec,Ep,Ap,ec_p,sigma_p_0,M_sw):
    '''return a rough estimation of the stress in concrete at the level of 
    the tendon.

    :param Ac: cross-section area of concrete
    :param Ic: moment of inertia of the concrete cross-section
    :param Ec: modulus of elasticity of concrete
    :param Ep: modulus of elasticity of prestressing steel
    :param Ap: area of prestressing steel
    :param ec_p: eccentricity of tendon (with respect to COG of concrete 
                 section)
    :param sigma_p_0: initial stress in tendon
    :param M_sw: bending moment in the section due to self weight
    '''
    r=(Ic/Ac)**0.5  #radius of gyration of section [m]
    m=Ep/Ec    
    fco=sigma_p_0/(m+Ac/Ap/(1+ec_p**2/r**2))-M_sw*ec_p/Ic
    return fco


def loss_friction(s,sigma_p_0,alpha_unit,mu,unint_dev):
    '''return a rough estimation of the loss of prestress due to friction.

    :param s: cable length from start to section considered
    :param sigma_p_0: initial stress in tendon
    :param alpha_unit: mean angular deviation per unit lenght. 
        E.g.\: Assimilating the parabolic profile of the cable to a 
        circular profile the angular deviation is constant in the beam length 
        and can be expressed as\:
          8*eccentricity_mid_span/Lbeam**2
    :param unint_dev: unintentional angular deviation per unit length
    '''
    loss_prestr=sigma_p_0*(1-math.exp(-mu*(alpha_unit+unint_dev)*s))
    return loss_prestr


class prestressingWire(object):
    trace= None
    x0= 0.0
    xL= 1.0
    y= 0.0
    def __init__(self,p0,p1,p2,x0,xL,y):
        self.trace= pb.Parabola(p0,p1,p2)
        self.x0= x0
        self.xL= xL
        self.y= y
    def z(self,x):
        return self.trace.y(x)
    def getL(self):
        return self.xL-self.x0
    def alpha(self,x):
        return self.trace.alpha(x)
    def curvature(self,x):
        return self.trace.curvature(x)
    def getPoint(self,x):
        return geom.Pos3d(x,self.y,self.z(x))
    def getForce(self,x,P):
        alpha= self.alpha(x)
        return P*geom.Vector3d(math.cos(alpha),0.0,math.sin(alpha))
    def getU(self,x,P):
        alpha= self.alpha(x)
        r= self.curvature(x)
        return P*r
    def getIntegratedU(self,P):
        numInt= 10
        x= []
        y= []
        L= self.getL()
        for i in range(0,numInt+1):
            v= float(i)/float(numInt)*L
            x.append(v)
            y.append(self.getU(v,P))
        return integrate.simps(y,x)

