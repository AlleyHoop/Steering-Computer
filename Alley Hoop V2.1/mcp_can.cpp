#include "mcp_can.h"

#define spi_readwrite SPI.transfer
#define spi_read() spi_readwrite(0x00)

/*********************************************************************************************************
** Function name:           mcp2515_reset
** Descriptions:            Performs a software reset
*********************************************************************************************************/
void MCP_CAN::mcp2515_reset(void){
	SPI.beginTransaction(SPISettings(10000000, 1, 0x00));
	MCP2515_SELECT();
	spi_readwrite(MCP_RESET);
	MCP2515_UNSELECT();
	SPI.endTransaction();
	_delay_ms(10);
}

/*********************************************************************************************************
** Function name:           mcp2515_readRegister
** Descriptions:            Read data register
*********************************************************************************************************/
uint8_t MCP_CAN::mcp2515_readRegister(const uint8_t address){
	uint8_t ret;
	SPI.beginTransaction(SPISettings(10000000, 1, 0x00));
	MCP2515_SELECT();
	spi_readwrite(MCP_READ);
	spi_readwrite(address);
	ret = spi_read();
	MCP2515_UNSELECT();
	SPI.endTransaction();
	return ret;
}

/*********************************************************************************************************
** Function name:           mcp2515_readRegisterS
** Descriptions:            Reads sucessive data registers
*********************************************************************************************************/
void MCP_CAN::mcp2515_readRegisterS(const uint8_t address, uint8_t values[], const uint8_t n){
	uint8_t i;
	SPI.beginTransaction(SPISettings(10000000, 1, 0x00));
	MCP2515_SELECT();
	spi_readwrite(MCP_READ);
	spi_readwrite(address);
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<n; i++)
	values[i] = spi_read();

	MCP2515_UNSELECT();
	SPI.endTransaction();
}

/*********************************************************************************************************
** Function name:           mcp2515_setRegister
** Descriptions:            Sets data register
*********************************************************************************************************/
void MCP_CAN::mcp2515_setRegister(const uint8_t address, const uint8_t value){
	SPI.beginTransaction(SPISettings(10000000, 1, 0x00));
	MCP2515_SELECT();
	spi_readwrite(MCP_WRITE);
	spi_readwrite(address);
	spi_readwrite(value);
	MCP2515_UNSELECT();
	SPI.endTransaction();
}

/*********************************************************************************************************
** Function name:           mcp2515_setRegisterS
** Descriptions:            Sets sucessive data registers
*********************************************************************************************************/
void MCP_CAN::mcp2515_setRegisterS(const uint8_t address, const uint8_t values[], const uint8_t n){
	uint8_t i;
	SPI.beginTransaction(SPISettings(10000000, 1, 0x00));
	MCP2515_SELECT();
	spi_readwrite(MCP_WRITE);
	spi_readwrite(address);
	
	for (i=0; i<n; i++)
	spi_readwrite(values[i]);
	
	MCP2515_UNSELECT();
	SPI.endTransaction();
}

/*********************************************************************************************************
** Function name:           mcp2515_modifyRegister
** Descriptions:            Sets specific bits of a register
*********************************************************************************************************/
void MCP_CAN::mcp2515_modifyRegister(const uint8_t address, const uint8_t mask, const uint8_t data){
	SPI.beginTransaction(SPISettings(10000000, 1, 0x00));
	MCP2515_SELECT();
	spi_readwrite(MCP_BITMOD);
	spi_readwrite(address);
	spi_readwrite(mask);
	spi_readwrite(data);
	MCP2515_UNSELECT();
	SPI.endTransaction();
}

/*********************************************************************************************************
** Function name:           mcp2515_readStatus
** Descriptions:            Reads status register
*********************************************************************************************************/
uint8_t MCP_CAN::mcp2515_readStatus(void){
	uint8_t i;
	SPI.beginTransaction(SPISettings(10000000, 1, 0x00));
	MCP2515_SELECT();
	spi_readwrite(MCP_READ_STATUS);
	i = spi_read();
	MCP2515_UNSELECT();
	SPI.endTransaction();
	return i;
}

