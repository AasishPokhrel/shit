/* BSD Zero Clause License */

/* Copyright (C) 2022-2025 Mintsuki and contributors.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef LIMINE_H
#define LIMINE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Misc */

#ifdef LIMINE_NO_POINTERS
#define LIMINE_PTR(TYPE) uint64_t
#else
#define LIMINE_PTR(TYPE) TYPE
#endif

#ifndef LIMINE_API_REVISION
#define LIMINE_API_REVISION 0
#endif

#if LIMINE_API_REVISION > 3
#error "limine.h API revision unsupported"
#endif

#ifdef __GNUC__
#define LIMINE_DEPRECATED __attribute__((__deprecated__))
#define LIMINE_DEPRECATED_IGNORE_START                                         \
    _Pragma("GCC diagnostic push")                                             \
        _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#define LIMINE_DEPRECATED_IGNORE_END _Pragma("GCC diagnostic pop")
#else
#define LIMINE_DEPRECATED
#define LIMINE_DEPRECATED_IGNORE_START
#define LIMINE_DEPRECATED_IGNORE_END
#endif

#define LIMINE_REQUESTS_START_MARKER                                           \
    uint64_t limine_requests_start_marker[4] = {                               \
        0xf6b8f4b39de7d1ae, 0xfab91a6940fcb9cf, 0x785c6ed015d3e316,            \
        0x181e920a7852b9d9};
#define LIMINE_REQUESTS_END_MARKER                                             \
    uint64_t limine_requests_end_marker[2] = {0xadc0e0531bb10d03,              \
                                              0x9572709f31764c62};

#define LIMINE_REQUESTS_DELIMITER LIMINE_REQUESTS_END_MARKER

#define LIMINE_BASE_REVISION(N)                                                \
    uint64_t limine_base_revision[3] = {0xf9562b2d5c95a6c8,                    \
                                        0x6a7b384944536bdc, (N)};

#define LIMINE_BASE_REVISION_SUPPORTED (limine_base_revision[2] == 0)

#define LIMINE_LOADED_BASE_REV_VALID                                           \
    (limine_base_revision[1] != 0x6a7b384944536bdc)
#define LIMINE_LOADED_BASE_REVISION (limine_base_revision[1])

#define LIMINE_COMMON_MAGIC 0xc7b1dd30df4c8b88, 0x0a82e883a194f07b

struct limine_uuid {
    uint32_t a;
    uint16_t b;
    uint16_t c;
    uint8_t d[8];
};

#define LIMINE_MEDIA_TYPE_GENERIC 0
#define LIMINE_MEDIA_TYPE_OPTICAL 1
#define LIMINE_MEDIA_TYPE_TFTP    2

struct limine_file {
    uint64_t revision;
    LIMINE_PTR(void *) address;
    uint64_t size;
    LIMINE_PTR(char *) path;
#if LIMINE_API_REVISION >= 3
    LIMINE_PTR(char *) string;
#else
    LIMINE_PTR(char *) cmdline;
#endif
    uint32_t media_type;
    uint32_t unused;
    uint32_t tftp_ip;
    uint32_t tftp_port;
    uint32_t partition_index;
    uint32_t mbr_disk_id;
    struct limine_uuid gpt_disk_uuid;
    struct limine_uuid gpt_part_uuid;
    struct limine_uuid part_uuid;
};

/* Boot info */

#define LIMINE_BOOTLOADER_INFO_REQUEST                                         \
    {LIMINE_COMMON_MAGIC, 0xf55038d8e2a1202f, 0x279426fcf5f59740}

struct limine_bootloader_info_response {
    uint64_t revision;
    LIMINE_PTR(char *) name;
    LIMINE_PTR(char *) version;
};

struct limine_bootloader_info_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_bootloader_info_response *) response;
};

/* Executable command line */

#define LIMINE_EXECUTABLE_CMDLINE_REQUEST                                      \
    {LIMINE_COMMON_MAGIC, 0x4b161536e598651e, 0xb390ad4a2f1f303a}

struct limine_executable_cmdline_response {
    uint64_t revision;
    LIMINE_PTR(char *) cmdline;
};

