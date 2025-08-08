# -*- coding: utf-8 -*-
''' Code related to tabular LaTeX environment.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

def get_tabular_head_tail(headTitles, justif,caption,tit2ndLine=None,label=None):
    '''Generate the heading tex lines for a long table

    :param headTitles: list of titles for the table (first line e.g. ['tit1','tit2',...]
    :param justif: justification of columns (e.g. 'lcr')
    :param caption: caption text (e.g. 'Caption text')
    :param tit2ndLine:  list of titles, if any, for the second line of the head titles (defaults to None)
    :param label: label for the table (defaults to None)
    '''
    tableHead='\\begin{table} \n \\centering \n'
    tableHead+='\\caption{'+caption+'} \\\\ \n'
    if label:
        tableHead+='\\label{'+label+'} \n'
    tableHead+='\\begin{tabular}{|'+justif+'|} \n'
    head='\\hline \n '
    for tit in headTitles:
        head+=tit+' & '
    head=head.removesuffix('& ')
    head+=' \\\\ \n'
    if tit2ndLine:
        for tit in tit2ndLine:
            head+=tit+' & '
    head=head.removesuffix('& ')
    if tit2ndLine:
        head+=' \\\\'
    head+='  \n'
    #head+=' \\hline \n'
    tableHead+=head + '\\hline \n'
    tableTail='\\hline \\end{tabular} \n'
    tableTail+='\\end{table} \n'
    return tableHead,tableTail
