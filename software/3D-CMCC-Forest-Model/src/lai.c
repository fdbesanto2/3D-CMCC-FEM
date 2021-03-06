/*lai.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

//TO COMPUTE YEARLY PEAK LAI FROM PROVIOUS YEARLY LAI

void Get_initial_lai (SPECIES *const s, const int years, const int month, const int day)
{


	if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
	{
		if (settings->time == 'm')
		{

			Log("\n--GET_INITIAL_MONTH_LAI--\n");

			if (s->counter[VEG_MONTHS]  == 1)
			{
				Log("++Reserves pools = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);
				Log("++Reserve biomass for each tree in g = %g \n", (s->value[BIOMASS_RESERVE_CTEM] * 1000000) / s->counter[N_TREE]);

				//just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
				//the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
				s->value[BIOMASS_FOLIAGE_CTEM] = s->value[BIOMASS_RESERVE_CTEM] * (1.0 / s->value[STEM_LEAF]);
				Log("Biomass foliage = %g\n", s->value[BIOMASS_FOLIAGE_CTEM]);

				Log("ratio of reserve for foliage = %g% \n", (1.0 / s->value[STEM_LEAF] * 100));

				s->value[BIOMASS_RESERVE_CTEM] -= s->value[BIOMASS_FOLIAGE_CTEM];
				Log("++Reserves pools less foliage = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);


				//not sure if allocate the remaining reserves for stem
				s->value[BIOMASS_STEM_CTEM] += (s->value[BIOMASS_RESERVE_CTEM]-s->value[BIOMASS_FOLIAGE_CTEM]);
				Log("Biomass stem = %g\n", s->value[BIOMASS_STEM_CTEM]);

				s->value[BIOMASS_RESERVE_CTEM] = 0;
				Log("++Reserves pools less foliage + stem = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);


				Log ("++Biomass foliage from reserves for initial LAI = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);
				//Log ("++Biomass stem from reserves for initial LAI = %g \n", s->value[BIOMASS_STEM_CTEM]);
				//Log ("++Biomass stem increment from reserves for initial LAI = %g \n", s->value[BIOMASS_RESERVE_CTEM]-s->value[BIOMASS_FOLIAGE_CTEM]);

				//todo LAI is computed from biomass in DM while SLA is in C!!! probably SLA has to be converted into DM multiplying it per 2
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				// * 1000 to convert reserve biomass from tDM into KgDM

				Log("++Lai from reserves = %g\n", s->value[LAI]);
				//Log("++Canopy Cover = %g\n", s->value[CANOPY_COVER_DBHDC]);
				//Log("++Size Cell = %g\n", settings->sizeCell);
				//Log("++Sla = %g\n", s->value[SLAmkg]);
			}
			else
			{
				Log("++Lai = %g\n", s->value[LAI]);
			}
		}
		else
		{
			Log("\n--GET_INITIAL_DAILY_LAI--\n");

			//todo make a better function that 'share' over a time the biomass_reserve and not only on one day
			if (s->counter[VEG_DAYS]  == 1)
			{
				Log("++Reserves pools = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);
				Log("++Reserve biomass for each tree in g = %g \n", (s->value[BIOMASS_RESERVE_CTEM] * 1000000) / s->counter[N_TREE]);

				//just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
				//the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
				s->value[BIOMASS_FOLIAGE_CTEM] = s->value[BIOMASS_RESERVE_CTEM] * (1.0 / s->value[STEM_LEAF]);
				Log("Biomass foliage = %g\n", s->value[BIOMASS_FOLIAGE_CTEM]);

				Log("ratio of reserve for foliage = %g% \n", (1.0 / s->value[STEM_LEAF] * 100));

				s->value[BIOMASS_RESERVE_CTEM] -= s->value[BIOMASS_FOLIAGE_CTEM];
				Log("++Reserves pools less foliage = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);


				//not sure if allocate the remaining reserves for stem
				s->value[BIOMASS_STEM_CTEM] += (s->value[BIOMASS_RESERVE_CTEM]-s->value[BIOMASS_FOLIAGE_CTEM]);
				Log("Biomass stem = %g\n", s->value[BIOMASS_STEM_CTEM]);

				s->value[BIOMASS_RESERVE_CTEM] = 0;
				Log("++Reserves pools less foliage + stem = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);


				Log ("++Biomass foliage from reserves for initial LAI = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);
				//Log ("++Biomass stem from reserves for initial LAI = %g \n", s->value[BIOMASS_STEM_CTEM]);
				//Log ("++Biomass stem increment from reserves for initial LAI = %g \n", s->value[BIOMASS_RESERVE_CTEM]-s->value[BIOMASS_FOLIAGE_CTEM]);

				//todo LAI is computed from biomass in DM while SLA is in C!!! probably SLA has to be converted into DM multiplying it per 2
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				// * 1000 to convert reserve biomass from tDM into KgDM

				Log("++Lai from reserves = %g\n", s->value[LAI]);
				//Log("++Canopy Cover = %g\n", s->value[CANOPY_COVER_DBHDC]);
				//Log("++Size Cell = %g\n", settings->sizeCell);
				//Log("++Sla = %g\n", s->value[SLAmkg]);
			}
			else
			{
				Log("++Lai = %g\n", s->value[LAI]);
			}
		}
	}
	else
	{
		if (years == 0 && month == 0 && day == 0)
		{
			//Get biomass from reserves
			if (settings->time == 'm')
			{

				Log("\n--GET_INITIAL_MONTH_LAI--\n");

				if (s->counter[VEG_MONTHS]  == 1)
				{
					Log("++Reserves pools = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);
					Log("++Reserve biomass for each tree in g = %g \n", (s->value[BIOMASS_RESERVE_CTEM] * 1000000) / s->counter[N_TREE]);

					//just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
					//the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
					s->value[BIOMASS_FOLIAGE_CTEM] = s->value[BIOMASS_RESERVE_CTEM] * (1.0 / s->value[STEM_LEAF]);
					Log("Biomass foliage = %g\n", s->value[BIOMASS_FOLIAGE_CTEM]);

					Log("ratio of reserve for foliage = %g% \n", (1.0 / s->value[STEM_LEAF] * 100));

					s->value[BIOMASS_RESERVE_CTEM] -= s->value[BIOMASS_FOLIAGE_CTEM];
					Log("++Reserves pools less foliage = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);


					//not sure if allocate the remaining reserves for stem
					s->value[BIOMASS_STEM_CTEM] += (s->value[BIOMASS_RESERVE_CTEM]-s->value[BIOMASS_FOLIAGE_CTEM]);
					Log("Biomass stem = %g\n", s->value[BIOMASS_STEM_CTEM]);

					s->value[BIOMASS_RESERVE_CTEM] = 0;
					Log("++Reserves pools less foliage + stem = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);


					Log ("++Biomass foliage from reserves for initial LAI = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);
					//Log ("++Biomass stem from reserves for initial LAI = %g \n", s->value[BIOMASS_STEM_CTEM]);
					//Log ("++Biomass stem increment from reserves for initial LAI = %g \n", s->value[BIOMASS_RESERVE_CTEM]-s->value[BIOMASS_FOLIAGE_CTEM]);

					//todo LAI is computed from biomass in DM while SLA is in C!!! probably SLA has to be converted into DM multiplying it per 2
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
					// * 1000 to convert reserve biomass from tDM into KgDM

					Log("++Lai from reserves = %g\n", s->value[LAI]);
					//Log("++Canopy Cover = %g\n", s->value[CANOPY_COVER_DBHDC]);
					//Log("++Size Cell = %g\n", settings->sizeCell);
					//Log("++Sla = %g\n", s->value[SLAmkg]);
				}
				else
				{
					Log("++Lai = %g\n", s->value[LAI]);
				}
			}
			else
			{
				Log("\n--GET_INITIAL_DAILY_LAI--\n");

				//todo make a better function that 'share' over a time the biomass_reserve and not only on one day
				if (s->counter[VEG_DAYS]  == 1)
				{
					Log("++Reserves pools = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);
					Log("++Reserve biomass for each tree in g = %g \n", (s->value[BIOMASS_RESERVE_CTEM] * 1000000) / s->counter[N_TREE]);

					//just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
					//the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
					s->value[BIOMASS_FOLIAGE_CTEM] = s->value[BIOMASS_RESERVE_CTEM] * (1.0 / s->value[STEM_LEAF]);
					Log("Biomass foliage = %g\n", s->value[BIOMASS_FOLIAGE_CTEM]);

					Log("ratio of reserve for foliage = %g% \n", (1.0 / s->value[STEM_LEAF] * 100));

					s->value[BIOMASS_RESERVE_CTEM] -= s->value[BIOMASS_FOLIAGE_CTEM];
					Log("++Reserves pools less foliage = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);


					//not sure if allocate the remaining reserves for stem
					s->value[BIOMASS_STEM_CTEM] += (s->value[BIOMASS_RESERVE_CTEM]-s->value[BIOMASS_FOLIAGE_CTEM]);
					Log("Biomass stem = %g\n", s->value[BIOMASS_STEM_CTEM]);

					s->value[BIOMASS_RESERVE_CTEM] = 0;
					Log("++Reserves pools less foliage + stem = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);


					Log ("++Biomass foliage from reserves for initial LAI = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);
					//Log ("++Biomass stem from reserves for initial LAI = %g \n", s->value[BIOMASS_STEM_CTEM]);
					//Log ("++Biomass stem increment from reserves for initial LAI = %g \n", s->value[BIOMASS_RESERVE_CTEM]-s->value[BIOMASS_FOLIAGE_CTEM]);

					//todo LAI is computed from biomass in DM while SLA is in C!!! probably SLA has to be converted into DM multiplying it per 2
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
					// * 1000 to convert reserve biomass from tDM into KgDM

					Log("++Lai from reserves = %g\n", s->value[LAI]);
					//Log("++Canopy Cover = %g\n", s->value[CANOPY_COVER_DBHDC]);
					//Log("++Size Cell = %g\n", settings->sizeCell);
					//Log("++Sla = %g\n", s->value[SLAmkg]);
				}
				else
				{
					Log("++Lai = %g\n", s->value[LAI]);
				}
			}
		}
		else
		{
			Log("\n--GET_INITIAL_DAILY_LAI FROM PREVIOUS YEAR YEARS OF SIM %d--\n", years);
			Log("++Lai = %g\n", s->value[LAI]);
		}
	}


}


