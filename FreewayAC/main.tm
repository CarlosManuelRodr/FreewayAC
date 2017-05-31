:Begin:
:Function:       create_circular_ca
:Pattern:        CreateCircularCA[size_Integer, vmax_Integer, density_Real, randp_Real, initVel_Integer]
:Arguments:      { size, vmax, density, randp, initVel }
:ArgumentTypes:  { Integer, Integer, Real, Real, Integer }
:ReturnType:     Manual
:End:

:Begin:
:Function:       delete_ca
:Pattern:        DeleteCA[]
:Arguments:      Manual
:ReturnType:     Manual
:End:

:Begin:
:Function:       ca_step
:Pattern:        CAStep[]
:Arguments:      Manual
:ReturnType:     Manual
:End:

:Begin:
:Function:       ca_evolve
:Pattern:        CAEvolve[iterations_Integer]
:Arguments:      { iterations }
:ArgumentTypes:  { Integer }
:ReturnType:     Manual
:End:

:Begin:
:Function:       ca_count_cars
:Pattern:        CACountCars[]
:Arguments:      Manual
:ReturnType:     Integer
:End:

:Begin:
:Function:       ca_calculate_ocupancy
:Pattern:        CACalculateOcupancy[]
:Arguments:      Manual
:ReturnType:     Manual
:End:

:Begin:
:Function:       ca_calculate_flow
:Pattern:        CACalculateFlow[]
:Arguments:      Manual
:ReturnType:     Manual
:End:


:Begin:
:Function:       traffic_map
:Pattern:        TrafficMap[i_Integer, j_Integer]
:Arguments:      { i, j }
:ArgumentTypes:  { Integer, Integer }
:ReturnType:     Manual
:End:

:Begin:
:Function:       set_value
:Pattern:        SetMyValue[i_Integer]
:Arguments:      { i }
:ArgumentTypes:  { Integer }
:ReturnType:     Manual
:End:

:Begin:
:Function:       get_value
:Pattern:        GetValue[]
:Arguments:      Manual
:ReturnType:     Integer
:End: