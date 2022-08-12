#include "DynamixelInterface_v2.h"

void DynamixelInterface::transaction(bool aExpectStatus, uint8_t answerSize)
{
    prepareTransaction();
    sendPacket(mPacket);
    if (aExpectStatus)
    {
        receivePacket(mPacket, answerSize);
    }
    else
    {
        mPacket.mStatus = DYN_STATUS_OK;
    }
    //	digitalWrite(23, LOW);
    endTransaction();
}


DynamixelStatus DynamixelInterface::syncWrite(
    uint8_t nID, 
    const uint8_t *aID, 
    uint8_t aVer, 
    uint16_t aAddress, 
    uint16_t aSize, 
    const uint8_t *aPtr, 
    uint8_t aStatusReturnLevel)
{
    mPacket = DynamixelPacket(BROADCAST_ID, DYN_SYNC_WRITE, (aSize + 1) * nID + 4, aPtr, aAddress, aSize, nID, aID);
    transaction(false);
    return mPacket.mStatus;
}


DynamixelStatus DynamixelInterface::action(uint8_t aID, uint8_t aVer, uint8_t aStatusReturnLevel)
{
    mPacket = DynamixelPacket(aID, DYN_ACTION, 2, NULL);
    transaction(aStatusReturnLevel > 1 && aID != BROADCAST_ID);
    return mPacket.mStatus;
}

DynamixelStatus DynamixelInterface::reset(uint8_t aID, uint8_t aVer, uint8_t aStatusReturnLevel)
{
    mPacket = DynamixelPacket(aID, DYN_RESET, 2, NULL);
    transaction(aStatusReturnLevel > 1 && aID != BROADCAST_ID);
    return mPacket.mStatus;
}


/*******************************************************************/
/* DYNAMIXEL PROTOCOL 2.0                                          */
/*******************************************************************/
void DynamixelInterface::transaction_v2(bool aExpectStatus, uint16_t answerSize)
{
    prepareTransaction();
    sendPacket_v2(mPacket_v2);

    if (aExpectStatus)
        receivePacket_v2(mPacket_v2, answerSize);
    else
        mPacket_v2.mStatus = DYN2_STATUS_OK;

    endTransaction();
}

DynamixelStatus DynamixelInterface::ping(uint8_t aID, uint8_t aVer, uint8_t *rxBuf)
{
    if (aVer == DYN_PROTOCOL_V1) 
    {
        mPacket = DynamixelPacket(aID, DYN_PING, 2, NULL);
        transaction(true);
        return mPacket.mStatus;
    }
    else 
    {
        mPacket_v2 = DynamixelPacket_v2(aID, PING_TX_LENGTH, INST_PING);
        mPacket_v2.mRxData = rxBuf;
        mPacket_v2.mRxDataLength = PING_RX_PARAMS_LEN;
        transaction_v2(true, PING_RX_LENGTH);
        return mPacket_v2.mStatus;
    }
}

DynamixelStatus DynamixelInterface::read(
    uint8_t aID, 
    uint8_t aVer, 
    uint16_t aAddress, 
    uint16_t aRxSize, 
    uint8_t *aRxBuf, 
    uint8_t aStatusReturnLevel)
{
    if (aVer == DYN_PROTOCOL_V1) 
    {
        mPacket = DynamixelPacket(aID, DYN_READ, 4, aRxBuf, (uint8_t)aAddress, (uint8_t)aRxSize);
        transaction(aStatusReturnLevel > 0 && aID != BROADCAST_ID, aRxSize);
        return mPacket.mStatus;
    }
    else
    {
        uint8_t *params = (uint8_t *) malloc(READ_TX_PARAMS_LEN);
        params[0] = (DXL_LOBYTE(aAddress));
        params[1] = (DXL_HIBYTE(aAddress));
        params[2] = (DXL_LOBYTE(aRxSize));
        params[3] = (DXL_HIBYTE(aRxSize));

        mPacket_v2 = DynamixelPacket_v2(aID, READ_TX_LENGTH, INST_READ, params, READ_TX_PARAMS_LEN, aAddress, aRxBuf, aRxSize);
        transaction_v2(aStatusReturnLevel > 0 && aID != BROADCAST_ID, READ_RX_LENGTH + aRxSize);
        free(params);
        return mPacket_v2.mStatus;
    }
}

