# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import scipy.interpolate
from materials.aci import ACI_materials
from materials.sections import rebar_family as rf
import geom
from materials import limit_state_checking_base as lsc
from postprocess import control_vars as cv
from rough_calculations import ng_simple_bending_reinforcement
import math
import xc_base
import geom
from materials.sections.fiber_section import fiber_sets
from materials.sections import stressCalc as sc
from miscUtils import LogMessages as lmsg
from postprocess.reports import common_formats as fmt

class RebarController(object):
    '''Control of some parameters as development lenght 
       minimum reinforcement and so on.

       :ivar psi_t: reinforcement location factor; "concrete below" is 
                    taken as the depth from the rebar center to the bottom 
                    of the concrete section.
                    * 1.3 for concrete below ≥ 12 inches 
                    * 1.0 for concrete below < 12 inches 
       :ivar psi_e: coating factor 
                    * 1.5 for epoxy coated bars with cover (to center of bar)
                          less than 3db (spacing is not calculated)
                    * 1.2 for all other epoxy coated bars 
                    * 1.0 for uncoated bars
       :ivar Ktr: transverse reinforcement index. Factor that represents the 
                  contribution of confining reinforcement across potential 
                  splitting planes; is conservatively assumed to be zero.
       :ivar concreteCover: the distance from center of a bar or wire to 
                            nearest concrete surface.
       :ivar spacing: center-to-center spacing of bars or wires being 
                      developed, in.
    '''

    def __init__(self, psi_t= 1.3, psi_e= 1.0, concreteCover= 35e-3, spacing= 150e-3):
        '''Constructor.'''
        self.psi_t= psi_t
        self.psi_e= psi_e
        self.concreteCover= concreteCover
        self.spacing= spacing

    def getKtr(self, n= 1, Atr= 0.0):
        '''Return the transverse reinforcing index according to
           clause 25.4.2.3 of ACI 318-14.

        :param n: number of bars or wires being developed or
                  lap spliced along the plane of splitting.
        :param Atr: total cross-sectional area of all transverse reinforcement
                    within spacing s that crosses the potential plane of
                    splitting through the reinforcement being developed.
        '''
        Atr_inch2= Atr*(1.0/0.0254)**2
        return (40.0*Atr/self.spacing/n)*0.0254 #To meters.

    def getConfinementTerm(self, phi, n= 1, Atr= 0.0):
        '''Return the confinement term as defined in clause 25.4.2.3 
           of ACI 318-14.

        :param phi: nominal diameter of bar, wire, or prestressing strand.
        :param n: number of bars or wires being developed or
                  lap spliced along the plane of splitting.
        :param Atr: total cross-sectional area of all transverse reinforcement
                    within spacing s that crosses the potential plane of
                    splitting through the reinforcement being developed.
        '''
        Ktr= self.getKtr(n,Atr)
        return (self.concreteCover+Ktr)/phi
    def getBasicAnchorageLength(self, concrete, phi, steel):
        """Returns anchorage length in tension according to ACI-318 25.4.2.3.

        :param concrete: concrete material.
        :param phi: nominal diameter of bar, wire, or prestressing strand.
        :param fck: concrete characteristic compressive strength.
        :param steel: reinforcement steel.
        """
        psi_t_psi_e= min(self.psi_t*self.psi_e,1.7)
        psi_t_psi_e_psi_s= psi_t_psi_e*ACI_materials.getPsi_sFromDiameter(phi)
        #Clause 25.4.1.4:
        l= min(concrete.getLambdaSqrtFck(),concrete.Lambda*ACI_materials.toPascal*100.0)
        retval= 3.0/40.0*(steel.fyk/l)
        retval*= psi_t_psi_e_psi_s/self.getConfinementTerm(phi)
        retval*= phi
        return max(retval,12*0.0254) #Clause 25.4.2.1b

# Shear checking.

def VcNoShearRebars(concrete,Nd,b,d):
    '''Return concrete shear capacity on a (b x thickness)
       rectangular section according to clause 22.5.5.1 of ACI 318-14.

    :param concrete: concrete material.
    :param Nd: design axial force.
    :param b: width of the rectangular section.
    :param d: effective thickness of the RC section.
    '''
    retval= 2.0*concrete.getLambdaSqrtFck()*b*d
    if(Nd<0.0):
        retval*=(1-Nd/b/d/(2000.0*ACI_materials.toPascal))
    return retval

def V_max(concrete,Nd,b,d):
    '''Return the ultimate shear strength of the section 
       a (b x thickness) rectangular section according to clause 
       22.5.1.2 of ACI 318-14.

    :param concrete: concrete material.
    :param Nd: design axial force.
    :param b: width of the rectangular section.
    :param d: effective thickness of the RC section.
    '''
    retval= VcNoShearRebars(concrete,Nd,b,d)
    retval+= 8.0*concrete.getLambdaSqrtFck()*b*d
    retval/= concrete.gmmC
    return retval

