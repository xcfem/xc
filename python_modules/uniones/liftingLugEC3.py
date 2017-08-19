# -*- coding: utf-8 -*-
'''Design of a lifting lug according to clause 3.13 of EC3 part 1_8.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

def getAMinLiftingLug(F, gammaM0, t, d0, fy):
  '''
  Return the minimum value for "a" (width in front of the
  hole, see figure in EC3).

   :param F: Load on the lifting lug.
   :param gammaM0: Partial safety factor for steel.
   :param t: lifting lug thickness.
   :param d0: Hole diameter.
   :param fy: Steel yield strength
  
  '''
  return F*gammaM0/(2*t*fy)+2*d0/3


def getCMinLiftingLug(F, gammaM0, t, d0, fy):
  '''
   Return the minimum value for "c" (width at both sides of the
   hole, see figure in EC3).

   :param F: Load on the lifting lug.
   :param gammaM0: Partial safety factor for steel.
   :param t: lifting lug thickness.
   :param d0: Hole diameter.
   :param fy: Steel yield strength
  
  '''
  return F*gammaM0/(2*t*fy)+d0/3


def getBoltShearStrength(A, gammaM2, fup):
  '''
   Return the shear strength of the bolt.

   :param A: Bolt area.
   :param gammaM2: Partial safety factor for steel.
   :param fup: Ultimate steel strength.
  
  '''
  return 0.6*A*fup/gammaM2

def getPadEyeCrushingForce(t, d, fy, gammaM0):
  '''
   Return the crushing strength of the pad eye.
   The bold is not supposed to be replaceable (see table 3.10, 
   second failure mode).

   :param t: Thickness of the pad eye steel plate.
   :param d: Hole diameter.
   :param fy: Steel yield strength.
   :param gammaM0: Partial safety factor for steel.
  
  '''
  return 1.5*t*d*fy/gammaM0

def getBendingMomentoOnBolt(F, b, a, c):
  '''
   Return the maximum bending moment on the bolt from
   the dimensions of the split pin and the pad eye (see figure 3.11).

   :param F: Design value of the force that loads the joint.
   :param b: Pad eye steel plate thickness.
   :param a: Split pin steel plate thickness.
   :param c: Gap between each side of the split pin and the pad eye.
  
  '''
  return F/8*(b+4*c+2*a)

def getBoltMResist(W, fy, gammaM0):
  '''
   Return the bending resistance of the bolt.
   (see table table 3.10, third failure mode)

   :param W: Elastic section modulus.
   :param fy : Steel yield strength
   :param gammaM0: Partial safety factor for steel.
  
  '''
  return 1.5*W*fy/gammaM0

def getBoltCapacityFactor(M, MR, V, VR):
  '''
   Returns a value lesser than 1 if the bolt can
   resist the combination of bending moment and shear force.
   (see table 3.10, fourth failure mode)

   :param M: Bending moment.
   :PARAM MR: Resisting bending moment.
   :param V: Shear force.
   :PARAM VR: Resisting shear force.
  '''
  return sqr(M/MR)+sqr(V/VR)



