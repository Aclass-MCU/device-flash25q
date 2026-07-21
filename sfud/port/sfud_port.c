#include <sfud.h>
#include <stdarg.h>
#include <stdio.h>
#include "aDrv_qspi.h"
#include "aShell.h"

#define FLASH_PRESCALER 4
#define FLASH_QSPI_SIZE 25

static aDrvQspiHandle_t *g_qspi = NULL;
static bool g_addr_4byte = false;
static char log_buf[256];
static char dbg_buf[128];

extern aShellHandle_t shell_handle;

void sfud_dbg_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(log_buf, sizeof(log_buf), fmt, args);
    va_end(args);
    aShellPrint(&shell_handle, "%s", log_buf);
}

void aDevFlash25qPortSetQspiHandle(aDrvQspiHandle_t *handle)
{
    g_qspi = handle;
}

static uint32_t sfud_lines_to_ccr(uint8_t lines, uint32_t bit_pos)
{
    uint32_t val = 0;
    if (lines >= 1) val |= 1u;
    if (lines >= 2) val |= 2u;
    return val << bit_pos;
}

static uint32_t sfud_addr_size_to_ccr(uint32_t bits)
{
    switch (bits) {
    case 8:  return 0;
    case 16: return ADRV_QSPI_ADDR_16BIT;
    case 24: return ADRV_QSPI_ADDR_24BIT;
    case 32: return ADRV_QSPI_ADDR_32BIT;
    default: return 0;
    }
}

static sfud_err qspi_command(uint8_t instruction, uint32_t address, uint32_t addr_size, uint32_t addr_mode,
                             uint8_t *tx_data, uint32_t tx_size,
                             uint8_t *rx_data, uint32_t rx_size,
                             uint32_t dummy_cycles)
{
    aDrvQspiCmd_t cmd = {0};

    cmd.Instruction             = instruction;
    cmd.InstructionMode         = ADRV_QSPI_INST_1_LINE;
    cmd.Address                 = address;
    cmd.AddressSize             = addr_size;
    cmd.AddressMode             = addr_mode;
    cmd.DataMode                = (tx_size > 0 || rx_size > 0) ? ADRV_QSPI_DATA_1_LINE : 0;
    cmd.NbData                  = (rx_size > 0) ? rx_size : tx_size;
    cmd.DummyCycles             = dummy_cycles;
    cmd.FunctionalMode          = (rx_size > 0) ? ADRV_QSPI_FMODE_INDIRECT_READ
                                                : ADRV_QSPI_FMODE_INDIRECT_WRITE;

    if (instruction == 0xB7) g_addr_4byte = true;
    if (instruction == 0xE9) g_addr_4byte = false;

    aDrvStatus_t ac = aDrvQspiCommand(g_qspi, &cmd);
    sfud_dbg_printf("qspi_cmd: ins=0x%02X nb=%lu rx=%lu fmode=%lu ret=%d\r\n",
           instruction, cmd.NbData, rx_size, (unsigned long)cmd.FunctionalMode, ac);

    if (ac != ADRV_OK)
        return SFUD_ERR_TIMEOUT;

    if (tx_size > 0 && tx_data != NULL)
    {
        aDrvStatus_t at = aDrvQspiTransmit(g_qspi, tx_data, tx_size);
        sfud_dbg_printf("qspi_tx: size=%lu ret=%d\r\n", tx_size, at);
        if (at != ADRV_OK)
            return SFUD_ERR_TIMEOUT;
    }
    else if (rx_size > 0 && rx_data != NULL)
    {
        aDrvStatus_t ar = aDrvQspiReceive(g_qspi, rx_data, rx_size);
        if (rx_size <= 8) {
            int p = 0;
            for (uint32_t q = 0; q < rx_size && p < (int)sizeof(dbg_buf) - 4; q++)
                p += snprintf(dbg_buf + p, sizeof(dbg_buf) - p, "%02X ", rx_data[q]);
            sfud_dbg_printf("qspi_rx: ret=%d data=[%s]\r\n", ar, dbg_buf);
        } else {
            sfud_dbg_printf("qspi_rx: ret=%d size=%lu\r\n", ar, rx_size);
        }
        if (ar != ADRV_OK)
            return SFUD_ERR_TIMEOUT;
    }

    return SFUD_SUCCESS;
}

