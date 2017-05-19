# -*- coding: utf-8 -*-
from __future__ import division
import math
import sys
from materials.ehe import EHE_concrete

'''Comprobación de una sección de hormigón frente a cortante según la EHE-08.#
'''

def getF1cdEHE08(fck,fcd):
    '''getF1cdEHE08(fck,fcd)

    :param fck: Valor característico de la resistencia a compresión simple del hormigón (N/m2).
    :param fcd: Valor de cálculo de la resistencia a compresión simple del hormigón (N/m2).

    Returns the value of f1cd (design value of the concrete strut strength)
    according to the article 44.2.3.1 of EHE-08.
    '''
    retval=0.6
    if fck>60e6:
      retval=max(0.9-fck/200.e6,0.5)
    retval= retval*fcd
    return retval



def getKEHE08(sgpcd,fcd):
    '''getKEHE08(sgpcd,fcd)
    :param sgpcd: Tensión axil efectiva en el hormigón Ncd/Ac.
    :param fcd: Valor de cálculo de la resistencia a compresión simple del hormigón (N/m2).
    Devuelve el valor de K (coeficiente que depende del esfuerzo axil)
    according to the article 44.2.3.1 de la EHE-08
    '''
    s=-sgpcd/fcd #Positive when compressed
    if s>1:
        print "getKEHE08; tensión excesiva en el hormigón: (",sgpcd,"<",-fcd,")\n"
    if s<=0:
        retval=1.0
    elif s<=0.25:
        retval=1+s
    elif s<=0.5:
        retval=1.25
    else:
        retval=2.5*(1-s)
    return retval



def getVu1EHE08(fck,fcd,Ncd,Ac,b0,d,alpha,theta):
    '''getVu1EHE08(fck,fcd,Ncd,Ac,b0,d,alpha,theta) [uds: N, m, rad]

    :param fck: Concrete characteristic compressive strength.
    :param fcd: Resistencia de cálculo del hormigón a compresión.
    :param Ncd: Axil de cálculo resistido por el hormigón (positivo si es de tracción).
    :param Ac: Area total de la sección de hormigón.
    :param b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
    :param d: Canto útil.
    :param alpha: Ángulo de las reinforcement de cortante con el eje de la pieza (figura 44.2.3.1 EHE-08).
    :param theta: Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).

    Devuelve el valor de Vu1 (cortante de agotamiento por compresión oblicua del alma)
    according to the article 44.2.3.1 de EHE-08.
    '''
    f1cd=getF1cdEHE08(fck,fcd)
    sgpcd=min(Ncd/Ac,0)
    K=getKEHE08(sgpcd,fcd)
    ctgTheta=min(max(1/math.tan(theta),0.5),2.0)
    return K*f1cd*b0*d*(ctgTheta+1/math.tan(alpha))/(1+ctgTheta**2)

def getVu2EHE08NoAtNoFis(fctd,I,S,b0,alphal,Ncd,Ac):
    '''getVu2EHE08NoAtNoFis(fctd,I,S,b0,alphal,Ncd,Ac) [uds: N, m, rad]

    :param fctd: Resistencia de cálculo del hormigón a tracción.
    :param I: Moment of inertia of the section with respect of its centroid.
    :param S: Momento estático de la parte de la sección que queda por encima del CDG.
    b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
    :param alphal: Coeficiente que, en su caso, introduce el efecto de la transferencia.
    :param Ncd: Axil de cálculo resistido por el hormigón (positivo si es de tracción).
    :param Ac: Area total de la sección de hormigón.
    Devuelve el valor de Vu2 (cortante de agotamiento por tracción en el alma)
    according to the article 44.2.3.2.1.1 de EHE-08.
    '''
    tmp= fctd
    if alphal!=1.0:
      sgpcd=min(Ncd/Ac,0)
      tmp=math.sqrt(fctd**2-alphal*sgpcd*fctd)
    return I*b0/S*tmp



