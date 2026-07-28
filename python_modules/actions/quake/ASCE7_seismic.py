def asce7_like_target_horiz_spectrum(periods, Ss, S1, TL,Fa,Fv):
    """
    Simple ASCE 7 - style design 5%-damped response spectrum shape
    periods = list of periods
    Ss = short-period spectral response acceleration (g),
    S1 = spectral response acceleration (g) for 1-second period
    Fa = site coefficient at short period (table 11.4-1)
    Fv= site coefficient at 1s period (table 11.4-2)
    """
    Sds = (2.0 / 3.0) * Fa * Ss
    Sd1 = (2.0 / 3.0) * Fv * S1
    T0 = 0.2 * Sd1 / Sds
    Ts = Sd1 / Sds

    Sa = np.zeros_like(periods)
    for i, T in enumerate(periods):
        if T <= T0:
            Sa[i] = Sds * (0.4 + 0.6 * T / T0) if T0 > 0 else Sds
        elif T <= Ts:
            Sa[i] = Sds
        elif T <= TL:
            Sa[i] = Sd1 / T
        else:
            Sa[i] = Sd1 * TL / T ** 2
    return Sa

