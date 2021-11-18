
import math

def Fi1X(x:float,L:float):
    '''Devuelve la primera forma modal de la viga biapoyada de
    acuerdo con la figura B.5 de la IAPF, siendo:

    :param x: Abcisa para la que se obtiene el valor.
    :param L: Luz entre apoyos de la viga.
      '''
    return math.sin(math.pi*x/L)


def amplitudCargaAisladaEnPuente(P:float,m:float,L:float,w0:float,psi:float,V:float,t:float):
    '''Devuelve el valor de la amplitud del movimiento de vibración para el 
    primer modo de vibración de acuerdo con la expresión 3.10 de la tesis 
    titulada «Interacción vehículo-estructura y efectos de resonancia en puentes
     isostáticos de ferrocarril para líneas de alta velocidad» de 
    Pedro Museros Romero, siendo:

    :param P: Carga que produce la oscilación.
    :param m: Masa por unidad de longitud.
    :param L: Luz entre apoyos.
    :param w0: Pulsación correspondiente al modo fundamental.
    :param psi: Amortiguamiento.
    :param V: Velocidad con que se desplaza la carga.
    :param t: Instante de tiempo en el que se calcula la amplitud.
    '''
    assert(t<=L/V)
    n0= w0/2/math.pi
    K= V/2/n0/L
    WOt= w0*t
    return 2*P/m/L/(w0**2)/(1-(K**2))*(math.sin(K*WOt)-K*math.exp(-psi*WOt)*math.sin(WOt))

def amplitudDotCargaAisladaEnPuente(P:float,m:float,L:float,w0:float,psi:float,V:float,t:float):
    '''Devuelve el valor de la derivada primera (velocidad) de la amplitud 
       del movimiento de vibración para el primer modo de vibración de acuerdo 
       con la expresión 3.10 de la tesis titulada «Interacción 
       vehículo-estructura y efectos de resonancia en puentes isostáticos de 
       ferrocarril para líneas de alta velocidad» de Pedro Museros Romero, 
       siendo:

    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    t: Instante de tiempo en el que se calcula la amplitud.
      '''
    assert(t<=L/V)
    n0= w0/2/math.pi
    K= V/2/n0/L
    return 2*(w0*K*math.cos(t*w0*K)+psi*w0*math.exp(-(psi*t*w0))*math.sin(t*w0)*K-w0*math.exp(-(psi*t*w0))*math.cos(t*w0)*K)*P/(m*(w0**2)*(1-(K**2))*L)

def amplitudDotDotCargaAisladaEnPuente(P:float,m:float,L:float,w0:float,psi:float,V:float,t:float):
    '''Devuelve el valor de la derivada segunda (aceleración) de la amplitud 
       del movimiento de vibración para el primer modo de vibración de
       acuerdo con la expresión 3.10 de la tesis titulada «Interacción 
       vehículo-estructura y efectos de resonancia en puentes isostáticos de
       ferrocarril para líneas de alta velocidad» de Pedro Museros Romero, 
       siendo:

    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    t: Instante de tiempo en el que se calcula la amplitud.
      '''
    assert(t<=L/V)
    n0= w0/2/math.pi
    K= V/2/n0/L
    return 2*(-w0**2*K**2*math.sin(t*w0*K)-psi**2*w0**2*math.exp(-(psi*t*w0))*math.sin(t*w0)*K+w0**2*math.exp(-(psi*t*w0))*math.sin(t*w0)*K+2*psi*w0**2*math.exp(-(psi*t*w0))*math.cos(t*w0)*K)*P/(m*(w0**2)*(1-(K**2))*L)


