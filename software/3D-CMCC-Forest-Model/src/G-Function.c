//
#include <math.h>
#include "types.h"

extern void Set_tree_period (SPECIES *s, AGE *a, CELL *c)
{
	/*Set Tree period*/
	// 0 = adult tree
	// 1 = young tree

	if ( a->value > s->value[ADULT_AGE])
	{
		s->period = 0;

		if (!c->saplings_counter)
		{
			c->saplings_counter -= 1;
		}
	}
	else
	{
		s->period = 1;
		//Log("- Class Period = Saplings\n");
	}
}





extern void Get_Veg_Months (MATRIX *const m, const YOS *const yos, const int month, const int years)
{
	MET_DATA *met;
	static int cell;
	static int height;
	static int age;
	static int species;


	met = (MET_DATA*) yos[years].m;


	for ( cell = 0; cell < m->cells_count; cell++)
	{
		for ( height = m->cells[cell].heights_count - 1; height >= 0; height-- )
		{
			for ( age = m->cells[cell].heights[height].ages_count - 1 ; age >= 0 ; age-- )
			{
				for (species = 0; species < m->cells[cell].heights[height].ages[age].species_count; species++)
				{
					if (settings->version == 'u')
					{
						if (!month)
						{
							m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] = 0;
						}
						if (m->cells[cell].heights[height].ages[age].species[species].phenology == D)
						{
							//todo decidere se utlizzare growthend o mindaylenght
							if ((met[month].tav >= m->cells[cell].heights[height].ages[age].species[species].value[GROWTHSTART] && month < 6)
									|| (met[month].tav >= m->cells[cell].heights[height].ages[age].species[species].value[MINDAYLENGTH] && month >= 6))
							{
								m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] += 1;
								//Log("MONTHs = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
								//Log("----- Vegetative month = %d for species %s\n", month + 1, m->cells[cell].heights[height].ages[age].species[species].name );
							}
						}
						else
						{
							m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] = 12;
						}
						if (month == 11)
						{
							Log("----- TOTAL VEGETATIVE MONTHS for species %s = %d \n\n", m->cells[cell].heights[height].ages[age].species[species].name, m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
						}
					}
					else
					{
						if (!month)
						{
							m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] = 0;
						}
						if (m->cells[cell].heights[height].ages[age].species[species].phenology == 0)
						{
							if (met[month].ndvi_lai >= 0.5)
							{
								m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] += 1;
								//Log("MONTHs = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
								//Log("----- Vegetative month = %d \n", month + 1);
							}
						}
						else
						{
							m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] = 12;
						}
						if (month == 11)
						{
							Log("----- TOTAL VEGETATIVE MONTHS = %d \n\n", m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
						}
					}
				}
			}
		}
	}
}



//----------------------------------------------------------------------------//
//                                                                            //
//                             GetDayLength                                   //
//                                                                            //
//----------------------------------------------------------------------------//



/*

   3PG version

   float GetDayLength (float latitude, int MonthLength)
   {
// gets fraction of day when sun is "up"
float sLat, cLat, sinDec, cosH0;

sLat = sin(Pi * latitude / 180);
cLat = cos(Pi * latitude / 180);

sinDec = 0.4 * sin(0.0172 * (dayOfYear - 80));
cosH0 = sinDec * sLat / (cLat * sqrt(1 - pow(sinDec,2)));
if (cosH0 > 1)
return 0;
else if (cosH0 < -1)
return 1;
else {
return acos(cosH0) / Pi;
}
 */


//BIOME-BGC version
//Running-Coughlan 1998, Ecological Modelling

void GetDayLength ( CELL * c,  int MonthLength)
{
	//int cell;
	//Log("MonthLenght = %d \n", MonthLength);

	float ampl;  //seasonal variation in Day Length from 12 h
	ampl = (exp (7.42 + (0.045 * site->lat))) / 3600;


	c->daylength = ampl * (sin ((MonthLength - 79) * 0.01721)) + 12;


	Log("daylength = %g \n", c->daylength);
}


void Get_Abscission_DayLength ( CELL * c)
{
	//from Schwalm and Ek, 2004
	c->abscission_daylength = (39132 + (pow (1.088, (site->lat + 60.753))))/(60*60);
	Log("Abscission day length = %g \n", c->abscission_daylength);

}