def getFcvEHE08(fact,fcv,gammaC,b0,d,chi,sgpcd,AsPas,AsAct):
    '''getFcvEHE08(fact,fcv,gammaC,b0,d,chi,sgpcd,AsPas,AsAct)  [uds: N, m, rad]

    :param fact: Factor que toma el valor 0.18 para piezas SIN reinforcement de cortante y 0.15 para piezas CON reinforcement de cortante.

    :param fcv: Resistencia efectiva del hormigón a cortante. En piezas sin reinforcement de cortante
    será fcv= min(fck,60MPa). En piezas con reinforcement de cortante fcv= min(fck,100MPa).
    En ambos casos, si el control del hormigón es indirecto fcv=15MPa.
    :param gammaC: Coeficiente de minoración de la resistencia del hormigón.
    b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
    :param d: Canto útil (meters).
    :param chi: Coeficiente que introduce el efecto de los áridos en el depth útil.
    :param sgpcd: Tensión axial media en el alma (compresión positiva).
    :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.

    Devuelve el valor de fcv (resistencia virtual a cortante del hormigón)
    para piezas CON Ó SIN reinforcement de cortante en regiones fisuradas, según los
    artículos 44.2.3.2.1.2 y  44.2.3.2.2 de EHE-08.
    '''
    rol=min((AsPas+AsAct)/(b0*d),0.02)
    return fact/gammaC*chi*(rol*fcv/1e4)**(1/3)*1e6-0.15*sgpcd


def getFcvMinEHE08(fcv,gammaC,d,chi,sgpcd):
    '''getFcvMinEHE08(fcv,gammaC,d,chi,sgpcd)

    :param fcv: Resistencia efectiva del hormigón a cortante. En piezas sin reinforcement de cortante
    será fcv= min(fck,60MPa). En piezas con reinforcement de cortante fcv= min(fck,100MPa).
    En ambos casos, si el control del hormigón es indirecto fcv=15MPa.
    :param gammaC: Coeficiente de minoración de la resistencia del hormigón.
    :param d: Canto útil (meters).
    :param chi: Coeficiente que introduce el efecto de los áridos en el depth útil.
    :param sgpcd: Tensión axial media en el alma (compresión positiva).

    Devuelve el valor mínimo de fcv (resistencia virtual a cortante del hormigón)
    para piezas SIN reinforcement de cortante en regiones fisuradas, según el
    artículo 44.2.3.2.1.2 de EHE-08.
    '''
    return 0.075/gammaC*math.pow(chi,1.5)*math.sqrt(fcv)*1e3-0.15*sgpcd


def getVu2EHE08NoAtSiFis(fcv,fcd,gammaC,Ncd,Ac,b0,d,AsPas,AsAct):
    '''getVu2EHE08NoAtSiFis(fcv,fcd,gammaC,Ncd,Ac,b0,d,AsPas,AsAct) [uds: N, m]

    :param fcv: Resistencia efectiva del hormigón a cortante. En piezas sin reinforcement de cortante
    será fcv= min(fck,60MPa). En piezas con reinforcement de cortante fcv= min(fck,100MPa).
    En ambos casos, si el control del hormigón es indirecto fcv=15MPa.
    :param fcd: Resistencia de cálculo del hormigón a compresión.
    :param gammaC: Coeficiente de minoración de la resistencia del hormigón.
    :param Ncd: Axil de cálculo resistido por el hormigón (positivo si es de tracción).
    :param Ac: Area total de la sección de hormigón.
    b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
    :param d: Canto útil (meters).
    :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.

    Devuelve el valor de Vu2 (cortante de agotamiento por tracción en el alma)
    para piezas SIN reinforcement de cortante en regiones fisuradas, according to the article 44.2.3.2.1.2 de EHE-08.
    '''
    chi=min(2,1+math.sqrt(200/(d*1000.))) #HA DE EXPRESARSE EN METROS.
    sgpcd=max(max(Ncd/Ac,-0.3*fcd),-12e6)
    fcvTmp=getFcvEHE08(0.18,fcv,gammaC,b0,d,chi,sgpcd,AsPas,AsAct)
    fcvMinTmp=getFcvMinEHE08(fcv,gammaC,d,chi,sgpcd)
    return max(fcvTmp,fcvMinTmp)*b0*d

