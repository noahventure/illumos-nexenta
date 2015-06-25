#!/usr/bin/ksh -p
#
# CDDL HEADER START
#
# The contents of this file are subject to the terms of the
# Common Development and Distribution License (the "License").
# You may not use this file except in compliance with the License.
#
# You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
# or http://www.opensolaris.org/os/licensing.
# See the License for the specific language governing permissions
# and limitations under the License.
#
# When distributing Covered Code, include this CDDL HEADER in each
# file and include the License file at usr/src/OPENSOLARIS.LICENSE.
# If applicable, add the following below this CDDL HEADER, with the
# fields enclosed by brackets "[]" replaced with your own identifying
# information: Portions Copyright [yyyy] [name of copyright owner]
#
# CDDL HEADER END
#

#
# Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#

#
# Copyright (c) 2013, 2014 by Delphix. All rights reserved.
# Copyright 2015 Nexenta Systems, Inc. All rights reserved.
#

. $STF_SUITE/include/libtest.shlib

#
# DESCRIPTION:
# Offlining disks in a non-redundant pool should fail.
#
# STRATEGY:
# 1. Create a multidisk stripe and start some random I/O
# 2. zpool offline should fail on each disk.
#

verify_runnable "global"

function cleanup
{
	if poolexists $TESTPOOL1; then
		destroy_pool $TESTPOOL1
	fi

	$KILL $killpid >/dev/null 2>&1
	[[ -e $TESTDIR1 ]] && log_must $RM -rf $TESTDIR1
	[[ -e $TESTDIR ]] && log_must $RM -rf $TESTDIR/*
}

log_assert "Offlining disks in a non-redundant pool should fail."

log_onexit cleanup

specials_list=""
for i in 0 1 2; do
	$MKFILE 64m $TESTDIR/$TESTFILE1.$i
	specials_list="$specials_list $TESTDIR/$TESTFILE1.$i"
done
disk=($specials_list)

create_pool $TESTPOOL1 $specials_list
log_must $ZFS create $TESTPOOL1/$TESTFS1
log_must $ZFS set mountpoint=$TESTDIR1 $TESTPOOL1/$TESTFS1

$FILE_TRUNC -f $((64 * 1024 * 1024)) -b 8192 -c 0 -r $TESTDIR/$TESTFILE1 &
typeset killpid="$! "

for i in 0 1 2; do
	log_mustnot $ZPOOL offline $TESTPOOL1 ${disk[$i]}
	check_state $TESTPOOL1 ${disk[$i]} "online"
done

log_must $KILL $killpid
$SYNC

log_pass
