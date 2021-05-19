# -*- coding: utf-8 -*-

import scipy.interpolate

def interpolateLine(x0,y0,x1,y1,x):
    ''' Return the value obtained interpolating with a line
    that passes through the points (x0,y0) y (x1,y1)'''
    return (y1-y0)/(x1-x0)*(x-x0)+y0 

def interpEquidistPoints(xi,yi,nDiv):
    '''Return the interpolated values of the function y=f(x) at equally spaced 
    x coordinates. The function is defined by a list of 'yi' ordinate at 
    abcissae 'xi'  

    :param xi: list of abcissae.
    :param yi: list of ordinate.
    :param nDiv: number of divisions in segment xi[0] to xi[-1] where to obtain 
                 the abcisae to be interpolated.
    '''
    fInterp=scipy.interpolate.interp1d(xi, yi)
    step=(xi[-1]-xi[0])/nDiv
    xInterp=[xi[0]+i*step for i in range(nDiv+1)]
    yInterp=[float(fInterp(x)) for x in xInterp]
    return yInterp
    
