/*
 * wue.c
 *
 *  Created on: 14/nov/2012
 *      Author: alessio
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "wue.h"
#include "common.h"

/* Last cumulative days in months */
extern int MonthLength [];
extern int MonthLength_Leap [];

void water_use_efficiency( cell_t *const c, const int height, const int dbh, const int age, const int species, const int day, const int month, const int year)
{

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* WATER USE EFFICIENCY */
	/* it computes daily, monthly and annual water use efficiency following:
	 * Beer et al., 2009;
	 * Zhang et al., 2016 Agricultural and Forest Meteorology
	 */
	/* note: differently from the authors above we use canopy transpiration for computation of WUE */

	/* daily WUE */
	s->value[WUE] = s->value[DAILY_GPP_gC] / s->value[CANOPY_TRANSP];

	/* monthly WUE */
	/* last day of the month */
	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[month] ) : (MonthLength[month] )) == c->doy )
	{
		s->value[M_WUE] = s->value[MONTHLY_GPP_gC] / s->value[MONTHLY_CANOPY_TRANSP];
	}

	/* annual WUE */
	/* last day of the year */
	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365) )
	{
		s->value[Y_WUE] = s->value[YEARLY_GPP_gC] / s->value[YEARLY_CANOPY_TRANSP];
	}

}