extern void Print_init_month_stand_data (CELL *c, const MET_DATA *const met, const int month, const int years, int height, int age, int species)
{
	Log("******************************************************\n\n");
	Log("cell = \n");
	Log("- x = %g\n", c->x);
	Log("- y = %g\n", c->y);
	Log("- z = %d\n", c->heights[height].z);
	Log("- Class Age = %d years \n", c->heights[height].ages[age].value);
	Log("- Species = %s\n", c->heights[height].ages[age].species[species].name);
	Log("- Height = %g m\n", c->heights[height].value);
	Log("- Number of trees = %d trees \n", c->heights[height].ages[age].species[species].counter[N_TREE]);

	if ( c->heights[height].ages[age].species[species].phenology == D)
	{
		Log("- Vegetative Months =  %d months\n", c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
	}
	else
	{
		Log("- Vegetative Months = %d months\n", c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
	}

	if (settings->version == 's')
	{
		Log("- Monthly LAI from NDVI = %g \n",c->heights[height].z, met[month].ndvi_lai);
	}
	else
	{
		Log("- Monthly LAI from Model= %g \n",c->heights[height].z, c->heights[height].ages[age].species[species].value[LAI]);
	}
	Log("- ASW layer %d month %d  = %g mm\n",  c->heights[height].z, month + 1, c->available_soil_water);

	if (!month)
	{
		/*Phenology*/
		if ( c->heights[height].ages[age].species[species].phenology == D)
		{
			Log("- Phenology = DECIDUOUS\n");
		}
		else
		{
			Log("- Phenology = EVERGREEN\n");
		}


		/*Management*/
		if ( c->heights[height].ages[age].species[species].management == T)
		{
			Log("- Management type = TIMBER\n");
		}
		else
		{
			Log("- Management type = COPPICE\n");
		}
		//Log("+ Lai = %g\n", c->heights[height].ages[age].species[species].value[LAI]);
		Log("+ AvDBH = %g cm\n",  c->heights[height].ages[age].species[species].value[AVDBH]);
		if (settings->version == 'u')
		{
			Log("+ Wf = %g tDM/ha\n", c->heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM]);
		}
		Log("+ Ws = %g tDM/ha\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM]);
		Log("+ Wrc = %g tDM/ha\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]);
		Log("+ Wrf = %g tDM/ha\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM]);
		Log("+ Wr Tot = %g tDM/ha\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_CTEM]);
		Log("+ Wres = %g tDM/ha\n", c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM]);
	}

}

extern void Print_end_month_stand_data (CELL *c, const YOS *const yos, const MET_DATA *const met, const int month, const int years, int height, int age, int species)
{
	Log("> x = %g\n", c->x);
	Log("> y = %g\n", c->y);
	Log("> z = %d\n", c->heights[height].z);
	Log("> height = %g\n", c->heights[height].value);
	Log("> age = %d\n", c->heights[height].ages[age].value);
	Log("> species = %s\n", c->heights[height].ages[age].species[species].name);
	Log("> phenology = %d\n", c->heights[height].ages[age].species[species].phenology);
	Log("> management = %d\n", c->heights[height].ages[age].species[species].management);
	//Log("[%d] PEAK Y LAI IN THIS YEAR LAYER %d = %g\n",yos[years].year, m->cells[cell].heights[height].z,  m->cells[cell].heights[height].ages[age].species[species].value[PEAK_Y_LAI]);
	Log("[%d] layer %d n tree = %d\n", yos[years].year,  c->heights[height].z, c->heights[height].ages[age].species[species].counter[N_TREE]);
	Log("[%d] layer %d > avdbh = %g\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[AVDBH]);
	Log("[%d] layer %d > wf = %g\n",yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM]);
	Log("[%d] layer %d > wr coarse = %g\n",yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]);
	Log("[%d] layer %d > wr fine = %g\n",yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM]);
	Log("[%d] layer %d > wr Tot = %g\n",yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM] + c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM] );
	Log("[%d] layer %d > ws = %g\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM]);
	Log("[%d] layer %d > wres = %g tDM/ha\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM]);
	Log("[%d] layer %d > wres tree = %g gC/trees\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM] * 1000000 / c->heights[height].ages[age].species[species].counter[N_TREE]);
	Log("[%d] layer %d > Dead Trees = %d\n",yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].counter[DEL_STEMS]);
	Log("> New Saplings = %d\n", c->heights[height].ages[age].species[species].counter[N_TREE_SAP]);
	Log("*****************************\n");
}