class ShearController(lsc.ShearControllerBase):
    '''Object that controls shear limit state according to ACI 318.'''
    def __init__(self,limitStateLabel):
        super(ShearController,self).__init__(limitStateLabel)
    def setSection(self,rcSection):
        self.concrete= rcSection.concrType #Arreglar
        self.steel= rcSection.reinfSteelType
        self.width= rcSection.b
        self.effectiveDepth= 0.9*rcSection.h
        self.mechanicLeverArm= 0.9*self.effectiveDepth #Mejorar
        self.AsTrsv= rcSection.shReinfY.getAs()
        self.s= rcSection.shReinfY.shReinfSpacing
        self.Vc= 0.0 # Concrete contribution to the shear strength.
        self.Vu= 0.0 # Ultimate shear strength.
        
    def getVcNoShearRebars(self,Nd):
        '''Return the shear resistance carried by the concrete on a (b x thickness)
           rectangular section according to clause 22.5.5.1 of ACI 318-14.

        :param Nd: design axial force.
        '''
        return VcNoShearRebars(self.concrete, Nd, self.width, self.effectiveDepth)
    
    def getV_max(self,Nd):
        '''Return the maximum shear resistance that can be carried by 
           the concrete of the section according to clause 
           22.5.1.2 of ACI 318-14.

        :param Nd: design axial force.
        '''
        return V_max(self.concrete, Nd, self.width, self.effectiveDepth)
        
    def calcVc(self, Nd):
        ''' Computes the shear strength of the section without 
            shear reinforcement.

        :param concrete: concrete material.
        :param Nd: design axial force.
        '''
        self.Vc= self.getVcNoShearRebars(Nd)
        return self.Vc
    
    def getVu(self,Nd):
        '''Return the shear strength of the concrete section
           according to clause 16.5.2.3 of ACI 318-14.

        :param Nd: design axial force.
        '''
        self.calcVc(Nd)
        V_max= self.getV_max(Nd)
        retval= self.Vc/self.concrete.gmmC # ACI 9.6.3.1
        Av= self.AsTrsv*self.effectiveDepth
        if(Av>0.0): # ACI 22.5.1.1, 22.5.10.1, 20.5.10.5.3
            retval+= Av*self.steel.fyd()
        retval= min(V_max,retval) #ACI 22.5.1.2
        self.Vsu= retval-self.Vc
        return retval
    
    def calcVu(self, Nd):
        '''Return the ultimate shear resistance carried by the concrete on 
           a (b x thickness) rectangular section according to clause 
           22.5.1.2 of ACI 318-14.

        :param Nd: design axial force.
        '''
        self.Vu= self.getVu(Nd)
        return self.Vu
    
    def check(self,elements,nmbComb):
        '''
        Check the shear strength of the RC section.
           Transverse reinforcement is not
           taken into account yet.
        '''
        lmsg.log("Postprocesing combination: "+nmbComb)
        # XXX torsional deformation ingnored.

        for e in elements:
            e.getResistingForce()
            scc= e.getSection()
            idSection= e.getProp("idSection")
            section= scc.getProp("datosSecc")
            self.setSection(section)
            NTmp= scc.getStressResultantComponent("N")
            VuTmp= self.getVu(NTmp) 
            VyTmp= scc.getStressResultantComponent("Vy")
            VzTmp= scc.getStressResultantComponent("Vz")
            VTmp= math.sqrt((VyTmp)**2+(VzTmp)**2)
            if(VuTmp!=0.0):
              FCtmp= abs(VTmp)/VuTmp
            else:
              FCtmp= 10
            if(FCtmp>=e.getProp(self.limitStateLabel).CF):
              MyTmp= scc.getStressResultantComponent("My")
              MzTmp= scc.getStressResultantComponent("Mz")
              Mu= 0.0 # Not used in ACI-318
              theta= None # Not used in ACI-318
              e.setProp(self.limitStateLabel,cv.RCShearControlVars(idSection,nmbComb,FCtmp,NTmp,MyTmp,MzTmp,Mu,VyTmp,VzTmp,theta,self.Vc,self.Vsu,VuTmp)) # Worst cas

##################
# Rebar families.#
##################

