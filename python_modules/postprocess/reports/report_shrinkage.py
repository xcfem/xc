# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from postprocess.reports import common_formats

def latex_spanish(fck, RH, Ac, u, h0mm, Epscainf, Betaast, t, Epsca, BetaRH, Alfads1, Alfads2, Epscd0, Kh, Betadstts, Epscd, Epscs):
    ''' Writes a report of the shrinkage values in spanish.

    :param fck: characteristic (5%) cylinder strength of the concrete
    :param RH: ambient relative humidity(%)
    :param Ac: area of the concrete member (m2)
    :param u: perimeter of the concrete member exposed to drying (m)
    :param h0mm: notional size of the member h0 expressed in millimeters.
    :param Epscainf: coefficient for calculating the autogenous shrinkage strain
    :param Betaast: coefficient for calculating the autogenous shrinkage strain
    :param t: age of the concrete expressed in days.
    :param Epsca: autogenous shrinkage strain
    :param BetaRH: coefficient for the calculation of the basic drying shrinkage strain
    :param Alfads1: coefficient for the calculation of the basic drying shrinkage strain
    :param Alfads2: coefficient for the calculation of the basic drying shrinkage strain
    :param Epscd0: basic drying shrinkage strain 
    :param Kh: coefficient for the calculation of the drying shrinkage strain
    :param Betadstts: coefficient for the calculation of the drying shrinkage strain
    :param Epscd: drying shrinkage strain
    :param Epscs: total shrinkage
    '''
    outputStr= '\\begin{center}\n'
    outputStr+='\\begin{tabular}{llr}\n'
    outputStr+='Resistencia característica del hormigón & $ f_{ck}$ & '+common_formats.Stress.format(-fck/1e6)+' [$N/mm^2$] \\\\\n'
    outputStr+='Humedad relativa & HR & '+str(RH)+ ' [\\%] \\\\\n'
    outputStr+='Área de hormigón & $A_c$ & '+str(Ac)+ ' [$m^2$] \\\\\n'
    outputStr+='Perímetro en contacto con la atmósfera & u & '+str(round(u,3))+ ' [m] \\\\\n'
    outputStr+='Espesor medio en milímetros & $2A_c/u$ & '+str(round(h0mm,1))+ ' [mm] \\\\\n'

    outputStr+='\\multicolumn{3}{l}{\\textbf{Retracción autógena:}} \\\\\n'
    outputStr+='Coeficiente $t=\\infty$ & $\\epsilon_{ca,\\infty}$ & '+'{:.2e}'.format(Epscainf)+' \\\\\n'
    outputStr+='Coeficiente t= '+ str(t)+ ' días & $\\beta_{as}(t)$ & '+str(round(Betaast,3))+' \\\\\n'
    outputStr+='Deformación de retracción autógena & $\\epsilon_{ca}(t)$ & '+ '{:.2e}'.format(Epsca)+ ' \\\\\n'

    outputStr+='\\multicolumn{3}{l}{\\textbf{Retracción por secado: }} \\\\\n'
    outputStr+='Coeficiente $\\beta_{HR}$ & $\\beta_{HR}$ & '+str(BetaRH)+ ' \\\\\n'
    outputStr+='Coeficiente $\\alpha_{ds1}$ & $\\alpha_{ds1}$ & '+str(Alfads1)+ ' \\\\\n'
    outputStr+='Coeficiente $\\alpha_{ds2}$ & $\\alpha_{ds2}$ & '+str(Alfads2)+ ' \\\\\n'
    outputStr+='Coeficiente $\\epsilon_{cd0}$ & $\\epsilon_{cd0}$ & '+'{:.2e}'.format(Epscd0)+' \\\\\n'
    outputStr+='Coeficiente $K_{h}$ & $K_{h}$ & '+str(Kh)+ ' \\\\\n'

    outputStr+='Coeficiente de evolución temporal & $\\beta_{ds}(t-t_s)$ & '+'{:.2e}'.format(Betadstts)+ ' \\\\\n'

    outputStr+='Deformación de retracción por secado & $\\epsilon_{cd}(t)$ & '+ '{:.2e}'.format(Epscd)+ ' \\\\\n'

    outputStr+='\\multicolumn{3}{l}{\\textbf{Retracción total:}} \\\\\n'
    outputStr+='Retracción total & $\\epsilon_{cs}$ & \\textbf{'+'{:.2e}'.format(Epscs)+'} \\\\\n'
    outputStr+='\\end{tabular}\n'
    outputStr+='\\end{center}\n'
    return outputStr
