/*
F * phenology.c
 *
 *  Created on: 21/mag/2013
 *      Author: alessio
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "matrix.h"
#include "settings.h"
#include "soil_settings.h"
#include "phenology.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;
extern soil_settings_t *g_soil_settings;


void phenology(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily, const int month)
{

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_log, "\n--DAILY PHENOLOGY for SPECIES %s phenology = %.1f--\n", s->name, s->value[PHENOLOGY]);
	logger(g_log, "-LAI = %g\n-PEAK_LAI = %g\n", s->value[LAI], s->value[PEAK_LAI]);

	/*for deciduous*/
	if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
	{
		if (s->counter[VEG_UNVEG] == 1 )
		{
			/* Beginning of growing season */
			/* BUDBURST PHASE */
			if (s->counter[VEG_DAYS] <= ((int)s->value[BUD_BURST]))
			{
				/* Maximum growth */
				if (s->value[LAI] < s->value[PEAK_LAI])
				{
					s->phenology_phase = 1;
				}
				/* Normal Growth */
				else
				{
					s->phenology_phase = 2;
				}
			}
			else
			{
				/* Normal growth */
				if (month+1 <= 6)
				{
					s->phenology_phase = 2;
				}
				else
				{
					/* Normal growth */
					if (meteo_daily->daylength > s->value[MINDAYLENGTH])
					{
						s->phenology_phase = 2;
					}
					else
					{
						/* Leaf fall */
						s->phenology_phase = 3;
					}
				}
			}
		}
		else
		{
			/* Un-vegetative period */
			s->phenology_phase = 0;
		}
	}
	/*for evergreen*/
	else
	{

		/* a very simplistic way to define a phenological phase for evergreen*/
		/*just two phase are considered
		 * shoot elongation
		 * secondary growth*/
		/*see Ludeke et al., 1994*/

		/* Beginning of a "growing season" */
		/* Maximum growth */
		// old if ( meteo_daily->thermic_sum >= s->value[GROWTHSTART] && s->value[LAI] < s->value[PEAK_LAI] )
		if ( s->value[LAI] < s->value[PEAK_LAI] &&
				( ( g_soil_settings->values[SOIL_LAT] > 0 && month < 5 ) ||
				( g_soil_settings->values[SOIL_LAT] < 0 && month > 11 ) ) )
		{
			s->phenology_phase = 1;
		}
		/* Normal growth*/
		else
		{
			s->phenology_phase = 2;
		}
	}
	logger(g_log, "phenology phase = %d\n", s->phenology_phase);
}

//void Phenology_phase (species_t *const s, const meteo_t* const met, const int year, const int month, const int day)
//{
//	logger(g_log, "--GET_DAILY PHENOLOGY for SPECIES %s phenology = %.1f--\n", s->name, s->value[PHENOLOGY]);
//
//	/*for deciduous*/
//	if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
//	{
//		if (s->counter[VEG_UNVEG] == 1 )
//		{
//			//Beginning of growing season
//
//			//BUDBURST
//			if (s->counter[VEG_DAYS] <= s->value[BUD_BURST])
//			{
//				if (s->value[LAI] < (s->value[PEAK_LAI] * 0.5))
//				{
//					s->phenology_phase = 1;
//				}
//				//Maximum Growth
//				else if (s->value[LAI] > (s->value[PEAK_LAI] * 0.5)	&& s->value[LAI] < s->value[PEAK_LAI])
//				{
//					logger(g_log, "LAI = %f\n", s->value[LAI]);
//					logger(g_log, "PEAK LAI = %f\n", s->value[PEAK_LAI]);
//					s->phenology_phase = 2;
//				}
//				//Full growing season
//				else
//				{
//					s->phenology_phase = 4;
//				}
//			}
//			else
//			{
//				//Maximum Growth
//				if (s->value[LAI] <= (s->value[PEAK_LAI] * 0.5))
//				{
//					s->phenology_phase = 2;
//				}
//				//Maximum Growth
//				if (s->value[LAI] > (s->value[PEAK_LAI] * 0.5)&& s->value[LAI] < s->value[PEAK_LAI])
//				{
//					s->phenology_phase = 3;
//				}
//				//Full growing season
//				if(fabs (s->value[LAI] - s->value[PEAK_LAI]) < 0.1)
//				{
//					s->phenology_phase = 4;
//				}
//				if (met[month].d[day].daylength < s->value[MINDAYLENGTH] && month+1 > 6 /*c->abscission_daylength*/)
//				{
//					//Leaf fall
//					s->phenology_phase = 5;
//				}
//			}
//		}
//		else
//		{
//			//Unvegetative period
//			s->phenology_phase = 0;
//		}
//	}
//	/*for evergreen*/
//	else
//	{
//		//fixme
//		/* a very simplistic way to define a phenological phase for evergreen*/
//		/*just two phase are considered
//		 * shoot elongation
//		 * secondary growth*/
//		/*see Ludeke et al., 1994*/
//		/*Beginning of a "growing season"*/
//		if (met[month].d[day].thermic_sum >= s->value[GROWTHSTART] && s->value[LAI] < s->value[PEAK_LAI]																										   && month < 6)
//		{
//			s->phenology_phase = 1;
//			s->counter[LEAF_FALL_COUNTER] = 0;
//		}
//		/*normal growth*/
//		else
//		{
//			s->phenology_phase = 2;
//			if (s->counter[LEAF_FALL_COUNTER] == 0)
//			{
//				s->counter[LEAF_FALL_COUNTER] = 1;
//			}
//			else if (s->counter[LEAF_FALL_COUNTER] == 1)
//			{
//				s->counter[LEAF_FALL_COUNTER] = 2;
//			}
//		}
//		if (day == 0 && month == 0 && year == 0)
//		{
//			s->phenology_phase = 1;
//			s->counter[LEAF_FALL_COUNTER] = 0;
//			s->value[DAILY_LEAVES_BIOMASS_TO_REMOVE] = 0.0;
//		}
//	}
//
//	logger(g_log, "phenology phase = %d\n", s->phenology_phase);
//}


