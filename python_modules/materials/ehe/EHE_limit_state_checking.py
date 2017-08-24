# -*- coding: utf-8 -*-
from __future__ import division
''' Limit state checking according to structural concrete spanish standard EHE-08.'''

# Cuantía geométrica mínima en pilares.
def cuantiaGeomMinPilares(Ac):
  return 0.004*Ac 

# Cuantía mecánica mínima en pilares.
def cuantiaMecMinPilares(Nd, fyd):
  return 0.1*Nd/fyd 

# Cuantía máxima en pilares.
def cuantiaMaxPilares(Ac, fcd, fyd):
  return min((fcd*Ac)/fyd,0.08*Ac)
 
# Verificación de la cuantía de la reinforcement longitudinal en pilares.
def verifCuantiaPilar(Ac, As, fcd, fyd, Nd):
  ctMinGeom= cuantiaGeomMinPilares(Ac)
  if(As<ctMinGeom):
    print "Cuantía de reinforcement dispuesta: ",As*1e4," cm2, inferior a la mínima geométrica: ",ctMinGeom*1e4," cm2\n"
  ctMinMec= cuantiaMecMinPilares(Nd,fyd)
  if(As<ctMinMec):
    print "Cuantía de reinforcement dispuesta: ",As*1e4," cm2, inferior a la mínima mecánica: ",ctMinMec*1e4," cm2\n"
  ctMax= cuantiaMaxPilares(Ac,fcd,fyd)
  if(As>ctMax):
    print "Cuantía de reinforcement dispuesta: ",As*1e4," cm2, superior a la máxima: ",ctMax*1e4," cm2\n"


class ConcreteCorbel(object):
    '''Concrete corbel as in EHE-08 design code.'''
    def __init__(self,concrete, steel, jointType):
        '''
        Constructor.

        :param concrete: concrete material for corbel.
        :param concrete: steel material for corbel reinforcement.
        :param jointType: corbel to column joint quality
        ("monolitica", "junta" o "junta_debil").
        '''
        self.concrete= concrete
        self.steel= steel
        self.jointType= jointType
        
    def getCotgStrutAngle(self):
        '''getCotgStrutAngle()
 
          Return the cotangent of the angle between the concrete
          compressed strut an the vertical according to 
          clause 64.1.2.1 de EHE-08.
        '''
        if self.jointType=="monolitica":
            retval=1.4
        elif self.jointType=="junta":
            retval=1.0
        elif self.jointType=="junta_debil":
            retval=0.6
        return retval

    def getCantoUtilMinimo(self, a):
        '''getCantoUtilMinimo(self, a)

        :param a: Distancia (m) entre el eje de la carga aplicada y la sección
        de empotramiento de la ménsula (ver figura 64.1.2 de EHE-08).

        Devuelve el depth útil mínimo de la ménsula según
        el apartado 64.1.2.1 de EHE-08.
        '''
        return a*self.getCotgStrutAngle()/0.85

    def getTraccionMainReinforcement(self, Fv,Fh):
        '''getTraccionMainReinforcement(self, Fv,Fh)

        :param Fv: Vertical load on the corbel, positive downwards (N).
        :param Fh: Horizontal load on the corbel, positive outwards (N).

        Devuelve la tracción en la armadura principal de la ménsula según
        el apartado 64.1.2.1.1 de EHE-08.
        '''
        return Fv/self.getCotgStrutAngle()+Fh

    def getAreaNecMainReinforcement(self, Fv,Fh):
        '''getAreaNecMainReinforcement(self, Fv,Fh)

        :param Fv: Vertical load on the corbel, positive downwards (N).
        :param Fh: Horizontal load on the corbel, positive outwards (N).

        Devuelve el área necesaria para la reinforcement principal de 
        la ménsula según el apartado 64.1.2.1.1 de EHE-08.
        '''
        return self.getTraccionMainReinforcement(Fv,Fh)/min(self.steel.fyd(),400e6)
    @staticmethod
    def getTraccionCercos(Fv):
        ''' getTraccionCercos(Fv)

        :param Fv: Vertical load on the corbel, positive downwards (N).

        Devuelve la tracción en los cercos según el apartado 64.1.2.1.1 
        de EHE-08.
        '''
        return 0.2*Fv

    def getAreaNecCercos(self,Fv):
        '''getAreaNecCercos(self,Fv)

        :param Fv: Vertical load on the corbel, positive downwards (N).

        Devuelve el área necesaria para los cercos de la ménsula según
        el apartado 64.1.2.1.1 de EHE-08.
        '''
        return self.getTraccionCercos(Fv)/min(self.steel.fyd(),400e6)

    def getAreaNecApoyo(self,Fv):
        '''getAreaNecApoyo(self,Fv)

        :param Fv: Vertical load on the corbel, positive downwards (N).

        Devuelve el área necesaria para el apoyo según
        el apartado 64.1.2.1.2 de EHE-08.
        '''
        return Fv/0.7/-self.concrete.fcd()



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


# Comprobación de cargas concentradas sobre macizos, según el artículo
# 61 de la EHE-08


# Devuelve el valor de f3cd
def getF3cd(Ac, Ac1, fcd):
  return min(sqrt(Ac/Ac1),3.3)*fcd

def getNuCargaConcentrada(Ac, Ac1, fcd):
  '''
  Devuelve el valor último de la carga que puede soportar una pieza
  de seccion Ac siendo el área cargada Ac1 (ver figura 61.1.a page 302 EHE-08)
  '''
  return Ac1*getF3cd(Ac,Ac1,fcd)

def getUad(a, a1, Nd):
    '''
    Devuelve la capacidad mecánica necesaria para la reinforcement paralela al
    lado a (ver figura 61.1.a page 302 EHE-08)
      a: Dimensión de la sección.
      a1: Dimensión del área cargada paralela a a.
      Nd: Carga concentrada.

    '''
    return 0.25*((a-a1)/a)*Nd

def getAreaArmadAd(a, a1, Nd, fyd):
    '''
    Devuelve el área necesaria para la reinforcement paralela al
    lado a (ver figura 61.1.a page 302 EHE-08)
      a: Dimensión de la sección.
      a1: Dimensión del área cargada paralela a a.
      Nd: Carga concentrada.
      fyd: steel yield strength.

    '''
    return getUad(a,a1,Nd)/min(fyd,400e6)


def getUbd(b, b1, Nd):
    '''
    Devuelve la capacidad mecánica necesaria para la reinforcement paralela al
    lado b (ver figura 61.1.a page 302 EHE-08)
      b: Dimensión de la sección.
      b1: Dimensión del área cargada paralela a b.
      Nd: Carga concentrada.

    '''
    return 0.25*((b-b1)/b)*Nd

def getAreaArmadBd(b, b1, Nd, fyd):
    '''
    Devuelve el área necesaria para la reinforcement paralela al
    lado b (ver figura 61.1.a page 302 EHE-08)
      b: Dimensión de la sección.
      b1: Dimensión del área cargada paralela a a.
      Nd: Carga concentrada.
      fyd: steel yield strength.

    '''
    return getUad(b,b1,Nd)/min(fyd,400e6)
