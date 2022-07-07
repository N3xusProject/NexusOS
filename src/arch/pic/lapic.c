// Local APIC Registers
#define LAPIC_ID                        0x0020  // Local APIC ID
#define LAPIC_VER                       0x0030  // Local APIC Version
#define LAPIC_TPR                       0x0080  // Task Priority
#define LAPIC_APR                       0x0090  // Arbitration Priority
#define LAPIC_PPR                       0x00a0  // Processor Priority
#define LAPIC_EOI                       0x00b0  // EOI
#define LAPIC_RRD                       0x00c0  // Remote Read
#define LAPIC_LDR                       0x00d0  // Logical Destination
#define LAPIC_DFR                       0x00e0  // Destination Format
#define LAPIC_SVR                       0x00f0  // Spurious Interrupt Vector
#define LAPIC_ISR                       0x0100  // In-Service (8 registers)
#define LAPIC_TMR                       0x0180  // Trigger Mode (8 registers)
#define LAPIC_IRR                       0x0200  // Interrupt Request (8 registers)
#define LAPIC_ESR                       0x0280  // Error Status
#define LAPIC_ICRLO                     0x0300  // Interrupt Command
#define LAPIC_ICRHI                     0x0310  // Interrupt Command [63:32]
#define LAPIC_TIMER                     0x0320  // LVT Timer
#define LAPIC_THERMAL                   0x0330  // LVT Thermal Sensor
#define LAPIC_PERF                      0x0340  // LVT Performance Counter
#define LAPIC_LINT0                     0x0350  // LVT LINT0
#define LAPIC_LINT1                     0x0360  // LVT LINT1
#define LAPIC_ERROR                     0x0370  // LVT Error
#define LAPIC_TICR                      0x0380  // Initial Count (for Timer)
#define LAPIC_TCCR                      0x0390  // Current Count (for Timer)
#define LAPIC_TDCR                      0x03e0  // Divide Configuration (for Timer)

// Delivery Mode
#define ICR_FIXED                       0x00000000
#define ICR_LOWEST                      0x00000100
#define ICR_SMI                         0x00000200
#define ICR_NMI                         0x00000400
#define ICR_INIT                        0x00000500
#define ICR_STARTUP                     0x00000600

// Destination Mode
#define ICR_PHYSICAL                    0x00000000
#define ICR_LOGICAL                     0x00000800

// Delivery Status
#define ICR_IDLE                        0x00000000
#define ICR_SEND_PENDING                0x00001000

// Level
#define ICR_DEASSERT                    0x00000000
#define ICR_ASSERT                      0x00004000

// Trigger Mode
#define ICR_EDGE                        0x00000000
#define ICR_LEVEL                       0x00008000

// Destination Shorthand
#define ICR_NO_SHORTHAND                0x00000000
#define ICR_SELF                        0x00040000
#define ICR_ALL_INCLUDING_SELF          0x00080000
#define ICR_ALL_EXCLUDING_SELF          0x000c0000


#include <arch/pic/lapic.h>
#include <arch/io/io.h>                 //< For I/O calls.
#include <libkern/log.h>                //< For debug logs.

extern void* bsp_lapic_base;            //< Used so we can write to LAPICs MMIO registers.



/*
 * @brief   This function reads LAPIC 
 *          registers.
 *
 */
static uint32_t read(uint32_t reg)
{
    return *(volatile uint32_t*)(bsp_lapic_base + reg);
}



/*
 * @brief   This function writes LAPIC 
 *          registers.
 *
 */
static void write(uint32_t reg, uint32_t val)
{
    *(volatile uint32_t*)(bsp_lapic_base + reg) = val;
}


/*
 *  @brief Sends End Of Interrupt.
 *
 */
void lapic_send_eoi(void)
{
    write(LAPIC_EOI, 0);
}

void lapic_init(void) {
    // We are doing to disable 8259 PICs.
    outportb(0xA1, 0xFF);
    outportb(0x21, 0xFF);
    printf("<LAPIC_INIT>: 8258 PICs disabled.\n");

    write(LAPIC_DFR, 0xFFFFFFFF);            // Use FLAT model.
    write(LAPIC_LDR, 0x01000000);            // Ensure all CPUs use logical ID of 1 (logical ID: bits 24-27).

    printf("<LAPIC_INIT>: Destination format: FLAT, All CPUs have logical ID of 1.\n");

    // Configure SVR (Spurious Vector Register).
    // Bit 8 enables LAPIC.
    write(LAPIC_SVR, 0x100 | 0xFF);
    printf("<LAPIC_INIT>: Spurious Vector Register has been configured. Bit 8 set, LAPIC is now enabled!\n");
}
