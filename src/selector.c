/*
 * Copyright (C) 2012-2013 Red Hat, Inc.
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

// libsolv
#include <solv/solver.h>
#include <solv/util.h>

// hawkey
#include "errno.h"
#include "goal_internal.h"
#include "package_internal.h"
#include "packagelist.h"
#include "query_internal.h"
#include "sack_internal.h"
#include "selector_internal.h"

static void
replace_filter(struct _Filter **fp, int keyname, int cmp_type, const char *match)
{
    if (*fp == NULL)
	*fp = filter_create(1);
    else
	filter_reinit(*fp, 1);

    struct _Filter *f = *fp;

    f->keyname = keyname;
    f->match_type = _HY_STR;
    f->cmp_type = cmp_type;
    f->matches[0].str = solv_strdup(match);
}

static int
valid_setting(int keyname, int cmp_type)
{
    switch (keyname) {
    case HY_PKG_ARCH:
    case HY_PKG_EVR:
    case HY_PKG_VERSION:
    case HY_PKG_PROVIDES:
	return cmp_type == HY_EQ;
    case HY_PKG_NAME:
	return (cmp_type == HY_EQ || cmp_type == HY_GLOB);
    default:
	return 0;
    }
}

HySelector
hy_selector_create(HySack sack)
{
    HySelector sltr = solv_calloc(1, sizeof(*sltr));
    sltr->sack = sack;
    return sltr;
}

void
hy_selector_free(HySelector sltr)
{
    filter_free(sltr->f_arch);
    filter_free(sltr->f_evr);
    filter_free(sltr->f_name);
    filter_free(sltr->f_provides);
    solv_free(sltr);
}

int
hy_selector_set(HySelector sltr, int keyname, int cmp_type, const char *match)
{
    if (!valid_setting(keyname, cmp_type))
	return HY_E_SELECTOR;

    switch (keyname) {
    case HY_PKG_ARCH:
	replace_filter(&sltr->f_arch, keyname, cmp_type, match);
	break;
    case HY_PKG_EVR:
    case HY_PKG_VERSION:
	replace_filter(&sltr->f_evr, keyname, cmp_type, match);
	break;
    case HY_PKG_NAME:
	if (sltr->f_provides)
	    return HY_E_SELECTOR;
	replace_filter(&sltr->f_name, keyname, cmp_type, match);
	break;
    case HY_PKG_PROVIDES:
	if (sltr->f_name)
	    return HY_E_SELECTOR;
	replace_filter(&sltr->f_provides, keyname, cmp_type, match);
	break;
    default:
	return HY_E_SELECTOR;
    }
    return 0;
}

HyPackageList
hy_selector_matches(HySelector sltr)
{
    HySack sack = selector_sack(sltr);
    Pool *pool = sack_pool(sack);
    Queue job;

    queue_init(&job);
    sltr2job(sltr, &job, 0);
    Id p, pp;
    HyPackageList plist = hy_packagelist_create();

    for (int i = 0; i < job.count; i += 2)
	FOR_JOB_SELECT(p, pp, job.elements[0], job.elements[1])
	    hy_packagelist_push(plist, package_create(sack, p));
    queue_free(&job);
    return plist;
}