struct limine_executable_cmdline_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_executable_cmdline_response *) response;
};

/* Firmware type */

#define LIMINE_FIRMWARE_TYPE_REQUEST                                           \
    {LIMINE_COMMON_MAGIC, 0x8c2f75d90bef28a8, 0x7045a4688eac00c3}

#define LIMINE_FIRMWARE_TYPE_X86BIOS 0
#define LIMINE_FIRMWARE_TYPE_UEFI32  1
#define LIMINE_FIRMWARE_TYPE_UEFI64  2
#define LIMINE_FIRMWARE_TYPE_SBI     3

struct limine_firmware_type_response {
    uint64_t revision;
    uint64_t firmware_type;
};

struct limine_firmware_type_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_firmware_type_response *) response;
};

/* Stack size */

#define LIMINE_STACK_SIZE_REQUEST                                              \
    {LIMINE_COMMON_MAGIC, 0x224ef0460a8e8926, 0xe1cb0fc25f46ea3d}

struct limine_stack_size_response {
    uint64_t revision;
};

struct limine_stack_size_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_stack_size_response *) response;
    uint64_t stack_size;
};

/* HHDM */

#define LIMINE_HHDM_REQUEST                                                    \
    {LIMINE_COMMON_MAGIC, 0x48dcf1cb8ad2b852, 0x63984e959a98244b}

struct limine_hhdm_response {
    uint64_t revision;
    uint64_t offset;
};

struct limine_hhdm_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_hhdm_response *) response;
};

/* Framebuffer */

#define LIMINE_FRAMEBUFFER_REQUEST                                             \
    {LIMINE_COMMON_MAGIC, 0x9d5827dcd881dd75, 0xa3148604f6fab11b}

#define LIMINE_FRAMEBUFFER_RGB 1

struct limine_video_mode {
    uint64_t pitch;
    uint64_t width;
    uint64_t height;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
};

struct limine_framebuffer {
    LIMINE_PTR(void *) address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
    uint8_t unused[7];
    uint64_t edid_size;
    LIMINE_PTR(void *) edid;
    /* Response revision 1 */
    uint64_t mode_count;
    LIMINE_PTR(struct limine_video_mode **) modes;
};

struct limine_framebuffer_response {
    uint64_t revision;
    uint64_t framebuffer_count;
    LIMINE_PTR(struct limine_framebuffer **) framebuffers;
};

struct limine_framebuffer_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_framebuffer_response *) response;
};

/* Terminal */

#define LIMINE_TERMINAL_REQUEST                                                \
    {LIMINE_COMMON_MAGIC, 0xc8ac59310c2b0844, 0xa68d0c7265d38878}

#define LIMINE_TERMINAL_CB_DEC           10
#define LIMINE_TERMINAL_CB_BELL          20
#define LIMINE_TERMINAL_CB_PRIVATE_ID    30
#define LIMINE_TERMINAL_CB_STATUS_REPORT 40
#define LIMINE_TERMINAL_CB_POS_REPORT    50
#define LIMINE_TERMINAL_CB_KBD_LEDS      60
#define LIMINE_TERMINAL_CB_MODE          70
#define LIMINE_TERMINAL_CB_LINUX         80

#define LIMINE_TERMINAL_CTX_SIZE     ((uint64_t)(-1))
#define LIMINE_TERMINAL_CTX_SAVE     ((uint64_t)(-2))
#define LIMINE_TERMINAL_CTX_RESTORE  ((uint64_t)(-3))
#define LIMINE_TERMINAL_FULL_REFRESH ((uint64_t)(-4))

/* Response revision 1 */
#define LIMINE_TERMINAL_OOB_OUTPUT_GET ((uint64_t)(-10))
#define LIMINE_TERMINAL_OOB_OUTPUT_SET ((uint64_t)(-11))

