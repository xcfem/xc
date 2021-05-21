# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

import math
from misc_utils import log_messages as lmsg
from materials.astm_aisc import ASTM_materials
from materials.sections.structural_shapes import aisc_metric_shapes as lb
from postprocess import limit_state_data
from connections.steel_connections import connection_design
from numpy import piecewise

class BracingEndTee(object):
    '''Design of an end tee connection following example 5.1 of 
    AISC design guide 24: Hollow structural section connections 

    :ivar diagShape: HSS shape
    :ivar diagSteel: steel type of the HSS
    :ivar capThck: thickness of the cap plate (flange of the tee)
    :ivar weldMetal: weld metal
    :ivar weldFactor: weld factor used to calculate welds between minimum 
                       and maximum values
    :ivar wStemCap: leg size of the weld betweem tee stem and cap-plate 
                     (defaults to None in which case it's calculated 
                      using weldFactor)
    :ivar wHSScap: leg size of the weld betweem HSS shape and cap-plate
                     (defaults to None in which case it's calculated 
                      using weldFactor)

    '''
    
    def __init__(self,setCalc,diagShape,diagSteel,capThck,weldMetal,weldFactor,wStemCap=None,wHSScap=None):
        self.setCalc=setCalc
        self.diagShape=diagShape
        self.diagSteel=diagSteel
        self.capThck=capThck
        self.weldMetal=weldMetal
        self.weldFactor=weldFactor
        self.wStemCap=wStemCap
        self.wHSScap=wHSScap
        
    def check(self,cfg,boltArray,boltedPlate):
        '''
        :param cfg: config 
        :param boltArray: bolt array defined as instance of BoltArray class
        :param boltedPlate: bolted-plate defined as instance of BoltedPlate class
        '''
        Nd=get_Nd(cfg,self.setCalc,boltedPlate)
        #   *** Design of the end T connection
        # Adjust dimensions of the bolted plate
        boltedPlate.width=0.005 * math.ceil(boltedPlate.getMinWidth()/0.005)
        CFbp=boltedPlate.checkThickness(Nd)
        print('init. CFbp', CFbp)
        while CFbp>0.95:
            print('minimum thickness of bolted-plate incremented 5 mm')
            boltedPlate.width+=0.005
            CFbp=boltedPlate.checkThickness(Nd)
            print('CFbp',CFbp)
                
        init_bolted_plate_length=boltedPlate.length
        adj_bolted_plate_length=0.005 * math.ceil(boltedPlate.getMinLength()/0.005)
        overlapped_length=adj_bolted_plate_length+(init_bolted_plate_length-adj_bolted_plate_length)/2 #gusset has been dimensioned with the not-adjusted length 
        gap_length=50e-3 # 5cm between the gusset edge and the cap-plate
        bolted_plate_length=overlapped_length+gap_length
        # nominal strenght of the weld metal per unit area
        Fw=0.6*self.weldMetal.fu
        print ('\n ________________________________________________________')
        print('\n SET: ',self.setCalc.name + ' - ',self.setCalc.description.upper())
        print('\n            ***** Checking *****')
        # Welds connecting the tee flange (cap plate) to the HSS
        # Check of the thickness of the capMinimum thickness of the cap-plate
        # for allowing all four HSS walls welds to be fully effective
        # It's assumed that the force from the stemspreads on both sides
        # through the the cap plate at 2.5:1 slope
        if not(self.wStemCap):
            w_min=boltedPlate.getFilletMinimumLeg(self.capThck)
            w_max=boltedPlate.getFilletMaximumLeg(self.capThck)
            print('w_min=',round(w_min*1e3,1),'w_max=',round(w_max*1e3,1))
            self.wStemCap=w_min if w_max<w_min else round(w_min+self.weldFactor*(w_max-w_min),3)
        spreadL=boltedPlate.thickness+2*self.wStemCap+2*2.5*self.capThck

        # Available strenght of the welds connecting the cap-plate to the HSS
        if not(self.wHSScap):
            w_min=ASTM_materials.getFilletWeldMinimumLegSheets(self.capThck,self.diagShape.get('t'))
            w_max=ASTM_materials.getFilletWeldMaximumLegSheets(self.capThck,self.diagShape.get('t'))
            print('w_min=',round(w_min*1e3,1),'w_max=',round(w_max*1e3,1))
            self.wHSScap=w_min if w_max<w_min else round(w_min+self.weldFactor*(w_max-w_min),3)
        l=2*(self.diagShape.b()+self.diagShape.h()) # length of weld
        Aw=l*self.wHSScap/math.sqrt(2) # area of weld
        Rn=Fw*Aw   # weld strenght
        fi=0.75
        CF_weld_HSS_cap=Nd/(fi*Rn)   #capacity factor of the weld
        print('Weld cap-plate to HSS: leg size w= ',round(self.wHSScap*1e3,0), ' mm, CF=',round(CF_weld_HSS_cap,2))
        # Dimensions of the cap plate (minimum 5 mm around the weld to the HSS)
        capplat_B=(self.diagShape.b()+2*self.wHSScap+2*5e-3)
        capplat_B=0.005*math.ceil(capplat_B/0.005) # cap plate width
        capplat_H=(self.diagShape.h()+2*self.wHSScap+2*5e-3)
        capplat_H=max(boltedPlate.width,0.005 * math.ceil(capplat_H/0.005)) # cap plata height
        # Welds connecting the stem to the tee flange
        l=2*capplat_H
        Aw=l*self.wStemCap/math.sqrt(2) # area of weld
        Rn=Fw*Aw   # weld strenght
        fi=0.75
        CF_weld_stem_cap=Nd/(fi*Rn)   #capacity factor of the weld
        print ('Weld tee flange (cap plate) to tee stem: leg size w= ',round(self.wStemCap*1e3,0), ' mm, CF=',round(CF_weld_stem_cap,2))


        # Local yielding of the HSS wall
        if spreadL < self.diagShape.b():
            lmsg.error('Thickness of cap plane is insufficient to spread axial force to all four sides of the HSS diagonal')
        else:  # contribution of all the four walls of the HSS
            fi=1
            Rn=self.diagSteel.fy*self.diagShape.A() #available strenght of the SHH
            CF_yield_HSS=Nd/(fi*Rn)
            print('Local yielding of the HSS wall: CF=',round(CF_yield_HSS,2))

        # Shear yielding through the cap-plate thickness
        Ag=2*capplat_H*self.capThck  #shear area
        Rn=0.6*boltedPlate.steelType.fy*Ag
        fi=1
        CF_shear_yield_cap=Nd/(fi*Rn)
        print('Shear yielding through the cap-plate thickness: CF=',round(CF_shear_yield_cap,2))

        #Shear rupture through the cap-plate thickness
        fi=0.75
        Rn=0.6*boltedPlate.steelType.fu*Ag
        fi=0.75
        CF_shear_rupt_cap=Nd/(fi*Rn)
        print('Shear rupture through the cap-plate thickness: CF=',round(CF_shear_rupt_cap,2))

        print('\n            ***** Results *****')
        if spreadL < self.diagShape.b():
            lmsg.error('Thickness of cap plate is insufficient to spread axial force to all four sides of the HSS diagonal')
        else:
            print('Diagonal : ',lb.getMetricLabel(self.diagShape.name))
            print('Bolts : '+ boltedPlate.boltArray.bolt.getName())
            print('Lengh overlapped bolted-plate with gusset: ',round(overlapped_length*1e3,1), 'mm')
            print('Bolted-plate dimensions: ', round(bolted_plate_length*1e3,1), 'x',boltedPlate.width*1e3,'x',boltedPlate.thickness*1e3, '[L/W/T, mm]')
            print('Cap-plate dimensions: ', capplat_H*1e3, 'x',capplat_B*1e3,'x',self.capThck*1e3, '[L(in-plane)/W(out-of-plane)/T, mm]')
            print ('Weld bolted-plate to cap-plate w= ',round(self.wStemCap*1e3,0), ' mm')
            print('Weld HSS to cap-plate w= ',round(self.wHSScap*1e3,0), ' mm')
        print ('\n --------------------------------------------------------')


