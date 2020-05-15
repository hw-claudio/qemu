/*
 * TCG specific methods for X86CPUClass
 *
 *  Copyright (c) 2020 SUSE LINUX LLC
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef I386_XCC_TCG_H
#define I386_XCC_TCG_H

uint32_t xcc_tcg_compute_eflags(CPUX86State *env);
void xcc_tcg_set_mxcsr(CPUX86State *env, uint32_t mxcsr);
void xcc_tcg_set_fpuc(CPUX86State *env, uint16_t fpuc);
void xcc_tcg_report_tpr_access(CPUX86State *env, TPRAccess access);
void xcc_tcg_post_load(CPUX86State *env);

#endif /* I386_XCC_TCG_H */
