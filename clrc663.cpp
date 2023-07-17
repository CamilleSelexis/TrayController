#include "api/Print.h"
#include <Arduino.h>
#include <Wire.h>
#include "clrc663.h"

#define DEBUG_CLR 0

#if DEBUG_CLR
#define PRINTLN(str);        Serial.println(str);
#define PRINT(str);          Serial.print(str);
#define PRINTHEX(str,flag);  Serial.print(str,flag);
#define PRINT_BLOCK(a,b);    print_block(a,b);
#else
#define PRINTLN(str);
#define PRINT(str); 
#define PRINTHEX(str,flag);
#define PRINT_BLOCK(a,b); 
#endif

Clrc663::Clrc663(uint8_t address) :
  addr(address)
{
	
}

int Clrc663::init(uint8_t address)
{
  
  addr = address;
  PRINTLN("\n**************************");
  PRINTLN("* Selexis reader          ");
  PRINT("* address: ");
  PRINTHEX(addr,HEX);
  //Try to communicate with the CLRC663 on the I2C bus, if we receive nothing then no need to try to communicate with the reader
  int ver = read_reg(CLRC663_REG_VERSION);

  PRINT("\n* version: ");
  PRINTHEX(ver,HEX);
  if(ver != 0){
    PRINT("Reader addr : 0x");PRINTHEX(addr,HEX);
  //Serial.println(ver,HEX);
  }
  PRINT("\n--------------------------\n");
	return ver;
}

void Clrc663::pcaselect(uint8_t paddr, uint8_t channel)
{
  
  if (channel > 7) return;
  Wire.beginTransmission(paddr);
  Wire.write(1 << channel);
  Wire.endTransmission(); 

}
void Clrc663::pcaclose(uint8_t paddr, uint8_t channel)
{
  PRINTLN("PCA select ");
  PRINT("device address: ");
  PRINTHEX(paddr,HEX);
  if (channel > 7) return;
  Wire.beginTransmission(paddr);
  Wire.write(channel >> channel);
  Wire.endTransmission(); 
  PRINTLN("port close "); 
}



// ---------------------------------------------------------------------------
// Register interaction functions.
// ---------------------------------------------------------------------------
uint8_t Clrc663::read_reg(uint8_t reg) 
{
  //long time_1 = millis();
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  //long time_2 = millis();
  Wire.requestFrom((int)addr, (int)1);
  //long time_3 = millis();
  return Wire.read();
}

void Clrc663::write_reg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

int Clrc663::read_regs(uint8_t reg, uint8_t* values, uint8_t len)
{
  if(values == NULL)
    return -1;
  if(len == 0)
    return -1;
  // you check register address validity here before permorming action
    
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
    
  //read values
  Wire.requestFrom((int)addr, (int)len);
  uint8_t *ptr = values;
  int n = 0;
  for( n = 0; n< len; n++){
    *ptr++ = Wire.read();
  }
  return n;
}

int Clrc663::write_regs(uint8_t reg, const uint8_t* values, uint8_t len) 
{
	if(values == NULL)
    return -1;

  if(len == 0)
    return -1;
  // Start I2C transaction
  Wire.beginTransmission(addr);
  
  // Set the register address
  Wire.write(reg);
  
  // Write values
  uint8_t *ptr = (uint8_t*)values;
  int n = 0;
  for( n = 0; n< len; n++){
    Wire.write(*ptr++);
  }
    
  // End the transaction
 	Wire.endTransmission();
    return n;
}

void Clrc663::write_fifo(const uint8_t* data, uint16_t len) 
{
  write_regs(CLRC663_REG_FIFODATA, data,len);
}

void Clrc663::read_fifo(uint8_t* rx, uint16_t len) 
{
  read_regs(CLRC663_REG_FIFODATA,rx,len);
}


// ---------------------------------------------------------------------------
// Command functions.
// ---------------------------------------------------------------------------

void Clrc663::cmd_read_E2(uint16_t address, uint16_t length) {
  uint8_t parameters[3] = {(uint8_t) (address >> 8), (uint8_t) (address & 0xFF), length};
  flush_fifo();
  write_fifo(parameters, 3);
  write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_READE2);
}

void Clrc663::cmd_load_reg(uint16_t address, uint8_t regaddr, uint16_t length) {
  uint8_t parameters[4] = {(uint8_t) (address >> 8), (uint8_t) (address & 0xFF), regaddr, length};
  flush_fifo();
  write_fifo(parameters, 4);
  write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_LOADREG);
}



void Clrc663::cmd_load_protocol(uint8_t rx, uint8_t tx) {
  uint8_t parameters[2] = {rx, tx};
  flush_fifo();
  write_fifo(parameters, 2);
  write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_LOADPROTOCOL);
}

void Clrc663::transmit_enable(bool enable)
{
  write_reg(CLRC663_REG_DRVMOD, enable ? 0x89 : 0x81);
}

void Clrc663::cmd_transceive(const uint8_t* data, uint16_t len) {
  cmd_idle();
  flush_fifo();
  write_fifo(data, len);
  write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_TRANSCEIVE);
}

void Clrc663::cmd_idle() {
  write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_IDLE);
}

void Clrc663::cmd_load_key_E2(uint8_t key_nr) {
  uint8_t parameters[1] = {key_nr};
  flush_fifo();
  write_fifo(parameters, 1);
  write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_LOADKEYE2);
}

