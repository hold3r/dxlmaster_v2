/**
 * \file Dynamixel.h
 * \brief Define classes for dynamixel protocol
*/

#ifndef DYNAMIXEL_H
#define DYNAMIXEL_H

#include <stdint.h>
#include <stdlib.h> 

#define DXL_LOBYTE(w) ((uint8_t)((w) & 0xff))
#define DXL_HIBYTE(w) ((uint8_t)(((w) >> 8) & 0xff))

/** \brief ID for broadcast */
#define BROADCAST_ID 0xFE

#define NO_ADDRESS  0xFFFF
#define NO_DATA     0xFFFF

#define HEADERS_LENGTH  4
#define ID_LENGTH       1
#define LEN_LENGTH      2
#define INST_LENGTH     1
#define ERR_LENGTH      1
#define CRC_LENGTH      2

#define PREAMBLE_LENGTH     (HEADERS_LENGTH + ID_LENGTH + LEN_LENGTH)
#define INST_ERR_CRC_LENGTH (INST_LENGTH + ERR_LENGTH + CRC_LENGTH)
#define RX_BUFF             (PREAMBLE_LENGTH + INST_LENGTH + ERR_LENGTH)

#define PING_LENGTH     0
#define READ_LENGTH     4
#define WRITE_LENGTH    2

#define WRITE_V2_LENGTH     (INST_LENGTH + 2 + CRC_LENGTH) 

/** \brief Type of dynamixel device ID */
typedef uint8_t DynamixelID;
/** \brief Type of dynamixel status code */
typedef uint8_t DynamixelStatus;
/** \brief Type of dynamixel instructions */
typedef uint8_t DynamixelInstruction;


/**
 * \brief Dynamixel intruction values
*/
enum DynInstruction
{
	DYN_PING		    = 0x01,
	DYN_READ		    = 0x02,
	DYN_WRITE		    = 0x03,
	DYN_REG_WRITE	    = 0x04,
	DYN_ACTION		    = 0x05,
	DYN_RESET		    = 0x06,
    DYN_REBOOT		    = 0x08,
    DYN_CLEAR		    = 0x10,
    DYN_CTBACKUP	    = 0x20,
    DYN_RSTATUS	        = 0x55,
    DYN_SYNC_READ	    = 0x82,
    DYN_SYNC_WRITE	    = 0x83,
    DYN_FAST_SYNK_READ	= 0x8A,
    DYN_BULK_READ	    = 0x92,
    DYN_BULK_WRITE	    = 0x93,
    DYN_FAST_BULK_READ	= 0x9A
};

/**
 * \brief Dynamixel status values
 *
 * How to interpret status value :
 *
 * If (status & DYN_STATUS_COM_ERROR) == 0 , the value is the 
 * the status returned by the motor, describing its internal
 * error.
 * If (status & DYN_STATUS_COM_ERROR) == 1, there was an error during
 * communication, and the value describe that error.
 *
 * DYN_STATUS_CHECKSUM_ERROR may appear in both cases, in the first
 * case, it means there was an error in the checksum of the 
 * instruction packet, in second case in the response packet.
*/
enum DynStatus
{
	DYN_STATUS_OK				    = 0,
	DYN_STATUS_INPUT_VOLTAGE_ERROR	= 1,
	DYN_STATUS_ANGLE_LIMIT_ERROR	= 2,
	DYN_STATUS_OVERHEATING_ERROR	= 4,
	DYN_STATUS_RANGE_ERROR			= 8,
	DYN_STATUS_CHECKSUM_ERROR		= 16,
	DYN_STATUS_OVERLOAD_ERROR		= 32,
	DYN_STATUS_INSTRUCTION_ERROR	= 64,

	DYN_STATUS_TIMEOUT			    = 1,
	DYN_STATUS_COM_ERROR			= 128,
	DYN_STATUS_INTERNAL_ERROR		= 255
};

enum DynV2_Status
{
	DYN2_STATUS_OK	            = 0x00,
	DYN2_STATUS_RESULT_FAIL	    = 0x01, // Failed to process the sent Instruction Packet
	DYN2_STATUS_INSTRUCTION_ERR	= 0x02, // Undefined Instruction has been used, Action has been used without Reg Write
	DYN2_STATUS_CRC_ERR	        = 0x03, // CRC of the sent Packet does not match
	DYN2_STATUS_DATA_RANGE_ERR  = 0x04, // Data to be written in the corresponding Address is outside the range of the minimum/maximum value
	DYN2_STATUS_DATA_LENGHT_ERR	= 0x05, // Attempt to write Data that is shorter than the data length of the corresponding Address
	DYN2_STATUS_DATA_LIMIT_ERR	= 0x06, // Data to be written in the corresponding Address is outside of the Limit value
	DYN2_STATUS_ACCESS_ERR	    = 0x07, // Attempt to read/write <> Write/Read Only or has not been defined or ROM lock