def amplitudCargaAisladaTrasPuente(P:float,m:float,L:float,w0:float,psi:float,V:float,t:float):
    '''Devuelve el valor de la amplitud del movimiento de vibración para el primer modo de vibración de
    acuerdo con la expresión 3.11 de la tesis titulada «Interacción vehículo-estructura y efectos
    de resonancia en puentes isostáticos de ferrocarril para líneas de alta velocidad» de Pedro
    Museros Romero, siendo:
    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    t: Instante de tiempo en el que se calcula la amplitud.
      '''
    n0= w0/2/math.pi
    K= V/2/n0/L
    WOt= w0*t
    t2= t-L/V
    assert(t2>=0.0)
    WOt2= w0*t2
    return 2*P/m/L/(w0**2)*K/(1-(K**2))*(math.exp(-psi*WOt)*math.sin(WOt)-math.exp(-psi*WOt2)*math.sin(WOt2))

def amplitudDotCargaAisladaTrasPuente(P:float,m:float,L:float,w0:float,psi:float,V:float,t:float):
    '''Devuelve el valor de la derivada primera de la amplitud (velocidad) del movimiento de vibración para el primer modo de vibración de
    acuerdo con la expresión 3.11 de la tesis titulada «Interacción vehículo-estructura y efectos
    de resonancia en puentes isostáticos de ferrocarril para líneas de alta velocidad» de Pedro
    Museros Romero, siendo:
    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    t: Instante de tiempo en el que se calcula la amplitud.
      '''
    n0= w0/2/math.pi
    K= V/2/n0/L
    assert(t>=L/V)
    return 2*K*P*(psi*w0*math.sin(w0*(t-L/V))*math.exp(-(psi*w0*(t-L/V)))-w0*math.cos(w0*(t-L/V))*math.exp(-(psi*w0*(t-L/V)))-psi*w0*math.exp(-(psi*t*w0))*sin(t*w0)+w0*math.exp(-(psi*t*w0))*math.cos(t*w0))/(m*(w0**2)*(1-(K**2))*L)

def amplitudDotDotCargaAisladaTrasPuente(P:float,m:float,L:float,w0:float,psi:float,V:float,t:float):
    '''Devuelve el valor de la derivada segunda de la amplitud (aceleración) del movimiento de vibración para el primer modo de vibración de
    acuerdo con la expresión 3.11 de la tesis titulada «Interacción vehículo-estructura y efectos
    de resonancia en puentes isostáticos de ferrocarril para líneas de alta velocidad» de Pedro
    Museros Romero, siendo:
    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    t: Instante de tiempo en el que se calcula la amplitud.
      '''
    n0= w0/2/math.pi
    K= V/2/n0/L
    assert(t>=L/V)
    return 2*K*P*(-psi**2*w0**2*math.sin(w0*(t-L/V))*math.exp(-(psi*w0*(t-L/V)))+w0**2*math.sin(w0*(t-L/V))*math.exp(-(psi*w0*(t-L/V)))+2*psi*w0**2*math.cos(w0*(t-L/V))*math.exp(-(psi*w0*(t-L/V)))+psi**2*w0**2*math.exp(-(psi*t*w0)*math.sin(t*w0))-w0**2*math.exp(-(psi*t*w0))*math.sin(t*w0)-2*psi*w0**2*math.exp(-(psi*t*w0))*math.cos(t*w0))/(m*(w0**2)*(1-(K**2))*L)


def amplitudCargaAislada(P:float,m:float,L:float,w0:float,psi:float,V:float,t:float):
    '''Devuelve el valor de la amplitud del movimiento de vibración para el primer modo de vibración de
    acuerdo con las expresiones 3.10 y 3.11 de la tesis titulada «Interacción vehículo-estructura y efectos
    de resonancia en puentes isostáticos de ferrocarril para líneas de alta velocidad» de Pedro
    Museros Romero, siendo:
    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    t: Instante de tiempo en el que se calcula la amplitud.
      '''
    retval= None
    if(t<=0):
        retval= 0
    elif(t<=L/V):
        retval= amplitudCargaAisladaEnPuente(P,m,L,w0,psi,V,t)
    else:
        retval= amplitudCargaAisladaTrasPuente(P,m,L,w0,psi,V,t)
    return retval

