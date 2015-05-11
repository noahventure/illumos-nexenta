/*
 * Copyright 2015 Nexenta Systems, Inc. All rights reserved.
 */

#include <sys/uuid.h>
#include <sys/debug.h>
#include <string.h>

#include "krrp.h"
#include "libkrrp.h"
#include "libkrrp_impl.h"

static int
krrp_sess_create_common(libkrrp_handle_t *hdl, uuid_t sess_id,
    const char *sess_kstat_id, const char *auth_digest, boolean_t fake_mode,
    nvlist_t *params)
{
	int rc;
	krrp_sess_id_str_t sess_id_str;

	VERIFY(hdl != NULL);
	VERIFY(sess_kstat_id != NULL);

	libkrrp_reset(hdl);

	uuid_unparse(sess_id, sess_id_str);

	(void) krrp_param_put(KRRP_PARAM_SESS_ID, params, sess_id_str);

	(void) krrp_param_put(KRRP_PARAM_SESS_KSTAT_ID, params,
	    (void *)sess_kstat_id);

	if (auth_digest != NULL) {
		(void) krrp_param_put(KRRP_PARAM_AUTH_DATA, params,
		    (void *)auth_digest);
	}

	if (fake_mode)
		(void) krrp_param_put(KRRP_PARAM_FAKE_MODE, params, NULL);

	rc = krrp_ioctl_perform(hdl, KRRP_IOCTL_SESS_CREATE, params, NULL);

	return (rc);
}

int
krrp_sess_create_sender(libkrrp_handle_t *hdl, uuid_t sess_id,
    const char *sess_kstat_id, const char *auth_digest, boolean_t fake_mode)
{
	nvlist_t *params = NULL;
	int rc;

	params = fnvlist_alloc();

	(void) krrp_param_put(KRRP_PARAM_SESS_SENDER, params, NULL);

	rc = krrp_sess_create_common(hdl, sess_id, sess_kstat_id, auth_digest,
	    fake_mode, params);

	fnvlist_free(params);
	return (rc);
}

int
krrp_sess_create_receiver(libkrrp_handle_t *hdl, uuid_t sess_id,
    const char *sess_kstat_id, const char *auth_digest, boolean_t fake_mode)
{
	nvlist_t *params = NULL;
	int rc;

	params = fnvlist_alloc();

	rc = krrp_sess_create_common(hdl, sess_id, sess_kstat_id, auth_digest,
	    fake_mode, params);

	fnvlist_free(params);
	return (rc);
}

int
krrp_sess_create_compound(libkrrp_handle_t *hdl, uuid_t sess_id,
    const char *sess_kstat_id, boolean_t fake_mode)
{
	nvlist_t *params = NULL;
	int rc;

	params = fnvlist_alloc();

	(void) krrp_param_put(KRRP_PARAM_SESS_COMPOUND, params, NULL);

	rc = krrp_sess_create_common(hdl, sess_id, sess_kstat_id, NULL,
	    fake_mode, params);

	fnvlist_free(params);
	return (rc);
}

int
krrp_sess_destroy(libkrrp_handle_t *hdl, uuid_t sess_id)
{
	nvlist_t *params = NULL;
	int rc;
	krrp_sess_id_str_t sess_id_str;

	VERIFY(hdl != NULL);

	libkrrp_reset(hdl);

	uuid_unparse(sess_id, sess_id_str);
	params = fnvlist_alloc();
	(void) krrp_param_put(KRRP_PARAM_SESS_ID, params, sess_id_str);

	rc = krrp_ioctl_perform(hdl, KRRP_IOCTL_SESS_DESTROY, params, NULL);

	fnvlist_free(params);
	return (rc);
}

int
krrp_sess_create_conn(libkrrp_handle_t *hdl, uuid_t sess_id,
    const char *address, const uint16_t port, const uint32_t conn_timeout)
{
	nvlist_t *params = NULL;
	int rc;
	krrp_sess_id_str_t sess_id_str;

	VERIFY(hdl != NULL);
	VERIFY(address != NULL);

	libkrrp_reset(hdl);

	uuid_unparse(sess_id, sess_id_str);
	params = fnvlist_alloc();

	(void) krrp_param_put(KRRP_PARAM_SESS_ID, params, sess_id_str);
	(void) krrp_param_put(KRRP_PARAM_REMOTE_HOST, params, (void *)address);
	(void) krrp_param_put(KRRP_PARAM_PORT, params, (void *)&port);

	if (conn_timeout != 0) {
		(void) krrp_param_put(KRRP_PARAM_CONN_TIMEOUT, params,
		    (void *)&conn_timeout);
	}

	rc = krrp_ioctl_perform(hdl, KRRP_IOCTL_SESS_CREATE_CONN, params, NULL);

	fnvlist_free(params);
	return (rc);
}

