/*
 * Copyright (C) 2001, 2004, 2007, 2009, 2014-2017  Internet Systems Consortium, Inc. ("ISC")
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/* $Id: ntpaths.c,v 1.15 2009/07/14 22:54:57 each Exp $ */

/*
 * This module fetches the required path information that is specific
 * to NT systems which can have its configuration and system files
 * almost anywhere. It can be used to override whatever the application
 * had previously assigned to the pointer. Basic information about the
 * file locations are stored in the registry.
 */

#include <config.h>

#include <isc/bind_registry.h>
#include <isc/ntpaths.h>
#include <isc/string.h>

/*
 * Module Variables
 */

static char systemDir[MAX_PATH];
static char namedBase[MAX_PATH];
static char ns_confFile[MAX_PATH];
static char rndc_confFile[MAX_PATH];
static char ns_defaultpidfile[MAX_PATH];
static char ns_lockfile[MAX_PATH];
static char local_state_dir[MAX_PATH];
static char sys_conf_dir[MAX_PATH];
static char rndc_keyFile[MAX_PATH];
static char session_keyFile[MAX_PATH];
static char resolv_confFile[MAX_PATH];

static DWORD baseLen = MAX_PATH;
static BOOL Initialized = FALSE;

void
isc_ntpaths_init(void) {
	HKEY hKey;
	BOOL keyFound = TRUE;

	memset(namedBase, 0, MAX_PATH);
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, BIND_SUBKEY, 0, KEY_READ, &hKey)
		!= ERROR_SUCCESS)
		keyFound = FALSE;

	if (keyFound == TRUE) {
		/* Get the named directory */
		if (RegQueryValueEx(hKey, "InstallDir", NULL, NULL,
			(LPBYTE)namedBase, &baseLen) != ERROR_SUCCESS)
			keyFound = FALSE;
		RegCloseKey(hKey);
	}

	GetSystemDirectory(systemDir, MAX_PATH);

	if (keyFound == FALSE)
		/* Use the System Directory as a default */
		strcpy(namedBase, systemDir);

	strcpy(ns_confFile, namedBase);
	strcat(ns_confFile, "\\etc\\named.conf");

	strcpy(rndc_keyFile, namedBase);
	strcat(rndc_keyFile, "\\etc\\rndc.key");

	strcpy(session_keyFile, namedBase);
	strcat(session_keyFile, "\\etc\\session.key");

	strcpy(rndc_confFile, namedBase);
	strcat(rndc_confFile, "\\etc\\rndc.conf");

	strcpy(ns_defaultpidfile, namedBase);
	strcat(ns_defaultpidfile, "\\etc\\named.pid");

	strcpy(ns_lockfile, namedBase);
	strcat(ns_lockfile, "\\etc\\named.lock");

	strcpy(local_state_dir, namedBase);
	strcat(local_state_dir, "\\bin");

	strcpy(sys_conf_dir, namedBase);
	strcat(sys_conf_dir, "\\etc");

	/* Added to avoid an assert on NULL value */
	strlcpy(resolv_confFile, namedBase, sizeof(resolv_confFile));
	strlcat(resolv_confFile, "\\etc\\resolv.conf",
		sizeof(resolv_confFile));

	Initialized = TRUE;
}

char *
isc_ntpaths_get(int ind) {
	if (!Initialized)
		isc_ntpaths_init();

	switch (ind) {
	case NAMED_CONF_PATH:
		return (ns_confFile);
		break;
	case RESOLV_CONF_PATH:
		return (resolv_confFile);
		break;
	case RNDC_CONF_PATH:
		return (rndc_confFile);
		break;
	case NAMED_PID_PATH:
		return (ns_defaultpidfile);
		break;
	case NAMED_LOCK_PATH:
		return (ns_lockfile);
		break;
	case LOCAL_STATE_DIR:
		return (local_state_dir);
		break;
	case SYS_CONF_DIR:
		return (sys_conf_dir);
		break;
	case RNDC_KEY_PATH:
		return (rndc_keyFile);
		break;
	case SESSION_KEY_PATH:
		return (session_keyFile);
		break;
	default:
		return (NULL);
	}
}
