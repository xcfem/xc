# -*- coding: utf-8 -*-

import sys
import math

def rasanteAgotamiento(fck,gammac,hf,Asf,Sf,fyd):
    #Cálculo del rasante medio por unidad de longitud que agota la sección
    #del ala de la viga, según el artículo 4.3.2.5 del Eurocódigo 2
    #el resultado está expresado en kN/m (ó N/mm)
    #Datos:
    #fck: resistencia característica a compresión del hormigón (N/mm2)
    #gammac= Partial safety factor for concrete.
    #hf: flange thickness (m)
    #Asf: reinforcement por unidad de longitud que atraviesa la sección (mm2)
    #Sf: spacement of the rebars that cross the section (mm)
    #fyd: resistencia de cálculo de la reinforcement (N/mm2)
    hf=hf*1000     #Flange thickness in mm
    #Esfuerzo rasante de agotamiento por compresión oblicua en la sección
    fcd=fck/gammac
    VRd2=0.2*fcd*hf
    #Esfuerzo rasante de agotamiento por tracción en la sección
    tabla48EC2={12:0.18,16:0.22,20:0.26,25:0.30,30:0.34,35:0.37,40:0.41,45:0.44,50:0.48}
    taoRd=tabla48EC2[fck]
    VRd3=2.5*taoRd*hf+Asf/Sf*fyd
    return min(VRd2,VRd3)




#Ejemplo:
#  esfRasMax=rasanteAgotamiento(25,1.5,300,565,200,500)