void Clrc663::cmd_auth(uint8_t key_type, uint8_t block_address, const uint8_t* card_uid) {
  cmd_idle();
  uint8_t parameters[6] = {key_type, block_address, card_uid[0], card_uid[1], card_uid[2], card_uid[3]};
  flush_fifo();
  write_fifo(parameters, 6);
  write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_MFAUTHENT);
}

void Clrc663::cmd_load_key(const uint8_t* key) {
  cmd_idle();
  flush_fifo();
  write_fifo(key, 6);
  write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_LOADKEY);
}

// ---------------------------------------------------------------------------
// Utility functions.
// ---------------------------------------------------------------------------

void Clrc663::flush_fifo() 
{
  write_reg(CLRC663_REG_FIFOCONTROL, 1<<4);
}

uint16_t Clrc663::fifo_length() {
  // should do 512 byte fifo handling here.
  return read_reg(CLRC663_REG_FIFOLENGTH);
}

void Clrc663::clear_irq0() 
{
  write_reg(CLRC663_REG_IRQ0, (uint8_t) ~(1<<7));
}

void Clrc663::clear_irq1() {
  write_reg(CLRC663_REG_IRQ1, (uint8_t) ~(1<<7));
}
uint8_t Clrc663::clrc_irq0() {
  return read_reg(CLRC663_REG_IRQ0);
}

uint8_t Clrc663::clrc_irq1() {
  return read_reg(CLRC663_REG_IRQ1);
}

uint8_t Clrc663::transfer_E2_page(uint8_t* dest, uint8_t page) {
  cmd_read_E2(page*64, 64);
  uint8_t res = fifo_length();
  read_fifo(dest, 64);
  return res;
}

void Clrc663::print_block(const uint8_t* data, uint16_t len) {
  
//#if DEBUG_CLR
  uint16_t i;
  for (i=0; i < len; i++) {
    Serial.print("0x");
    Serial.print(data[i],HEX);
    Serial.print(" ");
  }
//#endif
}

void Clrc663::print_buffer_hex(const uint8_t* data, uint16_t len)
{
  uint16_t i;
  for (i=0; i < len; i++) {
    Serial.print("0x");
    Serial.print(data[i],HEX);
    Serial.print(" ");
  }
}

// ---------------------------------------------------------------------------
// Timer functions
// ---------------------------------------------------------------------------
void Clrc663::activate_timer(uint8_t timer, uint8_t active) {
  write_reg(CLRC663_REG_TCONTROL, ((active << timer) << 4) | (1 << timer));
}

void Clrc663::timer_set_control(uint8_t timer, uint8_t value) {
  write_reg(CLRC663_REG_T0CONTROL + (5 * timer), value);
}
void Clrc663::timer_set_reload(uint8_t timer, uint16_t value) {
  write_reg(CLRC663_REG_T0RELOADHI + (5 * timer), value >> 8);
  write_reg(CLRC663_REG_T0RELOADLO + (5 * timer), value & 0xFF);
}
void Clrc663::timer_set_value(uint8_t timer, uint16_t value) {
  write_reg(CLRC663_REG_T0COUNTERVALHI + (5 * timer), value >> 8);
  write_reg(CLRC663_REG_T0COUNTERVALLO + (5 * timer), value & 0xFF);
}
uint16_t Clrc663::timer_get_value(uint8_t timer) {
  uint16_t res = read_reg(CLRC663_REG_T0COUNTERVALHI + (5 * timer)) << 8;
  res += read_reg(CLRC663_REG_T0COUNTERVALLO + (5 * timer));
  return res;
}

// ---------------------------------------------------------------------------
// From documentation
// ---------------------------------------------------------------------------
void Clrc663::AN11145_start_IQ_measurement() {
  // Part-1, configurate LPCD Mode
  // Please remove any PICC from the HF of the reader.
  // "I" and the "Q" values read from reg 0x42 and 0x43
  // shall be used in part-2 "Detect PICC"
  //  reset CLRC663 and idle
  write_reg(0, 0x1F);
  // Should sleep here... for 50ms... can do without.
  write_reg(0, 0);
  // disable IRQ0, IRQ1 interrupt sources
  write_reg(0x06, 0x7F);
  write_reg(0x07, 0x7F);
  write_reg(0x08, 0x00);
  write_reg(0x09, 0x00);
  write_reg(0x02, 0xB0);  // Flush FIFO
  // LPCD_config
  write_reg(0x3F, 0xC0);  // Set Qmin register
  write_reg(0x40, 0xFF);  // Set Qmax register
  write_reg(0x41, 0xC0);  // Set Imin register
  write_reg(0x28, 0x89);  // set DrvMode register
  // Execute trimming procedure
  write_reg(0x1F, 0x00);  // Write default. T3 reload value Hi
  write_reg(0x20, 0x10);  // Write default. T3 reload value Lo
  write_reg(0x24, 0x00);  // Write min. T4 reload value Hi
  write_reg(0x25, 0x05);  // Write min. T4 reload value Lo
  write_reg(0x23, 0xF8);  // Config T4 for AutoLPCD&AutoRestart.Set AutoTrimm bit.Start T4.
  write_reg(0x43, 0x40);  // Clear LPCD result
  write_reg(0x38, 0x52);  // Set Rx_ADCmode bit
  write_reg(0x39, 0x03);  // Raise receiver gain to maximum
  write_reg(0x00, 0x01);  // Execute Rc663 command "Auto_T4" (Low power card detection and/or Auto trimming)
}