/*********************************************************************************************************
** Function name:           setMode
** Descriptions:            Sets control mode
*********************************************************************************************************/
uint8_t MCP_CAN::setMode(const uint8_t opMode){
	mcpMode = opMode;
	return mcp2515_setCANCTRL_Mode(mcpMode);
}

/*********************************************************************************************************
** Function name:           mcp2515_setCANCTRL_Mode
** Descriptions:            Set control mode
*********************************************************************************************************/
uint8_t MCP_CAN::mcp2515_setCANCTRL_Mode(const uint8_t newmode){
	uint8_t i;
	mcp2515_modifyRegister(MCP_CANCTRL, MODE_MASK, newmode);
	i = mcp2515_readRegister(MCP_CANCTRL);
	i &= MODE_MASK;
	if ( i == newmode )
	return MCP2515_OK;
	return MCP2515_FAIL;
}

/*********************************************************************************************************
** Function name:           mcp2515_configRate
** Descriptions:            Set baudrate
*********************************************************************************************************/
uint8_t MCP_CAN::mcp2515_configRate(const uint8_t canSpeed, const uint8_t canClock)
{
	mcp2515_setRegister(MCP_CNF1, MCP_16MHz_500kBPS_CFG1);
	mcp2515_setRegister(MCP_CNF2, MCP_16MHz_500kBPS_CFG2);
	mcp2515_setRegister(MCP_CNF3, MCP_16MHz_500kBPS_CFG3);
	return MCP2515_OK;
}

/*********************************************************************************************************
** Function name:           mcp2515_initCANBuffers
** Descriptions:            Initialize Buffers, Masks, and Filters
*********************************************************************************************************/
void MCP_CAN::mcp2515_initCANBuffers(void){
	uint8_t i, a1, a2, a3;
	uint8_t std = 0;
	uint8_t ext = 1;
	uint32_t ulMask = 0x00, ulFilt = 0x00;


	mcp2515_write_mf(MCP_RXM0SIDH, ext, ulMask);			/*Set both masks to 0           */
	mcp2515_write_mf(MCP_RXM1SIDH, ext, ulMask);			/*Mask register ignores ext bit */
	
	/* Set all filters to 0         */
	mcp2515_write_mf(MCP_RXF0SIDH, ext, ulFilt);			/* RXB0: extended               */
	mcp2515_write_mf(MCP_RXF1SIDH, std, ulFilt);			/* RXB1: standard               */
	mcp2515_write_mf(MCP_RXF2SIDH, ext, ulFilt);			/* RXB2: extended               */
	mcp2515_write_mf(MCP_RXF3SIDH, std, ulFilt);			/* RXB3: standard               */
	mcp2515_write_mf(MCP_RXF4SIDH, ext, ulFilt);
	mcp2515_write_mf(MCP_RXF5SIDH, std, ulFilt);

	/* Clear, deactivate the three  */
	/* transmit buffers             */
	/* TXBnCTRL -> TXBnD7           */
	a1 = MCP_TXB0CTRL;
	a2 = MCP_TXB1CTRL;
	a3 = MCP_TXB2CTRL;
	for (i = 0; i < 14; i++) {                                          /* in-buffer loop               */
		mcp2515_setRegister(a1, 0);
		mcp2515_setRegister(a2, 0);
		mcp2515_setRegister(a3, 0);
		a1++;
		a2++;
		a3++;
	}
	mcp2515_setRegister(MCP_RXB0CTRL, 0);
	mcp2515_setRegister(MCP_RXB1CTRL, 0);
}

