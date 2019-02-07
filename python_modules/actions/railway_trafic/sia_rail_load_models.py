# -*- coding: utf-8 -*-
from __future__ import division

'''Railway trafic load models from SIA 261 2003.'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2016, A_OO   LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"


import math
import scipy.interpolate
from miscUtils import LogMessages as lmsg

x_tab16= [0,6.4,20,2e6]
y_tab16= [220e3,220e3,145e3,145e3]
table16Interpolation= scipy.interpolate.interp1d(x_tab16,y_tab16)


class LoadModel(object):
  ''' Railway load model SIA 261. '''

  def __init__(self,mn,Qk= 160.0e3,nA= 4,dQk=1.6,qk= 70e3,dQq= 0.8):
    self.loadModelNumber= mn
    self.Qk= Qk
    self.numberOfAxes= nA
    self.dQk= dQk # Distance between Qk loads (m)
    self.qk= qk
    self.dQq= dQq # Distance between Qk and qk loads (m)

  def trainAccelerationLoad123(self,l):
    ''' returns locomotive acceleration load for models 1, 2 and 3 (table 14)'''
    retval= min(0.33e3*l,1000e3)
    return retval

  def locomotiveAccelerationLoadByAxe456(self):
    ''' returns locomotive acceleration load in each axe for models 4, 5 and 6 (table 19)'''
    retval= 0.3*self.Qk
    if(self.loadModelNumber==4):
      retval= min(retval,250e3)
    elif(self.loadModelNumber==5 or self.loadModelNumber==6):
      retval= min(retval,500e3)
    return retval

  def locomotiveAccelerationLoad456(self):
    ''' returns locomotive acceleration load for models 4, 5 and 6 (table 19)'''
    return self.locomotiveAccelerationLoadByAxe456()*self.numberOfAxes

  def trainBrakingLoad123(self,l):
    ''' returns train braking load for models 1, 2 and 3 (table 14)'''
    retval= 20e3*l
    if(self.loadModelNumber==1):
      retval= min(retval,6000e3)
    elif(self.loadModelNumber==3):
      retval*= 7/4.0 # 35/20
    return retval

  def trainBrakingLoad123(self,l):
    ''' returns train braking load for models 1, 2 and 3 (table 19)'''
    retval= 20e3*l
    if(self.loadModelNumber==1):
      retval= min(retval,6000e3)
    elif(self.loadModelNumber==3):
      retval*= 7/4.0 # 35/20
    return retval

  def trainBrakingLoad456(self,l):
    ''' returns train braking load for models 4, 5 and 6'''
    retval= 0.25*self.qk*l
    if(self.loadModelNumber==4):
      retval= min(retval,1250e3)
    elif(self.loadModelNumber==5):
      retval= min(retval,2500e3)
    elif(self.loadModelNumber==6):
      retval= min(retval,3500e3)
    return retval
  
  def brakingLoad(self,l):
    ''' returns braking load'''
    retval= 0.0
    if(self.loadModelNumber<4):
      retval= self.trainBrakingLoad123(l)
    else:
      retval= self.trainBrakingLoad456(l)
    return retval

  def accelerationLoad(self,l):
    ''' returns acceleration load'''
    retval= 0.0
    if(self.loadModelNumber<4):
      retval= self.trainAccelerationLoad123(l)
    else:
      retval= self.locomotiveAccelerationLoad456()
    return retval
  def nosingForce(self):
    ''' nosing force (11.2.3 et 12.2.3).'''
    retval= 100e3
    if(self.loadModelNumber==4):
      retval= 50e3
    elif(self.loadModelNumber==5):
      retval= 60e3
    elif(self.loadModelNumber==6 or self.loadModelNumber==7):
      retval= 80e3
    elif(self.loadModelNumber==8):
      retval= 80e3/70.0*15
    return retval

  def centrifugalForceReductionFactor(self,v):
    ''' centrifugal force reduction factor (11.3.2)'''
    retval= 1.0
    if(v>=(120/3.6)): # 120 km/h
      lmsg.warning('computing of centrifugal force reduction factor not implemented for speeds greather than 120 km/h (v= '+ str(v) + ' km/h).')
    return retval

  def centrifugalAcceleration(self,r,v):
    ''' centrifugal acceleration 11.2.4.3 '''
    eta= self.centrifugalForceReductionFactor(v)
    return eta*v*v/r
  def locomotiveCentrifugalForce(self,r,v):
    ''' centrifugal force for Qk loads 11.2.4.3 '''
    m= self.Qk/9.81
    a= self.centrifugalAcceleration(r,v)
    return m*a
  def trainCentrifugalForce(self,r,v):
    ''' centrifugal force for qk loads 11.2.4.3 '''
    m= self.qk/9.81
    a= self.centrifugalAcceleration(r,v)
    return m*a

  def locomotiveDerailmentModel1(self):
    ''' returns locomotive loads dues to derailment model 1 
        -tables 16 (standard gauge) and 19 (narrow-gauge)-'''
    retval= 350e3 # table 16
    # Narrow-gauge railway -> table 19 
    if(self.loadModelNumber==4):
      retval= 180e3 
    elif(self.loadModelNumber==5 or self.loadModelNumber==6):
      retval= 220e3
    elif(self.loadModelNumber==7):
      retval= 280e3
    elif(self.loadModelNumber==8):
      retval= 0
    return retval

  def locomotiveDerailmentModel2(self):
    ''' returns locomotive loads dues to derailment model 2 
        -tables 16 (standard gauge) and 19 (narrow-gauge)-'''
    return 0.0

  def trainDerailmentModel1(self):
    ''' returns train loads dues to derailment model 1 
        -tables 16 (standard gauge) and 19 (narrow-gauge)-'''
    retval= 110e3 # table 16
    # Narrow-gauge railway -> table 19
    if(self.loadModelNumber==4):
      retval= 35e3
    elif(self.loadModelNumber==5):
      retval= 70e3
    elif(self.loadModelNumber==6):
      retval= 100e3
    elif(self.loadModelNumber>=7):
      retval= 0.0
    return retval

  def trainDerailmentModel2(self, l):
    ''' returns train loads dues to derailment model 2 
        -tables 16 (standard gauge) and 19 (narrow-gauge)-

        :param l: length of the structure (in meters).'''
    # Standard gauge -> table 16
    if(self.loadModelNumber<4):
      retval= table16Interpolation(l) # table 16
    else: # Narrow-gauge railway -> table 19
      factor= 1.0
      if(l<20):
        factor= 1.4
      if(self.loadModelNumber==4):
        retval= factor*50e3
      elif(self.loadModelNumber==5):
        retval= factor*80e3
      elif(self.loadModelNumber==6):
        retval= factor*110e3
      elif(self.loadModelNumber==7):
        retval= 180e3 # For length l= 6.5 m
      elif(self.loadModelNumber==8):
        retval= 0
    return retval

    
    
    
  def writeData(self,f):
    print "LM ", self.loadModelNumber, " QBk= ", self.brakingLoad(1.0)/1e3, " kN/m"
    print "LM ", self.loadModelNumber, " QAk= ", self.accelerationLoad(1.0)/1e3, " kN"
    print "LM ", self.loadModelNumber, " QSk= ", self.nosingForce()/1e3, " kN"
    print "LM ", self.loadModelNumber, " derailment model 1 QEd= ", self.locomotiveDerailmentModel1()/1e3, " kN"
    print "LM ", self.loadModelNumber, " derailment model 1 qEd= ", self.trainDerailmentModel1()/1e3, " kN/m"
    print "LM ", self.loadModelNumber, " derailment model 2 qEd= ", self.trainDerailmentModel2()/1e3, " kN/m"

def dynamicFactor(lPhi):
  retval= 1.44/(math.sqrt(lPhi)-0.2)+0.82
  retval= max(1,retval)
  retval= min(retval,1.67)
  return retval

def determinantLengthSingleSpanPortalFrame(ls1,l,ls2):
  return 1.3*(ls1+l+ls2)/3.0


LM1= LoadModel(1,250e3,4,1.6,80e3,0.8)
LM2= LoadModel(2,0.0,0,1.6,133e3,5.3)
LM3= LoadModel(3,0.0,0,1.6,150e3,7.0)

LM4= LoadModel(4,130e3,2,1.6,25e3,0.8)
LM5= LoadModel(5,160e3,2,1.6,50e3,0.8)
LM6= LoadModel(6,160e3,4,1.6,70e3,0.8)
LM7= LoadModel(7,200e3,4,1.6,0.0,1e6)
LM8= LoadModel(8,0.0,0,0.0,15e3,0.0)