class BracingXcross(object):
    ''' Design of a concentric X-brace with HSS bracing. 
    One of the braces is continuous and the other spliced with
    a sandwich connection. Both sandwich plates are welded to 
    the HSS with flare bevel groove welds
    
    :ivar diagShape: HSS shape
    :ivar diagSteel: steel type of the HSS
    :ivar weldMetal: weld metal
    :ivar sandwPlateWidth: sandwich-plate width (defaults to None, in
                           which case it's computed from diagShape height)
    :ivar sandwPlateThck: sandwich-plate thickness (defaults to None, in
                           which case it calculated to have the same
                           capacity as the HSS)
    :ivar plateSteel: steel type of the sandwich-plates
    :ivar weldLength: length of each weld sandwich-plate to bracing 
                      (defaults to None, in which case minimum 
                       length is computed)
    
    '''
    
    def __init__(self,setCalc,diagShape,diagSteel,plateSteel,weldMetal,sandwPlateWidth=None,sandwPlateThck=None,weldLength=None):
        self.setCalc=setCalc
        self.diagShape=diagShape
        self.diagSteel=diagSteel
        self.plateSteel=plateSteel
        self.weldMetal=weldMetal
        self.sandwPlateWidth=sandwPlateWidth
        self.sandwPlateThck=sandwPlateThck
        self.weldLength=weldLength
        
    def check(self,cfg,boltedPlate):
        '''
        :param cfg: config 
        :param boltedPlate: bolted-plate defined as instance of BoltedPlate clas        '''
        Nd=get_Nd(cfg,self.setCalc,boltedPlate)
        Fw=0.6*self.weldMetal.fu
        print ('\n ________________________________________________________')
        print('\n SET: ',self.setCalc.name + ' - ',self.setCalc.description.upper())
        print('\n            ***** Checking *****')

        #Local yielding of the HSS
        fi=1
        Rn=self.diagSteel.fy*self.diagShape.A() #available strenght of the SHH
        CF_yield_HSS=Nd/(fi*Rn)
        print('Local yielding of the HSS wall:                CF=',round(CF_yield_HSS,2))
        #Local yielding of the sandwich plates
        if not self.sandwPlateWidth:
            self.sandwPlateWidth=self.diagShape.h()+2*20e-3
            print('Sandwich-plate width (strict) = ',self.sandwPlateWidth*1e3, 'mm')
        elif self.sandwPlateWidth-(self.diagShape.h()+2*20e-3)<0:
            lmsg.warning('Check width of sandwich plate: gap plate-HSS =' +str(round((self.sandwPlateWidth-self.diagShape.h())/2*1e3,1)))
        if not self.sandwPlateThck:
            strictThck=self.diagShape.A()*self.diagSteel.fy/(2*self.sandwPlateWidth*self.plateSteel.fy)
            self.sandwPlateThck=1e-3*math.ceil(strictThck/1e-3)
            print('Sandwich-plate thickness (strict) = ',round(self.sandwPlateThck*1e3,1), 'mm')
            
        Aplates=2*self.sandwPlateThck*self.sandwPlateWidth
        Rn=self.plateSteel.fy*Aplates
        fi=1
        CF_yield_plates=Nd/(fi*Rn)
        
        print('Local yielding of the sandwich plates:         CF=',round(CF_yield_plates,2))
        
        # outside radius of HSS shape {table J2.2 AISC}
        R=self.diagShape.getOutsideRadius()
        if R < round(3/8*0.0254,5): #3/8in
            lmsg.error('R= '+ str(round(R*1e3,1))+ 'mm. For flare bevel groove with R<3/8in, only allowed to use reinforcing fillet weld on filled flush joint')
        # {Table J2.2 AISC} Effective throat of flare groove welds
        eff_thr=5/16*R
        # {Table J2.3 AISC} Minimum Effective throat of flare groove welds
        tmin=min(self.diagShape.t(),self.sandwPlateThck)
        x=tmin*1e3
        min_eff_thr=piecewise(x,
               [x<=6,x>6 and x<=13,x>13 and x<=19,x>19 and x<=38,
                x>38 and x<=57,x>57 and x<=150,x>150],
               [3,5,6,8,10,13,16])*1e-3
        eff_thr=max(eff_thr,min_eff_thr)
        print('Effective throat of welds plate-HSS = ',round(eff_thr*1e3,1),'mm')
        if not self.weldLength:
            self.weldLength=Nd/(0.75*Fw*4*eff_thr)
        Aw=4*self.weldLength*eff_thr
        Rn=Fw*Aw
        fi=0.75
        CF_weld_sndw_HSS=Nd/(fi*Rn)
        print ('Length of welds plate-HSS: lw= ', round(self.weldLength*1e3,0))
        print ('Flare Bevel groove weld plate - spliced HSS:   CF=',round(CF_weld_sndw_HSS,2))
        print('\n            ***** Results *****') 
        print('Diagonal : ',lb.getMetricLabel(self.diagShape.name))
        print('Sandwich-plate width: ', self.sandwPlateWidth*1e3,' mm')
        print('Sandwich-plate thickness: ', round(self.sandwPlateThck*1e3,1),' mm')
        print('Weld: ',1*math.ceil(R*1e3/1),'(',eff_thr*1e3,') , length=', round(self.weldLength*1e3,0),'mm')

        print ('\n --------------------------------------------------------')
       
              
def get_Nd(cfg,setCalc,boltedPlate):
        limit_state_data.LimitStateData.envConfig= cfg 
        limitState= limit_state_data.normalStressesResistance
        outCfg= limit_state_data.VerifOutVars(setCalc,appendToResFile='N',listFile='N',calcMeanCF='Y')
        limitState= limit_state_data.normalStressesResistance
        intForcesFileName= limitState.getInternalForcesFileName()
        controller= connection_design.BoltedPlateController(boltedPlate)
        worstCase= controller.predimConnection(intForcesFileName,setCalc)
        Nd=worstCase.forces.N #maximum axial force
        return Nd
