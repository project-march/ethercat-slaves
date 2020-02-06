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

#include "Ethercat.h"

#ifdef DEBUG_IMP
    #undef DEBUG_IMP
#endif
#ifdef DEBUG_PRN
    #undef DEBUG_PRN
#endif
#ifdef DEBUG_ARG
    #undef DEBUG_ARG
#endif

#define DEBUG_IMP Serial * pc, const int debugLevel
#define DEBUG_PRN(fmt) if((pc != NULL) & (debugLevel > 0)) pc->printf fmt
#define DEBUG_ARG pc, debugLevel - 1

#define LAN_WAIT_TIME (125000) // microseconds

bufferMiso Ethercat::pdoTx = {};
bufferMosi Ethercat::pdoRx = {};

//Constructors
Ethercat::Ethercat(PinName mosi, PinName miso, PinName sclk, PinName nChipSelect, int PDORXsize, int PDOTXsize, DEBUG_IMP){
    m_chip = new SPI(mosi, miso, sclk);
    m_nChipSelect = new DigitalOut(nChipSelect);
    this->PDORX_size = PDORXsize;
    this->PDOTX_size = PDOTXsize;
    DEBUG_PRN(("\r\nEtherCAT device constructed with pinname arguments."));
    DEBUG_PRN(("\r\n\tmosi = %i.\r\n\tmiso = %i.\r\n\tsclk = %i.\r\n\tnCS = %i.",
        mosi, miso, sclk, nChipSelect));
    DEBUG_PRN(("\r\n\tchip @ 0x%08X.", m_chip));
    DEBUG_PRN(("\r\n\tchipselect @ 0x%08X.", m_nChipSelect));
    this->status = this->init(DEBUG_ARG);
} //Ethercat::Ethercat(PinName mosi, PinName miso, PinName sclk, PinName nChipSelect)

//Public member functions
void Ethercat::update(DEBUG_IMP)
{
    uint8_t operational = 0;
    uint16_t state;
    if(this->status)
    {
        DEBUG_PRN(("\r\nInitialization failed. Clearing mosi buffer."));
        this->clear_mosi_buffer(DEBUG_ARG);
        return;
    }

    //read EtherCAT State Machine status to see if we are in operational state.
    state = this->read_register_indirect(AL_STATUS, DEBUG_ARG);
    //check last 4 bits of read word
    operational = ( (state & 0x000F) == ESM_OP);

    if(!operational)
    {
        DEBUG_PRN(("\r\nNot in operational state. Clearing Rb buffer."));
        //If watchdog is active or we are not in operational state,
        //  reset the MOSI buffer.
        this->clear_mosi_buffer(DEBUG_ARG);
    }
    else
    {
        //Otherwise, transfer process data from the EtherCAT core
        //  to the MOSI buffer.
        this->read_process_ram(DEBUG_ARG);
    }
    //Always transfer process data from the MISO buffer to the EtherCAT core
    this->write_process_ram(DEBUG_ARG);
} //Ethercat::update()


//Private member functions
int Ethercat::init(DEBUG_IMP)
{
    // Chip must be deselected
    *(this->m_nChipSelect) = 1;
    // Setup the spi for 8 bit data, MODE 0 SPI,
    // second edge capture, with a 15.6MHz clock rate
    this->m_chip->format(8,0);
    this->m_chip->frequency(15625000);
    DEBUG_PRN(("\r\nInitialized SPI"));
    *(this->m_nChipSelect) = 0;

    // Wait for internal registers to initialize
    wait_us(LAN_WAIT_TIME);

    // check test register
    uint32_t testdata;
    int maxchecks = 10;
    for(int i = 0; i < maxchecks; i++)
    {
        // wait until registers are updated
        wait_us(LAN_WAIT_TIME/maxchecks);
        testdata = read_register(BYTE_TEST, DEBUG_ARG);
        if (testdata == BYTE_TEST_RES)
        {
            break;
        }
        if(i == maxchecks - 1)
        {
            DEBUG_PRN(("\r\nTest register wasn't correct."));
            return -1;
        }
    }

    //check ready flag
    uint32_t readydata;
    for(int i = 0; i < maxchecks; i++)
    {
        // wait for internal registers
        wait_us(LAN_WAIT_TIME/maxchecks);
        readydata = read_register(HW_CFG, DEBUG_ARG);
        if ((readydata & READY) != 0)
        {
            break;
        }
        if(i == maxchecks - 1)
        {
            DEBUG_PRN(("\r\nWaited too long for ready flag."));
            return -2;
        }
    }
    this->clear_mosi_buffer(DEBUG_ARG);
    return 0;
}   //Ethercat::init()

