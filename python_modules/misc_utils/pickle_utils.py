import scipy.interpolate

class interp1dPicklable:
    ''' class wrapper for piecewise linear function
    '''
    def __init__(self, xi, yi, **kwargs):
        self.xi = xi
        self.yi = yi
        self.args = kwargs
        self.f = scipy.interpolate.interp1d(xi, yi, **kwargs)

    def __call__(self, xnew):
        return self.f(xnew)

    def __getstate__(self):
        return self.xi, self.yi, self.args

    def __setstate__(self, state):
        self.f = scipy.interpolate.interp1d(state[0], state[1], **state[2])