def amplitudDotCargaAislada(P:float,m:float,L:float,w0:float,psi:float,V:float,t:float):
    '''Devuelve el valor de la derivada primera de la amplitud (velocidad) del movimiento de vibración
    para el primer modo de vibración de acuerdo con las expresiones 3.10 y 3.11 de la tesis
    titulada «Interacción vehículo-estructura y efectos de resonancia en puentes isostáticos de
    ferrocarril para líneas de alta velocidad» de Pedro Museros Romero, siendo:
    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    t: Instante de tiempo en el que se calcula la amplitud.
      '''
    retval= None
    
    if(t<=0):
        retval= 0.0
    elif(t<=L/V):
        retval= amplitudDotCargaAisladaEnPuente(P,m,L,w0,psi,V,t)
    else:
        retval= amplitudDotCargaAisladaTrasPuente(P,m,L,w0,psi,V,t)
    return retval

def amplitudDotDotCargaAislada(P:float,m:float,L:float,w0:float,psi:float,V:float,t:float):
    '''Devuelve el valor de la derivada segunda de la amplitud (aceleración) del movimiento de vibración
    para el primer modo de vibración de acuerdo con las expresiones 3.10 y 3.11 de la tesis
    titulada «Interacción vehículo-estructura y efectos de resonancia en puentes isostáticos de
    ferrocarril para líneas de alta velocidad» de Pedro Museros Romero, siendo:
    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    t: Instante de tiempo en el que se calcula la amplitud.
      '''
    retval= None
    if(t<=0):
        retval= 0.0
    elif(t<=L/V):
        retval= amplitudDotDotCargaAisladaEnPuente(P,m,L,w0,psi,V,t)
    else:
        retval= amplitudDotDotCargaAisladaTrasPuente(P,m,L,w0,psi,V,t)
    return retval


def flechaDinamicaCargaAislada(P:float,m:float,L:float,w0:float,psi:float,V:float,t:float,x:float):
    '''Devuelve el valor de la flecha dinámica para el punto de abcisa x, 
       siendo:

    P: Carga aislada que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    t: Instante de tiempo en el que se calcula la amplitud.
    x: Abcisa en la que se calcula la flecha.
      '''
    return Fi1X(x,L)*amplitudCargaAislada(P,m,L,w0,psi,V,t)

def aceleracionCargaAislada(P:float,m:float,L:float,w0:float,psi:float,V:float,t:float,x:float):
    '''Devuelve el valor de la aceleración para el punto de abcisa x, siendo:
    P: Carga aislada que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    t: Instante de tiempo en el que se calcula la amplitud.
    x: Abcisa en la que se calcula la flecha.
      '''
    return Fi1X(x,L)*amplitudDotDotCargaAislada(P,m,L,w0,psi,V,t)

def flechaDinamicaMinimaCargaAislada(P:float,m:float,L:float,w0:float,psi:float,V:float,x:float,tIni:float,tFin:float):
    '''Devuelve el valor mínimo de la flecha dinámica para el punto de abcisa x, siendo:
    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    tIni: Instante inicial.
    tFin: Instante final.
    x: Abcisa en la que se calcula la flecha.
      '''
    incT= 2*math.pi/w0/10 #10 puntos por ciclo (5 puntos en cada semionda)
    fDinMin= 1e12
    instT=tIni
    while(instT<tFin):
        fTmp= flechaDinamicaCargaAislada(P,m,L,w0,psi,V,instT,x)
        if(fTmp<fDinMin):
          fDinMin= fTmp
        instT+= incT
    return fDinMin

