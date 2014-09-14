/*   ___  _ __ ___   __ _
 *  / _ \| '_ ` _ \ / _` |
 * | (_) | | | | | | (_| |
 *  \___/|_| |_| |_|\__, |
 *                  |___/
 *
 * This file includes everything needed by tigerbeetle that is
 * unfortunately not exported by libbabeltrace. Care must be taken to
 * ensure the structures and definitions in this file match exactly the
 * ones of the libbabeltrace version used by tigerbeetle. This should
 * not be a problem since the Babeltrace project is included as a Git
 * submodule in the tigerbeetle repository, fixed at a specific commit.
 *
 * All structure names are prefixed with "tibee_" to avoid conflicts
 * with libbabeltrace's public API.
 *
 * The Babeltrace project copyright notice follows.
 *
 *
 * BabelTrace
 *
 * Copyright 2011-2012 EfficiOS Inc. and Linux Foundation
 *
 * Author: Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *         Julien Desfossez <julien.desfossez@efficios.com>
 *
 * Copyright 2010 - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef _BABELTRACE_INTERNALS_H
#define _BABELTRACE_INTERNALS_H

#include <sys/types.h>
#include <glib.h>
#include <dirent.h>

#include <babeltrace/ctf/events.h>

struct tibee_bt_context;
struct tibee_ctf_trace;
struct tibee_ctf_stream_declaration;
struct tibee_ctf_event_declaration;
struct tibee_ctf_clock;
struct tibee_ctf_callsite;
struct tibee_definition_struct;
struct tibee_declaration_scope;
struct tibee_definition_scope;
struct tibee_bt_stream_callbacks;
struct tibee_bt_trace_descriptor;
struct tibee_bt_format;
struct tibee_bt_stream_pos;
struct tibee_ctf_stream_definition;
struct tibee_bt_definition;

struct tibee_bt_declaration {
    enum ctf_type_id id;
    size_t alignment;   /* type alignment, in bits */
    int ref;        /* number of references to the type */
    /*
     * declaration_free called with declaration ref is decremented to 0.
     */
    void (*declaration_free)(struct tibee_bt_declaration *declaration);
    struct tibee_bt_definition *
        (*definition_new)(struct tibee_bt_declaration *declaration,
                  struct tibee_definition_scope *parent_scope,
                  GQuark field_name, int index,
                  const char *root_name);
    /*
     * definition_free called with definition ref is decremented to 0.
     */
    void (*definition_free)(struct tibee_bt_definition *definition);
};

struct tibee_declaration_field {
    GQuark name;
    struct tibee_bt_declaration *declaration;
};

struct tibee_declaration_struct {
    struct tibee_bt_declaration p;
    GHashTable *fields_by_name; /* Tuples (field name, field index) */
    struct tibee_declaration_scope *scope;
    GArray *fields;         /* Array of declaration_field */
};

/*
 * trace_handle : unique identifier of a trace
 *
 * The trace_handle allows the user to manipulate a trace file directly.
 * It is a unique identifier representing a trace file.
 */
struct tibee_bt_trace_handle {
    int id;
    struct tibee_bt_trace_descriptor *td;
    struct tibee_bt_format *format;
    char path[PATH_MAX];
    uint64_t real_timestamp_begin;
    uint64_t real_timestamp_end;
    uint64_t cycles_timestamp_begin;
    uint64_t cycles_timestamp_end;
};

/* Parent trace descriptor */
struct tibee_bt_trace_descriptor {
    char path[PATH_MAX];        /* trace path */
    struct tibee_bt_context *ctx;
    struct tibee_bt_trace_handle *handle;
    struct trace_collection *collection;    /* Container of this trace */
    GHashTable *clocks;
    struct tibee_ctf_clock *single_clock;       /* currently supports only one clock */
};

typedef int (*tibee_rw_dispatch)(struct tibee_bt_stream_pos *pos,
               struct tibee_bt_definition *definition);

struct tibee_bt_stream_pos {
    /* read/write dispatch table. Specific to plugin used for stream. */
    tibee_rw_dispatch *rw_table;    /* rw dispatch table */
    int (*event_cb)(struct tibee_bt_stream_pos *pos,
            struct tibee_ctf_stream_definition *stream);
    int (*pre_trace_cb)(struct tibee_bt_stream_pos *pos,
            struct tibee_bt_trace_descriptor *trace);
    int (*post_trace_cb)(struct tibee_bt_stream_pos *pos,
            struct tibee_bt_trace_descriptor *trace);
    struct tibee_bt_trace_descriptor *trace;
};

