/*
 * Copyright 2015 Nexenta Systems, Inc.  All rights reserved.
 */

#ifndef	_KREPLICATION_COMMON_H
#define	_KREPLICATION_COMMON_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/param.h>
#include <sys/nvpair.h>

typedef struct kreplication_buffer_s {
	void	*data;
	size_t	buffer_size;
	size_t	data_size;
	struct	kreplication_buffer_s *next;
} kreplication_buffer_t;

typedef struct kreplication_ops_s {
    void* (*init_cb)(void*);
    int (*fini_cb)(void*);
    int (*fill_buf_cb)(void*, kreplication_buffer_t *);
    int (*put_buf_cb)(void*, kreplication_buffer_t *);
    void* (*init_stream_cb)();
    void (*fini_stream_cb)(void*);
} kreplication_ops_t;

typedef struct kreplication_zfs_args {
	char from_ds[MAXNAMELEN];
	char from_snap[MAXNAMELEN];
	char from_incr_base[MAXNAMELEN];
	char to_ds[MAXNAMELEN];
	char to_snap[MAXNAMELEN];
	char rep_cookie[MAXNAMELEN];
	boolean_t force;
	boolean_t properties;
	boolean_t recursive;
	boolean_t do_all;
	nvlist_t *ignore_list;
	nvlist_t *replace_list;
	boolean_t strip_head;
	boolean_t leave_tail;
	boolean_t force_thread;
	boolean_t force_cksum;
	boolean_t embedok;
	void *stream_handler;
} kreplication_zfs_args_t;

typedef enum {
	SBS_UNAVAIL,
	SBS_AVAIL,
	SBS_USED,
	SBS_DONE,
	SBS_DESTROYED,
	SBS_NUMTYPES
} dmu_krrp_state_t;

typedef struct dmu_krrp_stream {
	void *custom_recv_buffer;
	int custom_recv_buffer_size;
	int stream_affinity;
} dmu_krrp_stream_t;

typedef struct dmu_krrp_task {
	kmutex_t buffer_state_lock;
	kcondvar_t buffer_state_cv;
	kcondvar_t buffer_destroy_cv;
	kreplication_buffer_t *buffer;
	size_t buffer_bytes_read;
	boolean_t is_read;
	boolean_t is_full;
	dmu_krrp_state_t buffer_state;
	int buffer_error;
	kthread_t *buffer_user_thread;
	taskqid_t buffer_user_task;
	dmu_krrp_stream_t *stream_handler;
	kreplication_zfs_args_t buffer_args;
	char cookie[MAXNAMELEN];
} dmu_krrp_task_t;

typedef int (*arc_bypass_io_func)(void *, int, void *);

#ifdef	__cplusplus
}
#endif

#endif /* _KREPLICATION_COMMON_H */
