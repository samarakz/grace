/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 2001 Grace Development Team
 * 
 * Maintained by Evgeny Stambulchik
 * 
 * 
 *                           All Rights Reserved
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * Misc stuff using dictionaries
 */

#include <config.h>

#include "grace.h"
#include "defines.h"
#include "core_utils.h"
#include "dicts.h"
#include "xstrings.h"

int grace_rt_init_dicts(RunTime *rt)
{
    /* Dictionary tables */
    const DictEntry graph_type_defaults =
        {GRAPH_BAD, "unknown", "Unknown"};
    const DictEntry graph_type_entries[] = 
        {
            {GRAPH_XY,    "xy",    "XY"   },
            {GRAPH_CHART, "chart", "Chart"},
            {GRAPH_POLAR, "polar", "Polar"},
            {GRAPH_SMITH, "smith", "Smith"},
            {GRAPH_FIXED, "fixed", "Fixed"},
            {GRAPH_PIE,   "pie",   "Pie"  }
        };
    
    const DictEntry set_type_defaults =
        {SET_BAD, "unknown", "Unknown"};
    const DictEntry set_type_entries[] = 
        {
            {SET_XY,         "xy",         "XY"        },
            {SET_XYDX,       "xydx",       "XYdX"      },
            {SET_XYDY,       "xydy",       "XYdY"      },
            {SET_XYDXDX,     "xydxdx",     "XYdXdX"    },
            {SET_XYDYDY,     "xydydy",     "XYdYdY"    },
            {SET_XYDXDY,     "xydxdy",     "XYdXdY"    },
            {SET_XYDXDXDYDY, "xydxdxdydy", "XYdXdXdYdY"},
            {SET_BAR,        "bar",        "Bar"       },
            {SET_BARDY,      "bardy",      "BardY"     },
            {SET_BARDYDY,    "bardydy",    "BardYdY"   },
            {SET_XYHILO,     "xyhilo",     "XYHiLo"    },
            {SET_XYZ,        "xyz",        "XYZ"       },
            {SET_XYR,        "xyr",        "XYR"       },
            {SET_XYSIZE,     "xysize",     "XYSize"    },
            {SET_XYCOLOR,    "xycolor",    "XYColor"   },
            {SET_XYCOLPAT,   "xycolpat",   "XYColPat"  },
            {SET_XYVMAP,     "xyvmap",     "XYVMap"    },
            {SET_BOXPLOT,    "boxplot",    "BoxPlot"   }
        };

    const DictEntry object_type_defaults =
        {DO_NONE, VStrNone, "Unknown"};
    const DictEntry object_type_entries[] = 
        {
            {DO_LINE, VStrLine, "Line"},
            {DO_BOX,  VStrBox,  "Box" },
            {DO_ARC,  VStrArc,  "Arc" }
        };

    const DictEntry inout_placement_defaults =
        {TICKS_IN, VStrIn, "In"};
    const DictEntry inout_placement_entries[] = 
        {
            {TICKS_IN,   VStrIn,   "In"  },
            {TICKS_OUT,  VStrOut,  "Out" },
            {TICKS_BOTH, VStrBoth, "Both"}
        };

    const DictEntry side_placement_defaults =
        {PLACEMENT_NORMAL, VStrNormal, "Normal"};
    const DictEntry side_placement_entries[] = 
        {
            {PLACEMENT_NORMAL,   VStrNormal,   "Normal"  },
            {PLACEMENT_OPPOSITE, VStrOpposite, "Opposite"},
            {PLACEMENT_BOTH,     VStrBoth,     "Both"    }
        };

    const DictEntry axis_position_defaults =
        {AXIS_POS_NORMAL, VStrNormal, "Normal"};
    const DictEntry axis_position_entries[] = 
        {
            {AXIS_POS_NORMAL,   VStrNormal,   "Normal"  },
            {AXIS_POS_OPPOSITE, VStrOpposite, "Opposite"},
            {AXIS_POS_ZERO,     VStrZero,     "Zero"    }
        };

    const DictEntry spec_ticks_defaults =
        {TICKS_SPEC_NONE, VStrNone, "None"};
    const DictEntry spec_ticks_entries[] = 
        {
            {TICKS_SPEC_NONE,  VStrNone,  "None"                 },
            {TICKS_SPEC_MARKS, VStrTicks, "Tick marks"           },
            {TICKS_SPEC_BOTH,  VStrBoth,  "Tick marks and labels"}
        };

    const DictEntry region_type_defaults =
        {REGION_POLYGON, VStrPolygon, "Polygon"};
    const DictEntry region_type_entries[] = 
        {
            {REGION_POLYGON, VStrPolygon, "Polygon"},
            {REGION_BAND,    VStrBand,    "Band"   },
            {REGION_FORMULA, VStrFormula, "Formula"}
        };

    if (!(rt->graph_type_dict =
        DICT_NEW_STATIC(graph_type_entries, &graph_type_defaults))) {
        return RETURN_FAILURE;
    }
    if (!(rt->set_type_dict =
        DICT_NEW_STATIC(set_type_entries, &set_type_defaults))) {
        return RETURN_FAILURE;
    }
    if (!(rt->object_type_dict =
        DICT_NEW_STATIC(object_type_entries, &object_type_defaults))) {
        return RETURN_FAILURE;
    }
    if (!(rt->inout_placement_dict =
        DICT_NEW_STATIC(inout_placement_entries, &inout_placement_defaults))) {
        return RETURN_FAILURE;
    }
    if (!(rt->side_placement_dict =
        DICT_NEW_STATIC(side_placement_entries, &side_placement_defaults))) {
        return RETURN_FAILURE;
    }
    if (!(rt->axis_position_dict =
        DICT_NEW_STATIC(axis_position_entries, &axis_position_defaults))) {
        return RETURN_FAILURE;
    }
    if (!(rt->spec_ticks_dict =
        DICT_NEW_STATIC(spec_ticks_entries, &spec_ticks_defaults))) {
        return RETURN_FAILURE;
    }
    if (!(rt->region_type_dict =
        DICT_NEW_STATIC(region_type_entries, &region_type_defaults))) {
        return RETURN_FAILURE;
    }
    
    return RETURN_SUCCESS;
}

