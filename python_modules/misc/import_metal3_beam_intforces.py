# -*- coding: utf-8 -*-

import os

multINode= "*10000+"

def getNombreAccion(str):
  # Devuelve el nombre de la acción.
  retval= eliminaCar(copiaDesde(":",copiaHasta(char(40),str))," ")    
  if(re.match(".*Modo.*",str)):
    nmbModoSismo= replaceAll(str,": Modo ","#")
    nmbModoSismo= copiaDesde("#",nmbModoSismo)
    nmbModoSismo= copiaHasta(char(41),nmbModoSismo)
    retval= retval+"M"+nmbModoSismo
  return retval


def metal3DcreaTablaEsfBarra(nmbArch, nmbDBase, nmbTabla):
  '''
  Importa el listado de esfuerzos en barras de metal 3D que se pasa como parámetro
     nmbArchivo: Nombre del archivo que contiene el listado.
     nmbDBase: Nombre de la base de datos donde se guardará la tabla.
     nmbTabla: Nombre de la tabla que contendrá los esfuerzos.
  '''
  os.sys("fromdos " + nmbArch)
  os.sys("csplit -s "+ nmbArch + " '/.*Esfuerzos.*/' '/.*Esfuerzos.*/'")
  nmbArchEsf= nmbArch + ".esf"
  os.sys("mv xx02 " + nmbArchEsf)
  os.sys("rm -f xx*")
  SQLTcreaTabla(nmbDBase,nmbTabla,"(idSecc int,ACCION string,ELEM integer,xSEC double,AXIL double,Q_1 double,Q_2 double,M_1 double,M_2 double,TORSOR double)")
  listado= open(nmbArchEsf,"r")
  str1= ""
  str1SB= ""
  csv= ""
  lst= []
  lstXSec= ""
  nmbAccion= ""
  idElem= 0.0
  idSecc= 0.0
  numSec= 0.0
  xSec= 0.0
  tmpStr= ""
  j= 0.0
  listado= open(nmbArchEsf)
  for line in listado:
    str1= line
    str1SB= str.strip()
    if(strlen(str1SB)>1):
      if(re.match(".*Hipótesis.*",str1SB)):
        nmbAccion= getNombreAccion(str1SB)
        for j in range(1,numSec):
          xSec= tonum(replaceAll(lst[2+j],"m",""))
          idSecc= idElem*10+j
          cur.execute("insert into "+nmbTabla+" (idSecc,ACCION,ELEM,xSEC) values("+sqlValue(int(idSecc))+","+sqlValue(nmbAccion)+","+sqlValue(idElem)+","+sqlValue(xSec)+")")
      else:
        csv= replace(eliminaCar(str1SB," "),"|",",")
        if(re.match(",[0-9]+/[0-9]+.*",csv)):
          lst=separaCsv(csv)
          idElem= tonum(replaceAll(lst[1],"/",multINode))
          numSec= lst.size-4
  listado.close()

def getCodEsf(cod):
  retval= ifte(cod=="N","AXIL",ifte(cod=="Ty","Q_1",ifte(cod=="Tz","Q_2",ifte(cod=="Mt","TORSOR",ifte(cod=="My","M_1",ifte(cod=="Mz","M_2","?"))))))
  return retval

def metal3DToEsfBarra(nmbArch, nmbDBase, nmbTabla):
  '''
  Importa el listado de esfuerzos en barras de metal 3D que se pasa como parámetro
     nmbArchivo: Nombre del archivo que contiene el listado.
     nmbDBase: Nombre de la base de datos donde se guardará la tabla.
     nmbTabla: Nombre de la tabla que contendrá los esfuerzos.
  '''
  os.sys("fromdos " + nmbArch)
  os.sys("csplit -s "+ nmbArch + " '/.*Esfuerzos.*/' '/.*Esfuerzos.*/'")
  nmbArchEsf= nmbArch + ".esf"
  os.sys("mv xx02 " + nmbArchEsf)
  os.sys("rm -f xx*")
  metal3DcreaTablaEsfBarra(nmbArch,nmbDBase,nmbTabla)
  listado= open(nmbArchEsf,"r")
  str1= ""
  str1SB= ""
  csv= ""
  lst= []
  nmbAccion= ""
  idElem= 0.0
  numSec= 0.0
  tmpStr= ""
  tmpDbl= 0.0
  codEsf= ""
  N= 0.0
  Ty= 0.0
  Tz= 0.0
  Mt= 0.0
  My= 0.0
  Mz= 0.0
  for line in listado:
    str1= readln("listado")
    str1SB= str1.strip()
    if(strlen(str1SB)>1):
      if(re.match(".*Hipótesis.*",str1SB)):
        nmbAccion= getNombreAccion(str1SB)
      else:
        csv= replace(eliminaCar(str1SB," "),"|",",")
        if(re.match(",[0-9]+/[0-9]+.*",csv)):
          lstXSec=separaCsv(csv)
          idElem= tonum(replaceAll(lstXSec[1],"/",multINode))
          numSec= lstXSec.size-4
        else:
          if(re.match(",,[MNT].*",csv)):
            lst=separaCsv(csv)
            codEsf= getCodEsf(lst[2])
            if(codEsf=="?"):
              print "El código de esfuerzo: ",lst[2]," en la línea",str1SB," es desconocido"
            else:
              for j in range(1,numSec):
                xSec= tonum(replaceAll(lstXSec[2+j],"m",""))
                tmpDbl= lst[2+j]
                tmpStr= "update "+nmbTabla+" set " + codEsf + " = "
                tmpStr= tmpStr + sqlValue(tmpDbl) + " where ELEM= " + sqlValue(idElem)
                tmpStr= tmpStr + " and xSEC= " + sqlValue(xSec)+" and ACCION= "+sqlValue(nmbAccion)
                cur.execute(tmpStr)
  listado.close()