#define LIMINE_TERMINAL_OOB_OUTPUT_OCRNL  (1 << 0)
#define LIMINE_TERMINAL_OOB_OUTPUT_OFDEL  (1 << 1)
#define LIMINE_TERMINAL_OOB_OUTPUT_OFILL  (1 << 2)
#define LIMINE_TERMINAL_OOB_OUTPUT_OLCUC  (1 << 3)
#define LIMINE_TERMINAL_OOB_OUTPUT_ONLCR  (1 << 4)
#define LIMINE_TERMINAL_OOB_OUTPUT_ONLRET (1 << 5)
#define LIMINE_TERMINAL_OOB_OUTPUT_ONOCR  (1 << 6)
#define LIMINE_TERMINAL_OOB_OUTPUT_OPOST  (1 << 7)

LIMINE_DEPRECATED_IGNORE_START

struct LIMINE_DEPRECATED limine_terminal;

typedef void (*limine_terminal_write)(struct limine_terminal *, const char *,
                                      uint64_t);
typedef void (*limine_terminal_callback)(struct limine_terminal *, uint64_t,
                                         uint64_t, uint64_t, uint64_t);

struct LIMINE_DEPRECATED limine_terminal {
    uint64_t columns;
    uint64_t rows;
    LIMINE_PTR(struct limine_framebuffer *) framebuffer;
};

struct LIMINE_DEPRECATED limine_terminal_response {
    uint64_t revision;
    uint64_t terminal_count;
    LIMINE_PTR(struct limine_terminal **) terminals;
    LIMINE_PTR(limine_terminal_write) write;
};

struct LIMINE_DEPRECATED limine_terminal_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_terminal_response *) response;
    LIMINE_PTR(limine_terminal_callback) callback;
};

LIMINE_DEPRECATED_IGNORE_END

/* Paging mode */

#define LIMINE_PAGING_MODE_REQUEST                                             \
    {LIMINE_COMMON_MAGIC, 0x95c1a0edab0944cb, 0xa4e5cb3842f7488a}

#if defined(__x86_64__) || defined(__i386__)
#define LIMINE_PAGING_MODE_X86_64_4LVL 0
#define LIMINE_PAGING_MODE_X86_64_5LVL 1
#define LIMINE_PAGING_MODE_MIN         LIMINE_PAGING_MODE_X86_64_4LVL
#define LIMINE_PAGING_MODE_DEFAULT     LIMINE_PAGING_MODE_X86_64_4LVL
#elif defined(__aarch64__)
#define LIMINE_PAGING_MODE_AARCH64_4LVL 0
#define LIMINE_PAGING_MODE_AARCH64_5LVL 1
#define LIMINE_PAGING_MODE_MIN          LIMINE_PAGING_MODE_AARCH64_4LVL
#define LIMINE_PAGING_MODE_DEFAULT      LIMINE_PAGING_MODE_AARCH64_4LVL
#elif defined(__riscv) && (__riscv_xlen == 64)
#define LIMINE_PAGING_MODE_RISCV_SV39 0
#define LIMINE_PAGING_MODE_RISCV_SV48 1
#define LIMINE_PAGING_MODE_RISCV_SV57 2
#define LIMINE_PAGING_MODE_MIN        LIMINE_PAGING_MODE_RISCV_SV39
#define LIMINE_PAGING_MODE_DEFAULT    LIMINE_PAGING_MODE_RISCV_SV48
#elif defined(__loongarch__) && (__loongarch_grlen == 64)
#define LIMINE_PAGING_MODE_LOONGARCH64_4LVL 0
#define LIMINE_PAGING_MODE_MIN              LIMINE_PAGING_MODE_LOONGARCH64_4LVL
#define LIMINE_PAGING_MODE_DEFAULT          LIMINE_PAGING_MODE_LOONGARCH64_4LVL
#else
#error Unknown architecture
#endif

struct limine_paging_mode_response {
    uint64_t revision;
    uint64_t mode;
};

struct limine_paging_mode_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_paging_mode_response *) response;
    uint64_t mode;
    uint64_t max_mode;
    uint64_t min_mode;
};

/* 5-level paging */

#define LIMINE_5_LEVEL_PAGING_REQUEST                                          \
    {LIMINE_COMMON_MAGIC, 0x94469551da9b3192, 0xebe5e86db7382888}

