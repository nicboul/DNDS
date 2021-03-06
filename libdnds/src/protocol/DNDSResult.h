/*
 * Generated by asn1c-0.9.24 (http://lionet.info/asn1c)
 * From ASN.1 module "DNDS"
 * 	found in "dnds.asn1"
 * 	`asn1c -fnative-types`
 */

#ifndef	_DNDSResult_H_
#define	_DNDSResult_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum DNDSResult {
	DNDSResult_success	= 1,
	DNDSResult_operationError	= 2,
	DNDSResult_protocolError	= 3,
	DNDSResult_noSuchObject	= 4,
	DNDSResult_busy	= 5,
	DNDSResult_secureStepUp	= 6,
	DNDSResult_insufficientAccessRights	= 7
	/*
	 * Enumeration is extensible
	 */
} e_DNDSResult;

/* DNDSResult */
typedef long	 DNDSResult_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_DNDSResult;
asn_struct_free_f DNDSResult_free;
asn_struct_print_f DNDSResult_print;
asn_constr_check_f DNDSResult_constraint;
ber_type_decoder_f DNDSResult_decode_ber;
der_type_encoder_f DNDSResult_encode_der;
xer_type_decoder_f DNDSResult_decode_xer;
xer_type_encoder_f DNDSResult_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _DNDSResult_H_ */
#include <asn_internal.h>
