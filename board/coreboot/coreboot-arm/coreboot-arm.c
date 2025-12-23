#include <asm/armv8/mmu.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <cb_sysinfo.h>
#include <env.h>
#include <linux/sizes.h>
#include <lmb.h>
#include <mapmem.h>

DECLARE_GLOBAL_DATA_PTR;

#define MAX_MEM_MAP_REGIONS 16

static struct mm_region
    coreboot_mem_map[MAX_MEM_MAP_REGIONS] __section(".data") = {0};
struct mm_region *mem_map = coreboot_mem_map;

int dram_init(void) {
	return coreboot_dram_init();
}

phys_addr_t board_get_usable_ram_top(phys_size_t total_size) {
	return coreboot_board_get_usable_ram_top(total_size);
}

int dram_init_banksize(void) {
	return coreboot_dram_init_banksize();
}

void reset_cpu(void) { /* Stub */ };

int board_init(void) {
  printf("board_init().\n");
  // sc7180 display
  /*
  writel(0x1, 0x0AE6B800);
  lib_sysinfo.framebuffer->physical_address = readl(0x0AE05014);
  */
  return 0;
}

#define KERNEL_COMP_SIZE	SZ_64M
#define lmb_alloc(size, addr) lmb_alloc_mem(LMB_MEM_ALLOC_ANY, SZ_2M, addr, size, LMB_NONE)

int board_late_init(void) {
  log_err("board_late_init().\n");

	// Configure env
	// Stolen from mach-snapdragon
	u32 status = 0, fdt_status = 0;
	phys_addr_t addr;
	struct fdt_header *fdt_blob = (struct fdt_header *)gd->fdt_blob;

	status |= !lmb_alloc(SZ_128M, &addr) ?
		env_set_hex("loadaddr", addr) : 1;
	status |= env_set_hex("kernel_addr_r", addr);
	status |= !lmb_alloc(SZ_128M, &addr) ?
		env_set_hex("ramdisk_addr_r", addr) : 1;
	status |= !lmb_alloc(KERNEL_COMP_SIZE, &addr) ?
		env_set_hex("kernel_comp_addr_r", addr) : 1;
	status |= env_set_hex("kernel_comp_size", KERNEL_COMP_SIZE);
	status |= !lmb_alloc(SZ_4M, &addr) ?
		env_set_hex("scriptaddr", addr) : 1;
	status |= !lmb_alloc(SZ_4M, &addr) ?
		env_set_hex("pxefile_addr_r", addr) : 1;
	fdt_status |= !lmb_alloc(SZ_2M, &addr) ?
		env_set_hex("fdt_addr_r", addr) : 1;

	if (status || fdt_status)
		log_warning("%s: Failed to set run time variables\n", __func__);

	/* By default copy U-Boots FDT, it will be used as a fallback */
	if (fdt_status)
		log_warning("%s: Failed to reserve memory for copying FDT\n",
			    __func__);
	else
		memcpy((void *)addr, (void *)gd->fdt_blob,
		       fdt32_to_cpu(fdt_blob->totalsize));
	
  return 0;
}

void enable_caches() {
    log_err("skip enable_caches().\n");
}
