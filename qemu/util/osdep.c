/*
 * QEMU low level functions
 *
 * Copyright (c) 2003 Fabrice Bellard
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "qemu/osdep.h"

#include "qemu-common.h"
#include "qemu/cutils.h"
#include "uc_priv.h"

static int qemu_mprotect__osdep(struct uc_struct *uc, void *addr, size_t size, int prot)
{
    g_assert(!((uintptr_t)addr & ~uc->qemu_real_host_page_mask));
    g_assert(!(size & ~uc->qemu_real_host_page_mask));

#ifdef _WIN32
    DWORD old_protect;

    if (!VirtualProtect(addr, size, prot, &old_protect)) {
        //error_report("%s: VirtualProtect failed with error code %ld",
        //             __func__, GetLastError());
        return -1;
    }
    return 0;
#else
    if (mprotect(addr, size, prot)) {
        //error_report("%s: mprotect failed: %s", __func__, strerror(errno));
        return -1;
    }
    return 0;
#endif
}

int qemu_mprotect_rwx(struct uc_struct *uc, void *addr, size_t size)
{
#ifdef _WIN32
    return qemu_mprotect__osdep(uc, addr, size, PAGE_EXECUTE_READWRITE);
#else
    return qemu_mprotect__osdep(uc, addr, size, PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
}

int qemu_mprotect_none(struct uc_struct *uc, void *addr, size_t size)
{
#ifdef _WIN32
    return qemu_mprotect__osdep(uc, addr, size, PAGE_NOACCESS);
#else
    return qemu_mprotect__osdep(uc, addr, size, PROT_NONE);
#endif
}