LIMINE_DEPRECATED_IGNORE_START

struct LIMINE_DEPRECATED limine_5_level_paging_response {
    uint64_t revision;
};

struct LIMINE_DEPRECATED limine_5_level_paging_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_5_level_paging_response *) response;
};

LIMINE_DEPRECATED_IGNORE_END

/* MP */

#if LIMINE_API_REVISION >= 1
#define LIMINE_MP_REQUEST                                                      \
    {LIMINE_COMMON_MAGIC, 0x95a67b819a1b857e, 0xa0b61b723b6a73e0}
#define LIMINE_MP(TEXT) limine_mp_##TEXT
#else
#define LIMINE_SMP_REQUEST                                                     \
    {LIMINE_COMMON_MAGIC, 0x95a67b819a1b857e, 0xa0b61b723b6a73e0}
#define LIMINE_MP(TEXT) limine_smp_##TEXT
#endif

struct LIMINE_MP(info);

typedef void (*limine_goto_address)(struct LIMINE_MP(info) *);

#if defined(__x86_64__) || defined(__i386__)

#if LIMINE_API_REVISION >= 1
#define LIMINE_MP_X2APIC (1 << 0)
#else
#define LIMINE_SMP_X2APIC (1 << 0)
#endif

struct LIMINE_MP(info) {
    uint32_t processor_id;
    uint32_t lapic_id;
    uint64_t reserved;
    LIMINE_PTR(limine_goto_address) goto_address;
    uint64_t extra_argument;
};

struct LIMINE_MP(response) {
    uint64_t revision;
    uint32_t flags;
    uint32_t bsp_lapic_id;
    uint64_t cpu_count;
    LIMINE_PTR(struct LIMINE_MP(info) **) cpus;
};

#elif defined(__aarch64__)

struct LIMINE_MP(info) {
    uint32_t processor_id;
    uint32_t reserved1;
    uint64_t mpidr;
    uint64_t reserved;
    LIMINE_PTR(limine_goto_address) goto_address;
    uint64_t extra_argument;
};

struct LIMINE_MP(response) {
    uint64_t revision;
    uint64_t flags;
    uint64_t bsp_mpidr;
    uint64_t cpu_count;
    LIMINE_PTR(struct LIMINE_MP(info) **) cpus;
};

#elif defined(__riscv) && (__riscv_xlen == 64)

struct LIMINE_MP(info) {
    uint64_t processor_id;
    uint64_t hartid;
    uint64_t reserved;
    LIMINE_PTR(limine_goto_address) goto_address;
    uint64_t extra_argument;
};

struct LIMINE_MP(response) {
    uint64_t revision;
    uint64_t flags;
    uint64_t bsp_hartid;
    uint64_t cpu_count;
    LIMINE_PTR(struct LIMINE_MP(info) **) cpus;
};

#elif defined(__loongarch__) && (__loongarch_grlen == 64)

struct LIMINE_MP(info) {
    uint64_t reserved;
};

struct LIMINE_MP(response) {
    uint64_t cpu_count;
    LIMINE_PTR(struct LIMINE_MP(info) **) cpus;
};

#else
#error Unknown architecture
#endif

struct LIMINE_MP(request) {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct LIMINE_MP(response) *) response;
    uint64_t flags;
};

/* Memory map */

#define LIMINE_MEMMAP_REQUEST                                                  \
    {LIMINE_COMMON_MAGIC, 0x67cf3d9d378a806f, 0xe304acdfc50c3c62}

#define LIMINE_MEMMAP_USABLE                 0
#define LIMINE_MEMMAP_RESERVED               1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE       2
#define LIMINE_MEMMAP_ACPI_NVS               3
#define LIMINE_MEMMAP_BAD_MEMORY             4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#if LIMINE_API_REVISION >= 2
#define LIMINE_MEMMAP_EXECUTABLE_AND_MODULES 6
#else
#define LIMINE_MEMMAP_KERNEL_AND_MODULES 6
#endif
#define LIMINE_MEMMAP_FRAMEBUFFER 7

