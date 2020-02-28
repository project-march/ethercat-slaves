#ifndef ETHERCAT_DEFINES_
#define ETHERCAT_DEFINES_

#define BIT(x) (1 << (x))

#define MAX_PDO_SIZE 0x80      // Max size of a SyncManager
#define DEFAULT_PDO_SIZE 0x20  // Default size
//---- LAN9252 registers --------------------------------------------------------------------------

//---- access to EtherCAT registers -------------------

#define ECAT_CSR_DATA 0x0300  // EtherCAT CSR Interface Data Register
#define ECAT_CSR_CMD 0x0304   // EtherCAT CSR Interface Command Register

//---- access to EtherCAT process RAM -----------------

#define ECAT_PRAM_RD_ADDR_LEN 0x0308  // EtherCAT Process RAM Read Address and Length Register
#define ECAT_PRAM_RD_CMD 0x030C       // EtherCAT Process RAM Read Command Register
#define ECAT_PRAM_WR_ADDR_LEN 0x0310  // EtherCAT Process RAM Write Address and Length Register
#define ECAT_PRAM_WR_CMD 0x0314       // EtherCAT Process RAM Write Command Register

#define ECAT_PRAM_RD_FIFO 0x0000  // EtherCAT Process RAM Read Data FIFO
#define ECAT_PRAM_WR_FIFO 0x0020  // EtherCAT Process RAM Write Data FIFO

#define SM2_ADDR 0x1100  // SyncManager 2 Address (MOSI)
#define SM3_ADDR 0x1180  // SyncManager 3 Address (MISO)

//---- EtherCAT registers -----------------------------

#define AL_STATUS 0x0130    // AL status
#define WDOG_STATUS 0x0440  // watch dog status

//---- LAN9252 registers ------------------------------

#define HW_CFG 0x0074             // hardware configuration register
#define BYTE_TEST 0x0064          // byte order test register
#define BYTE_TEST_RES 0x87654321  // byte order test expected result
#define RESET_CTL 0x01F8          // reset register
#define ID_REV 0x0050             // chip ID and revision

//---- LAN9252 flags ------------------------------------------------------------------------------

#define ECAT_CSR_BUSY 0x80
#define PRAM_READ_BUSY BIT(31)
#define PRAM_READ_AVAIL BIT(0)
#define PRAM_WRITE_BUSY BIT(31)
#define PRAM_WRITE_AVAIL BIT(0)
#define READY 0x08000000

#define DIGITAL_RST 0x00000001
#define ETHERCAT_RST 0x00000040

//---- EtherCAT flags -----------------------------------------------------------------------------

// EtherCAT state machine

#define ESM_INIT 0x01    // init
#define ESM_PREOP 0x02   // pre-operational
#define ESM_BOOT 0x03    // bootstrap
#define ESM_SAFEOP 0x04  // safe-operational
#define ESM_OP 0x08      // operational

//--- ESC commands --------------------------------------------------------------------------------

#define ESC_WRITE 0x80
#define ESC_READ 0xC0

//---- SPI ----------------------------------------------------------------------------------------

#define COMM_SPI_READ 0x03
#define COMM_SPI_WRITE 0x02
#define COMM_SPI_FASTREAD 0x0B

#define DUMMY_BYTE 0xFF

#endif  // ETHERCAT_DEFINES_