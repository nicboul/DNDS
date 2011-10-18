/*
 * Generated by asn1c-0.9.23 (http://lionet.info/asn1c)
 * From ASN.1 module "DNDS"
 * 	found in "./dnds.asn1"
 */

#include "DNDSObject.h"

static asn_TYPE_member_t asn_MBR_DNDSObject_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct DNDSObject, choice.acl),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Acl,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"acl"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DNDSObject, choice.aclgroup),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_AclGroup,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"aclgroup"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DNDSObject, choice.ippool),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_IpPool,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ippool"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DNDSObject, choice.context),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Context,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"context"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DNDSObject, choice.host),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Host,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"host"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DNDSObject, choice.node),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Node,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"node"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DNDSObject, choice.peer),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Peer,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"peer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DNDSObject, choice.permission),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Permission,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"permission"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DNDSObject, choice.user),
		(ASN_TAG_CLASS_CONTEXT | (8 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_User,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"user"
		},
};
static asn_TYPE_tag2member_t asn_MAP_DNDSObject_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* acl at 69 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* aclgroup at 70 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* ippool at 71 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* context at 72 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* host at 73 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* node at 74 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* peer at 75 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* permission at 76 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 } /* user at 77 */
};
static asn_CHOICE_specifics_t asn_SPC_DNDSObject_specs_1 = {
	sizeof(struct DNDSObject),
	offsetof(struct DNDSObject, _asn_ctx),
	offsetof(struct DNDSObject, present),
	sizeof(((struct DNDSObject *)0)->present),
	asn_MAP_DNDSObject_tag2el_1,
	9,	/* Count of tags in the map */
	0,
	9	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_DNDSObject = {
	"DNDSObject",
	"DNDSObject",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	asn_MBR_DNDSObject_1,
	9,	/* Elements count */
	&asn_SPC_DNDSObject_specs_1	/* Additional specs */
};