struct limine_memmap_entry {
    uint64_t base;
    uint64_t length;
    uint64_t type;
};

struct limine_memmap_response {
    uint64_t revision;
    uint64_t entry_count;
    LIMINE_PTR(struct limine_memmap_entry **) entries;
};

struct limine_memmap_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_memmap_response *) response;
};

/* Entry point */

#define LIMINE_ENTRY_POINT_REQUEST                                             \
    {LIMINE_COMMON_MAGIC, 0x13d86c035a1cd3e1, 0x2b0caa89d8f3026a}

typedef void (*limine_entry_point)(void);

struct limine_entry_point_response {
    uint64_t revision;
};

struct limine_entry_point_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_entry_point_response *) response;
    LIMINE_PTR(limine_entry_point) entry;
};

/* Executable File */

#if LIMINE_API_REVISION >= 2
#define LIMINE_EXECUTABLE_FILE_REQUEST                                         \
    {LIMINE_COMMON_MAGIC, 0xad97e90e83f1ed67, 0x31eb5d1c5ff23b69}
#else
#define LIMINE_KERNEL_FILE_REQUEST                                             \
    {LIMINE_COMMON_MAGIC, 0xad97e90e83f1ed67, 0x31eb5d1c5ff23b69}
#endif

#if LIMINE_API_REVISION >= 2
struct limine_executable_file_response {
#else
struct limine_kernel_file_response {
#endif
    uint64_t revision;
#if LIMINE_API_REVISION >= 2
    LIMINE_PTR(struct limine_file *) executable_file;
#else
    LIMINE_PTR(struct limine_file *) kernel_file;
#endif
};

#if LIMINE_API_REVISION >= 2
struct limine_executable_file_request {
#else
struct limine_kernel_file_request {
#endif
    uint64_t id[4];
    uint64_t revision;
#if LIMINE_API_REVISION >= 2
    LIMINE_PTR(struct limine_executable_file_response *) response;
#else
    LIMINE_PTR(struct limine_kernel_file_response *) response;
#endif
};

/* Module */

#define LIMINE_MODULE_REQUEST                                                  \
    {LIMINE_COMMON_MAGIC, 0x3e7e279702be32af, 0xca1c4f3bd1280cee}

#define LIMINE_INTERNAL_MODULE_REQUIRED   (1 << 0)
#define LIMINE_INTERNAL_MODULE_COMPRESSED (1 << 1)

struct limine_internal_module {
    LIMINE_PTR(const char *) path;
#if LIMINE_API_REVISION >= 3
    LIMINE_PTR(const char *) string;
#else
    LIMINE_PTR(const char *) cmdline;
#endif
    uint64_t flags;
};

struct limine_module_response {
    uint64_t revision;
    uint64_t module_count;
    LIMINE_PTR(struct limine_file **) modules;
};

struct limine_module_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_module_response *) response;

    /* Request revision 1 */
    uint64_t internal_module_count;
    LIMINE_PTR(struct limine_internal_module **) internal_modules;
};

/* RSDP */

#define LIMINE_RSDP_REQUEST                                                    \
    {LIMINE_COMMON_MAGIC, 0xc5e77b6b397e7b43, 0x27637845accdcf3c}

struct limine_rsdp_response {
    uint64_t revision;
#if LIMINE_API_REVISION >= 1
    uint64_t address;
#else
    LIMINE_PTR(void *) address;
#endif
};

struct limine_rsdp_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_rsdp_response *) response;
};

/* SMBIOS */

#define LIMINE_SMBIOS_REQUEST                                                  \
    {LIMINE_COMMON_MAGIC, 0x9e9046f11e095391, 0xaa4a520fefbde5ee}

struct limine_smbios_response {
    uint64_t revision;
#if LIMINE_API_REVISION >= 1
    uint64_t entry_32;
    uint64_t entry_64;
#else
    LIMINE_PTR(void *) entry_32;
    LIMINE_PTR(void *) entry_64;
#endif
};

struct limine_smbios_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_smbios_response *) response;
};

/* EFI system table */

