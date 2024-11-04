/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <memory/host.h>
#include <memory/paddr.h>
#include <cstdint>
#include <common.h>
#include <utils.h>
#include <debug.h>

// All memory accesses should be done in this file
static uint8_t pmem[CONFIG_MSIZE] __attribute((aligned(4096))) = {};

uint8_t* guest_to_host(paddr_t paddr) { return pmem + paddr - CONFIG_MBASE; }
paddr_t host_to_guest(uint8_t *haddr) { return haddr - pmem + CONFIG_MBASE; }

static word_t pmem_read(paddr_t addr) {
  word_t ret = host_read(guest_to_host(addr), 4);
#ifdef CONFIG_MTRACE
  log_write("Reading %d bytes from physical memory address " FMT_WORD " with value " FMT_WORD "\n", len, addr, ret);
#endif
  return ret;
}

static void pmem_write(paddr_t addr, word_t data, byte mask) {
  // Mask is used to indicate which bytes in a byte are to be written
  int len;
  switch (mask) {
    case 0x1: len = 1; break;
    case 0x3: len = 2; break;
    case 0xf: len = 4; break;
    default: Assert(0, "Invalid mask %x for pmem_write", mask);
  }
  host_write(guest_to_host(addr), len, data);
#ifdef CONFIG_MTRACE
  log_write("Writing %d bytes to physical memory address " FMT_WORD " with value " FMT_WORD "\n", len, addr, data);
#endif
}

static void out_of_bound(paddr_t addr) {
  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR "]",
      addr, PMEM_LEFT, PMEM_RIGHT);
}

void init_mem() {
  IFDEF(CONFIG_MEM_RANDOM, memset(pmem, rand(), CONFIG_MSIZE));
  Log("physical memory area [" FMT_PADDR ", " FMT_PADDR "]", PMEM_LEFT, PMEM_RIGHT);
}

word_t paddr_read(paddr_t addr) {
  int len = 4;
#ifdef CONFIG_MTRACE
  log_write("Reading %d bytes from address " FMT_WORD "\n", len, addr);
#endif
  if (likely(in_pmem(addr))) return pmem_read(addr);
  out_of_bound(addr);
  return 0;
}

void paddr_write(paddr_t addr, word_t data, byte mask) {
  if (likely(in_pmem(addr))) { pmem_write(addr, data, mask); return; }
  out_of_bound(addr);
}

extern "C" int ysyx_24070014_paddr_read(int addr) {
  return (int)paddr_read((paddr_t)addr);
}

extern "C" void ysyx_24070014_paddr_write(int addr, int data, byte mask) {
  paddr_write((paddr_t)addr, (word_t)data, (byte)mask);
}