def getVu2EHE08NoAt(M,Mfis,fcv,fck,gammaC,I,S,alphaL,Ncd,Ac,b0,d,AsPas,AsAct):
    '''getVu2EHE08NoAt(M,Mfis,fcv,fck,gammaC,I,S,alphaL,Ncd,Ac,b0,d,AsPas,AsAct)   [uds: N, m, rad]

    :param M: Momento que actúa sobre la sección.
    :param Mfis: Momento de fisuración de la sección correspondiente al mismo plano y sentido
    flexión que M.
    :param fcv: Resistencia efectiva del hormigón a cortante. En piezas sin reinforcement de cortante
    será fcv= min(fck,60MPa). En piezas con reinforcement de cortante fcv= min(fck,100MPa).
    En ambos casos, si el control del hormigón es indirecto fcv=15MPa.
    :param fck: Valor característico de la resistencia del hormigón a compresión.
    :param gammaC: Coeficiente de minoración de la resistencia del hormigón.
    :param Ncd: Axil de cálculo resistido por el hormigón (positivo si es de tracción).
    :param Ac: Area total de la sección de hormigón.
    b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
    :param d: Canto útil (meters).
    :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
 
    Devuelve el valor de Vu2 (cortante de agotamiento por tracción en el alma)
    para piezas SIN reinforcement de cortante, según los artículos 44.2.3.2.1.1 y 44.2.3.2.1.2 de EHE-08.
    '''
    concrTmp=EHE_concrete.EHEConcrete("HA",fck,gammaC)
    fctdTmp=concrTmp.fctkEHE08()/gammaC
    fcdTmp=fck/gammaC
    if M<=Mfis:
        retval=getVu2EHE08NoAtNoFis(fctdTmp,I,S,b0,alphaL,Ncd,Ac)
    else:
        retval=getVu2EHE08NoAtSiFis(fck,fcdTmp,1.5,Ncd,Ac,b0,d,AsPas,AsAct)
    return retval


def getVsuEHE08(z,alpha,theta,AsTrsv,fyd):
    '''getVsuEHE08(z,alpha,theta,AsTrsv,fyd)  [uds: N, m, rad]

    :param z: Lever arm.
    :param alpha: Ángulo de la reinforcement transversal con el eje de la pieza.
    :param theta: Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
    :param AsTrsv: Área de la reinforcement transversal cuya contribución se calcula.
    :param fyd: Valor de cálculo de la resistencia de la reinforcement a cortante.

    Devuelve el valor de Vsu (contribución de la reinforcement transversal de alma
    a la resistencia al esfuerzo cortante) para piezas CON reinforcement de cortante,
    according to the article 44.2.3.2.2 de EHE-08.
    '''
    fyalphad=min(fyd,400e6)
    return z*math.sin(alpha)*(1/math.tan(alpha)+1/math.tan(theta))*AsTrsv*fyalphad



def getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue):
    '''getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue) [uds: N, m, rad]

    :param Nd: Valor de cálculo del axil (aquí positivo si es de tracción)
    :param Md: Valor absoluto del momento de cálculo.
    :param Vd: Valor absoluto del cortante efectivo de cálculo (artículo 42.2.2).
    :param Td: Torsor de cálculo.
    :param z: Lever arm.
    :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param Es: Módulo elástico del acero de la reinforcement pasiva.
    :param Ep: Módulo elástico del acero de la reinforcement activa.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Área encerrada por la línea media de la sección hueca eficaz.
    :param ue: Perímetro de la línea media de la sección hueca eficaz.

    Devuelve la deformación de longitudinal en el alma según la
    expresión que figura en los comentarios al artículo 44.2.3.2.2 de EHE-08.
    '''
    denomEpsilonX=2*(Es*AsPas+Ep*AsAct) 
    Md= max(Md,Vd*z)
    assert z>0
    assert Ae>0
    if denomEpsilonX>0:
      retvalEpsilonX=(Md/z+math.sqrt(Vd**2+(ue*Td/2.0/Ae)**2)+Nd/2.+Fp)/denomEpsilonX  
    else:
      retvalEpsilonX=0
    retvalEpsilonX= max(0,retvalEpsilonX)
    return retvalEpsilonX
  


def getAnguloInclinacionFisurasEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue):
    '''getAnguloInclinacionFisurasEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue) [uds: N, m, rad]

    :param Nd: Valor de cálculo del axil (aquí positivo si es de tracción)
    :param Md: Valor absoluto del momento de cálculo.
    :param Vd: Valor absoluto del cortante efectivo de cálculo (artículo 42.2.2).
    :param Td: Torsor de cálculo.
    :param z: Lever arm.
    :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param Es: Módulo elástico del acero de la reinforcement pasiva.
    :param Ep: Módulo elástico del acero de la reinforcement activa.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Área encerrada por la línea media de la sección hueca eficaz.
    :param ue: Perímetro de la línea media de la sección hueca eficaz.

    Returns the reference angle of inclination of cracks from
    la deformación longitudinal en el alma de la sección
    expresado en radianes. Ver el artículo 44.2.3.2.2 de EHE-08.
    '''
    return math.radians(getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue)*7000+29) 



  
def getBetaVcuEHE08(theta,thetaE):
    '''getBetaVcuEHE08(theta,thetaE) [uds: N, m, rad]

    :param thetaE: Reference angle of inclination of cracks.
    :param theta: Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
    
    Return the value of «beta» for the expression
    of Vcu according to the article 44.2.3.2.2 of EHE-08.
    '''
    cotgTheta=1/math.tan(theta)
    cotgThetaE=1/math.tan(thetaE)
    retval=0.0
    if cotgTheta<0.5:
        print"getAnguloInclinacionFisurasEHE08; angle theta is too small."
    else:
        if(cotgTheta<cotgThetaE):
            retval= (2*cotgTheta-1)/(2*cotgThetaE-1)
        else:
            if cotgTheta<=2.0:
                retval= (cotgTheta-2)/(cotgThetaE-2)
            else:
                print"getAnguloInclinacionFisurasEHE08; el ángulo theta es demasiado grande."
    return retval
  

 
def getVcuEHE08(fcv,fcd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue):
    '''getVcuEHE08(fcv,fcd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue) [uds: N, m, rad]

    :param fcv: Resistencia efectiva del hormigón a cortante. En piezas sin reinforcement de cortante
    será fcv= min(fck,60MPa). En piezas con reinforcement de cortante fcv= min(fck,100MPa).
    En ambos casos, si el control del hormigón es indirecto fcv=15MPa.
    :param fcd: Valor de cálculo de la resistencia del hormigón a compresión.
    :param gammaC: Coeficiente de minoración de la resistencia del hormigón.
    :param Ncd: Axil de cálculo resistido por el hormigón (positivo si es de tracción).
    :param Ac: Area total de la sección de hormigón.
    b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
    :param d: Canto útil (meters).
    :param z: Lever arm.
    :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param theta: Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE)
    :param Nd: Valor de cálculo del axil (positivo si es de tracción)
    :param Md: Valor absoluto del momento de cálculo.
    :param Vd: Valor absoluto del cortante efectivo de cálculo (artículo 42.2.2).
    :param Td: Torsor de cálculo.
    :param Es: Módulo elástico del acero de la reinforcement pasiva.
    :param Ep: Módulo elástico del acero de la reinforcement activa.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Área encerrada por la línea media de la sección hueca eficaz.
    :param ue: Perímetro de la línea media de la sección hueca eficaz.

    Devuelve el valor de Vcu (contribución del hormigón a la resistencia al esfuerzo cortante)
    para piezas CON reinforcement de cortante, according to the article 44.2.3.2.2 de EHE-08.
    '''
  
    chi=min(2,1+math.sqrt(200/(d*1000.)))   #HA DE EXPRESARSE EN METROS.
    sgpcd=max(max(Ncd/Ac,-0.3*fcd),-12e6)
    FcvVcu=getFcvEHE08(0.15,fcv,gammaC,b0,d,chi,sgpcd,AsPas,AsAct)
    thetaEVcu=getAnguloInclinacionFisurasEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue)
    betaVcu=getBetaVcuEHE08(theta,thetaEVcu)
    return FcvVcu*betaVcu*b0*d
  


  