void Clrc663::AN11145_stop_IQ_measurement() {
  // Flush cmd and Fifo
  write_reg(0x00, 0x00);
  write_reg(0x02, 0xB0);
  write_reg(0x38, 0x12);  // Clear Rx_ADCmode bit
  //> ------------ I and Q Value for LPCD ----------------
  // read_reg(CLRC663_REG_LPCD_I_RESULT) & 0x3F
  // read_reg(CLRC663_REG_LPCD_Q_RESULT) & 0x3F
}

void Clrc663::AN1102_recommended_registers_skip(uint8_t protocol, uint8_t skip) {
  switch (protocol) {
    case CLRC663_PROTO_ISO14443A_106_MILLER_MANCHESTER:
      {
        const uint8_t buf[] = CLRC663_RECOM_14443A_ID1_106;
        write_regs(CLRC663_REG_DRVMOD+skip, buf+skip, sizeof(buf)-skip);
      }
      break;
    case CLRC663_PROTO_ISO14443A_212_MILLER_BPSK:
      {
        const uint8_t buf[] = CLRC663_RECOM_14443A_ID1_212;
        write_regs(CLRC663_REG_DRVMOD+skip, buf+skip, sizeof(buf)-skip);
      }
      break;
    case CLRC663_PROTO_ISO14443A_424_MILLER_BPSK:
      {
        const uint8_t buf[] = CLRC663_RECOM_14443A_ID1_424;
        write_regs(CLRC663_REG_DRVMOD+skip, buf+skip, sizeof(buf)-skip);
      }
      break;
    case CLRC663_PROTO_ISO14443A_848_MILLER_BPSK:
      {
        const uint8_t buf[] = CLRC663_RECOM_14443A_ID1_848;
        write_regs(CLRC663_REG_DRVMOD+skip, buf+skip, sizeof(buf)-skip);
      }
      break;
  }
}
void Clrc663::AN1102_recommended_registers(uint8_t protocol) {
  AN1102_recommended_registers_skip(protocol, 0);
}

void Clrc663::AN1102_recommended_registers_no_transmitter(uint8_t protocol) {
  AN1102_recommended_registers_skip(protocol, 5);
}


// ---------------------------------------------------------------------------
// ISO 14443A
// ---------------------------------------------------------------------------
void Clrc663::iso14443a_init()
{
  //write_reg(WaterLevel, 0xFE);        //Set WaterLevel =(FIFO length -1),cause fifo length has been set to 255=0xff,so water level is oxfe
  //write_reg(RxBitCtrl, 0x80);         //RxBitCtrl_Reg(0x0c)  Received bit after collision are replaced with 1.
  write_reg(CLRC663_REG_DRVMOD, 0x80);  //DrvMod reg(0x28), Tx2Inv=1,Inverts transmitter 1 at TX1 pin
  write_reg(CLRC663_REG_TXAMP,  0x00);  // TxAmp_Reg(0x29),output amplitude  0: TVDD -100 mV(maxmum)
  write_reg(CLRC663_REG_DRVCON, 0x01);  // TxCon register (address 2Ah),TxEnvelope
  write_reg(CLRC663_REG_TXL,    0x05);  //
  write_reg(CLRC663_REG_RXSOFD, 0x00);  //
  write_reg(CLRC663_REG_RCV,    0x12);  //  
}
uint16_t Clrc663::iso14443a_REQA() {
  return iso14443a_WUPA_REQA(CLRC663_ISO14443_CMD_REQA);
}
uint16_t Clrc663::iso14443a_WUPA() {
  return iso14443a_WUPA_REQA(CLRC663_ISO14443_CMD_WUPA);
}

uint16_t Clrc663::iso14443a_WUPA_REQA(uint8_t instruction) {
  cmd_idle();
  // AN1102_recommended_registers_no_transmitter(CLRC663_PROTO_ISO14443A_106_MILLER_MANCHESTER);
  flush_fifo();

  // Set register such that we sent 7 bits, set DataEn such that we can send
  // data.
  write_reg(CLRC663_REG_TXDATANUM, 7 | CLRC663_TXDATANUM_DATAEN);

  // disable the CRC registers.
  write_reg(CLRC663_REG_TXCRCPRESET, CLRC663_RECOM_14443A_CRC | CLRC663_CRC_OFF);
  write_reg(CLRC663_REG_RXCRCCON, CLRC663_RECOM_14443A_CRC | CLRC663_CRC_OFF);

  write_reg(CLRC663_REG_RXBITCTRL, 0);
  // ready the request.
  uint8_t send_req[] = {instruction};

  // clear interrupts
  clear_irq0();
  clear_irq1();

  // enable the global IRQ for Rx done and Errors.
  write_reg(CLRC663_REG_IRQ0EN, CLRC663_IRQ0EN_RX_IRQEN | CLRC663_IRQ0EN_ERR_IRQEN);
  write_reg(CLRC663_REG_IRQ1EN, CLRC663_IRQ1EN_TIMER0_IRQEN);  // only trigger on timer for irq1
  // configure a timeout timer.
  bool timer_for_timeout = 0;

  // Set timer to 221 kHz clock, start at the end of Tx.
  timer_set_control(timer_for_timeout, CLRC663_TCONTROL_CLK_211KHZ | CLRC663_TCONTROL_START_TX_END);
  // Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
  // FWI defaults to four... so that would mean wait for a maximum of ~ 5ms

  timer_set_reload(timer_for_timeout, 1000);  // 1000 ticks of 5 usec is 5 ms.
  timer_set_value(timer_for_timeout, 1000);
  
  // Go into send, then straight after in receive.
  cmd_transceive(send_req, 1);
  PRINTLN("Sending REQA");
  long count = 0;
  // block until we are done
  bool irq1_value = 0;
  while (!(irq1_value & (1 << timer_for_timeout))) {
    count++;
    irq1_value = clrc_irq1();
    if (irq1_value & CLRC663_IRQ1_GLOBAL_IRQ) {  // either ERR_IRQ or RX_IRQ
      break;  // stop polling irq1 and quit the timeout loop.
    }
    if(count > 5){
      break;
    }
  }
  PRINTLN("After waiting for answer\n");
  //delay(100);
  cmd_idle();
  // if no Rx IRQ, or if there's an error somehow, return 0
  uint8_t irq0 = clrc_irq0();
  if ((!(irq0 & CLRC663_IRQ0_RX_IRQ)) || (irq0 & CLRC663_IRQ0_ERR_IRQ)) {
    PRINT("No RX, irq1: 0x");
    PRINTHEX(irq1_value,HEX);
    PRINT(" irq0: ");
    PRINTHEX(irq0,HEX);
    PRINTLN(" ");
    //CLRC663_PRINTF("No RX, irq1: %hhx irq0: %hhx\n", irq1_value, irq0);
    return 0;
  }

  uint8_t rx_len = fifo_length();
  uint16_t res;
  PRINT("rx_len: 0x"); PRINTHEX(rx_len, HEX);PRINTLN(" ");
  //CLRC663_PRINTF("rx_len: %hhd\n", rx_len);
  if (rx_len == 2) {  // ATQA should answer with 2 bytes.
    read_fifo((uint8_t*) &res, rx_len);

    PRINT("ATQA answer: ");
    PRINT_BLOCK((uint8_t*) &res, 2);
    PRINTLN(" ");
    return rx_len;
    //return res;
  }
  return 0;
}

