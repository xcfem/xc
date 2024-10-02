# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import math
from postprocess.reports import common_formats as fmt
from misc_utils import log_messages as lmsg
from materials.sections import rebar_family


class RCSection(object):
    '''Reinforced concrete section.

    :ivar b: section width.
    :ivar h: section depth.
    :ivar concrete: concrete material.
    :ivar tensionRebars: main reinforcement.
    '''
    def __init__(self, tensionRebars, concrete, b, h, memberType= None):
        ''' Constructor.

        :param b: section width.
        :param h: section depth.
        :param concrete: concrete material.
        :param tensionRebars: main reinforcement.
        :param memberType: element type (wall, slab, foundation,...).
        '''
        self.tensionRebars= tensionRebars
        self.concrete= concrete
        self.b= b
        self.h= h
        self.stressLimitUnderPermanentLoads= 230e6
        self.memberType= memberType

    def d(self):
        ''' Return the effective depth of the reinforcement.

        :param thickness: height of the rectangular section.
        '''
        return self.tensionRebars.d(self.h)

    def getAc(self):
        ''' Return the concrete area.'''
        return self.b*self.h-self.getAs()

    def getAs(self):
        ''' Return the tension reinforcement area.'''
        if(hasattr(self.tensionRebars,'spacing')):
           return self.tensionRebars.getAs(width= self.b)
        else:
           return self.tensionRebars.getAs()
           
    def getDimensionlessMomentRatio(self, MEd):
        ''' Return the ratio: MEd/(fcd*b*d^2).'''
        return -MEd/self.concrete.fcd()/self.b/self.d()**2

    def getDimensionlessReinforcementRatio(self):
        ''' Return the ratio: As*fyd/(Ac*fcd).'''
        fcd= -self.concrete.fcd()
        return self.tensionRebars.getT()/self.getAc()/fcd

    def setReinforcement(self,tensionRebars):
        self.tensionRebars= tensionRebars
        
    def getMinReinfAreaInBending(self):
        return self.tensionRebars.getMinReinfAreaInBending(concrete= self.concrete, thickness= self.h, memberType= self.memberType)
    
    def getMinReinfAreaInTension(self):
        '''Return the minimun amount of bonded reinforcement.'''
        return self.tensionRebars.getMinReinfAreaInTension(concrete= self.concrete,thickness= self.h, memberType= self.memberType)
    
    def getCompressionReinforcementArea(self, dp, Nd, Md):
        ''' Return the required compression reinforcement area according
            to expression for U' in section 15.5 3º of the book:
            Hormigón Armado Jiménez Montoya et al. ISBN: 84-252-1825-X

        :param dp: effective cover of the compression reinforcement.
        :param Nd: axial force (negative if compression).
        :param Md: bending moment (positive if the returned area in compression).
        '''
        NNd= -Nd
        d= self.d() # effective depth of the cross-section.
        # Eccentricity of the axial force with respect to the tensioned
        # (or less compressed) reinforcement.
        e0= Md/NNd
        e= e0+(d-dp)/2.0

        C= 0.35*self.b*d**2*-self.concrete.fcd()
        retval= NNd*e-C
        if(retval<0.0): # No need of compression reinforcement.
            retval= 0.0
        else:
            retval/= (d-dp)
            retval/= self.tensionRebars.steel.fyd()        
        return retval
    
    def getTensionReinforcementArea(self, Ap, dp, Nd, Md):
        ''' Return the required tension reinforcement area according
            to expression for U' in section 15.5 3º of the book:
            Hormigón Armado Jiménez Montoya et al. ISBN: 84-252-1825-X

        :param Ap: area of the compression reinforcement.
        :param dp: effective cover of the compression reinforcement.
        :param Nd: axial force.
        :param Md: bending moment (positive if the returned area in compression).
        '''
        retval= 0.0
        NNd= -Nd
        d= self.d() # effective depth of the cross-section.
        # Eccentricity of the axial force with respect to the tensioned
        # (or less compressed) reinforcement.
        e0= Md/NNd
        e= e0+(d-dp)/2.0

        fcd= -self.concrete.fcd()
        fyd= self.tensionRebars.steel.fyd()
        Up= Ap*fyd
        C= self.b*d*fcd
        C1= 0.7*C+Up
        C2= 0.45*C+Up
        if(NNd>C1):
            retval= (NNd-C1)/fyd
        elif(NNd>C2): # No need of tension reinforcement.
            retval= 0.0
        else:
            num= NNd*e-Up*(d-dp)
            denom= self.b*d**2*fcd
            radicand= 1-2*num/denom
            if(radicand>=0):
                root= math.sqrt(radicand)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; section depth: '+str(self.h)+' is too small.')
                exit(1)
            y= d*(1-root)
            retval= (self.b*y*fcd+Up+Nd)/fyd
        return retval

    def getMR(self, Nd= 0.0, Md= 0.0, z= None):
        ''' Return the resisting moment of the section.

        :param Nd: axial force.
        :param Md: bending moment.
        :param z: inner lever arm (if None z= 0.9*d).
        '''
        if(Nd==0.0):
            return self.tensionRebars.getMR(concrete= self.concrete, b= self.b, thickness= self.h, z= z)
        else:
            Ap= 0.0 # compression reinforcement area.
            d= self.d() # effective depth of the cross-section.
            dp= self.h-d # effective cover of the compression reinforcement.
            AsReq= self.getTensionReinforcementArea(Ap= Ap, dp= dp, Nd= Nd, Md= Md)
            As= self.getAs()
            AsDiff= AsReq-As
            F= AsDiff*self.tensionRebars.steel.fyd()
            M= F*(d-dp)
            retval= Md-M # Be cautious about this somewhat "invented" moment. 
            return retval
    
    def getVR(self, Nd, Md):
        ''' Get the the shear resistance carried by the concrete.

        :param Nd: design axial force.
        :param Md: design bending moment.
        '''
        return self.tensionRebars.getVR(self.concrete, Nd= Nd, Md= Md,b= self.b, thickness= self.h)
    
    def writeResultFlexion(self,outputFile,Nd,Md,Vd):
        AsMin= self.getMinReinfAreaInBending()
        outputFile.write("  RC section dimensions; b= "+ fmt.Length.format(self.b)+ " m, h= "+ fmt.Length.format(self.h)+ " m\\\\\n")
        self.tensionRebars.writeRebars(outputFile, self.concrete, AsMin)
        if(abs(Md)>0):
            MR= self.getMR(Nd= Nd, Md= Md)
            outputFile.write("  Bending check: Nd= "+ fmt.Esf.format(Nd/1e3) +" kN, Md= "+ fmt.Esf.format(Md/1e3)+ " kN m, MR= "+ fmt.Esf.format(MR/1e3)+ "kN m")
            rebar_family.writeF(outputFile,"  F(M)", MR/Md)
        if(abs(Vd)>0):
            VR= self.getVR(Nd,Md)
            outputFile.write("  Shear check: Nd= "+ fmt.Esf.format(Nd/1e3) +" kN, Vd= "+ fmt.Esf.format(Vd/1e3)+ " kN,  VR= "+ fmt.Esf.format(VR/1e3)+ " kN")
            rebar_family.writeF(outputFile,"  F(V)",VR/Vd)
          
    def writeResultTraction(self, outputFile, Nd):
        AsMin= self.getMinReinfAreaInTension()/2.0
        self.tensionRebars.writeRebars(outputFile= outputFile, concrete= self.concrete, AsMin= AsMin)
        if(abs(Nd)>0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for axial loads.')
          
    def writeResultCompression(self,outputFile,Nd,AsTrsv):
        ''' Results for compressed rebars.

        :param AsTrsv: Rebar area in transverse direction.
         '''
        AsMin= 0.2*AsTrsv # 20% of the transversal area.
        self.tensionRebars.writeRebars(outputFile,self.concrete,AsMin)
        if(abs(Nd)!=0.0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for axial loads.')

    def writeResultStress(self,outputFile,M):
      '''Cheking of stresses under permanent loads (SIA 262 fig. 31)'''
      if(abs(M)>0):
          stress= M/(0.9*self.h*self.getAs())
          outputFile.write("  Stress check: M= "+ fmt.Esf.format(M/1e3)+ " kN m, $\\sigma_s$= "+ fmt.Esf.format(stress/1e6)+ " MPa\\\\\n")
          outputFile.write("    $\\sigma_{lim}$= "+ fmt.Esf.format(self.stressLimitUnderPermanentLoads/1e6)+ " MPa")
          rebar_family.writeF(outputFile,"  F($\\sigma_s$)", self.stressLimitUnderPermanentLoads/stress)
