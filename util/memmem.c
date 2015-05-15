/*
 * memmem replacement function
 *
 * Copyright (C) 2015 Huawei Technologies Duesseldorf GmbH
 * Written by Claudio Fontana <claudio.fontana@huawei.com>
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

#include <qemu-common.h>

/*
 * Search for the first occurrence of a binary string ("needle")
 * in a memory region ("haystack").
 *
 * If needle length is 0, it returns the pointer to the haystack.
 * Otherwise it returns the pointer to the first character of the first
 * occurrence of the needle in the haystack, or NULL if none are found.
 *
 */
void *memmem(const void *haystack, size_t hay_len, const void *needle, size_t s_len)
{
    const unsigned char *hay = (const unsigned char *)haystack;
    const unsigned char *s = (const unsigned char *)needle;
    const unsigned char *last = hay + (hay_len - s_len);

    if (s_len == 0) {
        return (void *)hay;
    }

    if (hay_len < s_len) {
        return NULL;
    }

    if (s_len == 1) {
        return memchr(hay, s[0], hay_len);
    }

    for (; hay <= last; hay++) {
        if (hay[0] == s[0] && memcmp(hay, s, s_len) == 0) {
            return (void *)hay;
        }
    }

    return NULL;
}
