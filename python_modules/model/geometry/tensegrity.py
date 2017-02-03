# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2017, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es l.pereztato@ciccp.es" 

import xc_base
import geom
import xc
import math
import pandas as pd
import numpy as np

class tensegrityPrism(object):
    '''This class is aimed at constructing the model of a rotationally symetric     tensegrity prism with n-polygons on two parallel planes, twisted over angle 
    alfa with respect to each other. The twist angle is obtained by the 
    theorem of Tobie and Kenner as: alfa=pi/2-pi/n.
    The origin of the cartesian coordinate system is placed at the center of 
    the base circle, with the z-axis in the axis of the cylinder and joint 
    n+1.

    :ivar nSidPol: number of sides of the regular n-polygon
    :ivar RbaseC:  radius of the base circle circunscribing the n-polygon
    :ivar RtopC:   radius of the top circle circunscribing the n-polygon
    :ivar Hprism:  heigth of the prism (defaults to 0, change its value only if we want to fix 
                   the height of the prism, otherwise Hprism is calculated as a function of the
                   given length of the struts)
    :ivar Lstruts: length of the stuts (defaults to 0, only change this value if we want to 
                   fix the length of the struts and calculate the height of the prism as a 
                   function of Lstruts)
    :ivar alpha:   twist angle
    '''
    def __init__(self,nSidPol,RbaseC,RtopC,Hprism=0,Lstruts=0):
        self.nSidPol= nSidPol
        self.RbaseC= RbaseC
        self.RtopC=RtopC
        self.Hprism=Hprism
        self.Lstruts=Lstruts
        self.alpha=math.pi*(1/2-1/self.nSidPol)
        if Hprism==0:
            self.Hprism=math.sqrt(self.Lstruts**2-self.RtopC**2-self.RbaseC**2+2*self.RtopC*self.RbaseC*math.cos(2*math.pi/self.nSidPol+alfa))
        self.angPoly=2*math.pi/self.nSidPol


    def genJointsCoor(self):
        ''':returns: the cart. coord. of the joinst of a rotationally symetric  
        tensegrity prism with n-polygons on two parallel planes, twisted over angle 
        alfa with respect to each other. The twist angle is obtained by the 
        theorem of Tobie and Kenner as: alfa=pi/2-pi/n.
        The origin of the cartesian coordinate system is placed at the center of 
        the base circle, with the z-axis in the axis of the cylinder and joint 
        n+1.
        'jt' corresponds to joints in the top circle
        'jb' corresponds to joints in the base circle

        '''
        retval=pd.DataFrame(columns=("X","Y","Z"))
        #joints in top circle (jt):
        for i in range(1,self.nSidPol+1):
            ang=self.alpha+i*self.angPoly
            retval.loc['jt'+str(i)]=self.RtopC*math.cos(ang),self.RtopC*math.sin(ang),self.Hprism
        #joints in base circle (jb):
        for i in range(1,self.nSidPol+1):
            ang=(i-1)*self.angPoly
            retval.loc['jb'+str(i+self.nSidPol)]=self.RbaseC*math.cos(ang),self.RbaseC*math.sin(ang),0
        return retval



    def genLineLinkedJoints(self):
        '''Return the joints id linked by each line (strut or cable)
        'strut' corresponds to compression bars
        'sadd' corresponds to saddle strings (cables forming the n-polygons)
        'diag' corresponds to diagonal strings 
        '''
        retval=pd.DataFrame(columns=("i_jt","j_jt"))
        for i in range(1,self.nSidPol+1):
            retval.loc['strut'+str(i)]=i+self.nSidPol,i
        for i in range(1,self.nSidPol):
            retval.loc['sadd'+str(i)]=self.nSidPol+i,self.nSidPol+i+1
        retval.loc['sadd'+str(self.nSidPol)]=2*self.nSidPol,self.nSidPol+1
        for i in range(1,self.nSidPol):
            retval.loc['sadd'+str(self.nSidPol+i)]=i,i+1
        retval.loc['sadd'+str(2*self.nSidPol)]=self.nSidPol,1
        for i in range(1,self.nSidPol):
            retval.loc['diag'+str(i)]=self.nSidPol+1+i,i
        retval.loc['diag'+str(self.nSidPol)]=self.nSidPol+1,self.nSidPol
        return retval.astype(int)
    
