/* minimal encoder for BCCH-DL-SCH-Message (SIB1) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* ASN.1 generated headers (from your asn1c output) */
#include "asn_application.h"
#include "BCCH-DL-SCH-Message.h"
#include "BCCH-DL-SCH-MessageType.h"
#include "SIB1.h"
#include "CellAccessRelatedInfo.h"
#include "PLMN-IdentityInfo.h"
#include "PLMN-Identity.h"
#include "MCC-MNC-Digit.h"
#include "MCC.h"
#include "MNC.h"

/* PER/XER helpers */
#include "per_encoder.h"
#include "xer_encoder.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <dst-ip> <dst-port>\n", argv[0]);
        return 2;
    }
    const char *dst = argv[1];
    int port = atoi(argv[2]);

    /* Allocate PDU */
    BCCH_DL_SCH_Message_t *pdu = calloc(1, sizeof(*pdu));
    if (!pdu) { perror("calloc pdu"); return 1; }

    /* set choice -> c1 -> systemInformationBlockType1 */
    pdu->message.present = BCCH_DL_SCH_MessageType_PR_c1;
    pdu->message.choice.c1 = calloc(1, sizeof(*pdu->message.choice.c1));
    if (!pdu->message.choice.c1) { perror("calloc c1"); ASN_STRUCT_FREE(asn_DEF_BCCH_DL_SCH_Message, pdu); return 1; }
    pdu->message.choice.c1->present = BCCH_DL_SCH_MessageType__c1_PR_systemInformationBlockType1;
    pdu->message.choice.c1->choice.systemInformationBlockType1 = calloc(1, sizeof(*(pdu->message.choice.c1->choice.systemInformationBlockType1)));
    if (!pdu->message.choice.c1->choice.systemInformationBlockType1) { perror("calloc sib1"); ASN_STRUCT_FREE(asn_DEF_BCCH_DL_SCH_Message, pdu); return 1; }

    SIB1_t *sib1 = pdu->message.choice.c1->choice.systemInformationBlockType1;

    /* Ensure cellSelectionInfo exists and set q_RxLevMin = -70 */
    if (!sib1->cellSelectionInfo) {
        sib1->cellSelectionInfo = calloc(1, sizeof(*sib1->cellSelectionInfo));
        if (!sib1->cellSelectionInfo) { perror("calloc cellSelectionInfo"); ASN_STRUCT_FREE(asn_DEF_BCCH_DL_SCH_Message, pdu); return 1; }
    }
    sib1->cellSelectionInfo->q_RxLevMin = -70;

    /* Create one PLMN_IdentityInfo with one PLMN identity (MCC=310, MNC=260) */
    PLMN_IdentityInfo_t *plmnInfo = calloc(1, sizeof(*plmnInfo));
    if (!plmnInfo) { perror("calloc plmnInfo"); ASN_STRUCT_FREE(asn_DEF_BCCH_DL_SCH_Message, pdu); return 1; }

    PLMN_Identity_t *plmn = calloc(1, sizeof(*plmn));
    if (!plmn) { perror("calloc plmn"); free(plmnInfo); ASN_STRUCT_FREE(asn_DEF_BCCH_DL_SCH_Message, pdu); return 1; }

    /* Fill MNC (260 -> digits 2,6,0) */
    MCC_MNC_Digit_t *d;
    d = calloc(1, sizeof(*d)); *d = 2; ASN_SEQUENCE_ADD(&plmn->mnc.list, d);
    d = calloc(1, sizeof(*d)); *d = 6; ASN_SEQUENCE_ADD(&plmn->mnc.list, d);
    d = calloc(1, sizeof(*d)); *d = 0; ASN_SEQUENCE_ADD(&plmn->mnc.list, d);

    /* Fill optional MCC (310 -> digits 3,1,0) */
    MCC_t *mcc = calloc(1, sizeof(*mcc));
    if (mcc) {
        MCC_MNC_Digit_t *md;
        md = calloc(1, sizeof(*md)); *md = 3; ASN_SEQUENCE_ADD(&mcc->list, md);
        md = calloc(1, sizeof(*md)); *md = 1; ASN_SEQUENCE_ADD(&mcc->list, md);
        md = calloc(1, sizeof(*md)); *md = 0; ASN_SEQUENCE_ADD(&mcc->list, md);
        plmn->mcc = mcc;
    }

    /* Attach PLMN -> PLMN-IdentityInfo -> SIB1 */
    ASN_SEQUENCE_ADD(&plmnInfo->plmn_IdentityList.list, plmn);
    ASN_SEQUENCE_ADD(&sib1->cellAccessRelatedInfo.plmn_IdentityInfoList.list, plmnInfo);

    /* XER dump for debugging */
    fprintf(stdout, "----- XER of constructed PDU -----\n");
    xer_fprint(stdout, &asn_DEF_BCCH_DL_SCH_Message, pdu);

    /* PER-encode */
    uint8_t buffer[8192];
    asn_enc_rval_t ec = uper_encode_to_buffer(&asn_DEF_BCCH_DL_SCH_Message, NULL, pdu, buffer, sizeof(buffer));
    if (ec.encoded == -1) {
        fprintf(stderr, "PER encoding failed: %s\n", ec.failed_type ? ec.failed_type->name : "(no type)");
        ASN_STRUCT_FREE(asn_DEF_BCCH_DL_SCH_Message, pdu);
        return 1;
    }
    size_t encoded_bytes = (ec.encoded + 7) / 8;
    fprintf(stderr, "PER encoded %zu bits -> %zu bytes\n", (size_t)ec.encoded, encoded_bytes);

    fprintf(stderr, "PER payload hex (%zu bytes):", encoded_bytes);
for (size_t i = 0; i < encoded_bytes; ++i) {
    fprintf(stderr, " %02X", buffer[i]);
}
fprintf(stderr, "\n");



    /* Send over TCP: 4 byte length (network order) then payload */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); ASN_STRUCT_FREE(asn_DEF_BCCH_DL_SCH_Message, pdu); return 1; }
    struct sockaddr_in sa = {0};
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    if (inet_pton(AF_INET, dst, &sa.sin_addr) != 1) { perror("inet_pton"); close(sock); ASN_STRUCT_FREE(asn_DEF_BCCH_DL_SCH_Message, pdu); return 1; }
    if (connect(sock, (struct sockaddr *)&sa, sizeof(sa)) < 0) { perror("connect"); close(sock); ASN_STRUCT_FREE(asn_DEF_BCCH_DL_SCH_Message, pdu); return 1; }

    uint32_t netlen = htonl((uint32_t)encoded_bytes);
    if (write(sock, &netlen, sizeof(netlen)) != sizeof(netlen)) { perror("write len"); close(sock); ASN_STRUCT_FREE(asn_DEF_BCCH_DL_SCH_Message, pdu); return 1; }

    ssize_t sent = 0;
    while (sent < (ssize_t)encoded_bytes) {
        ssize_t w = write(sock, buffer + sent, encoded_bytes - sent);
        if (w <= 0) { perror("write payload"); close(sock); ASN_STRUCT_FREE(asn_DEF_BCCH_DL_SCH_Message, pdu); return 1; }
        sent += w;
    }
    fprintf(stdout, "Sent %zd bytes to %s:%d\n", sent, dst, port);

    close(sock);
    ASN_STRUCT_FREE(asn_DEF_BCCH_DL_SCH_Message, pdu);
    return 0;
}

