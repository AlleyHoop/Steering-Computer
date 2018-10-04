#ifndef _MCP2515_H_
#define _MCP2515_H_

#include "mcp_can_dfs.h"
#define MAX_CHAR_IN_MESSAGE 8

class MCP_CAN
{
    private:
    
    uint8_t   m_nExtFlg;                                                  // Identifier Type
                                                                        // Extended (29 bit) or Standard (11 bit)
    uint32_t  m_nID;                                                      // CAN ID
    uint8_t   m_nDlc;                                                     // Data Length Code
    uint8_t   m_nDta[MAX_CHAR_IN_MESSAGE];                            	// Data array
    uint8_t   m_nRtr;                                                     // Remote request flag
    uint8_t   m_nfilhit;                                                  // The number of the filter that matched the message
    uint8_t   MCPCS;                                                      // Chip Select pin number
    uint8_t   mcpMode;                                                    // Mode to return to after configurations are performed.
    

/*********************************************************************************************************
 *  mcp2515 driver function 
 *********************************************************************************************************/
   // private:
   private:

    void mcp2515_reset(void);                                           // Soft Reset MCP2515

    uint8_t mcp2515_readRegister(const uint8_t address);                    // Read MCP2515 register
    
    void mcp2515_readRegisterS(const uint8_t address,                     // Read MCP2515 successive registers
	                             uint8_t values[], 
                               const uint8_t n);
   
    void mcp2515_setRegister(const uint8_t address,                       // Set MCP2515 register
                             const uint8_t value);

    void mcp2515_setRegisterS(const uint8_t address,                      // Set MCP2515 successive registers
                              const uint8_t values[],
                              const uint8_t n);
    
    void mcp2515_initCANBuffers(void);
    
    void mcp2515_modifyRegister(const uint8_t address,                    // Set specific bit(s) of a register
                                const uint8_t mask,
                                const uint8_t data);

    uint8_t mcp2515_readStatus(void);                                     // Read MCP2515 Status
    uint8_t mcp2515_setCANCTRL_Mode(const uint8_t newmode);                 // Set mode
    uint8_t mcp2515_configRate(const uint8_t canSpeed,                      // Set baud rate
                             const uint8_t canClock);
                             
    uint8_t mcp2515_init(const uint8_t canIDMode,                           // Initialize Controller
                       const uint8_t canSpeed,
                       const uint8_t canClock);
		       
    void mcp2515_write_mf( const uint8_t mcp_addr,                        // Write CAN Mask or Filter
                           const uint8_t ext,
                           const uint32_t id );
			       
    void mcp2515_write_id( const uint8_t mcp_addr,                        // Write CAN ID
                           const uint8_t ext,
                           const uint32_t id );

    void mcp2515_read_id( const uint8_t mcp_addr,                         // Read CAN ID
				uint8_t* ext,
                                uint32_t* id );

    void mcp2515_read_canMsg( const uint8_t buffer_sidh_addr);            // Read CAN message
/*********************************************************************************************************
 *  CAN operator function
 *********************************************************************************************************/

 
    uint8_t readMsg();                                                    // Read message
public:
    MCP_CAN(uint8_t _CS);
    uint8_t begin(uint8_t idmodeset, uint8_t speedset, uint8_t clockset);       // Initialize controller parameters
    uint8_t setMode(uint8_t opMode);                                        // Set operational mode
    uint8_t sendMsgBuf(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf);      // Send message to transmit buffer
    uint8_t sendMsgBuf(uint32_t id, uint8_t len, uint8_t *buf);                 // Send message to transmit buffer
    uint8_t readMsgBuf(uint32_t *id, uint8_t *len, uint8_t *buf);               // Read message from receive buffer         
};

#endif

/*********************************************************************************************************
 *  END FILE
 *********************************************************************************************************/