int
krrp_sess_conn_throttle(libkrrp_handle_t *hdl, uuid_t sess_id,
    const uint32_t limit)
{
	nvlist_t *params = NULL;
	int rc;
	krrp_sess_id_str_t sess_id_str;

	VERIFY(hdl != NULL);

	libkrrp_reset(hdl);

	uuid_unparse(sess_id, sess_id_str);
	params = fnvlist_alloc();
	(void) krrp_param_put(KRRP_PARAM_SESS_ID, params, sess_id_str);
	(void) krrp_param_put(KRRP_PARAM_THROTTLE, params, (void *)&limit);

	rc = krrp_ioctl_perform(hdl, KRRP_IOCTL_SESS_CONN_THROTTLE, params,
	    NULL);

	fnvlist_free(params);
	return (rc);
}

int
krrp_sess_create_pdu_engine(libkrrp_handle_t *hdl, uuid_t sess_id,
    const int memory_limit, const int dblk_sz, boolean_t use_preallocation)
{
	nvlist_t *params = NULL;
	int rc;
	krrp_sess_id_str_t sess_id_str;

	VERIFY(hdl != NULL);

	libkrrp_reset(hdl);

	uuid_unparse(sess_id, sess_id_str);
	params = fnvlist_alloc();
	(void) krrp_param_put(KRRP_PARAM_SESS_ID, params, sess_id_str);
	(void) krrp_param_put(KRRP_PARAM_MAX_MEMORY, params,
	    (void *)&memory_limit);
	(void) krrp_param_put(KRRP_PARAM_DBLK_DATA_SIZE, params,
	    (void *)&dblk_sz);

	if (use_preallocation) {
		(void) krrp_param_put(KRRP_PARAM_USE_PREALLOCATION,
		    params, NULL);
	}

	rc = krrp_ioctl_perform(hdl, KRRP_IOCTL_SESS_CREATE_PDU_ENGINE, params,
	    NULL);

	fnvlist_free(params);
	return (rc);
}

static void
krrp_sess_create_stream_common(libkrrp_handle_t *hdl, nvlist_t *params,
    uuid_t sess_id, const char *common_snap,
    krrp_sess_stream_flags_t krrp_sess_stream_flags, const char *zcookies)
{
	krrp_sess_id_str_t sess_id_str;

	VERIFY(hdl != NULL);

	libkrrp_reset(hdl);

	uuid_unparse(sess_id, sess_id_str);

	(void) krrp_param_put(KRRP_PARAM_SESS_ID, params, sess_id_str);

	if (common_snap != NULL) {
		(void) krrp_param_put(KRRP_PARAM_COMMON_SNAPSHOT,
		    params, (void *)common_snap);
	}

	if (zcookies != NULL) {
		(void) krrp_param_put(KRRP_PARAM_ZCOOKIES,
		    params, (void *)zcookies);
	}

	if (krrp_sess_stream_flags & KRRP_STREAM_ZFS_EMBEDDED) {
		(void) krrp_param_put(KRRP_PARAM_STREAM_EMBEDDED_BLOCKS,
		    params, NULL);
	}

	if (krrp_sess_stream_flags & KRRP_STREAM_ZFS_CHKSUM) {
		(void) krrp_param_put(KRRP_PARAM_ENABLE_STREAM_CHKSUM,
		    params, NULL);
	}
}

int
krrp_sess_create_write_stream(libkrrp_handle_t *hdl, uuid_t sess_id,
    const char *dataset, const char *common_snap,
    krrp_sess_stream_flags_t krrp_sess_stream_flags, nvlist_t *ignore_props,
    nvlist_t *replace_props, const char *zcookies)
{
	nvlist_t *params = NULL;
	int rc;

	params = fnvlist_alloc();

	krrp_sess_create_stream_common(hdl, params, sess_id, common_snap,
	    krrp_sess_stream_flags, zcookies);

	(void) krrp_param_put(KRRP_PARAM_DST_DATASET, params,
	    (void *)dataset);

	if (krrp_sess_stream_flags & KRRP_STREAM_FORCE_RECEIVE)
		(void) krrp_param_put(KRRP_PARAM_FORCE_RECEIVE, params, NULL);

	if (ignore_props != NULL) {
		(void) krrp_param_put(KRRP_PARAM_IGNORE_PROPS_LIST, params,
		    ignore_props);
	}

	if (replace_props != NULL) {
		(void) krrp_param_put(KRRP_PARAM_REPLACE_PROPS_LIST, params,
		    replace_props);
	}

	rc = krrp_ioctl_perform(hdl, KRRP_IOCTL_SESS_CREATE_WRITE_STREAM,
	    params, NULL);

	fnvlist_free(params);

	return (rc);
}