/*********************************************************************************************************
** Function name:           mcp2515_init
** Descriptions:            Initialize the controller
*********************************************************************************************************/
uint8_t MCP_CAN::mcp2515_init(const uint8_t canIDMode, const uint8_t canSpeed, const uint8_t canClock){
	uint8_t res;
	mcp2515_reset();
	mcpMode = MCP_LOOPBACK;
	res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
	if(res > 0)
	return res;
	// Set Baudrate
	if(mcp2515_configRate(canSpeed, canClock))
	return res;
	if ( res == MCP2515_OK ) {
		mcp2515_initCANBuffers();											//init canbuffers
		mcp2515_setRegister(MCP_CANINTE, MCP_RX0IF | MCP_RX1IF);			//interrupt mode
		mcp2515_setRegister(MCP_BFPCTRL,MCP_BxBFS_MASK | MCP_BxBFE_MASK);	//Sets BF pins as GPO
		mcp2515_setRegister(MCP_TXRTSCTRL,0x00);							//Sets RTS pins as GPI
		switch(canIDMode){
			case (MCP_ANY):
			mcp2515_modifyRegister(MCP_RXB0CTRL,
			MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
			MCP_RXB_RX_ANY | MCP_RXB_BUKT_MASK);
			mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
			MCP_RXB_RX_ANY);
			break;
			case (MCP_STDEXT):
			mcp2515_modifyRegister(MCP_RXB0CTRL,
			MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
			MCP_RXB_RX_STDEXT | MCP_RXB_BUKT_MASK );
			mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
			MCP_RXB_RX_STDEXT);
			break;
			default:
			return MCP2515_FAIL;
			break;
		}
		res = mcp2515_setCANCTRL_Mode(mcpMode);
		if(res)
		return res;
	}
	return res;
}

/*********************************************************************************************************
** Function name:           mcp2515_write_id
** Descriptions:            Write CAN ID
*********************************************************************************************************/
void MCP_CAN::mcp2515_write_id( const uint8_t mcp_addr, const uint8_t ext, const uint32_t id ){
	uint16_t canid;
	uint8_t tbufdata[4];

	canid = (uint16_t)(id & 0x0FFFF);

	if ( ext == 1){
		tbufdata[MCP_EID0] = (uint8_t) (canid & 0xFF);
		tbufdata[MCP_EID8] = (uint8_t) (canid >> 8);
		canid = (uint16_t)(id >> 16);
		tbufdata[MCP_SIDL] = (uint8_t) (canid & 0x03);
		tbufdata[MCP_SIDL] += (uint8_t) ((canid & 0x1C) << 3);
		tbufdata[MCP_SIDL] |= MCP_TXB_EXIDE_M;
		tbufdata[MCP_SIDH] = (uint8_t) (canid >> 5 );
	}
	else{
		tbufdata[MCP_SIDH] = (uint8_t) (canid >> 3 );
		tbufdata[MCP_SIDL] = (uint8_t) ((canid & 0x07 ) << 5);
		tbufdata[MCP_EID0] = 0;
		tbufdata[MCP_EID8] = 0;
	}

	mcp2515_setRegisterS( mcp_addr, tbufdata, 4 );
}

/*********************************************************************************************************
** Function name:           mcp2515_write_mf
** Descriptions:            Write Masks and Filters
*********************************************************************************************************/
void MCP_CAN::mcp2515_write_mf( const uint8_t mcp_addr, const uint8_t ext, const uint32_t id ){
	uint16_t canid;
	uint8_t tbufdata[4];
	canid = (uint16_t)(id & 0x0FFFF);
	if ( ext == 1){
		tbufdata[MCP_EID0] = (uint8_t) (canid & 0xFF);
		tbufdata[MCP_EID8] = (uint8_t) (canid >> 8);
		canid = (uint16_t)(id >> 16);
		tbufdata[MCP_SIDL] = (uint8_t) (canid & 0x03);
		tbufdata[MCP_SIDL] += (uint8_t) ((canid & 0x1C) << 3);
		tbufdata[MCP_SIDL] |= MCP_TXB_EXIDE_M;
		tbufdata[MCP_SIDH] = (uint8_t) (canid >> 5 );
	} else {
		tbufdata[MCP_EID0] = (uint8_t) (canid & 0xFF);
		tbufdata[MCP_EID8] = (uint8_t) (canid >> 8);
		canid = (uint16_t)(id >> 16);
		tbufdata[MCP_SIDL] = (uint8_t) ((canid & 0x07) << 5);
		tbufdata[MCP_SIDH] = (uint8_t) (canid >> 3 );
	}
	mcp2515_setRegisterS( mcp_addr, tbufdata, 4 );
}