#define LIMINE_EFI_SYSTEM_TABLE_REQUEST                                        \
    {LIMINE_COMMON_MAGIC, 0x5ceba5163eaaf6d6, 0x0a6981610cf65fcc}

struct limine_efi_system_table_response {
    uint64_t revision;
#if LIMINE_API_REVISION >= 1
    uint64_t address;
#else
    LIMINE_PTR(void *) address;
#endif
};

struct limine_efi_system_table_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_efi_system_table_response *) response;
};

/* EFI memory map */

#define LIMINE_EFI_MEMMAP_REQUEST                                              \
    {LIMINE_COMMON_MAGIC, 0x7df62a431d6872d5, 0xa4fcdfb3e57306c8}

struct limine_efi_memmap_response {
    uint64_t revision;
    LIMINE_PTR(void *) memmap;
    uint64_t memmap_size;
    uint64_t desc_size;
    uint64_t desc_version;
};

struct limine_efi_memmap_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_efi_memmap_response *) response;
};

/* Date at boot */

#if LIMINE_API_REVISION >= 3
#define LIMINE_DATE_AT_BOOT_REQUEST                                            \
    {LIMINE_COMMON_MAGIC, 0x502746e184c088aa, 0xfbc5ec83e6327893}
#else
#define LIMINE_BOOT_TIME_REQUEST                                               \
    {LIMINE_COMMON_MAGIC, 0x502746e184c088aa, 0xfbc5ec83e6327893}
#endif

#if LIMINE_API_REVISION >= 3
struct limine_date_at_boot_response {
#else
struct limine_boot_time_response {
#endif
    uint64_t revision;
#if LIMINE_API_REVISION >= 3
    int64_t timestamp;
#else
    int64_t boot_time;
#endif
};

#if LIMINE_API_REVISION >= 3
struct limine_date_at_boot_request {
#else
struct limine_boot_time_request {
#endif
    uint64_t id[4];
    uint64_t revision;
#if LIMINE_API_REVISION >= 3
    LIMINE_PTR(struct limine_date_at_boot_response *) response;
#else
    LIMINE_PTR(struct limine_boot_time_response *) response;
#endif
};

/* Executable address */

#if LIMINE_API_REVISION >= 2
#define LIMINE_EXECUTABLE_ADDRESS_REQUEST                                      \
    {LIMINE_COMMON_MAGIC, 0x71ba76863cc55f63, 0xb2644a48c516a487}
#else
#define LIMINE_KERNEL_ADDRESS_REQUEST                                          \
    {LIMINE_COMMON_MAGIC, 0x71ba76863cc55f63, 0xb2644a48c516a487}
#endif

#if LIMINE_API_REVISION >= 2
struct limine_executable_address_response {
#else
struct limine_kernel_address_response {
#endif
    uint64_t revision;
    uint64_t physical_base;
    uint64_t virtual_base;
};

#if LIMINE_API_REVISION >= 2
struct limine_executable_address_request {
#else
struct limine_kernel_address_request {
#endif
    uint64_t id[4];
    uint64_t revision;
#if LIMINE_API_REVISION >= 2
    LIMINE_PTR(struct limine_executable_address_response *) response;
#else
    LIMINE_PTR(struct limine_kernel_address_response *) response;
#endif
};

/* Device Tree Blob */

#define LIMINE_DTB_REQUEST                                                     \
    {LIMINE_COMMON_MAGIC, 0xb40ddb48fb54bac7, 0x545081493f81ffb7}

struct limine_dtb_response {
    uint64_t revision;
    LIMINE_PTR(void *) dtb_ptr;
};

struct limine_dtb_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_dtb_response *) response;
};

/* RISC-V Boot Hart ID */

#define LIMINE_RISCV_BSP_HARTID_REQUEST                                        \
    {LIMINE_COMMON_MAGIC, 0x1369359f025525f9, 0x2ff2a56178391bb6}

struct limine_riscv_bsp_hartid_response {
    uint64_t revision;
    uint64_t bsp_hartid;
};

struct limine_riscv_bsp_hartid_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_riscv_bsp_hartid_response *) response;
};

#ifdef __cplusplus
}
#endif

#endif
