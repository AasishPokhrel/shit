#include <drivers/pic.h>
#include <limine.h>
#include <shit/console.h>
#include <shit/idt.h>
#include <shit/interrupt.h>
#include <shit/io.h>
#include <shit/memory/kheap.h>
#include <shit/memory/pmm.h>
#include <shit/printk.h>
#include <shit/string.h>
#include <stddef.h>

__attribute__((
    used,
    section(".limine_requests"))) volatile struct limine_framebuffer_request
    framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

__attribute__((used,
               section(".limine_requests"))) volatile struct limine_hhdm_request
    hhdm_request = {.id = LIMINE_HHDM_REQUEST, .revision = 0};

__attribute__((
    used, section(".limine_requests"))) volatile struct limine_memmap_request
    memmap_request = {.id = LIMINE_MEMMAP_REQUEST, .revision = 0};

void kmain(void) {
  console_init();
  printk(KERN_INFO "Seren OS - Nucleus Kernel Booting...\n");
  printk(KERN_INFO "LFB GFX, PSF Font, Console Initialized.\n");

  idt_init();
  printk(KERN_INFO "IDT Initialized and Loaded.\n");

  pic_remap_and_init();
  printk(KERN_INFO "PICs remapped and initialized.\n");

  pmm_init(&memmap_request);

  printk(KERN_INFO "Enabling interrupts (STI).\n");
  __asm__ volatile("sti");

  printk(KERN_INFO "POOP OS OHHHHHHHHHHHHHH\n");
  printk(KERN_INFO "SHITINITILAZTIOEONRH COMPLETE ENTERING HALT LOOP BYE\n");

  goto halt_loop;

halt_loop:
  while (1) {
    __asm__ volatile("hlt");
  }
}