/*
 * Always update ctf_stream_pos with ctf_move_pos and ctf_init_pos.
 */
struct tibee_ctf_stream_pos {
    struct tibee_bt_stream_pos parent;
    int fd;         /* backing file fd. -1 if unset. */
    FILE *index_fp;     /* backing index file fp. NULL if unset. */
    GArray *packet_index;   /* contains struct packet_index */
    int prot;       /* mmap protection */
    int flags;      /* mmap flags */

    /* Current position */
    off_t mmap_offset;  /* mmap offset in the file, in bytes */
    off_t mmap_base_offset; /* offset of start of packet in mmap, in bytes */
    uint64_t packet_size;   /* current packet size, in bits */
    uint64_t content_size;  /* current content size, in bits */
    uint64_t *content_size_loc; /* pointer to current content size */
    struct mmap_align *base_mma;/* mmap base address */
    int64_t offset;     /* offset from base, in bits. EOF for end of file. */
    int64_t last_offset;    /* offset before the last read_event */
    int64_t data_offset;    /* offset of data in current packet */
    uint64_t cur_index; /* current index in packet index */
    uint64_t last_events_discarded; /* last known amount of event discarded */
    void (*packet_seek)(struct tibee_bt_stream_pos *pos, size_t index,
            int whence); /* function called to switch packet */

    int dummy;      /* dummy position, for length calculation */
    struct bt_stream_callbacks *cb; /* Callbacks registered for iterator. */
    void *priv;
};

struct tibee_ctf_stream_packet_limits {
    uint64_t begin;
    uint64_t end;
};

struct tibee_ctf_stream_packet_timestamp {
    struct tibee_ctf_stream_packet_limits cycles;
    struct tibee_ctf_stream_packet_limits real;
};

struct tibee_ctf_stream_definition {
    struct tibee_ctf_stream_declaration *stream_class;
    uint64_t real_timestamp;        /* Current timestamp, in ns */
    uint64_t cycles_timestamp;      /* Current timestamp, in cycles */
    uint64_t event_id;          /* Current event ID */
    int has_timestamp;
    uint64_t stream_id;

    struct tibee_definition_struct *trace_packet_header;
    struct tibee_definition_struct *stream_packet_context;
    struct tibee_definition_struct *stream_event_header;
    struct tibee_definition_struct *stream_event_context;
    GPtrArray *events_by_id;        /* Array of struct ctf_event_definition pointers indexed by id */
    struct tibee_definition_scope *parent_def_scope;    /* for initialization */
    int stream_definitions_created;

    struct tibee_ctf_clock *current_clock;

    /* Event discarded information */
    uint64_t events_discarded;
    struct tibee_ctf_stream_packet_timestamp prev;
    struct tibee_ctf_stream_packet_timestamp current;
    char path[PATH_MAX];            /* Path to stream. '\0' for mmap traces */
};

struct tibee_ctf_event_definition {
    struct tibee_ctf_stream_definition *stream;
    struct tibee_definition_struct *event_context;
    struct tibee_definition_struct *event_fields;
};

struct tibee_ctf_clock {
    GQuark name;
    GQuark uuid;
    char *description;
    uint64_t freq;  /* frequency, in HZ */
    /* precision in seconds is: precision * (1/freq) */
    uint64_t precision;
    /*
     * The offset from Epoch is: offset_s + (offset * (1/freq))
     * Coarse clock offset from Epoch (in seconds).
     */
    uint64_t offset_s;
    /* Fine clock offset from Epoch, in (1/freq) units. */
    uint64_t offset;
    int absolute;

    enum {                  /* Fields populated mask */
        CTF_CLOCK_name      =   (1U << 0),
        CTF_CLOCK_freq      =   (1U << 1),
    } field_mask;
};

#ifndef TRACER_ENV_LEN
#define TRACER_ENV_LEN  128
#endif

/* tracer-specific environment */
struct tibee_ctf_tracer_env {
    int vpid;       /* negative if unset */