uint8_t Clrc663::iso14443a_select(uint8_t* uid, uint8_t* sak) {
  cmd_idle();
  // AN1102_recommended_registers_no_transmitter(CLRC663_PROTO_ISO14443A_106_MILLER_MANCHESTER);
  flush_fifo();


  PRINT("UID input: ");
  
  PRINT_BLOCK(uid, 10);
  PRINTLN(" ");

  
  PRINT("\nStarting select\n");

  // we do not need atqa.
  // Bitshift to get uid_size; 0b00: single, 0b01: double, 0b10: triple, 0b11 RFU
  // uint8_t uid_size = (atqa & (0b11 << 6)) >> 6;
  // uint8_t bit_frame_collision = atqa & 0b11111;

  // enable the global IRQ for Rx done and Errors.
  write_reg(CLRC663_REG_IRQ0EN, CLRC663_IRQ0EN_RX_IRQEN | CLRC663_IRQ0EN_ERR_IRQEN);
  write_reg(CLRC663_REG_IRQ1EN, CLRC663_IRQ1EN_TIMER0_IRQEN);  // only trigger on timer for irq1

  // configure a timeout timer, use timer 0.
  uint8_t timer_for_timeout = 0;

  // Set timer to 221 kHz clock, start at the end of Tx.
  timer_set_control(timer_for_timeout, CLRC663_TCONTROL_CLK_211KHZ | CLRC663_TCONTROL_START_TX_END);
  // Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
  // FWI defaults to four... so that would mean wait for a maximum of ~ 5ms

  timer_set_reload(timer_for_timeout, 1000);  // 1000 ticks of 5 usec is 5 ms.
  timer_set_value(timer_for_timeout, 1000);
  uint8_t cascade_level;
  for (cascade_level=1; cascade_level <= 3; cascade_level++) {
    uint8_t cmd = 0;
    uint8_t known_bits = 0;  // known bits of the UID at this level so far.
    uint8_t send_req[7] = {0};  // used as Tx buffer.
    uint8_t* uid_this_level = &(send_req[2]);
    // pointer to the UID so far, by placing this pointer in the send_req
    // array we prevent copying the UID continuously.
    uint8_t message_length;

    switch (cascade_level) {
      case 1:
        cmd = CLRC663_ISO14443_CAS_LEVEL_1;
        break;
      case 2:
        cmd = CLRC663_ISO14443_CAS_LEVEL_2;
        break;
      case 3:
        cmd = CLRC663_ISO14443_CAS_LEVEL_3;
        break;
    }

    // disable CRC in anticipation of the anti collision protocol
    write_reg(CLRC663_REG_TXCRCPRESET, CLRC663_RECOM_14443A_CRC | CLRC663_CRC_OFF);
    write_reg(CLRC663_REG_RXCRCCON, CLRC663_RECOM_14443A_CRC | CLRC663_CRC_OFF);


    // max 32 loops of the collision loop.
    uint8_t collision_n;
    for (collision_n=0; collision_n < 32; collision_n++) {
      
      PRINTLN(" "); PRINT("CL: ");PRINT(cascade_level); PRINT(", coll loop: ");PRINT(collision_n);PRINT(", kb "); PRINT(known_bits);PRINT( "long: ");
//      CLRC663_PRINTF("\nCL: %hhd, coll loop: %hhd, kb %hhd long: ", cascade_level, collision_n, known_bits);

      PRINT_BLOCK(uid_this_level, (known_bits + 8 - 1) / 8);
      PRINTLN(" ");
      
      // clear interrupts
      clear_irq0();
      clear_irq1();


      send_req[0] = cmd;
      send_req[1] = 0x20 + known_bits;
      // send_req[2..] are filled with the UID via the uid_this_level pointer.

      // Only transmit the last 'x' bits of the current byte we are discovering
      // First limit the txdatanum, such that it limits the correct number of bits.
      write_reg(CLRC663_REG_TXDATANUM, (known_bits % 8) | CLRC663_TXDATANUM_DATAEN);

      // ValuesAfterColl: If cleared, every received bit after a collision is
      // replaced by a zero. This function is needed for ISO/IEC14443 anticollision (0<<7).
      // We want to shift the bits with RxAlign
      uint8_t rxalign = known_bits % 8;
      PRINT("etting rx align to: "); PRINT(rxalign); PRINTLN(" ");
      //CLRC663_PRINTF("Setting rx align to: %hhd\n", rxalign);
      write_reg(CLRC663_REG_RXBITCTRL, (0<<7) | (rxalign<<4));


      // then sent the send_req to the hardware,
      // (known_bits / 8) + 1): The ceiled number of bytes by known bits.
      // +2 for cmd and NVB.
      if ((known_bits % 8) == 0) {
        message_length = ((known_bits / 8)) + 2;
      } else {
        message_length = ((known_bits / 8) + 1) + 2;
      }

      PRINT("Send: "); PRINT(message_length); PRINT(" long: ");
//      CLRC663_PRINTF("Send:%hhd long: ", message_length);
      
      PRINT_BLOCK(send_req, message_length);
      PRINTLN(" ");

      cmd_transceive(send_req, message_length);


      // block until we are done
      uint8_t irq1_value = 0;
      while (!(irq1_value & (1 << timer_for_timeout))) {
        irq1_value = clrc_irq1();
        // either ERR_IRQ or RX_IRQ or Timer
        if (irq1_value & CLRC663_IRQ1_GLOBAL_IRQ) {
          break;  // stop polling irq1 and quit the timeout loop.
        }
      }
      cmd_idle();

      // next up, we have to check what happened.
      uint8_t irq0 = clrc_irq0();
      uint8_t error = read_reg(CLRC663_REG_ERROR);
      uint8_t coll = read_reg(CLRC663_REG_RXCOLL);
      PRINT("irq0: 0x");PRINTHEX(irq0,HEX);PRINTLN(" ");
      PRINT("error: 0x"); PRINTHEX(error, HEX); PRINTLN(" ");
//      CLRC663_PRINTF("irq0: %hhX\n", irq0);
//      CLRC663_PRINTF("error: %hhX\n", error);
      uint8_t collision_pos = 0;
      if (irq0 & CLRC663_IRQ0_ERR_IRQ) {  // some error occured.
        // Check what kind of error.
        // error = read_reg(CLRC663_REG_ERROR);
        if (error & CLRC663_ERROR_COLLDET) {
          // A collision was detected...
          if (coll & (1<<7)) {
            collision_pos = coll & (~(1<<7));
            PRINT("Collision at 0x"); PRINT(collision_pos); PRINTLN(" ");
//            CLRC663_PRINTF("Collision at %hhX\n", collision_pos);
            // This be a true collision... we have to select either the address
            // with 1 at this position or with zero
            // ISO spec says typically a 1 is added, that would mean:
            // uint8_t selection = 1;

            // However, it makes sense to allow some kind of user input for this, so we use the
            // current value of uid at this position, first index right byte, then shift such
            // that it is in the rightmost position, ten select the last bit only.
            // We cannot compensate for the addition of the cascade tag, so this really
            // only works for the first cascade level, since we only know whether we had
            // a cascade level at the end when the SAK was received.
            uint8_t choice_pos = known_bits + collision_pos;
            uint8_t selection = (uid[((choice_pos + (cascade_level-1)*3)/8)] >> ((choice_pos) % 8))&1;


            // We just OR this into the UID at the right position, later we
            // OR the UID up to this point into uid_this_level.
            uid_this_level[((choice_pos)/8)] |= selection << ((choice_pos) % 8);
            known_bits++;  // add the bit we just decided.

            PRINT("uid_this_level now kb "); PRINT(known_bits); PRINT(" long: ");
            //CLRC663_PRINTF("uid_this_level now kb %hhd long: ", known_bits);
            
            PRINT_BLOCK( uid_this_level, 10);
            PRINTLN(" ");

          } else {
            // Datasheet of mfrc630:
            // bit 7 (CollPosValid) not set:
            // Otherwise no collision is detected or
            // the position of the collision is out of the range of bits CollPos.
            PRINT("Collision but no valid collpos.\n");
            collision_pos = 0x20 - known_bits;
          }
        } else {
          // Can this ever occur?
          collision_pos = 0x20 - known_bits;
          PRINT("No collision, error was: 0x"); PRINTHEX(error,HEX); PRINT(", setting collision_pos to: "); PRINTHEX(collision_pos,HEX); PRINTLN(" ");
          //CLRC663_PRINTF("No collision, error was: %hhx, setting collision_pos to: %hhx\n", error, collision_pos);
        }
      } else if (irq0 & CLRC663_IRQ0_RX_IRQ) {
        // we got data, and no collisions, that means all is well.
        collision_pos = 0x20 - known_bits;
        PRINT("Got data, no collision, setting to: "); PRINT(collision_pos); PRINTLN(" ");
        //CLRC663_PRINTF("Got data, no collision, setting to: %hhx\n", collision_pos);
      } else {
        // We have no error, nor received an RX. No response, no card?
        return 0;
      }
      PRINT("collision_pos: 0x"); PRINTHEX(collision_pos, HEX); PRINTLN(" ");
      //CLRC663_PRINTF("collision_pos: %hhX\n", collision_pos);

      // read the UID Cln so far from the buffer.
      uint8_t rx_len = fifo_length();
      uint8_t buf[5];  // Size is maximum of 5 bytes, UID[0-3] and BCC.

      read_fifo(buf, rx_len < 5 ? rx_len : 5);

      PRINT("Fifo "); PRINT(rx_len); PRINT(" long: ");
      //CLRC663_PRINTF("Fifo %hhd long: ", rx_len);
      delay(5);
      PRINT_BLOCK(buf, rx_len);
      PRINTLN(" ");

      PRINT("uid_this_level kb "); PRINT(known_bits); PRINT(" long: ");
      //CLRC663_PRINTF("uid_this_level kb %hhd long: ", known_bits);
      
      PRINT_BLOCK(uid_this_level, (known_bits + 8 - 1) / 8);
      PRINTLN(" ");
      // move the buffer into the uid at this level, but OR the result such that
      // we do not lose the bit we just set if we have a collision.
      uint8_t rbx;
      for (rbx = 0; (rbx < rx_len); rbx++) {
        uid_this_level[(known_bits / 8) + rbx] |= buf[rbx];
      }
      known_bits += collision_pos;
      PRINT("known_bits: "); PRINT(known_bits); PRINTLN(" "); 
      //CLRC663_PRINTF("known_bits: %hhX\n", known_bits);

      if ((known_bits >= 32)) {
        PRINT("exit collision loop: uid_this_level kb "); PRINT(known_bits); PRINT(" long: ");
        //CLRC663_PRINTF("exit collision loop: uid_this_level kb %hhd long: ", known_bits);
        
        PRINT_BLOCK(uid_this_level, 10);
        PRINTLN(" ");

        break;  // done with collision loop
      }
    }  // end collission loop

    // check if the BCC matches
    uint8_t bcc_val = uid_this_level[4];  // always at position 4, either with CT UID[0-2] or UID[0-3] in front.
    uint8_t bcc_calc = uid_this_level[0]^uid_this_level[1]^uid_this_level[2]^uid_this_level[3];
    if (bcc_val != bcc_calc) {
      PRINTLN("Something went wrong, BCC does not match.");
      return 0;
    }

    // clear interrupts
    clear_irq0();
    clear_irq1();

    send_req[0] = cmd;
    send_req[1] = 0x70;
    // send_req[2,3,4,5] // contain the CT, UID[0-2] or UID[0-3]
    send_req[6] = bcc_calc;
    message_length = 7;

    // Ok, almost done now, we reenable the CRC's
    write_reg(CLRC663_REG_TXCRCPRESET, CLRC663_RECOM_14443A_CRC | CLRC663_CRC_ON);
    write_reg(CLRC663_REG_RXCRCCON, CLRC663_RECOM_14443A_CRC | CLRC663_CRC_ON);

    // reset the Tx and Rx registers (disable alignment, transmit full bytes)
    write_reg(CLRC663_REG_TXDATANUM, (known_bits % 8) | CLRC663_TXDATANUM_DATAEN);
    uint8_t rxalign = 0;
    write_reg(CLRC663_REG_RXBITCTRL, (0 << 7) | (rxalign << 4));

    // actually send it!
    cmd_transceive(send_req, message_length);
    PRINT("Sen_dreq ");PRINT(message_length);PRINT("long: ");
    //CLRC663_PRINTF("send_req %hhd long: ", message_length);
    
    PRINT_BLOCK(send_req, message_length);
    PRINTLN(" ");
    delay(2);
    // Block until we are done...
    uint8_t irq1_value = 0;
    while (!(irq1_value & (1 << timer_for_timeout))) {
      irq1_value = clrc_irq1();
      if (irq1_value & CLRC663_IRQ1_GLOBAL_IRQ) {  // either ERR_IRQ or RX_IRQ
        break;  // stop polling irq1 and quit the timeout loop.
      }
    }
    cmd_idle();

    // Check the source of exiting the loop.
    uint8_t irq0_value = clrc_irq0();
    if (irq0_value & CLRC663_IRQ0_ERR_IRQ) {
      // Check what kind of error.
      uint8_t error = read_reg(CLRC663_REG_ERROR);
      if (error & CLRC663_ERROR_COLLDET) {
        // a collision was detected with NVB=0x70, should never happen.
        return 0;
      }
    }

    // Read the sak answer from the fifo.
    uint8_t sak_len = fifo_length();
    if (sak_len != 1) {
      return 0;
    }
    uint8_t sak_value;
    read_fifo(&sak_value, sak_len);

    PRINT("SAK answer: ");
    
    PRINT_BLOCK(&sak_value, 1);
    PRINTLN(" ");;
    delay(3);
    if (sak_value & (1 << 2)) {
      // UID not yet complete, continue with next cascade.
      // This also means the 0'th byte of the UID in this level was CT, so we
      // have to shift all bytes when moving to uid from uid_this_level.
      uint8_t UIDn;
      for (UIDn=0; UIDn < 3; UIDn++) {
        // uid_this_level[UIDn] = uid_this_level[UIDn + 1];
        uid[(cascade_level-1)*3 + UIDn] = uid_this_level[UIDn + 1];
      }
    } else {
      // Done according so SAK!
      // Add the bytes at this level to the UID.
      uint8_t UIDn;
      for (UIDn=0; UIDn < 4; UIDn++) {
        uid[(cascade_level-1)*3 + UIDn] = uid_this_level[UIDn];
      }

      *sak = sak_value;
      // Finally, return the length of the UID that's now at the uid pointer.
      return cascade_level*3 + 1;
    }

    PRINT("Exit cascade "); PRINT(cascade_level); PRINT(" long: "); 
    //CLRC663_PRINTF("Exit cascade %hhd long: ", cascade_level);
    
    PRINT_BLOCK(uid, 10);
    PRINTLN(" ");
  }  // cascade loop
  return 0;  // getting an UID failed.
}

