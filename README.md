# neuland macros

neuland simulation and analysis macros 
to work with SpiRITROOT branch; neulandSim.

use clean.sh to clean up core_dumps

## directories

 - data :
 - configures : configuration files such as generator file

## run_mc_single

MC simulation using STSingleEventGenerators.
The event configuration files are in the configures/ directory with name of
[name].par, with the [name] inside the code.

## draw_event.C

draw (any spiritroot run) output event with, tpc and neuland frame and reference guide lines. 

## navigate_geomtry.C

navvigate_geometry made from geomSpiRIT_wNeuland.C and create list of geometry
with copyNo : list_geomtry.dat
