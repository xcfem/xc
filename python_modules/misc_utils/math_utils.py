# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2019 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import numpy as np

class LinearInterpolation(object):
    ''' Simple linear interpolation.

    :ivar x0: first point abscissa.
    :ivar y0: first point ordinate.
    :ivar x1: second point abscissa.
    :ivar y1: second point ordinate.
    '''
    def __init__(self,x0, y0, x1, y1):
        ''' Constructor.

        :param x0: first point abscissa.
        :param y0: first point ordinate.
        :param x1: second point abscissa.
        :param y1: second point ordinate.
        '''
        self.tang= (y1-y0)/(x1-x0)
        self.x0= x0; self.y0= y0
    def getY(self,x):
        ''' Return the linear interpolation
            result for x.

        :param x: abscissa.
        '''
        return self.tang*(x-self.x0)+self.y0 

class StaticGridBilinearInterpolator:
    '''2D bilinear interpolator optimized for static source grids.
    
    This class pre-caches a fixed source coordinate grid and layout matrix. 
    It provides an ultra-fast, vectorized __call__ method using binary search 
    (np.searchsorted) to evaluate arbitrary, matched (x, y) coordinate pairs 
    without Python loops. Out-of-bounds queries automatically return np.nan.
    '''
    
    def __init__(self, x, y, z):
        '''Initializes the interpolator with static source grid data.
        
        :param x (array_like): 1D array of shape (M,) representing X
                               coordinates. Must be strictly increasing.
        :param y (array_like): 1D array of shape (N,) representing Y
                               coordinates. Must be strictly increasing.
        :param z (array_like): 2D array of shape (M, N) representing Z values,
                               where z[i, j] maps directly to x[i] and y[j].
        '''
        # Ensure inputs are standard numpy float arrays
        self.x = np.asarray(x, dtype=float)
        self.y = np.asarray(y, dtype=float)
        self.z = np.asarray(z, dtype=float)
        
        # Cache absolute grid boundaries for fast out-of-bounds filtering
        self.x_min, self.x_max = self.x[0], self.x[-1]
        self.y_min, self.y_max = self.y[0], self.y[-1]
        
        # Cache dimensions for safe array index clipping
        self.nx = len(self.x)
        self.ny = len(self.y)

    def __call__(self, x_new, y_new):
        '''Evaluates matched, individual (x, y) coordinate pairs.
        
        :param x_new (array_like): 1D array of target X coordinates of length K.
        :param y_new (array_like): 1D array of target Y coordinates of length K.
            
        :returns np.ndarray: A 1D float array of shape (K,) containing
                             interpolated Z values. Points outside the
                             original grid bounds are set to np.nan.
        '''
        # Convert target vectors to numpy arrays
        x_new = np.asarray(x_new, dtype=float)
        y_new = np.asarray(y_new, dtype=float)
        
        # 1. Create a boolean mask identifying coordinates outside grid limits
        out_of_bounds = (
            (x_new < self.x_min) | (x_new > self.x_max) |
            (y_new < self.y_min) | (y_new > self.y_max)
        )
        
        # 2. Use fast C-level binary search to locate bounding boxes
        # Finds index 'i' such that self.x[i] <= x_new < self.x[i+1]
        i = np.searchsorted(self.x, x_new) - 1
        j = np.searchsorted(self.y, y_new) - 1
        
        # 3. Temporarily clamp out-of-bounds indices to avoid IndexError.
        # This keeps the matrix lookup safe; bad points are overwritten with NaN later.
        i = np.clip(i, 0, self.nx - 2)
        j = np.clip(j, 0, self.ny - 2)
        
        # 4. Calculate normalized linear interpolation weights along both axes
        t= (x_new - self.x[i]) / (self.x[i+1] - self.x[i])
        u= (y_new - self.y[j]) / (self.y[j+1] - self.y[j])
        
        # 5. Perform vectorized extraction of the 4 surrounding pixel values
        i_= min(i, self.nx-1)
        j_= min(j, self.ny-1)
        ii= min(i+1, self.nx-1)
        jj= min(j+1, self.ny-1)
        z00 = self.z[i_, j_] # Bottom-left corner
        z10 = self.z[ii, j_] # Bottom-right corner
        z01 = self.z[i_, jj] # Top-left corner
        z11 = self.z[ii, jj]  # Top-right corner
        
        # 6. Apply standard bilinear combination formula
        z_new = (1 - t) * (1 - u) * z00 + t * (1 - u) * z10 + (1 - t) * u * z01 + t * u * z11
        
        # 7. Suppress any out-of-bounds evaluation results to np.nan
        z_new[out_of_bounds] = np.nan
        
        return z_new
