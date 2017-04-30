import rough_calculations.ng_punzonamiento


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2016, AOO and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

#CASO 1

qk=2      #kN/m2
A=5*5     #m2

Vd= rough_calculations.ng_punzonamiento.esfuerzoPunzonamiento(qk,A)
#Da como resultado Vd=80 kN

fck=25
d=0.20
a=0.25
b=0.25

Vdmax= rough_calculations.ng_punzonamiento.punzMaximo(fck,d,a,b)
#Da como resultado Vdmax=387 kN

h=0.25
fyd=500/1.15

As= rough_calculations.ng_punzonamiento.reinforcementPunz(Vd,fck,d,a,b,h,fyd)
#Da como resultado Vd<Vcu -> No necesita reinforcement de punzonamiento


#CASO 2

qk=7        #kN/m2
A=6*6     #m2

Vd=rough_calculations.ng_punzonamiento.esfuerzoPunzonamiento(qk,A)
#Da como resultado Vd=403 kN

fck=25
d=0.15
a=0.25
b=0.25

Vdmax=rough_calculations.ng_punzonamiento.punzMaximo(fck,d,a,b)
#Da como resultado Vdmax=290 kN

h=0.20
fyd=500/1.15

As=rough_calculations.ng_punzonamiento.reinforcementPunz(Vd,fck,d,a,b,h,fyd)
#Da como resultado As=39 cm2
