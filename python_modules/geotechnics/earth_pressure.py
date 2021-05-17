# -*- coding: utf-8 -*-
from __future__ import division
from geotechnics import frictional_soil as fs
import math

'''earth_pressure.py: earth pressures for retaining structures.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# From Wikipedia:



class RankineSoil(fs.FrictionalSoil):
  '''From Wikipedia: Rankine's theory, developed in 1857, is a stress field solution that predicts active and passive earth pressure. It assumes that the soil is cohesionless, the wall is frictionless, the soil-wall interface is vertical, the failure surface on which the soil moves is planar, and the resultant force is angled parallel to the backfill surface. The equations for active and passive lateral earth pressure coefficients are given below. Note that φ is the angle of shearing resistance of the soil and the backfill is inclined at angle β to the horizontal.

  :ivar phi:    internal friction angle of the soil
  :ivar beta:   angle of backfill with horizontal
  '''
  def __init__(self,phi,beta= 0.0,rho= 2100.0):
    super(RankineSoil,self).__init__(phi,rho)
    self.beta= beta
  def Ka(self):
    '''Returns Rankine's active earth pressure coefficient.'''
    cBeta= math.cos(self.beta)
    cPhi= math.cos(self.phi)
    r= math.sqrt(cBeta**2-cPhi**2)
    return cBeta*(cBeta-r)/(cBeta+r)
  def Kp(self):
    '''Returns Rankine's passive earth pressure coefficient.'''
    cBeta= math.cos(self.beta)
    cPhi= math.cos(self.phi)
    r= math.sqrt(cBeta**2-cPhi**2)
    return cBeta*(cBeta+r)/(cBeta-r)

# Earth pressure according to Coulomb's Theory.
# This theory is valid if the backfill surface is plane and the wall-bacfill
# contact surface is also plane.
# Expressions taken from the books:
# @book{calavera2001muros,
#   title={Muros de contenci{\'o}n y muros de s{\'o}tano},
#   author={CALAVERA, J.A. and Ruiz, J.C. and Instituto Técnico de Materiales y Construcciones (Madrid)},
#   isbn={9788488764102},
#   url={https://books.google.ch/books?id=f41cOgAACAAJ},
#   year={2001},
#   publisher={INTEMAC INSTITUTO TECNICO DE MATERIALES Y CONSTRUCCIONES}
# # }
# @book{salas1971geotecnia,
#   title={Geotecnia y cimientos [por] J.A. Jim{\'e}nez Salas [y] J.L. de Justo Alpa{\~n}es},
#   author={Salas, J.A.J. and de Justo Alpa{\~n}es, J.L.},
#   url={https://books.google.ch/books?id=80tsHAAACAAJ},
#   year={1971},
#   publisher={Editorial Rueda}
# }

# Earth pressure coefficients.

def ka_coulomb(a,b,fi,d):
    '''Return the active earth pressure coefficient according to Coulomb theory.

    :param a:  angle of the back of the retaining wall (radians).
    :param b:  slope of the backfill (radians).
    :param fi: internal friction angle of the soil (radians).
    :param d:  friction angle between soil an back of retaining wall (radians).See Jiménez Salas, Geotecnia y Cimientos page 682.

    '''
    fSoil= fs.FrictionalSoil(fi)
    return fSoil.Ka_coulomb(a, b, d)

def kah_coulomb(a,b,fi,d):
    '''
    Return the horizontal component of the active earth pressure coefficient
    according to Coulomb's theory.

    :param a:  angle of the back of the retaining wall (radians).
    :param b:  slope of the backfill (radians).
    :param fi: internal friction angle of the soil (radians).
    :param d:  friction angle between soil an back of retaining wall (radians).
    '''
    fSoil= fs.FrictionalSoil(fi)
    return fSoil.Kah_coulomb(a, b, d)

def kav_coulomb(a,b,fi,d):
    '''
    Return the vertical component of the active earth pressure coefficient
    according to Coulomb's theory.

    :param a:  angle of the back of the retaining wall (radians).
    :param b:  slope of the backfill (radians).
    :param fi: internal friction angle of the soil (radians).
    :param d:  friction angle between soil an back of retaining wall (radians).
    '''
    fSoil= fs.FrictionalSoil(fi)
    return fSoil.Kav_coulomb(a, b, d)

def k_janssen(k,d,B,z):
    '''
    Lateral earth pressure coefficient for limited backfills according
    to Janssen's Theory (1895) and Kniss et Al (2007):

    :param k: lateral earth pressure (usually k= K_0).
    :param d: friction angle between soil an back of retaining wall (radians).
    :param B: width of the backfill (radians).
    :param z: depth from top of wall.
    '''
    retval= 0.0
    if(z!=0.0):
        tanD= math.tan(d)
        retval= 1.0/(2*tanD)*(B/z)*(1-math.exp(-2*k*z/B*tanD))
    return retval

def kp_coulomb(a,b,fi,d):
    '''
    Return the passive earth pressure coefficient according to Coulomb's theory.

    :param a:  angle of the back of the retaining wall (radians).
    :param b:  slope of the backfill (radians).
    :param fi: internal friction angle of the soil (radians).
    :param d:  friction angle between soil an back of retaining wall (radians).
    '''
    fSoil= fs.FrictionalSoil(fi)
    return fSoil.Kp_coulomb(a, b, d)

#Empujes unitarios debidos a cargas sobre el terreno.

def eq_coulomb(a,b,fi,d,p):
    '''
    Return the lateral earth pressure caused by a uniform load q
    action over the backfill surface according to Coulomb's theory.

    :param a: angle of the back of the retaining wall (radians).
    :param b: slope of the backfill (radians).
    :param fi: internal friction angle of the soil (radians).
    :param d: friction angle between soil an back of retaining wall (radians).
    :param q: Uniform load.
    '''
    fSoil= fs.FrictionalSoil(fi)
    return fSoil.eq_coulomb(a, b, d, p)

def eql_coulomb(x,H,z,ql):
    '''
    Return the earth pressure due to a linear load ql acting on a line
    parallel to the top of the wall on the surface of the retained backfill.

    :param x:  Distance from the top of the back and the linea where the load is applied.
    :param H:  Depth of the bottom end of the wall back.
    :param z:  depth of the point for which the pressure is computed.
    :param l: value of the linear load.
    '''
    m=x/float(H)
    n=z/float(H)
    if m<=0.4:
        return(0.2*m/float(math.pow((0.16+n*n),2))*ql/float(H))
    else:
        return(1.28*m**2*n/float((m**2+n**2)**2)*ql/float(H))
                


def eqp_coulomb(x,H,z,qp):
    '''
    Return the earth pressure resulting from a vertical point load qp
    acting behind a retaining wall and is contained in the same vertical
    plane of the wall section.

    :param x:  Distance from the top of the trasdós to the point of application 
        of the load.
    :param H:  Profundidad del extremo inferior del trasdós.
    :param z:  depth of the point for which the pressure is computed.
    :param qp: punctual load
    '''
    m=x/float(H)
    n= z/float(H)
    if m<=0.4:
        return(0.28*n**2/float(math.pow((0.16+n**2),3))*qp/float(H**2))
    else:
        return(1.77*m**2*n**2/float(math.pow((m**2+n**2),3))*qp/float(H**2))