void Ethercat::write_register(uint16_t address, uint32_t data, DEBUG_IMP)
{
    UWORD addressBytes;
    UINT dataBytes;
    addressBytes.Word = address;
    dataBytes.Int = data;

    DEBUG_PRN(("\r\nWriting data 0x%08X to register 0x%04X.", dataBytes.Int, addressBytes.Word));

    //Selecting chip
    *(this->m_nChipSelect) = 0;

    //Sending write command.
    this->m_chip->write(COMM_SPI_WRITE);

    //Sending address.
    this->m_chip->write(addressBytes.Byte[1]);
    this->m_chip->write(addressBytes.Byte[0]);

    //Sending register data.
    for( int i = 0; i < 4; i++)
    {
        this->m_chip->write(dataBytes.Byte[i]);
    }

    //Deselecting chip
    *(this->m_nChipSelect) = 1;
    return;
}

uint32_t Ethercat::read_register(uint16_t address, DEBUG_IMP)
{
    UWORD addressBytes;
    UINT dataBytes;

    DEBUG_PRN(("\r\nReading from register 0x%04X.", address));
    //Selecting chip.
    *(this->m_nChipSelect) = 0;

    //Sends the SPI read command.
    this->m_chip->write(COMM_SPI_READ);

    //Sends the two bytes of the register address.
    addressBytes.Word = address;
    this->m_chip->write(addressBytes.Byte[1]);
    this->m_chip->write(addressBytes.Byte[0]);

    //Reads the four bytes of the specified register's data.
    for(int i = 0; i < 4; i++)
    {
        dataBytes.Byte[i] = this->m_chip->write(DUMMY_BYTE);
    }
    //Deselecting chip.
    *(this->m_nChipSelect) = 1;
    DEBUG_PRN(("\r\nData = 0x%08X.", dataBytes.Int));
    return dataBytes.Int;
}

void Ethercat::write_register_indirect(uint16_t address, uint32_t data, DEBUG_IMP)
{
    DEBUG_PRN(("\r\nWriting data 0x%08X to register 0x%02X indirectly.", data, address));
    UWORD addressBytes;
    UINT tempInt;
    addressBytes.Word = address;

    //Write the data.
    this->write_register(ECAT_CSR_DATA, data, DEBUG_ARG);

    DEBUG_PRN(("\r\n\tWriting command to ECAT_CSR_CMD registry 0x%02X.", ECAT_CSR_CMD));
    tempInt.Byte[0] = addressBytes.Byte[0];
    tempInt.Byte[2] = addressBytes.Byte[1];
    tempInt.Byte[2] = 4;
    tempInt.Byte[3] = ESC_WRITE;
    this->write_register(ECAT_CSR_CMD, tempInt.Int, DEBUG_ARG);

    //wait for sommand execution.
    do
    {
        tempInt.Int = this->read_register(ECAT_CSR_CMD, DEBUG_ARG);
    }
    while(tempInt.Byte[3] & ECAT_CSR_BUSY);
    return;
}

uint32_t Ethercat::read_register_indirect(uint16_t address, DEBUG_IMP)
{
    DEBUG_PRN(("\r\nReading from indirectly addressable register 0x%02X.", address));
    UINT tempInt;
    UWORD addressBytes;
    addressBytes.Word = address;

    DEBUG_PRN(("\r\n\tWriting command to ECAT_CSR_CMD registry 0x%02X.", ECAT_CSR_CMD));
    tempInt.Byte[0] = addressBytes.Byte[0];
    tempInt.Byte[1] = addressBytes.Byte[1];
    tempInt.Byte[2] = 4;
    tempInt.Byte[3] = ESC_READ;
    this->write_register(ECAT_CSR_CMD, tempInt.Int, DEBUG_ARG);

    //Wait for command execution.
    do
    {
        tempInt.Int = this->read_register(ECAT_CSR_CMD, DEBUG_ARG);
    }
    while(tempInt.Byte[3] & ECAT_CSR_BUSY);

    //Read the required register.
    tempInt.Int = this->read_register(ECAT_CSR_DATA, DEBUG_ARG);
    DEBUG_PRN(("\r\nRead data 0x%08X from register 0x%02X indirectly.", tempInt.Int, address));

    return tempInt.Int;
}