void grace_rt_free_dicts(RunTime *rt)
{
    dict_free(rt->graph_type_dict);
    dict_free(rt->set_type_dict);
    dict_free(rt->object_type_dict);
    dict_free(rt->inout_placement_dict);
    dict_free(rt->side_placement_dict);
    dict_free(rt->axis_position_dict);
    dict_free(rt->spec_ticks_dict);
    dict_free(rt->region_type_dict);
}

char *graph_types(RunTime *rt, GraphType it)
{
    char *s;
    
    dict_get_name_by_key(rt->graph_type_dict, it, &s);
    
    return s;
}

char *graph_type_descr(RunTime *rt, GraphType it)
{
    char *s;
    
    dict_get_descr_by_key(rt->graph_type_dict, it, &s);
    
    return s;
}

GraphType graph_get_type_by_name(RunTime *rt, const char *name)
{
    int retval;
    
    dict_get_key_by_name(rt->graph_type_dict, name, &retval);
    
    return retval;
}


char *set_types(RunTime *rt, SetType it)
{
    char *s;
    
    dict_get_name_by_key(rt->set_type_dict, it, &s);
    
    return s;
}

char *set_type_descr(RunTime *rt, SetType it)
{
    char *s;
    
    dict_get_descr_by_key(rt->set_type_dict, it, &s);
    
    return s;
}

SetType get_settype_by_name(RunTime *rt, const char *name)
{
    int retval;
    
    dict_get_key_by_name(rt->set_type_dict, name, &retval);
    
    return retval;
}

char *object_types(RunTime *rt, OType it)
{
    char *s;
    
    dict_get_name_by_key(rt->object_type_dict, it, &s);
    
    return s;
}

char *object_type_descr(RunTime *rt, OType it)
{
    char *s;
    
    dict_get_descr_by_key(rt->object_type_dict, it, &s);
    
    return s;
}

OType get_objecttype_by_name(RunTime *rt, const char *name)
{
    int retval;
    
    dict_get_key_by_name(rt->object_type_dict, name, &retval);
    
    return retval;
}

char *inout_placement_name(RunTime *rt, int inout)
{
    char *s;
    
    dict_get_name_by_key(rt->inout_placement_dict, inout, &s);
    
    return s;
}

int get_inout_placement_by_name(RunTime *rt, const char *name)
{
    int retval;
    
    dict_get_key_by_name(rt->inout_placement_dict, name, &retval);
    
    return retval;
}

char *side_placement_name(RunTime *rt, int it)
{
    char *s;
    
    dict_get_name_by_key(rt->side_placement_dict, it, &s);
    
    return s;
}

int get_side_placement_by_name(RunTime *rt, const char *name)
{
    int retval;
    
    dict_get_key_by_name(rt->side_placement_dict, name, &retval);
    
    return retval;
}

char *spec_tick_name(RunTime *rt, int it)
{
    char *s;
    
    dict_get_name_by_key(rt->spec_ticks_dict, it, &s);
    
    return s;
}

int get_spec_tick_by_name(RunTime *rt, const char *name)
{
    int retval;
    
    dict_get_key_by_name(rt->spec_ticks_dict, name, &retval);
    
    return retval;
}


char *region_types(RunTime *rt, int it)
{
    char *s;
    
    dict_get_name_by_key(rt->region_type_dict, it, &s);
    
    return s;
}

int get_regiontype_by_name(RunTime *rt, const char *name)
{
    int retval;
    
    dict_get_key_by_name(rt->region_type_dict, name, &retval);
    
    return retval;
}

char *axis_position_name(RunTime *rt, int it)
{
    char *s;
    
    dict_get_name_by_key(rt->axis_position_dict, it, &s);
    
    return s;
}

int get_axis_position_by_name(RunTime *rt, const char *name)
{
    int retval;
    
    dict_get_key_by_name(rt->axis_position_dict, name, &retval);
    
    return retval;
}