// ---------------------------------------------------------------------------
// MIFARE
// ---------------------------------------------------------------------------

uint8_t Clrc663::MF_auth(const uint8_t* uid, uint8_t key_type, uint8_t block) {
  // Enable the right interrupts.

  // configure a timeout timer.
  uint8_t timer_for_timeout = 0;  // should match the enabled interupt.

  // According to datashet Interrupt on idle and timer with MFAUTHENT, but lets
  // include ERROR as well.
  write_reg(CLRC663_REG_IRQ0EN, CLRC663_IRQ0EN_IDLE_IRQEN | CLRC663_IRQ0EN_ERR_IRQEN);
  write_reg(CLRC663_REG_IRQ1EN, CLRC663_IRQ1EN_TIMER0_IRQEN);  // only trigger on timer for irq1

  // Set timer to 221 kHz clock, start at the end of Tx.
  timer_set_control(timer_for_timeout, CLRC663_TCONTROL_CLK_211KHZ | CLRC663_TCONTROL_START_TX_END);
  // Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
  // FWI defaults to four... so that would mean wait for a maximum of ~ 5ms

  timer_set_reload(timer_for_timeout, 2000);  // 2000 ticks of 5 usec is 10 ms.
  timer_set_value(timer_for_timeout, 2000);

  uint8_t irq1_value = 0;

  clear_irq0();  // clear irq0
  clear_irq1();  // clear irq1

  // start the authentication procedure.
  cmd_auth(key_type, block, uid);

  // block until we are done
  while (!(irq1_value & (1 << timer_for_timeout))) {
    irq1_value = clrc_irq1();
    if (irq1_value & CLRC663_IRQ1_GLOBAL_IRQ) {
      break;  // stop polling irq1 and quit the timeout loop.
    }
  }

  if (irq1_value & (1 << timer_for_timeout)) {
    // this indicates a timeout
    return 0;  // we have no authentication
  }

  // status is always valid, it is set to 0 in case of authentication failure.
  uint8_t status = read_reg(CLRC663_REG_STATUS);
  return (status & CLRC663_STATUS_CRYPTO1_ON);
}

