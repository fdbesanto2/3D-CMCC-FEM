/*
 * soil_water_balance.c
 *
 *  Created on: 12/nov/2012
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "soil_water_balance.h"
#include "common.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

//fixme  maybe it can be moved to soil_model.c
void Soil_water_balance(cell_t *c, const meteo_daily_t *const meteo_daily)
{
	double water_to_soil;
	double soilwater_to_atmosphere;

	logger(g_log, "\n**SOIL WATER BALACE**\n");
	/* water pools */
	c->old_daily_c_water_stored = c->daily_c_water_stored;
	c->old_asw = c->asw;

	water_to_soil = c->asw + c->prcp_rain + c->snow_melt;

	/* extracted water from the soil */
	soilwater_to_atmosphere = c->daily_c_transp + c->daily_soil_evapo;

	/*update balance*/
	if(meteo_daily->tavg>0.0)
	{
		c->asw = water_to_soil - soilwater_to_atmosphere;
	}
	else
	{
		c->asw = c->asw - soilwater_to_atmosphere;
	}
	logger(g_log, "ASW = %f mm\n", c->asw);
	logger(g_log, "snow pack = %f mm\n", c->snow_pack);


	if ( c->asw > c->max_asw_fc)
	{
		c->out_flow = c->asw - c->max_asw_fc;
		logger(g_log, "out_flow = %f\n", c->out_flow);
		logger(g_log, "ATTENTION Available Soil Water exceeds MAXASW!! \n");
		c->asw = c->max_asw_fc;
		logger(g_log, "Available soil water = %f\n", c->asw);
	}
	/* calculates the outflow flux from the difference between soilwater
	and maximum soilwater */
	/* water in excess of saturation to outflow */
//todo test it
//	if (c->asw > c->max_asw_sat)
//	{
//		logger(g_log, "c->asw = %f\n", c->asw);
//		c->out_flow = c->asw - c->max_asw_sat;
//		c->asw -= c->out_flow;
//		logger(g_log, "c->out_flow = %f\n", c->out_flow);
//	}
//	/* slow drainage from saturation to field capacity */
//	else if (c->asw > c->max_asw_fc)
//	{
//		logger(g_log, "c->asw > c->soilw_fc\n");
//		c->out_flow = 0.5 * (c->asw - c->max_asw_fc);
//		c->asw -= c->out_flow;
//		logger(g_log, "c->out_flow = %f\n", c->out_flow);
//	}
//	/* otherwise, no outflow */
//	else
//	{
//		c->out_flow = 0.0;
//	}

	/* from BIOME-BGC */
	/* the following special case prevents evaporation under very
	dry conditions from causing a negative soilwater content */
	/* negative soilwater */
	if (c->asw < 0.0)
	{
		/* add back the evaporation and transpiration fluxes, and
		set these fluxes to 0.0 */
		c->asw             += c->daily_soil_evapo;
		c->asw             += c->daily_c_transp;
		c->daily_soil_evapo = 0.0;
		c->daily_c_transp   = 0.0;

		/* test again for negative soilwater...should never be true */
		CHECK_CONDITION(c->asw, < 0.0)
	}

	//ALESSIOC
	/*
	c->swc = (c->asw * 100)/c->max_asw_fc;
	logger(g_log, "asw = %g\n", c->asw);
	logger(g_log, "max_asw_fc = %g\n", c->max_asw_fc);
	logger(g_log, "SWC = %g(%%vol)\n", c->swc);

	// check
	CHECK_CONDITION (c->swc, > 100.1);
	CHECK_CONDITION (c->swc, < 0);
	*/
}