def aceleracionExtremaCargaAislada(P:float,m:float,L:float,w0:float,psi:float,V:float,x:float,tIni:float,tFin:float):
    '''Devuelve el valor extremo de la aceleración para el punto de abcisa x, siendo:
    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    tIni: Instante inicial.
    tFin: Instante final.
    x: Abcisa en la que se calcula la flecha.
      '''
    incT= 2*math.pi/w0/10 # 10 puntos por ciclo (5 puntos en cada semionda)
    aExtrema= 0
    instT=tIni
    while(instT<tFin):
        aTmp= aceleracionCargaAislada(P,m,L,w0,psi,V,instT,x)
        if(abs(aTmp)>abs(aExtrema)):
            aExtrema= aTmp
        instT+= incT
    return aExtrema

def flechaDinamicaMinimaCargaAisladaRangoVel(P:float,m:float,L:float,w0:float,psi:float,x:float,tIni:float,tFin:float,vIni:float,vFin:float):
    '''Devuelve el valor mínimo de la flecha dinámica para el punto de 
       abcisa x, siendo:
    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    vIni: Instante inicial.
    vFin: Instante final.
    x: Abcisa en la que se calcula la flecha.
      '''
    incV= 10/3.6
    fDinMinR= 1e12
    v=vIni
    while(v<vFin):
        fTmpR= flechaDinamicaMinimaCargaAislada(P,m,L,w0,psi,v,x,tIni,tFin)
        if(fTmpR<fDinMinR):
            fDinMinR= fTmpR
        print("v= ",v*3.6," km/h fDin= ",fTmpR," m fDinMin= ",fDinMinR," m\n")
        v+= incV
    return fDinMinR

def aceleracionExtremaCargaAisladaRangoVel(P:float,m:float,L:float,w0:float,psi:float,x:float,tIni:float,tFin:float,vIni:float,vFin:float):
    '''Devuelve el valor extremo de la aceleración para el punto de abcisa x, 
       siendo:
    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    vIni: Instante inicial.
    vFin: Instante final.
    x: Abcisa en la que se calcula la aceleración.
      '''
    incV= 10/3.6
    aExtremaR= 0
    v=vIni  
    while(v<vFin):
        aTmpR= aceleracionExtremaCargaAislada(P,m,L,w0,psi,v,x,tIni,tFin)
        if(abs(aTmpR)>abs(aExtremaR)):
            aExtremaR= aTmpR
        print("v= ",v*3.6," km/h a= ",aTmpR," m aExtrema= ",aExtremaR," m\n")
        v+= incV
    return aExtremaR

def flechaDinamicaTren(ejesTren,m:float,L:float,w0:float,psi:float,V:float,t:float,x:float):
    '''Devuelve el valor de la flecha dinámica para el punto de abcisa x, siendo:
    ejesTren: Lista con las cargas por eje del tren que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    t: Instante de tiempo en el que se calcula la amplitud (el instante t=0 corresponde a la entrada del tren en la viga).
    x: Abcisa en la que se calcula la flecha.
      '''
    retval= 0.0
    xPEje= [0,0]
    tEje= 0.0
    fEje= 0.0
    for xPEje in ejesTren:
        tEje= t-xPEje[0]/V # Tiempo "local" para el eje.
        fEje= flechaDinamicaCargaAislada(-xPEje[1],m,L,w0,psi,V,tEje,x) # Flecha para el eje aislado.
        retval+= fEje
    return retval

def aceleracionInducidaTren(ejesTren,m:float,L:float,w0:float,psi:float,V:float,t:float,x:float):
    '''Devuelve el valor de la aceleración inducida por el paso de un tren:
    ejesTren: Lista con las cargas por eje del tren que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    t: Instante de tiempo en el que se calcula la amplitud (el instante t=0 corresponde a la entrada del tren en la viga).
    x: Abcisa en la que se calcula la flecha.
      '''
    retval= 0.0
    for xPEje in ejesTren:
        tEje= t-xPEje[0]/V # Tiempo "local" para el eje.
        fEje= aceleracionCargaAislada(-xPEje[1],m,L,w0,psi,V,tEje,x) # Flecha para el eje aislado.
        retval= retval+fEje
    return retval