uint8_t Clrc663::MF_read_block(uint8_t block_address, uint8_t* dest) {
  flush_fifo();

  write_reg(CLRC663_REG_TXCRCPRESET, CLRC663_RECOM_14443A_CRC | CLRC663_CRC_ON);
  write_reg(CLRC663_REG_RXCRCCON, CLRC663_RECOM_14443A_CRC | CLRC663_CRC_ON);

  uint8_t send_req[2] = {CLRC663_MF_CMD_READ, block_address};

  // configure a timeout timer.
  uint8_t timer_for_timeout = 0;  // should match the enabled interupt.

  // enable the global IRQ for idle, errors and timer.
  write_reg(CLRC663_REG_IRQ0EN, CLRC663_IRQ0EN_IDLE_IRQEN | CLRC663_IRQ0EN_ERR_IRQEN);
  write_reg(CLRC663_REG_IRQ1EN, CLRC663_IRQ1EN_TIMER0_IRQEN);


  // Set timer to 221 kHz clock, start at the end of Tx.
  timer_set_control(timer_for_timeout, CLRC663_TCONTROL_CLK_211KHZ | CLRC663_TCONTROL_START_TX_END);
  // Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
  // FWI defaults to four... so that would mean wait for a maximum of ~ 5ms
  timer_set_reload(timer_for_timeout, 2000);  // 2000 ticks of 5 usec is 10 ms.
  timer_set_value(timer_for_timeout, 2000);

  uint8_t irq1_value = 0;
  uint8_t irq0_value = 0;

  clear_irq0();  // clear irq0
  clear_irq1();  // clear irq1

  // Go into send, then straight after in receive.
  cmd_transceive(send_req, 2);

  // block until we are done
  while (!(irq1_value & (1 << timer_for_timeout))) {
    irq1_value = clrc_irq1();
    if (irq1_value & CLRC663_IRQ1_GLOBAL_IRQ) {
      break;  // stop polling irq1 and quit the timeout loop.
    }
  }
  cmd_idle();

  if (irq1_value & (1 << timer_for_timeout)) {
    // this indicates a timeout
    return 0;
  }

  irq0_value = clrc_irq0();
  if (irq0_value & CLRC663_IRQ0_ERR_IRQ) {
    // some error
    return 0;
  }

  // all seems to be well...
  uint8_t buffer_length = fifo_length();
  uint8_t rx_len = (buffer_length <= 16) ? buffer_length : 16;
  read_fifo(dest, rx_len);
  return rx_len;
}