int
krrp_sess_create_read_stream(libkrrp_handle_t *hdl, uuid_t sess_id,
    const char *dataset, const char *common_snap, const char *src_snap,
    uint64_t fake_data_sz, krrp_sess_stream_flags_t krrp_sess_stream_flags,
    const char *zcookies)
{
	nvlist_t *params = NULL;
	int rc;

	params = fnvlist_alloc();

	krrp_sess_create_stream_common(hdl, params, sess_id, common_snap,
	    krrp_sess_stream_flags, zcookies);

	(void) krrp_param_put(KRRP_PARAM_SRC_DATASET, params, (void *)dataset);

	if (fake_data_sz != 0) {
		(void) krrp_param_put(KRRP_PARAM_FAKE_DATA_SIZE, params,
		    &fake_data_sz);
	}

	if (src_snap != NULL) {
		(void) krrp_param_put(KRRP_PARAM_SRC_SNAPSHOT, params,
		    (void *)src_snap);
	}

	if (krrp_sess_stream_flags & KRRP_STREAM_SEND_RECURSIVE)
		(void) krrp_param_put(KRRP_PARAM_SEND_RECURSIVE, params, NULL);

	if (krrp_sess_stream_flags & KRRP_STREAM_SEND_PROPERTIES)
		(void) krrp_param_put(KRRP_PARAM_SEND_PROPERTIES, params, NULL);

	if (krrp_sess_stream_flags & KRRP_STREAM_INCLUDE_ALL_SNAPS) {
		(void) krrp_param_put(KRRP_PARAM_INCLUDE_ALL_SNAPSHOTS,
		    params, NULL);
	}

	rc = krrp_ioctl_perform(hdl, KRRP_IOCTL_SESS_CREATE_READ_STREAM,
	    params, NULL);

	fnvlist_free(params);

	return (rc);
}

int
krrp_sess_run(libkrrp_handle_t *hdl, uuid_t sess_id, boolean_t once)
{
	nvlist_t *params = NULL;
	int rc;
	krrp_sess_id_str_t sess_id_str;

	VERIFY(hdl != NULL);

	libkrrp_reset(hdl);

	uuid_unparse(sess_id, sess_id_str);
	params = fnvlist_alloc();

	(void) krrp_param_put(KRRP_PARAM_SESS_ID, params, sess_id_str);

	if (once)
		(void) krrp_param_put(KRRP_PARAM_ONLY_ONCE, params, NULL);

	rc = krrp_ioctl_perform(hdl, KRRP_IOCTL_SESS_RUN, params, NULL);

	fnvlist_free(params);
	return (rc);
}

int
krrp_sess_send_stop(libkrrp_handle_t *hdl, uuid_t sess_id)
{
	nvlist_t *params = NULL;
	int rc;
	krrp_sess_id_str_t sess_id_str;

	VERIFY(hdl != NULL);

	libkrrp_reset(hdl);

	uuid_unparse(sess_id, sess_id_str);
	params = fnvlist_alloc();
	(void) krrp_param_put(KRRP_PARAM_SESS_ID, params, sess_id_str);

	rc = krrp_ioctl_perform(hdl, KRRP_IOCTL_SESS_SEND_STOP, params, NULL);

	fnvlist_free(params);
	return (rc);
}

int
krrp_zfs_get_recv_cookies(libkrrp_handle_t *hdl, const char *dataset,
    char *res_zcookies, size_t res_zcookies_sz)
{
	int rc;
	nvlist_t *result_nvl = NULL;
	nvlist_t *params = NULL;
	const char *zcookies = NULL;

	VERIFY(hdl != NULL);

	res_zcookies[0] = '\0';

	libkrrp_reset(hdl);

	params = fnvlist_alloc();
	(void) krrp_param_put(KRRP_PARAM_DST_DATASET, params, (void *)dataset);

	rc = krrp_ioctl_perform(hdl, KRRP_IOCTL_ZFS_GET_RECV_COOKIES, params,
	    &result_nvl);

	fnvlist_free(params);

	if (rc != 0)
		return (rc);

	if (result_nvl == NULL)
		return (rc);

	VERIFY0(krrp_param_get(KRRP_PARAM_ZCOOKIES, result_nvl,
	    &zcookies));

	if (strlcpy(res_zcookies, zcookies,
	    res_zcookies_sz) >= res_zcookies_sz) {
		rc = EOVERFLOW;
		libkrrp_error_set(&hdl->libkrrp_error, LIBKRRP_ERRNO_ZCOOKIES,
		    rc, 0);
	}

	fnvlist_free(result_nvl);
	return (rc);
}