static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size,
                               uint8_t *read_buf, size_t read_size)
{
    (void)spi;
    uint8_t instruction;
    uint32_t address = 0;
    uint32_t addr_size = 0;
    uint32_t addr_mode = 0;
    uint32_t addr_bytes = 0;
    uint32_t dummy_cycles = 0;

    if (write_size == 0)
        return SFUD_ERR_NOT_FOUND;

    instruction = write_buf[0];

    if (read_size > 0)
    {
        if (write_size >= 4)
        {
            if (g_addr_4byte)
            {
                address = ((uint32_t)write_buf[1] << 24)
                        | ((uint32_t)write_buf[2] << 16)
                        | ((uint32_t)write_buf[3] << 8)
                        | ((uint32_t)write_buf[4]);
                addr_size  = ADRV_QSPI_ADDR_32BIT;
                addr_bytes = 4;
            }
            else
            {
                address = ((uint32_t)write_buf[1] << 16)
                        | ((uint32_t)write_buf[2] << 8)
                        | ((uint32_t)write_buf[3]);
                addr_size  = ADRV_QSPI_ADDR_24BIT;
                addr_bytes = 3;
            }
            addr_mode = ADRV_QSPI_ADDR_1_LINE;

            uint32_t data_start = 1 + addr_bytes;
            if (write_size > data_start)
                dummy_cycles = (write_size - data_start) * 8;
        }

        return qspi_command(instruction, address, addr_size, addr_mode,
                           NULL, 0, read_buf, read_size, dummy_cycles);
    }
    else
    {
        return qspi_command(instruction, 0, 0, 0,
                            (uint8_t *)write_buf + 1, write_size - 1, NULL, 0, 0);
    }
}

#ifdef SFUD_USING_QSPI
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr,
                          sfud_qspi_read_cmd_format *qspi_read_cmd_format,
                          uint8_t *read_buf, size_t read_size)
{
    (void)spi;
    aDrvQspiCmd_t cmd = {0};

    cmd.Instruction        = qspi_read_cmd_format->instruction;
    cmd.InstructionMode    = sfud_lines_to_ccr(qspi_read_cmd_format->instruction_lines, ADRV_QSPI_INST_POS);
    cmd.Address            = addr;
    cmd.AddressSize        = sfud_addr_size_to_ccr(qspi_read_cmd_format->address_size);
    cmd.AddressMode        = sfud_lines_to_ccr(qspi_read_cmd_format->address_lines, ADRV_QSPI_ADDR_POS);
    cmd.DataMode           = sfud_lines_to_ccr(qspi_read_cmd_format->data_lines, ADRV_QSPI_DATA_POS);
    cmd.NbData             = read_size;
    cmd.DummyCycles        = qspi_read_cmd_format->dummy_cycles;
    cmd.FunctionalMode     = ADRV_QSPI_FMODE_INDIRECT_READ;

    if (aDrvQspiCommand(g_qspi, &cmd) != ADRV_OK)
        return SFUD_ERR_TIMEOUT;

    if (aDrvQspiReceive(g_qspi, read_buf, read_size) != ADRV_OK)
        return SFUD_ERR_TIMEOUT;

    return SFUD_SUCCESS;
}
#endif /* SFUD_USING_QSPI */

sfud_err sfud_spi_port_init(sfud_flash *flash)
{
    flash->spi.wr     = spi_write_read;
#ifdef SFUD_USING_QSPI
    flash->spi.qspi_read = qspi_read;
#endif
    flash->spi.lock   = NULL;
    flash->spi.unlock = NULL;
    flash->spi.user_data = (void *)g_qspi;
    flash->retry.delay = NULL;
    flash->retry.times = 10000;

    return SFUD_SUCCESS;
}

void sfud_log_debug(const char *file, const long line, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    va_end(args);
    aShellPrint(&shell_handle, "[SFUD](%s:%ld) %s\r\n", file, line, log_buf);
}

void sfud_log_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    va_end(args);
    aShellPrint(&shell_handle, "[SFUD]%s\r\n", log_buf);
}
