/*
 * Copyright (c) 2006-2025 RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-07-11     RT-Thread    first version
 * 2025-01-16     wumingzi     add doxyzen comment
 */

#ifndef __LWP_DBG_H__
#define __LWP_DBG_H__

#include <rtthread.h>
#include <rthw.h>
#include <lwp.h>

/**
 * @brief Get lwp debug state
 *
 * @return int   lwp debug state
 * @ingroup lwp
 */
int dbg_thread_in_debug(void);

/**
 * @brief Register dbg_ops to struct dbg_ops_t
 *
 * @param dbg_ops
 * @return void
 * @ingroup lwp
 */
void dbg_register(struct dbg_ops_t *dbg_ops);

/**
 * @brief Get lwp instructure state
 *
 * @return uint32_t
 * @ingroup lwp
 */
uint32_t dbg_get_ins(void);

/**
 * @brief Enable step debug feature
 *
 * @return void
 * @ingroup lwp
 */
void dbg_activate_step(void);

/**
 * @brief Disable step debug feature
 *
 * @ingroup lwp
 */
void dbg_deactivate_step(void);

/**
 * @brief Check event for related instruction by error state register
 *
 * @param regs
 * @param esr
 * @return int
 * @ingroup lwp
 */
int dbg_check_event(struct rt_hw_exp_stack *regs, unsigned long esr);

/**
 * @brief Get gdb server channel and prepared by rt_channel_t
 *
 * @return rt_channel_t
 * @ingroup lwp
 */
rt_channel_t gdb_server_channel(void);

/**
 * @brief Get debug step type
 *
 * @return int
 * @ingroup lwp
 */
int dbg_step_type(void);

/**
 * @brief Start debug attach request
 *
 * @param pc
 * @ingroup lwp
 */
void dbg_attach_req(void *pc);

/**
 * @brief Suspend debug attach requestt
 *
 * @return int
 * @ingroup lwp
 */
int dbg_check_suspend(void);

#endif /* __LWP_DBG_H__ */