DynamixelStatus DynamixelInterface::write(
    uint8_t aID,
    uint8_t aVer,
    uint16_t aAddress,
    uint16_t aTxSize,
    const uint8_t *aTxBuf,
    uint8_t aStatusReturnLevel)
{
    if (aVer == DYN_PROTOCOL_V1)
    {
        mPacket = DynamixelPacket(aID, DYN_WRITE, (uint8_t)aTxSize + 3, aTxBuf, (uint8_t)aAddress);
        transaction(aStatusReturnLevel > 1 && aID != BROADCAST_ID);
        return mPacket.mStatus;
    }
    else
    {
        uint16_t params_size = WRITE_TX_PARAMS_LEN + aTxSize;
        uint8_t *params = (uint8_t *)malloc(params_size);
        params[0] = (DXL_LOBYTE(aAddress));
        params[1] = (DXL_HIBYTE(aAddress));
        
        memcpy(&params[WRITE_TX_PARAMS_LEN], aTxBuf, aTxSize);

        mPacket_v2 = DynamixelPacket_v2(aID, WRITE_TX_LENGTH + aTxSize, INST_WRITE, params, params_size, aAddress);

        transaction_v2(aStatusReturnLevel > 0 && aID != BROADCAST_ID, WRITE_RX_LENGTH);
        free(params);
        return mPacket_v2.mStatus;
    }
}


DynamixelStatus DynamixelInterface::regWrite(
    uint8_t aID,
    uint8_t aVer,
    uint16_t aAddress,
    uint16_t aTxSize,
    const uint8_t *aTxBuf,
    uint8_t aStatusReturnLevel)
{
    if (aVer == DYN_PROTOCOL_V1)
    {
        mPacket = DynamixelPacket(aID, DYN_REG_WRITE, (uint8_t)aTxSize + 3, aTxBuf, (uint8_t)aAddress);
        transaction(aStatusReturnLevel > 1 && aID != BROADCAST_ID);
        return mPacket.mStatus;
    }
    else
    {
        uint16_t params_size = WRITE_TX_PARAMS_LEN + aTxSize;
        uint8_t *params = (uint8_t *)malloc(params_size);
        params[0] = (DXL_LOBYTE(aAddress));
        params[1] = (DXL_HIBYTE(aAddress));
        
        memcpy(&params[WRITE_TX_PARAMS_LEN], aTxBuf, aTxSize);

        mPacket_v2 = DynamixelPacket_v2(aID, WRITE_TX_LENGTH + aTxSize, INST_REG_WRITE, params, params_size, aAddress);

        transaction_v2(aStatusReturnLevel > 0 && aID != BROADCAST_ID, WRITE_RX_LENGTH);
        free(params);
        return mPacket_v2.mStatus;
    }
}


DynamixelStatus DynamixelInterface::action(uint8_t aID, uint8_t aVer, uint8_t aStatusReturnLevel)
{
    if (aVer == DYN_PROTOCOL_V1)
    {
        mPacket = DynamixelPacket(aID, DYN_ACTION, 2, NULL);
        transaction(aStatusReturnLevel > 1 && aID != BROADCAST_ID);
        return mPacket.mStatus;
    }
    else
    {
        mPacket_v2 = DynamixelPacket_v2(aID, TX_MIN_LENGTH, INST_ACTION);
        transaction_v2(aStatusReturnLevel > 0 && aID != BROADCAST_ID, RX_MIN_LENGTH);
        return mPacket_v2.mStatus;
    }
}