    /* All strings below: "" if unset. */
    char procname[TRACER_ENV_LEN];
    char hostname[TRACER_ENV_LEN];
    char domain[TRACER_ENV_LEN];
    char sysname[TRACER_ENV_LEN];
    char release[TRACER_ENV_LEN];
    char version[TRACER_ENV_LEN];
};

/* Includes final \0. */
#ifndef BABELTRACE_UUID_STR_LEN
#define BABELTRACE_UUID_STR_LEN     37
#endif

#ifndef BABELTRACE_UUID_LEN
#define BABELTRACE_UUID_LEN     16
#endif

struct tibee_ctf_trace {
    struct tibee_bt_trace_descriptor parent;

    /* root scope */
    struct tibee_declaration_scope *root_declaration_scope;

    struct tibee_declaration_scope *declaration_scope;
    /* innermost definition scope. to be used as parent of stream. */
    struct tibee_definition_scope *definition_scope;
    GPtrArray *streams;         /* Array of struct ctf_stream_declaration pointers */
    struct tibee_ctf_stream_definition *metadata;
    char *metadata_string;
    int metadata_packetized;
    GHashTable *callsites;
    GPtrArray *event_declarations;      /* Array of all the struct bt_ctf_event_decl */

    struct tibee_declaration_struct *packet_header_decl;
    struct ctf_scanner *scanner;
    int restart_root_decl;

    uint64_t major;
    uint64_t minor;
    unsigned char uuid[BABELTRACE_UUID_LEN];
    int byte_order;     /* trace BYTE_ORDER. 0 if unset. */
    struct tibee_ctf_tracer_env env;

    enum {                  /* Fields populated mask */
        CTF_TRACE_major     =   (1U << 0),
        CTF_TRACE_minor     =   (1U << 1),
        CTF_TRACE_uuid      =   (1U << 2),
        CTF_TRACE_byte_order    =   (1U << 3),
        CTF_TRACE_packet_header =   (1U << 4),
    } field_mask;

    /* Information about trace backing directory and files */
    DIR *dir;
    int dirfd;
    int flags;      /* open flags */
};

struct tibee_ctf_stream_declaration {
    struct tibee_ctf_trace *trace;
    /* parent is lexical scope containing the stream scope */
    struct tibee_declaration_scope *declaration_scope;
    /* innermost definition scope. to be used as parent of event. */
    struct tibee_definition_scope *definition_scope;
    GPtrArray *events_by_id;        /* Array of struct ctf_event_declaration pointers indexed by id */
    GHashTable *event_quark_to_id;      /* GQuark to numeric id */

    struct tibee_declaration_struct *packet_context_decl;
    struct tibee_declaration_struct *event_header_decl;
    struct tibee_declaration_struct *event_context_decl;

    uint64_t stream_id;

    enum {                  /* Fields populated mask */
        CTF_STREAM_stream_id =  (1 << 0),
    } field_mask;

    GPtrArray *streams; /* Array of struct ctf_stream_definition pointers */
};

struct tibee_ctf_event_declaration {
    /* stream mapped by stream_id */
    struct tibee_ctf_stream_declaration *stream;
    /* parent is lexical scope containing the event scope */
    struct tibee_declaration_scope *declaration_scope;

    struct tibee_declaration_struct *context_decl;
    struct tibee_declaration_struct *fields_decl;

    GQuark name;
    uint64_t id;        /* Numeric identifier within the stream */
    uint64_t stream_id;
    int loglevel;
    GQuark model_emf_uri;

    enum {                  /* Fields populated mask */
        CTF_EVENT_name  =       (1 << 0),
        CTF_EVENT_id    =       (1 << 1),
        CTF_EVENT_stream_id =       (1 << 2),
        CTF_EVENT_loglevel =        (1 << 4),
        CTF_EVENT_model_emf_uri =   (1 << 5),
    } field_mask;
};

struct tibee_bt_ctf_event {
    struct tibee_ctf_event_definition *parent;
};

struct tibee_bt_ctf_event_decl {
    struct tibee_ctf_event_declaration parent;
    GPtrArray *context_decl;
    GPtrArray *fields_decl;
    GPtrArray *packet_header_decl;
    GPtrArray *event_context_decl;
    GPtrArray *event_header_decl;
    GPtrArray *packet_context_decl;
};

#endif /* _BABELTRACE_INTERNALS_H */
