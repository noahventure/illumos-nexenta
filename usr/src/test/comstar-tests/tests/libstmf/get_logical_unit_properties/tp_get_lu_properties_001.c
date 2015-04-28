/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 *
 * Copyright 2015 Nexenta Systems, Inc.  All rights reserved.
 */

#include <libstmftest.h>

static int
stmfGetLogicalUnitProperties001()
{
	int ret = 0;
	int i;
	boolean_t found = B_FALSE;
	stmfGuidList *luList;
	stmfLogicalUnitProperties luProps;
	char sbdadmDeleteLu[MAXPATHLEN];
	char guidAsciiBuf[33];
	int stmfRet;

	(void) system("touch /tmp/stmfGetLogicalUnitProperties.lu");
	(void) system("sbdadm create-lu -s 10g"
	    " /tmp/stmfGetLogicalUnitProperties.lu");
	stmfRet = stmfGetLogicalUnitList(&luList);
	if (stmfRet != STMF_STATUS_SUCCESS) {
		ret = 1;
		goto cleanup;
	}

	for (i = 0; i < luList->cnt; i++) {
		stmfRet = stmfGetLogicalUnitProperties(&(luList->guid[i]),
		    &luProps);
		if (strncmp(luProps.alias,
		    "/tmp/stmfGetLogicalUnitProperties.lu",
		    sizeof (luProps.alias)) == 0) {
			found = B_TRUE;
			break;
		}
	}
	if (!found) {
		ret = 2;
		goto cleanup;
	}

	if (strcmp(luProps.providerName, "sbd") != 0) {
		ret = 3;
		goto cleanup;
	}

	if (luProps.status != STMF_LOGICAL_UNIT_ONLINE) {
		ret = 4;
		goto cleanup;
	}

	guidToAscii(&luList->guid[i], guidAsciiBuf);

	(void) snprintf(sbdadmDeleteLu, sizeof (sbdadmDeleteLu), "%s %s",
	    "sbdadm delete-lu", guidAsciiBuf);

cleanup:
	(void) system(sbdadmDeleteLu);
	(void) system("rm /tmp/stmfGetLogicalUnitProperties.lu");
	return (ret);
}

int
main()
{
	return (stmfGetLogicalUnitProperties001());
}
