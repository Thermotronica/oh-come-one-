/*
 * *****************************************************************************
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018-2020 Gavin D. Howard and contributors.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * *****************************************************************************
 *
 * The private header for the bc library.
 *
 */

#ifndef LIBBC_PRIVATE_H
#define LIBBC_PRIVATE_H

#include <bc.h>

#include "num.h"

#define BC_FUNC_HEADER_LOCK(l)     \
	do {                           \
		BC_SIG_LOCK;               \
		BC_SETJMP_LOCKED(l);       \
		vm.err = BC_ERROR_SUCCESS; \
		vm.running = 0;            \
	} while (0)

#define BC_FUNC_FOOTER_UNLOCK(e) \
	do {                         \
		BC_SIG_ASSERT_LOCKED;    \
		e = vm.err;              \
		vm.running = 0;          \
		BC_UNSETJMP;             \
		BC_LONGJMP_STOP;         \
		vm.sig_lock = 0;         \
	} while (0)

#define BC_FUNC_HEADER(l)          \
	do {                           \
		BC_SETJMP(l);              \
		vm.err = BC_ERROR_SUCCESS; \
		vm.running = 0;            \
	} while (0)

#define BC_FUNC_FOOTER_NO_ERR \
	do {                      \
		vm.running = 0;       \
		BC_UNSETJMP;          \
		BC_LONGJMP_STOP;      \
		vm.sig_lock = 0;      \
	} while (0)


#define BC_FUNC_FOOTER(e)      \
	do {                       \
		e = vm.err;            \
		BC_FUNC_FOOTER_NO_ERR; \
	} while (0)

#define BC_FUNC_RESETJMP(l)   \
	do {                      \
		BC_SIG_ASSERT_LOCKED; \
		BC_UNSETJMP;          \
		BC_SETJMP_LOCKED(l);  \
	} while (0)

#define BC_MAYBE_SETUP(c, e, n, i)                  \
	do {                                            \
		if (BC_ERR((e) != BC_ERROR_SUCCESS)) {      \
			if ((n).num != NULL) bc_num_free(&(n)); \
			i = 0 - (BcNumber) (e);                 \
		}                                           \
		else i = bcl_num_insert(c, &(n));           \
	} while (0)

#define BC_CHECK_CTXT(c)                                    \
	do {                                                    \
		c = bcl_context();                                  \
		if (BC_ERR(c == NULL)) {                            \
			return 0 - (BcNumber) BC_ERROR_INVALID_CONTEXT; \
		}                                                   \
	} while (0)

#define BC_CHECK_CTXT_ERR(c)                 \
	do {                                     \
		c = bcl_context();                   \
		if (BC_ERR(c == NULL)) {             \
			return BC_ERROR_INVALID_CONTEXT; \
		}                                    \
	} while (0)

#define BC_CHECK_CTXT_ASSERT(c) \
	do {                        \
		c = bcl_context();      \
		assert(c != NULL);      \
	} while (0)

#define BC_CHECK_NUM(c, n)                                    \
	do {                                                      \
		if (BC_ERR((n) >= (c)->nums.len)) {                   \
			if (n > 0 - (BcNumber) BC_ERROR_NELEMS) return n; \
			else return 0 - (BcNumber) BC_ERROR_INVALID_NUM;  \
		}                                                     \
	} while (0)

#define BC_CHECK_NUM_ERR(c, n)                                \
	do {                                                      \
		if (BC_ERR((n) >= (c)->nums.len)) {                   \
			if (n > 0 - (BcNumber) BC_ERROR_NELEMS)           \
				return (BcError) (0 - n);                     \
			else return BC_ERROR_INVALID_NUM;                 \
		}                                                     \
	} while (0)

#define BC_NUM(c, i) ((BcNum*) bc_vec_item(&(c)->nums, (i)))

typedef size_t (*BcReqOp)(const BcNum*, const BcNum*, size_t);

typedef struct BcCtxt {

	size_t scale;
	size_t ibase;
	size_t obase;

	BcVec nums;
	BcVec free_nums;

} BcCtxt;

#endif // LIBBC_PRIVATE_H