def flechaDinamicaMinimaTren(ejesTren,m:float,L:float,w0:float,psi:float,V:float,x:float):
    '''Devuelve el valor mínimo de la flecha dinámica para el punto de 
       abcisa x, siendo:

    ejesTren: Lista con las cargas por eje del tren que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    tIni: Instante inicial.
    tFin: Instante final.
    x: Abcisa en la que se calcula la flecha.
      '''
    tIni= 0
    ultEjeTren= ejesTren[-1]
    longTren= ultEjeTren[0]
    tFin= 1.5*(longTren+L)/V
    incT= 2*math.pi/w0/10 # 10 puntos por ciclo (5 puntos en cada semionda)
    fDinMin= 1e12
    instT= tIni
    while(instT<tFin):
        fTmp= flechaDinamicaTren(ejesTren,m,L,w0,psi,V,instT,x)
        if(fTmp<fDinMin):
            fDinMin= fTmp
        instT+= incT
    return fDinMin

def aceleracionExtremaInducidaTren(ejesTren,m:float,L:float,w0:float,psi:float,V:float,x:float):
    '''Devuelve el valor extremo de la aceleración inducida por el tren en 
       el punto de abcisa x, siendo:

    ejesTren: Lista con las cargas por eje del tren que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    tIni: Instante inicial.
    tFin: Instante final.
    x: Abcisa en la que se calcula la flecha.
      '''
    tIni= 0
    ultEjeTren= ejesTren[-1]
    longTren= ultEjeTren[0]
    tFin= 1.5*(longTren+L)/V
    incT= 2*math.pi/w0/10 # 10 puntos por ciclo (5 puntos en cada semionda)
    aExtrema= 0
    instT=tIni
    while(instT<tFin):
        aTmp= aceleracionInducidaTren(ejesTren,m,L,w0,psi,V,instT,x)
        if(abs(aTmp)>abs(aExtrema)):
            aExtrema= aTmp
        instT+= incT
    return aExtrema

def flechaDinamicaMinimaTrenRangoVel(ejesTren,m:float,L:float,w0:float,psi:float, x:float, vIni:float, vFin:float, outputFile):
    '''Devuelve el valor mínimo de la flecha dinámica para el punto de 
       abcisa x, siendo:

    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    vIni: Instante inicial.
    vFin: Instante final.
    x: Abcisa en la que se calcula la flecha.
      '''
    incV= 10/3.6
    fDinMinR= 1e12
    v=vIni     
    while(v<vFin):
        fTmpR= flechaDinamicaMinimaTren(ejesTren,m,L,w0,psi,v,x)
        if(fTmpR<fDinMinR):
            fDinMinR= fTmpR
        outputFile.write(str(v*3.6)+" "+str(fTmpR)+"\n")
        v+= incV
    return fDinMinR

def aceleracionExtremaTrenRangoVel(ejesTren,m:float,L:float,w0:float,psi:float,x:float,vIni:float, vFin:float, outputFile):
    '''Devuelve el valor extremo de la aceleración inducida por el tren para el punto de abcisa x, siendo:
    P: Carga que produce la oscilación.
    m: Masa por unidad de longitud.
    L: Luz entre apoyos.
    w0: Pulsación correspondiente al modo fundamental.
    psi: Amortiguamiento.
    V: Velocidad con que se desplaza la carga.
    vIni: Instante inicial.
    vFin: Instante final.
    x: Abcisa en la que se calcula la flecha.
      '''
    incV= 10/3.6
    aExtremaR= 0
    v=vIni
    while(v<vFin):
        aTmpR= aceleracionExtremaInducidaTren(ejesTren,m,L,w0,psi,v,x)
        if(abs(aTmpR)>abs(aExtremaR)):
            aExtremaR= aTmpR
        outputFile.write(str(v*3.6)+" "+str(abs(aTmpR))+"\n")
        v+= incV
    return aExtremaR