/*********************************************************************************************************
** Function name:           mcp2515_read_id
** Descriptions:            Read CAN ID
*********************************************************************************************************/
void MCP_CAN::mcp2515_read_id( const uint8_t mcp_addr, uint8_t* ext, uint32_t* id ){
	uint8_t tbufdata[4];
	*ext = 0;
	*id = 0;
	mcp2515_readRegisterS( mcp_addr, tbufdata, 4 );
	*id = (tbufdata[MCP_SIDH]<<3) + (tbufdata[MCP_SIDL]>>5);
	if ( (tbufdata[MCP_SIDL] & MCP_TXB_EXIDE_M) ==  MCP_TXB_EXIDE_M ){
		/* extended id                  */
		*id = (*id<<2) + (tbufdata[MCP_SIDL] & 0x03);
		*id = (*id<<8) + tbufdata[MCP_EID8];
		*id = (*id<<8) + tbufdata[MCP_EID0];
		*ext = 1;
	}
}

/*********************************************************************************************************
** Function name:           mcp2515_read_canMsg
** Descriptions:            Read message
*********************************************************************************************************/
void MCP_CAN::mcp2515_read_canMsg( const uint8_t buffer_sidh_addr){        /* read can msg                 */
	uint8_t mcp_addr, ctrl;
	mcp_addr = buffer_sidh_addr;
	mcp2515_read_id( mcp_addr, &m_nExtFlg,&m_nID );
	ctrl = mcp2515_readRegister( mcp_addr-1 );
	m_nDlc = mcp2515_readRegister( mcp_addr+4 );
	if (ctrl & 0x08)
		m_nRtr = 1;
	else
		m_nRtr = 0;
	m_nDlc &= MCP_DLC_MASK;
	mcp2515_readRegisterS( mcp_addr+5, &(m_nDta[0]), m_nDlc );
}

/*********************************************************************************************************
** Function name:           MCP_CAN
** Descriptions:            Public function to declare CAN class and the /CS pin.
*********************************************************************************************************/
MCP_CAN::MCP_CAN(uint8_t _CS)
{
	MCPCS = _CS;
	MCP2515_UNSELECT();
	DDRL |= (1 << PL2);		//????
}

/*********************************************************************************************************
** Function name:           begin
** Descriptions:            Public function to declare controller initialization parameters.
*********************************************************************************************************/
uint8_t MCP_CAN::begin(uint8_t idmodeset, uint8_t speedset, uint8_t clockset)
{
	uint8_t res;

	SPI.begin();
	res = mcp2515_init(idmodeset, speedset, clockset);
	if (res == MCP2515_OK)
	return CAN_OK;
	
	return CAN_FAILINIT;
}

/*********************************************************************************************************
** Function name:           readMsg
** Descriptions:            Read message
*********************************************************************************************************/
uint8_t MCP_CAN::readMsg(){
	uint8_t stat, res;
	stat = mcp2515_readStatus();

	if ( stat & MCP_STAT_RX0IF )                                        /* Msg in Buffer 0              */
	{
		mcp2515_read_canMsg( MCP_RXBUF_0);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
		res = CAN_OK;
	}
	else if ( stat & MCP_STAT_RX1IF )                                   /* Msg in Buffer 1              */
	{
		mcp2515_read_canMsg( MCP_RXBUF_1);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
		res = CAN_OK;
	}
	else
	res = CAN_NOMSG;
	
	return res;
}

/*********************************************************************************************************
** Function name:           readMsgBuf
** Descriptions:            Public function, Reads message from receive buffer.
*********************************************************************************************************/
uint8_t MCP_CAN::readMsgBuf(uint32_t *id, uint8_t *len, uint8_t buf[]){
	if(readMsg() == CAN_NOMSG)
	return CAN_NOMSG;

	if (m_nExtFlg)
	m_nID |= 0x80000000;

	if (m_nRtr)
	m_nID |= 0x40000000;
	*id  = m_nID;
	*len = m_nDlc;
	for(int i = 0; i<m_nDlc; i++)
	buf[i] = m_nDta[i];
	return CAN_OK;
}
