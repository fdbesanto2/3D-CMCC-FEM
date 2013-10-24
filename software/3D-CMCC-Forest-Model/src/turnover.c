/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"



void Get_turnover (SPECIES *const s, int DaysInMonth)
{
	//int oldWs;
	//int oldWrf;
	//int oldWrc;

	float fine_root_turnover_rate;
	float coarse_root_turnover_rate;
	float stem_turnover_rate;

	Log("****MONTHLY TURNOVER****\n");
	/*
	if (settings->time == 'd')
	{
		//parameter values are often in monthly time step
		fine_root_turnover_rate = s->value[FINERTTOVER]/DaysInMonth;
		coarse_root_turnover_rate = s->value[COARSERTTOVER]/DaysInMonth;
		stem_turnover_rate = s->value[SAPWOODTTOVER]/DaysInMonth;
	}
	else
	{
		fine_root_turnover_rate = s->value[FINERTTOVER];
		coarse_root_turnover_rate = s->value[COARSERTTOVER];
		stem_turnover_rate = s->value[SAPWOODTTOVER];
	}
	*/

	fine_root_turnover_rate = s->value[FINERTTOVER];
	coarse_root_turnover_rate = s->value[COARSERTTOVER];
	stem_turnover_rate = s->value[SAPWOODTTOVER];


	Log("****fine root turnover****\n");
	Log("Fine Root Biomass  = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
	s->value[DEL_ROSS_FINE] =  s->value[BIOMASS_ROOTS_FINE_CTEM] * fine_root_turnover_rate;
	Log("Average monthly fine Root turnover rate  = %g\n",fine_root_turnover_rate);
	Log("Fine Root Biomass lost = %g tDM/area\n", s->value[DEL_ROSS_FINE]);

	if (s->value[DEL_ROSS_FINE] > s->value[BIOMASS_ROOTS_FINE_CTEM])
	{
		Log("ATTENTION fine root turnover exceed DEL_WR !!! \n");
		s->value[BIOMASS_ROOTS_FINE_CTEM] -= s->value[DEL_ROOTS_FINE_CTEM];
	}

	//oldWrf = s->value[BIOMASS_ROOTS_FINE_CTEM];

	// Fine Root Biomass less root turnover

	s->value[BIOMASS_ROOTS_FINE_CTEM] -= s->value[DEL_ROSS_FINE];
	Log("Fine Root Biomass  = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);


	//fixme do not use 3-PG parameters value (it takes into account fine + coarse)
	Log("****   coarse root turnover****\n");
	Log("Coarse Root Biomass  = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);
	s->value[DEL_ROSS_COARSE] =  s->value[BIOMASS_ROOTS_COARSE_CTEM] * coarse_root_turnover_rate;
	Log("Average monthly coarse Root turnover rate  = %g\n", coarse_root_turnover_rate);
	Log("Coarse Root Biomass lost = %g tDM/ha\n", s->value[DEL_ROSS_COARSE]);


	if (s->value[DEL_ROSS_COARSE] > s->value[BIOMASS_ROOTS_COARSE_CTEM])
	{
		Log("ATTENTION coarse root turnover exceed DEL_WR !!! \n");
		s->value[BIOMASS_ROOTS_COARSE_CTEM] -= s->value[DEL_ROOTS_COARSE_CTEM];
	}

	//oldWrc = s->value[BIOMASS_ROOTS_COARSE_CTEM];


	//Coarse Root Biomass less root turnover

	s->value[BIOMASS_ROOTS_COARSE_CTEM] -= s->value[DEL_ROSS_COARSE];
	Log("Coarse Root Biomass  = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

	/*
    Log("****Stem turnover****\n");
    //in LPJ the turnover of the stem in considered as the amount of sapwood that pass to the heartwood

    s->value[DEL_ROSS_STEM] =  s->value[BIOMASS_STEM_CTEM]* stem_turnover_rate;
    Log("Average monthly Stem turnover rate  = %g\n", stem_turnover_rate);
    Log("Stem Biomass lost = %g tDM/ha\n", s->value[DEL_ROSS_STEM]);

    if (s->value[DEL_ROSS_STEM] > s->value[BIOMASS_STEM_CTEM])
    {
         Log("ATTENTION stem turnover exceed DEL_WS !!! \n");
         s->value[BIOMASS_STEM_CTEM] -= s->value[DEL_ROSS_STEM];
    }

    oldWs = s->value[BIOMASS_STEM_CTEM];

    // Fine Root Biomass less root turnover

    s->value[BIOMASS_STEM_CTEM] -= s->value[DEL_ROSS_STEM];
	 */

}