def getVu2EHE08SiAt(fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv, alpha, theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue):
    '''getVu2EHE08SiAt(fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv, alpha, theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue) [uds: N, m, rad]
    :param fcv: Resistencia efectiva del hormigón a cortante. En piezas sin reinforcement de cortante
    será fcv= min(fck,60MPa). En piezas con reinforcement de cortante fcv= min(fck,100MPa).
    En ambos casos, si el control del hormigón es indirecto fcv=15MPa.
    :param fcd: Valor de cálculo de la resistencia del hormigón a compresión.
    :param fyd: Valor de cálculo de la resistencia de la reinforcement a cortante.
    :param gammaC: Coeficiente de minoración de la resistencia del hormigón.
    :param Ncd: Axil de cálculo resistido por el hormigón (positivo si es de tracción).
    :param Ac: Area total de la sección de hormigón.
    b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
    :param d: Canto útil (meters).
    :param z: Lever arm.
    :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param AsTrsv: Área de la reinforcement transversal.
    :param alpha: Ángulo de la reinforcement transversal con el eje de la pieza.
    :param theta: Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
    :param Nd: Valor de cálculo del axil (positivo si es de tracción)
    :param Md: Valor absoluto del momento de cálculo.
    :param Vd: Valor absoluto del cortante efectivo de cálculo (artículo 42.2.2).
    :param Td: Torsor de cálculo.
    :param Es: Módulo elástico del acero de la reinforcement pasiva.
    :param Ep: Módulo elástico del acero de la reinforcement activa.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Área encerrada por la línea media de la sección hueca eficaz.
    :param ue: Perímetro de la línea media de la sección hueca eficaz.

    Devuelve el valor de Vu2 (esfuerzo cortante de agotamiento por tracción en el alma)
    para piezas CON reinforcement de cortante, according to the article 44.2.3.2.2 de EHE-08.
    '''
    return getVcuEHE08(fcv,fcd,gammaC,Ncd,Ac,b0,d,AsPas,AsAct,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue)+getVsuEHE08(z,alpha,theta,AsTrsv,fyd)
  


  
def getVuEHE08SiAt(fck,fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv, alpha, theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue):
    '''def getVuEHE08SiAt(fck,fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv, alpha, theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue) [uds: N, m, rad]
    :param fck: Concrete characteristic compressive strength.
    :param fcv: Resistencia efectiva del hormigón a cortante. En piezas sin reinforcement de cortante
    será fcv= min(fck,60MPa). En piezas con reinforcement de cortante fcv= min(fck,100MPa).
    En ambos casos, si el control del hormigón es indirecto fcv=15MPa.
    :param fcd: Valor de cálculo de la resistencia del hormigón a compresión.
    :param fyd: Valor de cálculo de la resistencia de la reinforcement a cortante.
    :param gammaC: Coeficiente de minoración de la resistencia del hormigón.
    :param Ncd: Axil de cálculo resistido por el hormigón (positivo si es de tracción).
    :param Ac: Area total de la sección de hormigón.
    b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
    :param d: Canto útil (meters).
    :param z: Lever arm.
    :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
    a una distancia mayor que el depth útil de la sección en estudio.
    :param AsTrsv: Área de la reinforcement transversal.
    :param alpha: Ángulo de la reinforcement transversal con el eje de la pieza.
    :param theta: Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
    :param Nd: Valor de cálculo del axil (positivo si es de tracción)
    :param Md: Valor absoluto del momento de cálculo.
    :param Vd: Valor absoluto del cortante efectivo de cálculo (artículo 42.2.2).
    :param Td: Torsor de cálculo.
    :param Es: Módulo elástico del acero de la reinforcement pasiva.
    :param Ep: Módulo elástico del acero de la reinforcement activa.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Área encerrada por la línea media de la sección hueca eficaz.
    :param ue: Perímetro de la línea media de la sección hueca eficaz.
    
    Devuelve el valor de Vu (cortante de agotamiento de la sección)
    para piezas CON reinforcement de cortante, according to the article 44.2.3.2.2 de EHE-08.
    '''
    return  min(getVu2EHE08(fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv,alpha,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue),getVu1EHE08(fck,fcd,Ncd,Ac,b0,d,alpha,theta))
  

