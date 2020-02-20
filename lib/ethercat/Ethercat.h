/* * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * * * * * * * * * * * * *\
 *  Library implementing the software interface for the LAN9252 EtherCAT chip.          *
 *  Hardware consists of a 64-pin chip.                                                 *
 *      Connection is done in the hardware of the chip.                                 *
 *  Requires a device to be specified, assumes standard connections for the chip for    *
 *      that device.                                                                    *
 *                                                                                      *
 *  Created by P. Verton for the MARCH 3 team of Project MARCH                          *
 *  Adapted by M. van der Marel for the MARCH 4 team of Project MARCH                   *
 *  Date: 04-APR-2018                                                                   *
\* * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * * * * * * * * * * * * */

#ifndef ETHERCAT_
#define ETHERCAT_

#include <mbed.h>
#include "DBS_pindefs.h"
#include "Ethercat_defines.h"
#include "utypes.h"

#ifdef DEBUG_DEC
#undef DEBUG_DEC
#endif
#define DEBUG_DEC Serial *pc = NULL, int debugLevel = 0

typedef union _UINT
{
  uint32_t Int;
  int8_t Byte[4];
} UINT;
typedef union _UWORD
{
  uint16_t Word;
  uint8_t Byte[2];
} UWORD;

// miso
typedef union _bufferMiso
{
  _Rbuffer Struct;
  char Byte[MAX_PDO_SIZE];
} bufferMiso;

// mosi
typedef union _bufferMosi
{
  _Wbuffer Struct;
  char Byte[MAX_PDO_SIZE];
} bufferMosi;

class Ethercat
{
public:
  // Constructors
  Ethercat(PinName mosi, PinName miso, PinName sclk, PinName nChipSelect = NC, int PDORXsize = DEFAULT_PDO_SIZE,
           int PDOTXsize = DEFAULT_PDO_SIZE, DEBUG_DEC);

  // Public member functions
  // Updates all input variables and sends out all output variables to the EtherCAT device.
  void update(DEBUG_DEC);

  // Public class members
  static bufferMiso pdoTx;
  static bufferMosi pdoRx;

private:
  // Private member functions
  // Initializes the EtherCAT device and all variables.
  //  int return              a status code indicating how well it went.
  //           0              indicates successful initialisation.
  //          -1              indicates it couldn't read a test register.
  //          -2              indicates a timeout when waiting for the ready flag.
  int init(DEBUG_DEC);

  // Writes to the specified register.
  //  uint16_t    address     Address of register to write to
  //  uint32_t    data        Data to write to the specified register
  void write_register(uint16_t address, uint32_t data, DEBUG_DEC);

  // Reads from the specified register.
  //  uint16_t    address     Address of register to read from
  //  uint32_t    return      Data pointer to read into
  uint32_t read_register(uint16_t address, DEBUG_DEC);

  // Writes to the specified register indirectly.
  //  uint16_t    address     Address of register to write to
  //  uint32_t    data        Data to write to the specified register
  //[NOTE: I do not know what "indirectly" means in this context.]

  void write_register_indirect(uint16_t address, uint32_t data, DEBUG_DEC);

  // Reads from the specified register.
  //  uint16_t    address     Address of register to read from
  //  uint32_t    return      Data pointer to read into
  uint32_t read_register_indirect(uint16_t address, DEBUG_DEC);

  // Reads the EtherCAT process ram into the global MOSI buffer
  void read_process_ram(DEBUG_DEC);

  // Writes the global MISO buffer into the EtherCAT process ram
  void write_process_ram(DEBUG_DEC);

  // Clears the global MOSI buffer to all zeros.
  void clear_mosi_buffer(DEBUG_DEC);

  // Private class members
  SPI* m_chip;
  DigitalOut* m_nChipSelect;
  int status;
  int PDORX_size;
  int PDOTX_size;
};
#endif  // ETHERCAT_