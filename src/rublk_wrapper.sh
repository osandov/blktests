#!/bin/bash
# SPDX-License-Identifier: GPL-3.0+
# Copyright (C) 2023 Ming Lei <ming.lei@redhat.com>
#
# rublk wrapper for adapting miniublk's command line

PARA=()
ACT=$1
for arg in "$@"; do
	if [ "$arg" = "-t" ]; then
		continue
	fi

	if [ "$ACT" = "recover" ]; then
		if [ "$arg" = "loop" ] || [ "$arg" = "null" ]; then
			continue;
		fi

		if [ -f "$arg" ]; then
			continue
		fi

		if [ "$arg" = "-f" ]; then
			continue
		fi
		PARA+=("$arg")
	else
		PARA+=("$arg")
	fi
done
rublk "${PARA[@]}"