void Ethercat::read_process_ram(DEBUG_IMP)
{
    DEBUG_PRN(("\r\nReading %i bytes of MOSI process RAM.", this->PDORX_size));
    UINT tempInt;

    // Write length in MS 2 bytes, address in LS two bytes
    uint32_t addr_len_data = ((this->PDORX_size << 16) | SM2_ADDR);
    this->write_register(ECAT_PRAM_RD_ADDR_LEN, addr_len_data, DEBUG_ARG);

    // Write BUSY bit in PRAM read command register and wait for AVAIL bit
    this->write_register(ECAT_PRAM_RD_CMD, PRAM_READ_BUSY, DEBUG_ARG);
    do
    {
        tempInt.Int = this->read_register(ECAT_PRAM_RD_CMD, DEBUG_ARG);
    }
    while(!(tempInt.Byte[0] & PRAM_READ_AVAIL));

    //Selecting chip.
    *(this->m_nChipSelect) = 0;

    // Write fast read command, then address, then dummy byte
    DEBUG_PRN(("\r\nWrites read command and address 0x0000."));
    this->m_chip->write(COMM_SPI_FASTREAD);
    this->m_chip->write((ECAT_PRAM_RD_FIFO >> 8) & 0xFF);      // most significant byte
    this->m_chip->write(ECAT_PRAM_RD_FIFO & 0xFF);             // least significant byte
    this->m_chip->write(DUMMY_BYTE);

    //Reading bytes from specified register.
    for(int i = 0; i < this->PDORX_size; i++)
    {
        pdoRx.Byte[i] = this->m_chip->write(DUMMY_BYTE);
        DEBUG_PRN(("%spdoRx.Byte[%i] = 0x%02X.", ( ( (i%4) == 0)? "\r\n" : "\t"), i, pdoRx.Byte[i]));

    }

    //Deselecting chip.
    *(this->m_nChipSelect) = 1;

    return;
}

void Ethercat::write_process_ram(DEBUG_IMP)
{
    DEBUG_PRN(("\r\nWriting %i bytes of MISO process RAM.", this->PDOTX_size));
    UINT tempInt;

    // Write length in MS 2 bytes, address in LS two bytes
    uint32_t addr_len_data = ((this->PDOTX_size << 16) | SM3_ADDR);
    this->write_register(ECAT_PRAM_WR_ADDR_LEN, addr_len_data, DEBUG_ARG);

    // Write BUSY bit in PRAM read command register and wait for AVAIL bit
    this->write_register(ECAT_PRAM_WR_CMD, PRAM_WRITE_BUSY, DEBUG_ARG);
    do
    {
        tempInt.Int = this->read_register(ECAT_PRAM_WR_CMD, DEBUG_ARG);
    }
    while(!(tempInt.Byte[0] & PRAM_WRITE_AVAIL));

    DEBUG_PRN(("\r\nFIFO count: %i", (tempInt.Byte[1] & 0xF1)));

     //Selecting chip.
    *(this->m_nChipSelect) = 0;

    DEBUG_PRN(("\r\nWrites write command and address 0x0020."));
    this->m_chip->write(COMM_SPI_WRITE);
    this->m_chip->write((ECAT_PRAM_WR_FIFO >> 8) & 0xFF);        // most significant byte
    this->m_chip->write(ECAT_PRAM_WR_FIFO & 0xFF);               // least significant byte

    //Writing bytes to specified register.
    for(int i = 0; i < this->PDOTX_size; i++)
    {
        this->m_chip->write(pdoTx.Byte[i]);
        DEBUG_PRN(("%spdoTx.Byte[%i] = 0x%02X.", ( ( (i%4) == 0)? "\r\n" : "\t"), i, pdoTx.Byte[i]));
    }

    //Deselecting chip.
    *(this->m_nChipSelect) = 1;
   
    return;
}

void Ethercat::clear_mosi_buffer(DEBUG_IMP)
{
    // Set all mosi bytes to zero
    DEBUG_PRN(("\r\nSetting all MOSI bytes to 0."));
    for(int i = 0; i < this->PDORX_size; i++)
    {
        pdoRx.Byte[i] = 0;
        DEBUG_PRN(("%spdoRx.Byte[%i] = 0x%02X.", ( ( (i%4) == 0)? "\r\n" : "\t"), i, pdoRx.Byte[i]));
    }
    return;
}