// The read and write block functions share a lot of code, the parts they have in common could perhaps be extracted
// to make it more readable.

uint8_t Clrc663:: MF_write_block(uint8_t block_address, const uint8_t* source) {
  flush_fifo();

  // set appropriate CRC registers, only for Tx
  write_reg(CLRC663_REG_TXCRCPRESET, CLRC663_RECOM_14443A_CRC | CLRC663_CRC_ON);
  write_reg(CLRC663_REG_RXCRCCON, CLRC663_RECOM_14443A_CRC | CLRC663_CRC_OFF);
  // configure a timeout timer.
  uint8_t timer_for_timeout = 0;  // should match the enabled interupt.

  // enable the global IRQ for idle, errors and timer.
  write_reg(CLRC663_REG_IRQ0EN, CLRC663_IRQ0EN_IDLE_IRQEN | CLRC663_IRQ0EN_ERR_IRQEN);
  write_reg(CLRC663_REG_IRQ1EN, CLRC663_IRQ1EN_TIMER0_IRQEN);

  // Set timer to 221 kHz clock, start at the end of Tx.
  timer_set_control(timer_for_timeout, CLRC663_TCONTROL_CLK_211KHZ | CLRC663_TCONTROL_START_TX_END);
  // Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
  // FWI defaults to four... so that would mean wait for a maximum of ~ 5ms
  timer_set_reload(timer_for_timeout, 2000);  // 2000 ticks of 5 usec is 10 ms.
  timer_set_value(timer_for_timeout, 2000);

  uint8_t irq1_value = 0;
  uint8_t irq0_value = 0;

  uint8_t res;

  uint8_t send_req[2] = {CLRC663_MF_CMD_WRITE, block_address};

  clear_irq0();  // clear irq0
  clear_irq1();  // clear irq1

  // Go into send, then straight after in receive.
  cmd_transceive(send_req, 2);

  // block until we are done
  while (!(irq1_value & (1 << timer_for_timeout))) {
    irq1_value = clrc_irq1();
    if (irq1_value & CLRC663_IRQ1_GLOBAL_IRQ) {
      break;  // stop polling irq1 and quit the timeout loop.
    }
  }
  cmd_idle();

  // check if the first stage was successful:
  if (irq1_value & (1 << timer_for_timeout)) {
    // this indicates a timeout
    return 0;
  }
  irq0_value = clrc_irq0();
  if (irq0_value & CLRC663_IRQ0_ERR_IRQ) {
    // some error
    return 0;
  }
  uint8_t buffer_length = fifo_length();
  if (buffer_length != 1) {
    return 0;
  }
  read_fifo(&res, 1);
  if (res != CLRC663_MF_ACK) {
    return 0;
  }

  clear_irq0();  // clear irq0
  clear_irq1();  // clear irq1

  // go for the second stage.
  cmd_transceive(source, 16);

  // block until we are done
  while (!(irq1_value & (1 << timer_for_timeout))) {
    irq1_value = clrc_irq1();
    if (irq1_value & CLRC663_IRQ1_GLOBAL_IRQ) {
      break;  // stop polling irq1 and quit the timeout loop.
    }
  }

  cmd_idle();

  if (irq1_value & (1 << timer_for_timeout)) {
    // this indicates a timeout
    return 0;
  }
  irq0_value = clrc_irq0();
  if (irq0_value & CLRC663_IRQ0_ERR_IRQ) {
    // some error
    return 0;
  }

  buffer_length = fifo_length();
  if (buffer_length != 1) {
    return 0;
  }
  read_fifo(&res, 1);
  if (res == CLRC663_MF_ACK) {
    return 16;  // second stage was responded with ack! Write successful.
  }

  return 0;
}

