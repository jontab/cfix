#include "cfix/fix40/tag.h"

bool cfix_fix40_tag_context_is_header_tag(TagContext *self, int tag);
bool cfix_fix40_tag_context_is_trailer_tag(TagContext *self, int tag);
bool cfix_fix40_tag_context_is_data_tag(TagContext *self, int tag);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

TagContext *cfix_fix40_tag_context(void)
{
    static TagContext context = {
        .is_header_tag = cfix_fix40_tag_context_is_header_tag,
        .is_trailer_tag = cfix_fix40_tag_context_is_trailer_tag,
        .is_data_tag = cfix_fix40_tag_context_is_data_tag,
    };
    return &context;
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

bool cfix_fix40_tag_context_is_header_tag(TagContext *self, int tag)
{
    (void)(self);
    switch (tag)
    {
    case CFIX_FIX40_BeginString:
    case CFIX_FIX40_BodyLength:
    case CFIX_FIX40_MsgType:
    case CFIX_FIX40_SenderCompID:
    case CFIX_FIX40_TargetCompID:
    case CFIX_FIX40_OnBehalfOfCompID:
    case CFIX_FIX40_DeliverToCompID:
    case CFIX_FIX40_SecureDataLen:
    case CFIX_FIX40_SecureData:
    case CFIX_FIX40_MsgSeqNum:
    case CFIX_FIX40_SenderSubID:
    case CFIX_FIX40_TargetSubID:
    case CFIX_FIX40_OnBehalfOfSubID:
    case CFIX_FIX40_DeliverToSubID:
    case CFIX_FIX40_PossDupFlag:
    case CFIX_FIX40_PossResend:
    case CFIX_FIX40_SendingTime:
    case CFIX_FIX40_OrigSendingTime:
        return true;
    default:
        return false;
    }
}

bool cfix_fix40_tag_context_is_trailer_tag(TagContext *self, int tag)
{
    (void)(self);
    switch (tag)
    {
    case CFIX_FIX40_SignatureLength:
    case CFIX_FIX40_Signature:
    case CFIX_FIX40_CheckSum:
        return true;
    default:
        return false;
    }
}

bool cfix_fix40_tag_context_is_data_tag(TagContext *self, int tag)
{
    (void)(self);
    switch (tag)
    {
    case CFIX_FIX40_SecureData:
    case CFIX_FIX40_RawData:
        return true;
    default:
        return false;
    }
}