    DYN2_STATUS_TIMEOUT	        = 0x81,
    DYN2_STATUS_HEADERS_ERR	    = 0x82,
    DYN2_STATUS_PACKET_ID_ERR	= 0x83,
    DYN2_STATUS_PACKET_LEN_ERR	= 0x84,
    DYN2_STATUS_PACKET_INST_ERR	= 0x85,
    DYN2_STATUS_TIMEOUT_DATA	= 0x86,
    DYN2_STATUS_TIMEOUT_CRC	    = 0x87,
    DYN2_STATUS_PACKET_CRC_ERR	= 0x88,
};


/**
 * \brief Dynamixel control table addresses (only addresses used by all models)
*/
enum DynCommonAddress
{
	/** \brief Model number, uint16_t , read only */
	DYN_ADDRESS_MODEL		= 0x00,
	/** \brief Firmware version, uint8_t, read only */
	DYN_ADDRESS_FIRMWARE	= 0x02,
	/** \brief Device ID, uint8_t, writable */
	DYN_ADDRESS_ID			= 0x03,
	/** \brief Communication baudrate, uint8_t, writable */
	DYN_ADDRESS_BAUDRATE	= 0x04,
	/** \brief Return Delay Time , uint8_t, writable */
	DYN_ADDRESS_RDT			= 0x05,
	/** \brief Status Return Level , uint8_t, writable 
	 *
	 * Define when the device will send back a status packet :
	 * 0 : Ping only
	 * 1 : Read and ping
	 * 2 : All instructions
	*/
	DYN_ADDRESS_SRL			= 0x10 
};

/**
 * \brief Dynamixel motor control table addresses 
 *        (only addresses used by all motor models)
*/
enum DynMotorAddress
{
	/** \brief Clockwise angle limit, uint16_t , writable */
	DYN_ADDRESS_CW_LIMIT		= 0x06,
	/** \brief Counnter clockwise angle limit, uint16_t , writable */
	DYN_ADDRESS_CCW_LIMIT		= 0x08,
	/** \brief Maximum torque, uint16_t , writable */
	DYN_ADDRESS_MAX_TORQUE		= 0x0E,
	/** \brief Enable torque, uint8_t , writable */
	DYN_ADDRESS_ENABLE_TORQUE	= 0x18,
	/** \brief LED state, uint8_t , writable */
	DYN_ADDRESS_LED			    = 0x19,
	/** \brief CW compliance margin, uint8_t , writable */
	DYN_ADDRESS_CW_COMP_MARGIN	= 0x1A,
	/** \brief CCW compliance margin, uint8_t , writable */
	DYN_ADDRESS_CCW_COMP_MARGIN = 0x1B,
	/** \brief CW compliance slope, uint8_t , writable */
	DYN_ADDRESS_CW_COMP_SLOPE	= 0x1C,
	/** \brief CCW compliance slope, uint8_t , writable */
	DYN_ADDRESS_CCW_COMP_SLOPE  = 0x1D,
	/** \brief Goal position, uint16_t , writable */
	DYN_ADDRESS_GOAL_POSITION	= 0x1E,
	/** \brief Goal speed, uint16_t , writable */
	DYN_ADDRESS_GOAL_SPEED		= 0x20,

	DYN_ADDRESS_TORQUE_LIMIT	= 0x22,
	/** \brief Current position, uint16_t , readable */
	DYN_ADDRESS_CURRENT_POSITION = 0x24,
	/** \brief Nonzero if any movement, uint8_t, readable */
	DYN_ADDRESS_MOVING		    = 0x2E
};

/**
 * \brief Dynamixel model number values
*/
enum DynModel
{
	DYN_MODEL_AX12A	= 0x0C,
	DYN_MODEL_AX12W	= 0x2C,
	DYN_MODEL_AX18A	= 0x12,
	 
	DYN_MODEL_DX113	= 0x71,
	DYN_MODEL_DX114	= 0x74,
	DYN_MODEL_DX117	= 0x75,
	 
	DYN_MODEL_RX10	= 0x0A,
	DYN_MODEL_RX24F	= 0x18,
	DYN_MODEL_RX28	= 0x1C,
	DYN_MODEL_RX64	= 0x40,
	 