void Clrc663::MF_deauth() {
  write_reg(CLRC663_REG_STATUS, 0);
}

void Clrc663::MF_example_dump() {
  uint16_t atqa = iso14443a_REQA();
  if (atqa != 0) {  // Are there any cards that answered?
    uint8_t sak;
    uint8_t uid[10] = {0};  // uids are maximum of 10 bytes long.

    // Select the card and discover its uid.
    uint8_t uid_len = iso14443a_select(uid, &sak);

    if (uid_len != 0) {  // did we get an UID?
      PRINT("UID of "); PRINT(uid_len); PRINT(" bytes (SAK:0x"); PRINTHEX(sak, HEX); PRINT(": ");
      //CLRC663_PRINTF("UID of %hhd bytes (SAK:0x%hhX): ", uid_len, sak);
      PRINT_BLOCK(uid, uid_len);
      PRINTLN(" ");

      // Use the manufacturer default key...
      uint8_t FFkey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

      cmd_load_key(FFkey);  // load into the key buffer

      // Try to athenticate block 0.
      if (MF_auth(uid, CLRC663_MF_AUTH_KEY_A, 0)) {
        PRINTLN("Yay! We are authenticated!");

        // Attempt to read the first 4 blocks.
        uint8_t readbuf[16] = {0};
        uint8_t len;
        uint8_t b;
        for (b=0; b < 4 ; b++) {
          len = MF_read_block(b, readbuf);
          PRINT("Read block 0x"); PRINTHEX(b, HEX); PRINT(": ");
          //CLRC663_PRINTF("Read block 0x%hhX: ", b);
          PRINT_BLOCK(readbuf, len);
          PRINTLN(" ");
        }
        MF_deauth();  // be sure to call this after an authentication!
      } else {
        PRINTLN("Could not authenticate :(");
      }
    } else {
      PRINTLN("Could not determine UID, perhaps some cards don't play");
      PRINTLN(" well with the other cards? Or too many collisions?");
    }
  } else {
    PRINTLN("No answer to REQA, no cards?");
  }
}