class ACIRebarFamily(rf.RebarFamily):
    ''' Family or reinforcement bars with checking according to ACI 318-14.

       :ivar barNumber: bar number in ACI 318 code.
       :ivar psi_t: reinforcement location factor; "concrete below" is 
                    taken as the depth from the rebar center to the bottom 
                    of the concrete section.
                    * 1.3 for concrete below ≥ 12 inches 
                    * 1.0 for concrete below < 12 inches 
       :ivar psi_e: coating factor 
                    * 1.5 for epoxy coated bars with cover (to center of bar)
                          less than 3db (spacing is not calculated)
                    * 1.2 for all other epoxy coated bars 
                    * 1.0 for uncoated bars
    '''
    def __init__(self,steel,barNumber,spacing,concreteCover):
      ''' Constructor.

      :param steel: reinforcing steel material.
      :param barNumber: diameter of the bars.
      :param spacing: spacing of the bars.
      :param concreteCover: concrete cover of the bars.
      '''
      self.barNumber= barNumber
      diam= ACI_materials.standard_bars_diameters[barNumber]
      super(ACIRebarFamily,self).__init__(steel,diam,spacing,concreteCover)
      self.psi_t= 1.3
      self.psi_e= 1.0 # uncoated bars.

    def getCopy(self,barController):
      return ACIRebarFamily(self.steel,self.barNumber,self.spacing,self.concreteCover)
    def getRebarController(self):
      return RebarController(psi_t= self.psi_t, psi_e= self.psi_e, concreteCover= self.concreteCover, spacing= self.spacing)
    def getBasicAnchorageLength(self,concrete):
      ''' Return the basic anchorage length of the bars.'''
      rebarController= self.getRebarController()
      return rebarController.getBasicAnchorageLength(concrete,self.getDiam(),self.steel)
    def getMinReinfAreaUnderFlexion(self, thickness, b= 1.0, type= 'slab', concrete= None):
        '''Return the minimun amount of bonded reinforcement to control cracking
           for reinforced concrete sections under flexion per unit length 
           according to clauses 7.6.1.1, 8.6.1.1, 9.6.1.2, 10.6.1.1, 11.6.1,
           12.6.1 

        :param steel: reinforcement steel.
        :param thickness: gross thickness of concrete section (doesn't include 
                          the area of the voids).
        '''
        retval= 0.0025*thickness*b
        fy= self.steel.fyk
        if(type=='slab'):
            limit= ACI_materials.toPascal*60e3
            retval= thickness # b= 1
            if(fy<limit):
                retval*= 0.0020
            else:
                retval*= max(0.0018*limit/fy,0.0014)
        elif(type=='wall'):
            retval= 0.0025*thickness # b= 1
        elif(type=='beam'):
            d= 0.9*thickness
            retval= d*b
            retval*= max(3.0*concrete.getSqrtFck(),ACI_materials.toPascal*200)
        elif(type=='column'):
            retval= 0.01*thickness*b
        return retval

    def getMinReinfAreaUnderTension(self,thickness, type= 'slab', concrete= None):
        '''Return the minimun amount of bonded reinforcement to control cracking
           for reinforced concrete sections under tension.

        :param concrete: concrete material.
        :param thickness: thickness of the tensioned member.
        '''
        return 2.0*self.getMinReinfAreaUnderFlexion(thickness= thickness, type= type, concrete= concrete)

    def getVR(self,concrete,Nd,Md,b,thickness):
        '''Return the shear resistance carried by the concrete on a (b x thickness)
           rectangular section according to clause 22.5.5.1 of ACI 318-14.

        :param concrete: concrete material.
        :param Nd: design axial force.
        :param Md: design bending moment.
        :param b: width of the rectangular section.
        :param thickness: height of the rectangular section.
        '''
        return VcNoShearRebars(concrete,Nd,Md,AsTrac,b,0.9*thickness)

    def writeRebars(self, outputFile,concrete,AsMin):
        '''Write rebar family data.'''
        self.writeDef(outputFile,concrete)
        outputFile.write("  area: As= "+ fmt.Areas.format(self.getAs()*1e4) + " cm2/m areaMin: " + fmt.Areas.format(AsMin*1e4) + " cm2/m")
        rf.writeF(outputFile,"  F(As)", self.getAs()/AsMin)

class ACIFamNBars(ACIRebarFamily):
    n= 2 #Number of bars.
    def __init__(self,steel,n,barNumber,spacing,concreteCover):
        RebarFamily.__init__(self,steel,barNumber,spacing,concreteCover)
        self.n= int(n)
    def __repr__(self):
        return str(n) + " x " + self.steel.name + ", diam: " + str(int(self.diam*1e3)) + " mm, e= " + str(int(self.spacing*1e3))
    def writeDef(self,outputFile,concrete):
        outputFile.write("  n= "+str(self.n)+" diam: "+ fmt.Diam.format(self.getDiam()*1000) + " mm, spacing: "+ fmt.Diam.format(self.spacing*1e3)+ " mm")
        ancrage= self.getBasicAnchorageLength(concrete)
        outputFile.write("  l. ancrage L="+ fmt.Lengths.format(ancrage) + " m ("+ fmt.Diam.format(ancrage/self.getDiam())+ " diamètres).\\\\\n")

class ACIDoubleRebarFamily(rf.DoubleRebarFamily):
    ''' Two reinforcement bars families.'''
    def getCopy(self,barController):
        return ACIDoubleRebarFamily(self.f1, self.f2)
    def getVR(self,concrete,Nd,Md,b,thickness):
        '''Return the shear resistance of the (b x thickness) rectangular section.
        :param concrete: concrete material.
        :param Nd: design axial force.
        :param Md: design bending moment.
        :param b: width of the rectangular section.
        :param thickness: height of the rectangular section.
        '''
        return self.f1.getVR(concrete,Nd,Md,b,thickness)
    def writeRebars(self, outputFile,concrete,AsMin):
        '''Write rebar family data.'''
        self.writeDef(outputFile,concrete)
        outputFile.write("  area: As= "+ fmt.Areas.format(self.getAs()*1e4) + " cm2/m areaMin: " + fmt.Areas.format(AsMin*1e4) + " cm2/m")
        rf.writeF(outputFile,"  F(As)", self.getAs()/AsMin)
