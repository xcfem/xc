# -*- coding: utf-8 -*-
# Rutinas para elaborar el estadillo de cargas.
def printCargas(nmbLst, nmbTipoCargas, ud):
    print "    \\item ",nmbTipoCargas,"\n"
    print "      \\begin{itemize}\n"
    tot= 0.0
    for l in nmbLst:
      print "      \\item ",l.descr," \\dotfill\\",format(l.pesoEsp/1e3,"%7.3f")," ",l.ud,"\n"
      tot+= l.pesoEsp
    print "      \\item \\textbf{Total ",nmbTipoCargas,"} \\dotfill\\ \\textbf{",format(tot/1e3,"%7.3f"),"} ",ud,"\n"

    print "      \\end{itemize}"
    return tot

def printCargasSup(nmbLst, nmbTipoCargas):
    tot= printCargas(nmbLst,nmbTipoCargas,"$kN/m^2$")
    return tot

def printCargasLin(nmbLst, nmbTipoCargas):
    tot= printCargas(nmbLst,nmbTipoCargas,"$kN/m$")
    return tot

def printCargasPlanta(nmbPlanta, nmbCP, nmbSC):
    print "\\item ",nmbPlanta,"\n"
    print "  \\begin{itemize}\n"
    tot = printCargasSup(nmbCP,"Carga permanente")
    tot = tot+ printCargasSup(nmbSC,"Sobrecarga")
    print "  \\item \\textbf{Total} "," \\dotfill\\ \\textbf{",format(tot/1e3,"%7.3f"),"} $kN/m^2$\n"
    print "  \\end{itemize}\n"