	DYN_MODEL_EX106	= 0x6B,
	 
	DYN_MODEL_MX12W	= 0x68,
	DYN_MODEL_MX28T	= 0x1D,
	DYN_MODEL_MX28R	= 0x1D,
	DYN_MODEL_MX64T	= 0x36,
	DYN_MODEL_MX64R	= 0x36,
	DYN_MODEL_MX106T= 0x40,
	DYN_MODEL_MX106R= 0x40,
	 
	DYN_MODEL_AXS1	= 0x0D
};



/**
 * \struct DynamixelPacket
 * \brief Struct of a dynamixel packet (instruction or status)
*/
struct DynamixelPacket
{
    DynamixelPacket() {}
    // note : allow to constuct from const data, but const_cast it (constness should be respected if code is correct)
    DynamixelPacket(
        uint8_t aID, 
        uint8_t aInstruction, 
        uint8_t aLength, 
        const uint8_t *aData, 
        uint8_t aAddress = 255, 
        uint8_t aDataLength = 255, 
        uint8_t aIDListSize = 0, 
        const uint8_t *aIDList = NULL) 
        : mID(aID), 
          mIDListSize(aIDListSize), 
          mIDList(const_cast<uint8_t *>(aIDList)), 
          mLength(aLength), 
          mInstruction(aInstruction), 
          mAddress(aAddress), 
          mDataLength(aDataLength), 
          mData(const_cast<uint8_t *>(aData))
    {
        mCheckSum = checkSum();
    }

    /** \brief Packet ID */
	DynamixelID mID;
	/** \brief ID list, used for sync write, set to 0 if not used */
	uint8_t mIDListSize;
	DynamixelID *mIDList;
	/** \brief Packet length (full length)*/
	uint8_t mLength;
	/** \brief Packet instruction or status */
	union{
		DynamixelInstruction mInstruction;
		DynamixelStatus mStatus;
	};
	/** \brief Address to read/write, set to 255 if not used */
	uint8_t mAddress;
	/** \brief Length of data to read/write, only needed for read and sync write, set to 255 if not used */
	uint8_t mDataLength;
	/** \brief Pointer to packet parameter (or NULL if no parameter) */
	uint8_t *mData;
	/** \brief Packet checksum */
	uint8_t mCheckSum;
	
	/**
	 * \brief Compute checksum of the packet
	 * \return Checksum value
	*/
	uint8_t checkSum();
};


/**
 * \struct DynamixelPacket v2
 * \brief Struct of a dynamixel packet (instruction or status)
*/
struct DynamixelPacket_v2
{
	DynamixelPacket_v2(){}
    
	/* note : allow to constuct from const data, but const_cast it 
       (constness should be respected if code is correct) */
	DynamixelPacket_v2(
        uint8_t aID, 
        uint8_t aInstruction, 
        uint16_t aLength, 
        const uint8_t *aData, 
        uint16_t aAddress = NO_ADDRESS,
        uint16_t aDataLength = NO_DATA, 
        uint8_t aIDListSize = 0, 
        const uint8_t *aIDList = NULL)
        : mID(aID), 
          mIDListSize(aIDListSize), 
          mIDList(const_cast<uint8_t*>(aIDList)), 
          mLength(aLength), 
          mInstruction(aInstruction), 
          mAddress(aAddress), 
          mDataLength(aDataLength), 
          mData(const_cast<uint8_t*>(aData))
	{
		
	}
	
	/** \brief Packet ID */
	DynamixelID mID;

	/** \brief ID list, used for sync write, set to 0 if not used */
	uint8_t mIDListSize;
	DynamixelID *mIDList;

	/** \brief The Length indicates the Byte size of Instruction, Parameters and CRC fields */
	uint16_t mLength;

	/** \brief Packet instruction or status */
	union{
		DynamixelInstruction mInstruction;
		DynamixelStatus mStatus;
	};

    /** \brief 16-bit Address to read/write, */
	uint16_t mAddress;

	/** \brief Length of data to read/write, only needed for read and */
	uint16_t mDataLength;

	/** \brief Pointer to packet parameter (or NULL if no parameter) */
	uint8_t *mData;
    
	/** \brief Packet checksum */
	uint16_t mCheckSum;
	
	/**
	 * \brief Compute checksum of the packet
	 * \return Checksum value
	*/
    uint16_t updateCRC(uint16_t crc_accum, uint8_t *data_blk_ptr, 
                                           uint16_t data_blk_size);
};






#endif

