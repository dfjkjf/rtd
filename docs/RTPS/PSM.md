---
layout: default
title: PSM
nav_order: 7
---


# 9  Platform Specific Model (PSM): UDP/IP  

## 9.1  Introduction  

This clause defines the Platform Specific Model (PSM) that maps the Protocol PIM to UDP/IP. The goal for this  PSM is to provide a mapping with minimal overhead directly on top of UDP/IP.  

The suitability of UDP/IP as a transport for DDS applications stems from several factors:  

•   Universal availability. Being a core part of the IP stack, UDP/IP is available on virtually all  operating systems.  •   Light-weight. UDP/IP is a very simple protocol that adds minimal services on top of IP. Its use  enables the use of IP- based networks with the minimal possible overhead.  •   Best-effort. UDP/IP provides a best-effort service that maps well to Quality-of-service needs of  many real-time data streams. In the situations where it is needed, the RTPS protocol provides the  mechanism to attain reliable delivery on top of the best-effort service provided by UDP.  •   Connection less. UDP/IP offers a connection less service; this allows multiple RTPS endpoints to  share a single operat- ing-system UDP resource (socket/port) while allowing for interleaving of  messages effectively providing an out-of- band mechanism for each separate data-stream.  •   Predictable behavior. Unlike TCP, UDP does not introduce timers that would cause operations to  block for varying amounts of time. As such, it is simpler to model the impact of using UDP on a  real-time application.  •   S cal ability and multicast support. UDP/IP natively supports multicast which allows efficient  distribution of a single message to a large number of recipients.  

## 9.2  Notational Conventions  

## 9.2.1  Name Space  

All the definitions in this document are part of the “RTPS” name-space. To facilitate reading and understanding,  the name-space prefix has been left out of the definitions and classes in this document.  

## 9.2.2  IDL Representation of Structures and CDR Wire Representation  

The following sub clauses often define structures, such as:  

typedef octet Octet Array 3[3];   struct EntityId_t {  Octet Array 3 entityKey; octet entityKind;  } ;  

These definitions use the OMG IDL (Interface Definition Language). When these structures are sent on the  wire, they are encoded using the corresponding CDR representation.  

## 9.2.3  Representation of Bits and Bytes  

This document often uses the following notation to represent an octet or byte:  

+-+-+-+-+-+-+-+-+  |7|6|5|4|3|2|1|0| +-+-+-+-+-+-+-+-+  

In this notation, the leftmost bit (bit 7) is the most significant bit ("MSB") and the rightmost bit (bit 0) is the  least significant bit (“LSB”).  

Streams of bytes are ordered per lines of 4 bytes each as follows:  
In this representation, the byte that comes first in the stream is on the left. The bit on the extreme left is the  MSB of the first byte; the bit on the extreme right is the LSB of the 4th byte.  

## 9.3  Mapping of the RTPS Types  

## 9.3.1  The Globally Unique Identifier (GUID)  

The GUID is an attribute present in all RTPS Entities that uniquely identifies them within the DDS domain (see  8.2.4.1). The PIM defines the GUID as composed of a  Gui d Prefix t  prefix  capable of holding 12 bytes, and an  EntityId_t  entityId  capable of holding 4 bytes. This sub clause defines how the PSM maps those structures.  

## 9.3.1.1  Mapping of the Gui d Prefix t  

The PSM maps the  Gui d Prefix t  to the following structure:  

typedef octet Gui d Prefix t[12];  

The reserved constant GUI D PREFIX UNKNOWN defined by the PIM is mapped to:  

#define GUI D PREFIX UNKNOWN {  $0\!\times\!0\,0$  ,  $0\!\times\!00$  ,  $0\!\times\!0\,0$  ,  $0\!\times\!0\,0$  ,  $0\!\times\!00$  ,  $0\!\times\!00$  ,  $0\!\times\!00$  ,  $0\!\times\!0\,0$  ,  $0\!\times\!0\,0$  ,  $0\!\times\!0\,0$  ,  $0\!\times\!00$  ,  $0\!\times\!0\,0$  }  

## 9.3.1.2  Mapping of the EntityId_t  

8.2.4.3 states that the  EntityId_t  is the unique identification of the  Endpoint  within the  Participant . The PSM  maps the  EntityId_t  to the following structure:  

typedef octet Octet Array 3[3]; struct {  Octet Array 3 entityKey; octet entityKind;  };  

The reserved constant ENTITY ID UNKNOWN defined by the PIM is mapped to:  #define ENTITY ID UNKNOWN {  $\{\,0\,{\bf x}\,0\,0$  ,   $0\!\times\!0\,0$  ,   $0\!\times\!0\,0$  },   $0\!\times\!0\,0$  }  

The  entityKind  field within  EntityId_t  encodes the kind of  Entity  ( Participant ,  Reader ,  Writer, Reader Group,  Writer Group ) and whether the  Entity  is a built-in  Entity  (fully pre-defined by the Protocol, automatically  instantiated), a user-defined  Entity  (defined by the Protocol, but instantiated by the user only as needed by the  application) or a vendor-specific  Entity  (defined by a vendor- specific extension to the Protocol, can therefore  be ignored by another vendor’s implementation).  

When not pre-defined (see below), the  entityKey  field within the  EntityId_t  can be chosen arbitrarily by the  middleware implementation as long as the resulting  EntityId_t  is unique within the  Participant.  

The information on whether the object is a built-in entity, a vendor-specific entity, or a user-defined entity is  encoded in the two most-significant bits of the  entityKind . These two bits are set to:  

•    $\mathrm{\Sigma^{\bullet}}00^{\bullet}$   for user-defined entities.  •   ‘11’ for built-in entities.  •   ‘01’ for vendor-specific entities.  

The information on the kind of  Entity  is encoded in the last six bits of the  entityKind  field. Table 9.1 provides a  complete list of the possible values of the  entityKind  supported in version 2.4 of the protocol. These are fixed  in this major version (2) of the protocol. New  entity Kinds  may be added in higher minor versions of the  protocol in order to extend the model with new kinds of  Entities .  
Table 9.1 - entityKind octet of an EntityId_t  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/815f9ef0262ce8a2992a6c9ece93d0c9944c750aeef35030f053d4ea95cfc774.jpg)  

## 9.3.1.3  Predefined EntityIds  

As mentioned above, the entity IDs for built-in entities are fully predefined by the RTPS Protocol.  

The PIM specifies that the  EntityId_t  of a  Participant  has the pre-defined value  ENTITY ID PARTICIPANT  (8.2.4.2). The corresponding PSM mapping of all pre-defined  Entity  IDs appears in Table 9.2 - EntityId_t  values fully predefined by the RTPS Protocol. The meaning of these  Entity  IDs cannot change in this major  version (2) of the protocol, but future minor versions may add additional reserved  Entity  IDs.  

Table 9.2 - EntityId_t values fully predefined by the RTPS Protocol  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/ab5602ec9ec774ea64786da1dbd0bc275b2478113c20fb40bfee060563b59f09.jpg)  
##   EntityIds Reserved by other Specifications  

Other specifications may reserve EntityIds. Table 9.3 lists the EntityIds reserved for use by other specifications  and future revisions thereof.  

Table 9.3 - EntityIds Reserved by other Specifications  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/6f65f899eafd48f55357ecd4369308ec36edd75a2606f8d0154de8dbbbaba95b.jpg)  

## 9.3.1.4  Deprecated EntityIds in version 2.2 of the Protocol  

The Discovery Protocol used in version 2.2 of the protocol deprecates the EntityIds shown in Table 9.4 -  Deprecated EntityIds in version 2.2 of the protocol. These EntityIds should not be used by future versions of the  protocol unless they are used with the same meaning as in versions prior to 2.2. Implementations that wish to  discover earlier versions should utilize these EntityIds.  

Table 9.4 - Deprecated EntityIds in version 2.2 of the protocol  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/140b1f9513d5b8f90b7c7366055f74ecb2d6ca9946995f24db2c18e9efbc796f.jpg)  

## 9.3.1.5  Mapping of the GUID_t  

The PSM maps the   $G U D_{-}t$    to the following structure:  

struct GUID_t {      Gui d Prefix t guidPrefix;      EntityId_t entityId;  } ;  

Sub clause 8.2.4 states that all RTPS Entities with a Domain Participant share the same  guidPrefix . Furthermore  8.2.4.2 states that implement or s have freedom to choose the  guidPrefix  as long as each Domain Participant  within a DDS Domain has a unique  guidPrefix . The PIM restricts this freedom.  

To comply with this specification, implementations of the RTPS protocol shall set the first two bytes of the  guidPrefix  to match their assigned  vendorId  (see 8.3.3.1.3). This ensures that the  guidPrefix  remains unique  within a DDS Domain even if multiple implementations of the protocol are used. In other words,  implementations of the RTPS protocol are free to use any technique they deem appropriate to generate unique  values for the  guidPrefix  as long as they meet the following constraint:  

Future versions of the RTPS  $2.\mathbf{X}$   protocol shall also follow this rule for generating the  guidPrefix .  
The value of these first two bytes is set as specified above with the sole purpose of enabling the generation of  unique  guidPrefix  across implementations. This value should not be relied upon for other purposes. This ensures  the change does not break interoperability with previous versions of the protocol.  

Use of the reserved  vendorId  is further described in 9.4.4.  The reserved constant GUI D UNKNOWN defined by the PIM is mapped to:  #define GUI D UNKNOWN{ GUI D PREFIX UNKNOWN, ENTITY ID UNKNOWN }  

## 9.3.2  Mapping of the Types that Appear Within Sub messages or Built-in  Topic Data  

The following IDL specifies the PSM mapping of the types that are introduced by the PIM that appear within  messages sent by the protocol. There is no need to map the types that are used exclusively by the virtual  machine, but do not appear in the messages. The subsections following the IDL provide additional information  for the mapped types which require further clarification beyond the IDL type.  

typedef unsigned long DomainId_t;  

// TIME_ZERO: seconds $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$ , fraction  $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$  // TIME INVALID: seconds $=$  0xffffffff, fraction  $=$  0xffffffff // TIME INFINITE: seconds   $=$   0xffffffff, fraction   $=$   0xfffffffe  struct Time_t {      unsigned long seconds; // time in seconds      unsigned long fraction; // time in sec/2^32  };    // DURATION ZERO: seconds  $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$ , fraction  $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$  // DURATION INFINITE: seconds   $=$   0x7fffffff, fraction   $=$   0xffffffff  struct Duration_t {      long seconds; // time in seconds      unsigned long fraction; // time in sec/  $\geq\frown32$    };    // VENDOR ID UNKNOWN: VendorId_  $\mathrm{~\boldmath~t~}[\;0\;]\;\;=\;\;0$  , VendorId_t[1] = 0  typedef octet VendorId_t[2];    // Using this structure, the 64-bit sequence number is:  // seq_num  $=$   high \*   $\geq\frown32$   + low  struct Sequence Number t {      long high;      unsigned long low;  };    typedef unsigned long Fragment Number t;  
const long LOCATOR KIND INVALID = -1;  const long LOCATOR KIND RESERVED   $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$  ;  const long LOCATOR KIND UDP v 4  $\begin{array}{r l}{\mathbf{\varepsilon}=}&{{}\mathbb{1}}\end{array}$  ;  const long LOCATOR KIND UDP v 6 $=\textit{Z}$ ; const unsigned long LOCATOR PORT INVALID   $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$  ;  

// LOCATOR_ADDRESS_INVALID: {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} 

 // LOCATOR INVALID: kind  $=$   LOCATOR KIND INVALID 

 //                  port  $=$   LOCATOR PORT INVALID 

 //                  address   $=$   LOCATOR ADDRESS INVALID  struct Locator_t { 

     long kind; 

     unsigned long port; 

     octet address[16]; 

 }; 

  

 // The values of the following constants as defined in the DDS Specification  

 // should be mapped to the below values before being sent on the wire.  const long BEST EFFORT   $\begin{array}{r l}{\mathbf{\varepsilon}=}&{{}\mathbb{1}}\end{array}$  ;  const long RELIABLE  $=\textit{Z}$  ;  typedef long Re lia bil iy Kind t;    typedef long Count_t; 

  

 // The implementations following this version of the document  

 // implement protocol version 2.4  struct Protocol Version t { 

     octet major; 

     octet minor; 

 }; 

   typedef octet KeyHash_t[16];  typedef octet Status Info t[4];  typedef short Parameter Id t;    struct Content Filter Property t { 

     string  $<\!256\!>$   content Filtered Topic Name; 

     string  $<\!256\!>$   related Topic Name; 

     string  $<\!256\!>$   filter Class Name; 

     string filter Expression; 

     sequence<string> expression Parameters;  };    typedef sequence<long> Filter Result t;  typedef long Filter Signature t[4];  typedef sequence<Filter Signature t> Filter Signature Sequence;  struct Content Filter Info t {      Filter Result t filter Result;      Filter Signature Sequence filter Signatures;  };    struct Property_t {      string name;      string value;  };    typedef string Entity Name t;    struct Original Writer Info t {      GUID_t original Writer GUI D;      Sequence Number t original WriterS N;      Parameter List original Writer Qo s;  };  
typedef octet Group Digest t[4];  

/\* The following bitmask identifies protocol-specific builtin endpoints.     Vendor-specific builtin endpoints may be identified by a new vendor-specific     Parameter Id. Refer to section 9.6.2.2.1 Parameter Id space for the range of      Parameter Ids that are available for vendor-specific extensions.  \*/  

bitmask Built in Endpoint Set t {      @position(0)  DISC BUILT IN ENDPOINT PARTICIPANT ANNOUNCER,      @position(1)  DISC BUILT IN ENDPOINT PARTICIPANT DETECTOR,      @position(2)  DISC BUILT IN ENDPOINT PUBLICATIONS ANNOUNCER,      @position(3)  DISC BUILT IN ENDPOINT PUBLICATIONS DETECTOR,      @position(4)  DISC BUILT IN ENDPOINT SUBSCRIPTIONS ANNOUNCER,      @position(5)  DISC BUILT IN ENDPOINT SUBSCRIPTIONS DETECTOR,  

/\* The following have been deprecated in version 2.4 of the          specification. These bits should not be used by versions of the             protocol equal to or newer than the deprecated version unless          they are used with the same meaning as in versions prior to the          deprecated version.          @position(6)  DISC BUILT IN ENDPOINT PARTICIPANT PROXY ANNOUNCER,          @position(7)  DISC BUILT IN ENDPOINT PARTICIPANT PROXY DETECTOR,          @position(8)  DISC BUILT IN ENDPOINT PARTICIPANT STATE ANNOUNCER,          @position(9)  DISC BUILT IN ENDPOINT PARTICIPANT STATE DETECTOR,      \*/  
@position(10) BUILT IN ENDPOINT PARTICIPANT MESSAGE DATA WRITER,      @position(11) BUILT IN ENDPOINT PARTICIPANT MESSAGE DATA READER,          /\* Bits 12-15 have been reserved by the DDS-Xtypes 1.2 Specification          and future revisions thereof.         Bits 16-27 have been reserved by the DDS-Security 1.1 Specification         and future revisions thereof.      \*/          @position(28) DISC BUILT IN ENDPOINT TOPICS ANNOUNCER,      @position(29) DISC BUILT IN ENDPOINT TOPICS DETECTOR };    bitmask Built in Endpoint Qo s t {      @position(0) BEST EFFORT PARTICIPANT MESSAGE DATA READER  };    // PROTOCOL RTP S:   //     ProtocolId_t[0] = 'R'  //     ProtocolId_t[1] = 'T'  //     ProtocolId_t[2] = 'P'  //     ProtocolId_t[3] = 'S'  typedef octet Protocol Id t[4];  

## 9.3.2.1  Time_t  

The representation of the time is the one defined by the IETF Network Time Protocol (NTP) Standard (IETF  RFC 1305). In this representation, time is expressed in seconds and fractions of seconds using the formula:  

time  $=$   seconds   $^+$   (fraction / 2^(32))  

The time origin is represented by the reserved value TIME_ZERO and corresponds to the UNIX prime epoch  0h, 1 January 1970.  

## 9.3.2.2  Duration_t  

The representation of the time is the one defined by the IETF Network Time Protocol (NTP) Standard (IETF  RFC 1305). In this representation, time is expressed in seconds and fractions of seconds using the formula:  
time  $=$   seconds   $^+$   (fraction /   $_2\wedge$  (32))  

Versions of the RTPS specification previous to version 2.4 did not specify the representation of Duration_t,  

therefore implementations should take into account the vendor and protocol version when interpreting these  fields.  

## 9.3.2.3  Locator_t  

If the Locator_t  kind  is  LOCATOR KIND UP Dv 4 , the  address  contains an IPv4 address. In this case, the leading  12 octets of the address must be zero. The last 4 octets are used to store the IPv4 address. The mapping between  the dot-notation “a.b.c.d” of an  $\mathrm{IPv4}$   address and its representation in the  address  field of a Locator_t is:  

$$
\begin{array}{l l l}{\mathsf{S S}}&{=}&{(\,0\,,\,0\,,\,0\,,\,0\,,\,0\,,\,0\,,\,0\,,\,0\,,\,0\,,\,0\,,\,0\,,\,0\,,\mathsf{a}\,,\mathsf{b}\,,\mathsf{c}\,,\mathsf{d}\,\}}\end{array}
$$  

If the Locator_t  kind  is  LOCATOR KIND UP Dv 6 , the  address  contains an IPv6 address. IPv6 addresses typically  use a shorthand hexadecimal notation that maps one-to-one to the 16 octets in the  address  field. For example,  the representation of the IPv6 address “FF00:4501:0:0:0:0:0:32” is:  

$$
\begin{array}{r l r}{\mathrm{\ttaddendres}}&{=}&{(0\mathrm{\ttxfff},0,0\mathrm{\ttx45},0\mathrm{\ttx01},0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
$$  

The range of Locator_t kinds has been divided into the following ranges:  

•    $0{\bf x}00000003\bf{\cdot}0{\bf x}01$  ffffff  (inclusive) are reserved for vendor-specific Locator_t kinds and will not  be used by any future versions of the RTPS protocol.  •    $0{\tt x}02000000\textrm{-}0{\tt x}02$  ffffff  (inclusive) are reserved for future use by the RTPS specification  •    $0\!\times\!0\!\times\!3000000$   and greater are reserved for Locator_t kinds that identify a transport developed by a  third-party (i.e., are neither vendor nor protocol-specific) and will not be used by any future versions of  the RTPS protocol.  

## 9.3.2.4  Group Digest t  

This type is used to represent a group of Entities belonging to the same Participant. The representation uses the  IDL structure  Entity Id Set t  defined below:  

typedef octet Octet Array 3[3];   struct {      Octet Array 3 entityKey;       octet entityKind;  };  struct Entity Id Set t {      sequence<EntityId_t> entityIds;  };  

In the construction of the  entityIds  sequence, the values are sorted by increasing values of the  EntityId_t . To  perform the ordering the  EntityId_t , which is 4 octets, is re-interpreted as if it was the little-endian serialized  representation of a 32-bit signed integer (the IDL4 int32 primitive type).  

The  Group Digest t  is computed from an  Entity Id Set t  by first computing a 128 bit MD5 Digest (IETF RFC  1321) applied to the CDR Big-Endian serialization of the structure  Entity Id Set t . The  Group Digest t  is the  leading 4 octets of the MD5 Digest.  

The empty group is represented by a zero value of the  Group Digest t . It is not computed as the hash of the  serialized empty sequence.  
## 9.4  Mapping of the RTPS Messages  

9.4.1  Overall Structure  

Sub clause 8.3.3 in the PIM defined the overall structure of a  Message  as composed of a leading  Header  followed by a variable number of  Sub messages .  

The PSM aligns each  Submessage  on a 32-bit boundary with respect to the start of the  Message .  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/ab91feeb4776552fd05e8183e15badce48903536a6aefb4be3358e53c3296b48.jpg)  

A  Message  has a well-known length. This length is not sent explicitly by the RTPS protocol but is part of the  underlying transport with which  Messages  are sent. In the case of UDP/IP, the length of the  Message  is the  length of the UDP payload.  

## 9.4.2  Mapping of the PIM Sub message Elements  

Each RTPS  Submessage  is built from a set of predefined atomic building blocks called “submessage elements,”  as defined in 8.3.5. This sub clause describes the PSM mapping for each of the  Sub message Elements  defined by the PIM.  

## 9.4.2.1  EntityId  

The PSM mapping for the  EntityId  Sub message Element defined in 8.3.5.1 is given by the following IDL  definition:  

Following the CDR encoding, the wire representation of the  EntityId  Sub message Element is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/06a4b3ff6ff4c333c9f95e223a218d324abec6c9ed12ee1d7330e28daae6eedc.jpg)  

## 9.4.2.2  GuidPrefix  

The PSM mapping for the  GuidPrefix  Sub message Element defined in 8.3.5.1 is given by the following IDL  definition:  

Following the CDR encoding, the wire representation of the  GuidPrefix  Sub message Element is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/2d5679dea69927e001a1b337ea8690793710a5431cfbf4e0489dcfc2de4b8dca.jpg)  
## 9.4.2.3  VendorId  

The PSM mapping for the  VendorId  Sub message Element defined in 8.3.5.2  is given by the following IDL  definition:  

Following the CDR encoding, the wire representation of the  VendorId  Sub message Element is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/9bc7b5591297514d0405bf605946acfaf175e4e78789906e31229ba56fa7062f.jpg)  

## 9.4.2.4  Protocol Version  

The PSM mapping for the  Protocol Version  Sub message Element defined in 8.3.5.3 is given by the  following IDL definition:  

Following the CDR encoding, the wire representation of the  Protocol Version  Sub message Element is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/47af20d4b482a2e7f7bb1a9d3faa28a0af7342071000f2db4235e40b8524f1bc.jpg)  

## 9.4.2.5 Sequence Number  

The PSM mapping for the  Sequence Number  Sub message Element defined in 8.3.5.4 is given by the  following IDL definition:  

typedef Sequence Number t Sequence Number;  Following the CDR encoding, the wire representation of the  Sequence Number  Sub message Element is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/82fa5ac980e2a8371070c81cc7e335622cde78c0fe5a48338cfeac3ac13b7969.jpg)  

## 9.4.2.6  Sequence Number Set  

The PSM maps the  Sequence Number Set  Sub message Element defined in 8.3.5.5 to the following structure:  

typedef sequence<long,   $8\!>$   LongSeq8;  

struct Sequence Number Set {  Sequence Number t bitmapBase;  LongSeq8 bitmap;  };  
The above structure offers a compact representation encoding a set of up to 256 sequence numbers. The  representation of the  Sequence Number Set  includes the first sequence number in the set ( bitmapBase ) and a  bitmap  of up to 256 bits. The number of bits in the  bitmap  is denoted by numBits. The value of each bit in the  bitmap  indicates whether the Sequence Number obtained by adding the offset of the bit to the  bitmapBase  is  included   $(\mathrm{bit}{=}1)$  ) or excluded (bit=0) from the  Sequence Number Set .  

More precisely a  Sequence Number  ‘ seqNum ’ belongs to the  Sequence Number Set ‘ seqNumSet,’  if and only if  the following two conditions apply:  

seqNumSet.bitmapBase  $<=$  seqNum  $<$  seqNumSet.bitmapBase                                          $^+$   seqNumSet.numBits(bitmap[deltaN/ 32]   $\mathrm{~\textit~{~\textcent~}~}\mathrm{~(1~\ll~(31~-~d e l t a N^{\circ}32)~)~}\;=\;\mathrm{~(1~\ll~(31~-~d e l t a N^{\circ}32)~)~}$  

where  

deltaN  $=$    seqNum  - seqNumSet.bitmapBase  

A valid  Sequence Number Set  must satisfy the following conditions: 

 •   bitmapBase  $>=1$   

 •    $0<=$   numBits  $<=256$   

 •   there are  $\scriptstyle{\mathrm{M}}=$  (numBits  $+31$  )/32 longs containing the pertinent bits This document uses the following  notation for a specific bitmap:  bitmapBase/numBits:bitmap  

In the  bitmap , the bit corresponding to sequence number  bitmapBase  is on the left. The ending   $"0"$   bits can be  represented as one   $"0$  ."  

For example, in  bitmap  “1234/12:00110”,  bitmapBase  $\overleftarrow{}$  1234 and  numBits  $\scriptstyle{\mathfrak{s}}=12$  . The bits apply as follows to the  sequence numbers:  

Table 9.5 - Example of bitmap: meaning of “1234/12:00110”  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/22921e9a5aa4bdafd9ef5e25d79d6605ff70960ab4ad2706488a7045e73b1d99.jpg)  

The wire representation of the  Sequence Number Set  Sub message Element is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/acfe83143278532185a50e4d095547cfcbeecaefd4f40ae4b132d2c2cc9ad418.jpg)  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/9fbe2369a55c2e59dcbe5efe46f7ce445b7102a77dafa3d66c9fc9fd83bdbb64.jpg)  

The  numBits  field encodes both the number of significant bits and the number of bitmap elements. Due to this  optimization, this Sub message Element does not follow CDR encoding.  

## 9.4.2.7  Fragment Number  

The PSM mapping for the  Fragment Number  Sub message Element defined in 8.3.5.6 is given by the  following IDL definition:  

typedef Fragment Number t Fragment Number;  Following the CDR encoding, the wire representation of the  Fragment Number  Sub message Element is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/afda6038f9c233b1a6879bf98c06b03b0415e7a66417d66913f2ad017e36a9eb.jpg)  

## 9.4.2.8  Fragment Number Set  

The PSM maps the  Fragment Number Set  Sub message Element defined in 8.3.5.7 to the following structure:  

typedef sequence<long,   $8\!>$   LongSeq8; struct  Fragment Number Set {  Fragment Number t  bitmapBase; LongSeq8  bitmap;  };  

The above structure offers a compact representation encoding a set of up to 256 fragment numbers. The  representation of the  Fragment Number Set  includes the first fragment number in the set ( bitmapBase ) and a  bitmap  of up to 256 bits. The interpretation matches that of a  Sequence Number Set .  

The wire representation of the  Fragment Number Set  Sub message Element is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/e24af0c62c4d9cb07952cbc535b5597cfc8a2c451fe1e43731eeacf37b674d78.jpg)  

The  numBits  field encodes both the number of significant bits and the number of bitmap elements. Due to this  optimization, this Sub message Element does not follow CDR encoding.  

## 9.4.2.9  Timestamp  

The PSM mapping for the  Timestamp  Sub message Element defined in 8.3.5.8 is given by the following IDL  definition:  
typedef Time_t Timestamp;  

Following the CDR encoding, the wire representation of the  Timestamp  Sub message Element is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/b7ce06b534ccfaf82efa67b8a9eb12d2eb8a81f6b242d0ec9b44e173549e43ac.jpg)  

## 9.4.2.10 Locator List  

The PSM mapping for the  Locator List  Sub message Element defined in 8.3.5.11 is given by the following  IDL definition:  

typedef sequence<Locator_t,   $8\!>$   Locator List;  

Following the CDR encoding, the wire representation of the  Locator List  Sub message Element is:  Locator List:  

0...2...........8...............16.............24. ............. 31  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |  unsigned long  num Locator s  |  +---------------+---------------+---------------+---------------+  | Locator_t locator_1 | \~  ...  \~  |  Locator_t  locator num Locator s  |  +---------------+---------------+---------------+---------------+  

Where each Locator_t has the following wire representation:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/a44ec1688556f02e56d07d5607d47dab0b0dc6180f26e86434ac9516a6190fc3.jpg)  

## 9.4.2.11 Parameter List  

A  Parameter List  contains a list of  Parameters , terminated with a sentinel. Each  Parameter  within  the Parameter List starts aligned on a 4-byte boundary with respect to the start of the  Parameter List .  

The IDL representation for each  Parameter  is:  

typedef short Parameter Id t;  struct Parameter {  Parameter Id t parameter Id;   short length;  octet value[length]; // Pseudo-IDL: array of non-const length  };  

The  parameter Id  identifies the type of parameter.  

The l ength  encodes the number of octets following the  length  to reach the ID of the next parameter (or the ID of  the sentinel). Because every  parameter Id  starts on a 4-byte boundary, the  length  is always a multiple of four.  
The  value  contains the CDR representation of the Parameter type that corresponds to the specified  parameter Id .  

For alignment purposes, the CDR stream is logically reset for each parameter value (i.e., no initial padding is  required) after the  parameter Id  and  length  are serialized.  

The  Parameter List  may contain multiple Parameters with the same value for the  parameter Id . This is used  to provide a collection of values for that kind of Parameter.  

The use of  Parameter List  representation makes it possible to extend the protocol and introduce new  parameters and still be able to preserve interoperability with earlier versions of the protocol.  

The wire representation for the  Parameter List  is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/1e6d1889c69cc0b2eb4d7b13124e99069a508aeb519e8c1b9557a34dcb2fd9a7.jpg)  

There are two predefined values of the  parameter Id :  

#define PID_PAD (0)  #define PID SENTINEL (1)  

The PID SENTINEL is used to terminate the parameter list and its length is ignored. The PID_PAD is used to  enforce alignment of the parameter that follows and its length can be anything (as long as it is a multiple of 4).  

The complete set of possible values for the  parameter Id  in version 2.4 of the protocol appears in 9.6.3.  

## 9.4.2.12 Serialized Payload  

A  Serialized Payload  Sub message Element contains the serialized representation of either value of an  application- defined data-object or the value of the key that uniquely identifies the data-object.  

The specification of the process used to encode the application-level data-type into a serialized byte-stream is  not strictly part of the RTPS protocol. For the purpose of interoperability, all implementations must however use  a consistent representation (See, 10 Serialized Payload Representation).  

The wire representation for the  Serialized Payload  is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/e865469696efd49e00ac11a1820d1521a1e5278ec44b20e2f975e45079592240.jpg)  
## 9.4.2.13 Count  

The PSM maps the  Count  Sub message Element defined in 8.3.5.10 to the structure:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/28412332863ceb366751a403fb6a94fb0b15701d99fbfad3939618a40671e318.jpg)  

Following the CDR encoding, the wire representation of the  Count  Sub message Element is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/da83d416a418bec5abcb917d480b688964140c354b6245c374ca9bac198cf329.jpg)  

## 9.4.2.14 Group Digest  

The PSM maps the  Group Digest  Sub message Element defined in 8.3.5.10 to the structure:  

Following the CDR encoding, the wire representation of the  Group Digest  Sub message Element is:  Group Digest  

0...2...........8...............16.............24...............32  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |                      octet    value[4]                        |  +---------------+---------------+---------------+---------------+  

## 9.4.3  Additional Sub message Elements  

In addition to the Sub message Elements introduced by the PIM, the UDP PSM introduces the following  additional Sub message Elements.  

## 9.4.3.1  Locator UDP v 4  

The  Locator UDP v 4  Sub message Element is identical to a  Locator List  Sub message Element containing  a single locator of kind  LOCATOR KIND UDP v 4 .  Locator UDP v 4  is introduced to provide a more compact  representation when using UDP on IPv4.  

Table 9.6 - Structure of the Locator UDP v 4 Sub message Element  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/347e0e6c1cd9bdc27f99bf65d19a573e2fb1131edeec5474e6fb193344966eee.jpg)  

The PSM maps the  Locator UDP v 4  Sub message Element to the structure:  

Following the CDR encoding, the wire representation of the  Locator UDP v 4  Sub message Element is:  

Locator UDP v 4:  0...2...........8...............16.............24. ............. 32  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |  unsigned long  address  |  +---------------+---------------+---------------+---------------+  |  unsigned long  port  |  +---------------+---------------+---------------+---------------+  
## 9.4.4  Mapping of the RTPS Header  

Sub clause 8.3.7 in the PIM specifies that all messages should include a leading RTPS Header. The PSM  mapping of the RTPS Header is shown below:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/170914bbf4f4950a71100c1c41efb0056a45233ea87ac6e2c0cfa54d27eca1aa.jpg)  

The structure of the Header cannot change in this major version (2) of the protocol.  

The RTPS Header includes a  vendorId  field, see 8.3.5.2. To be compliant with the DDS Interoperability  Specification a vendor must have a reserved Vendor ID and use it. See 8.3.3.1.3 for details on where to find the  current list of vendor IDs and how to request a new one to be assigned.  

## 9.4.5  Mapping of the RTPS Sub messages  

9.4.5.1  Submessage Header  

Sub clause 8.3.3.2 in the PIM defined the structure of all Sub messages as composed of a leading  Sub message Header  followed by a variable number of  Sub message Elements .  

The PSM maps the  Sub message Header  into the following structure:  

struct Sub message Header {       octet sub message Id; octet flags;      unsigned short sub message Length; /\* octets To Next Header \*/  };  

With the byte stream representation defined in 9.2.3, the sub message Length is defined as the number of octets  from the start of the contents of the Submessage to the start of the next Submessage header. Given this  definition, the remainder of the UDP PSM will refer to sub message Length as  octets To Next Header . See also  9.4.5.1.3.  

Following the CDR encoding, the wire representation of the  Sub message Header  is shown below:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/11cbce4f447ede6873664e38fc366033289d72cbf310207cfa748ccc981a8cd2.jpg)  

This general structure cannot change in this major version (2) of the protocol. The following sub clauses discuss  each member of the  Sub message Header  in more detail.  
##   Sub message Id  

This octet identifies the kind of  Submessage . Sub messages with IDs  $0\mathbf{x}00$   to 0x7f (inclusive) are protocolspecific. They are defined as part of the RTPS protocol. Version 2.4 defines the following Sub messages:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/8abcf5e80cbdd98d0907794953f722600014914888f1c9cdd2d5840f497fc516.jpg)  

The meaning of the Submessage IDs cannot be modified in this major version (2). Additional Sub messages can  be added in higher minor versions. Sub messages with ID's 0x80 to 0xff (inclusive) are vendor-specific; they  will not be defined by future versions of the protocol. Their interpretation is dependent on the  vendorId  that is  current when the Submessage is encountered.  

## 9.4.5.1.1.1   Submessage Ranges Reserved by other Specifications  

Other specifications may reserve portions of the protocol-specific range of Submessage IDs. Table 9.7 lists the  Submessage IDs reserved for use by other specifications and future revisions thereof.  

Table 9.7 - Submessage IDs Reserved by other Specifications (all ranges are inclusive)  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/49982e31739ad848942cfeb79ddcc028fe20d40d4c485e584c6f9a2cec66cbda.jpg)  

##   flags  

Sub clause 8.3.3.2 in the PIM defines the  Endian ness Flag  as a flag present in all Sub messages that indicates the  endianness used to encode the Submessage. The PSM maps the  Endian ness Flag  flag into the least-significant  bit (LSB) of the  flags . This bit is therefore always present in all  Sub messages  and represents the endianness  used to encode the information in the  Submessage . The  Endian ness Flag  is represented with the literal   $\mathrm{:E}'$  .  $\mathrm{E}{=}0$    means big-endian,  $\mathrm{E}{=}1$   means little-endian.  

The value of the  Endian ness Flag  can be obtained from the expression:  

$\begin{array}{r l}{\mathrm{~E~}}&{{}=}\end{array}$   Sub message Header.flags &   $0\!\times\!0\,\bot$  

Other bits in the  flags  have interpretations that depend on the type of  Submessage .  

In the following descriptions of the  Sub messages , the character  $\mathbf{\Delta}\mathbf{X}^{\prime}$   is used to indicate a flag that is unused in  version 2.4 of the protocol. Implementations of RTPS version 2.4 should set these to zero when sending and  ignore these when receiving. Higher minor versions of the protocol can use these flags.  

##   octets To Next Header  

The representation of this field is a CDR unsigned short (ushort).  

In case  octets To Next Header  $>0$  , it is the number of octets from the first octet of the contents of the Submessage  until the first octet of the header of the next  Submessage  (in case the  Submessage  is not the last  Submessage  in  the  Message ) OR it is the number of octets remaining in the  Message  (in case the  Submessage  is the last  
Submessage  in the  Message ). An interpreter of the  Message  can distinguish these two cases as it knows the  total length of the  Message .  

In case  octets To Next Header  ${}==0$   and the kind of Submessage is NOT PAD or INFO_TS, the  Submessage  is the  last  Submessage  in the  Message  and extends up to the end of the  Message . This makes it possible to send  Sub messages larger than  $64\mathrm{k}$   (the size that can be stored in the  octets To Next Header  field), provided they are the  last  Submessage  in the  Message .  

In case the  octets To Next He a de  $\scriptstyle{\mathcal{S}}=0$   and the kind of Submessage is PAD or INFO_TS, the next  Submessage  header starts immediately after the current  Submessage  header OR the PAD or INFO_TS is the last  Submessage  in the  Message .  

## 9.4.5.2  AckNack Submessage  

Sub clause 8.3.7.1 in the PIM defines the logical contents of the  AckNack  Submessage. The PSM maps the  AckNack  Submessage into the following wire representation:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/c92a47655fa66ddfb4ac60c9f928117c33fc84e90163e9bac14cbdb06bf565af.jpg)  

##   Flags in the Submessage Header  

In addition to the  Endian ness Flag , The  AckNack  Submessage introduces the  FinalFlag  (“Content” on page  46). The PSM maps the  FinalFlag  flag into the 2nd least-significant bit (LSB) of the flags.  

The  FinalFlag  is represented with the literal ‘F’.  $\mathrm{F}{=}1$   means the reader does not require a  Heartbeat  from the  writer.  $\mathrm{F}{=}0$   means the writer must respond to the AckNack message with a  Heartbeat  message.  

The value of the  FinalFlag  can be obtained from the expression:  

## 9.4.5.3  Data Submessage  

Sub clause 8.3.7.2 in the PIM defines the logical contents of the  Data  Submessage. The PSM maps the  Data   Submessage into the following wire representation:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/b0870710e0582ee63fd9b043ffbc0acb519e866c2aea31821693dfb7580fcd76.jpg)  
##   Flags in the Submessage Header  

In addition to the  Endian ness Flag , The  Data  Submessage introduces the  Inline Qo s Flag, DataFlag,  and  Key  (see 8.3.7.3.2). The PSM maps these flags as follows:  

The  Inline Qo s Flag  is represented with the literal ‘Q.’  $\mathrm{Q=}1$   means that the  Data  Submessage contains the  inlineQos Sub message Element.  

The value of the  Inline Qo s Flag  can be obtained from the expression:  

The  DataFlag  is represented with the literal ‘D.’ The value of the  DataFlag  can be obtained from the  expression.  

D = Sub message Header.flags &   $0\!\times\!0\,4$  

The  KeyFlag  is represented with the literal ‘K.’ The value of the  KeyFlag  can be obtained from the expression.  

The  DataFlag  is interpreted in combination with the  KeyFlag  as follows:  

•    $\scriptstyle{\mathrm{~D=0~}}$   and  $\mathrm{K}{=}0$   means that there is no  serialized Payload  Sub message Element.  •    $\scriptstyle{\mathrm{D}}=1$   and  $\mathrm{K}{=}0$   means that the  serialized Payload  Sub message Element contains the serialized  Data.  •    $\scriptstyle{\mathrm{~D=0~}}$   and  $\scriptstyle{\mathrm{K}}=1$   means that the  serialized Payload  Sub message Element contains the serialized Key.  •    $\scriptstyle{\mathrm{D}}=1$   and  $\scriptstyle{\mathrm{K}}=1$   is an invalid combination in this version of the protocol.  

The  NonStandard Payload Flag  is represented with the literal ‘N.’ The value of the  NonStandard Payload Flag  can be obtained from the expression.  

##   extraFlags  

The  extraFlags  field provides space for an additional 16 bits of flags beyond the 8 bits provided as in the  submessage header. These additional bits will support evolution of the protocol without compromising  backwards compatibility.  

This version of the protocol should set all the bits in the  extraFlags  to zero.  

##   octets To Inline Qo s  

The representation of this field is a CDR unsigned short (ushort).  

The  octets To Inline Qo s  field contains the number of octets starting from the first octet immediately following  this field until the first octet of the inlineQos Sub message Element. If the  inlineQos  Sub message Element is not  present (i.e., the  Inline Qo s Flag  is not set), then  octets To Inline Qo s  contains the offset to the next field after  the  inlineQos .  

Implementations of the protocol that are processing a received submessage should always use the  octets To Inline Qo s  to skip any submessage header elements it does not expect or understand and continue to  process the  inlineQos  Sub message Element (or the first submessage element that follows  inlineQos  if the  inlineQos  is not present). This rule is necessary so that the receiver will be able to interoperate with senders that  use future versions of the protocol which may include additional submessage headers before the  inlineQos .  
## 9.4.5.4  DataFrag Submessage  

Sub clause 8.3.7.3 in the PIM defines the logical contents of the  DataFrag  Submessage. The PSM maps the  DataFrag  

Submessage into the following wire representation:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/2a7a041b0dba3732d05e07fa1a7c69d7f2d29d546802703d4026c91f2cc2eace.jpg)  

##   Flags in the Submessage Header  

In addition to the  Endian ness Flag , The  DataFrag  Submessage introduces the  KeyFlag  and  Inline Qo s Flag  (see 8.3.7.1.2). The PSM maps these flags as follows:  

The  Inline Qo s Flag  is represented with the literal ‘Q’.  $\mathrm{Q=}1$   means that the  DataFrag  Submessage contains  the inlineQos Sub message Element.  

The value of the  Inline Qo s Flag  can be obtained from the expression:  

The  KeyFlag  is represented with the literal ‘K.’  

The value of the  KeyFlag  can be obtained from the expression:  

$\begin{array}{r l}{\mathbb{K}}&{{}=}\end{array}$   Sub message Header.flags &   $0\!\times\!0\,4$  

$\mathrm{K}{=}0$   means that the serialized Payload Sub message Element contains the serialized Data.   $\scriptstyle{\mathrm{K}}=1$   means that the  serialized Payload Sub message Element contains the serialized Key.  

The  NonStandard Payload Flag  is represented with the literal ‘N.’ The value of the  NonStandard Payload Flag   can be obtained from the expression.  

## 9.4.5.5  Gap Submessage  

Sub clause 8.3.7.4 in the PIM defines the logical contents of the  Gap  Submessage. The PSM maps the  Gap  Submessage into the following wire representation:  

0...2...........7...............15.............23...............31  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |  GAP  |X|X|X|X|X|X|X|E|  octets To Next Header  |  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/464d936f5c929ff4e52928fd829466729f8fc81baa95c3a4b62dd6cdffee6829.jpg)  

##   Flags in the Submessage Header  

This Submessage has no flags in addition to the  Endian ness Flag .  

## 9.4.5.6  HeartBeat Submessage  

Sub clause 8.3.7.5 in the PIM defines the logical contents of the  HeartBeat  Submessage. The PSM maps the  

HeartBeat  Submessage into the following wire representation:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/6ac336bf586fc61d52ae1ac6389b26cffd7ebdcd18b0b32325d3ef2f56a9aeb0.jpg)  

##   Flags in the Submessage Header  

In addition to the  Endian ness Flag , the  HeartBeat  Submessage introduces the  FinalFlag  and the  Liveliness Flag  (8.3.6.2). The PSM maps the  FinalFlag  flag into the 2nd least-significant bit (LSB) of the flags  and the  Liveliness Flag  into the 3rd least-significant bit (LSB) of the flags.  

The  FinalFlag  is represented with the literal ‘F’.  $\mathrm{F}{=}1$   means the  Writer  does not require a response from the  Reader .  $\mathrm{F}{=}0$   means the  Reader  must respond to the  HeartBeat  message.  

The value of the  FinalFlag  can be obtained from the expression:  

$\begin{array}{r l}{\mathrm{~F~}{}={}}\end{array}$   Sub message Header.flags &   $0\!\times\!0\,Z$  

The  Liveliness Flag  is represented with the literal ‘L’.  $\scriptstyle{\mathrm{L}}=1$   means the DDS DataReader associated with the  RTPS  Reader  should refresh the ‘manual’ liveliness of the DDS DataWriter associated with the RTPS  Writer  of  the message. The value of the  Liveliness Flag  can be obtained from the expression:  

$\begin{array}{r l}{\mathbb{L}}&{{}=}\end{array}$   Sub message Header.flags &   $0\!\times\!0\,4$  
## 9.4.5.7  HeartBeat Frag Submessage  

Sub clause 8.3.7.6 in the PIM defines the logical contents of the  HeartBeat Frag  Submessage. The PSM  maps the  HeartBeat Frag  Submessage into the following wire representation:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/4716834531dabd8ba7ee5781dc5a0cfeb78cfae6b05aeeebacf04a28abad0501.jpg)  

##   Flags in the Submessage Header  

The  HeartBeat Frag  Submessage introduces no other flags in addition to the  Endian ness Flag .  

## 9.4.5.8  Info Destination Submessage  

Sub clause 8.3.7.7 in the PIM defines the logical contents of the  Info Destination  Submessage. The PSM  maps the  Info Destination  Submessage into the following wire representation:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/de9afd78cc529f01169941aa53d8c81e07bb4020efc39abd5595befb5afd403e.jpg)  

##   Flags in the Submessage Header  

This Submessage has no flags in addition to the  Endian ness Flag .  

## 9.4.5.9  InfoReply Submessage  

Sub clause 8.3.7.8 in the PIM defines the logical contents of the  InfoReply  Submessage. The PSM maps the InfoReply  Submessage into the following wire representation:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/0b3390b8c8453fc0498f65e3de03409e41de23c3bfd86bfefb0a38ca58cee2dc.jpg)  
##   Flags in the Submessage Header  

In addition to the  Endian ness Flag , The  InfoReply  Submessage introduces the  Multi cast Flag  (8.3.6.2). The  PSM maps the  Multi cast Flag  flag into the 2nd least-significant bit (LSB) of the flags.  

The  Multi cast Flag  is represented with the literal   $\mathbf{\epsilon}^{\mathbf{\alpha}}\mathbf{M}^{\mathbf{\alpha}}$  .  $\scriptstyle{\mathrm{M}}=1$   means the  InfoReply  also includes a  multi cast Locator List . The value of the  Multi cast Flag  can be obtained from the expression:  

$\mathrm{~\mathsf~{~M~}~}=$   Sub message Header.flags &   $0\!\times\!0\,Z$  

## 9.4.5.10 InfoSource Submessage  

Sub clause 8.3.7.9 in the PIM defines the logical contents of the  InfoSource  Submessage. The PSM maps  the  InfoSource  Submessage into the following wire representation:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/85b3c4e2ee05acaf237c94232f2ad990073294a389d8b509b44507385a681697.jpg)  

##  Flags in the Submessage Header  

This Submessage has no flags in addition to the  Endian ness Flag .  

## 9.4.5.11 Info Timestamp Submessage  

Sub clause 8.3.7.10 in the PIM defines the logical contents of the  Info Timestamp  Submessage. The PSM  maps the  Info Timestamp  Submessage into the following wire representation:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/ae5651de2aa04116b038440bb9b5a9a3097677dc27ffcc5c91e1ff4d1911954e.jpg)  

##  Flags in the Submessage Header  

In addition to the  Endian ness Flag , The  Info Timestamp  Submessage introduces the  Invalidate Flag  (8.3.6.2). The PSM maps the  Invalidate Flag  flag into the 2nd least-significant bit (LSB) of the flags.  

The  Invalidate Flag  is represented with the literal ‘I’.  $\mathrm{I}{=}0$   means the  Info Timestamp  also includes a  timestamp .  $\mathrm{I}{=}1$   means subsequent Sub messages should not be considered to have a valid timestamp.  

The value of the  Invalidate Flag  can be obtained from the expression:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/77e7d3fec1af6271976f9fb3c17ffc976572f800b00299578d1733a8365ed2a9.jpg)  

## 9.4.5.12 Pad Submessage  

Sub clause 8.3.7.12 in the PIM defines the logical contents of the  Pad  Submessage. The PSM maps the  Pad  Submessage into the following wire representation:  
0...2...........8...............16.............24...............32  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |  PAD  |X|X|X|X|X|X|X|E|  octets To Next Header  |  +---------------+---------------+---------------+---------------+  

##  Flags in the Submessage Header  

This Submessage has no flags in addition to the  Endian ness Flag .  

## 9.4.5.13 NackFrag Submessage  

Sub clause 8.3.7.11 in the PIM defines the logical contents of the  NackFrag  Submessage. The PSM maps the NackFrag  Submessage into the following wire representation:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/7f377c55158744d7d525786ceda174cf76e6198061f2547b308285cbffdc9eac.jpg)  

##  Flags in the Submessage Header  

This Submessage has no flags in addition to the  Endian ness Flag .  

## 9.4.5.14 Info Reply Ip 4 Submessage (PSM specific)  

The  Info Reply Ip 4  Submessage is an additional Submessage introduced by the UDP PSM.  

Its use and interpretation are identical to those of an  InfoReply  Submessage containing a single unicast and  possibly a single multicast locator, both of kind  LOCATOR KIND UDP v 4 . It is provided for efficiency reasons and  can be used instead of the  InfoReply  Submessage to provide a more compact representation.  

The PSM maps the  Info Reply Ip 4  Submessage into the following wire representation:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/8f8f439d1d554d49d3bbdbe7c1c06b1290e3930460e3d109b82c7c6387a8381d.jpg)  

##  Flags in the Submessage Header  

In addition to the  Endian ness Flag , The  Info Reply Ip 4  Submessage introduces the  Multi cast Flag . The PSM  maps the  Multi cast Flag  flag into the 2nd least-significant bit (LSB) of the flags.  
The  Multi cast Flag  is represented with the literal   $\mathbf{\epsilon}^{\mathbf{\alpha}}\mathbf{M}^{\mathbf{\alpha}}$  .  $\scriptstyle{\mathrm{M}}=1$   means the  Info Reply Ip 4  also includes a  multi cast R Locator .  

The value of the  Multi cast Flag  can be obtained from the expression:  

$\mathrm{~\mathsf~{~M~}~}=$   Sub message Header.flags & 0x02  

## 9.5  Mapping to UDP/IP Transport Messages  

When RTPS is used over UDP/IP, a  Message  is the contents (payload) of exactly one UDP/IP Datagram.  

## 9.6  Mapping of the RTPS Protocol  

## 9.6.1  Default Locators  

9.6.1.1  Discovery traffic  

Discovery traffic is the traffic generated by the Participant and Endpoint Discovery Protocols. For the Simple  Discovery Protocols (SPDP and SEDP), discovery traffic is the traffic exchanged between the built-in  Endpoints .  

The SPDP built-in  Endpoints  are configured using well-known ports (see 8.5.3.4). The UDP PSM maps these  well-known ports to the port number expressions listed in Table 9.8.  

Table 9.8 - Ports used by built-in Endpoints  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/28df001847203a4216e808bc7659a7a32027cd7efea6607d31f4e425ee815925.jpg)  
where  domainId  $=$   DDS Domain identifier   participant Id  $=$   Participant identifier  PB, DG, d0, d1 $=$  tunable parameters (defined below)  

The  domainId  and  participant Id  identifiers are used to avoid port conflicts among  Participants  on the same  node. Each  Participant  on the same node and in the same domain must use a unique  participant Id . In the case  of multicast, all  Participants  in the same domain share the same port number, so the  participant Id  identifier is  not used in the port number expression.  

To simplify the configuration of the SPDP,  participant Id  values ideally start at 0 and are incremented for each  additional  Participant  on the same node and in the same domain. That way, for a given domain,  Participants  can announce their presence to up to N remote  Participants  on a given node, by announcing to port numbers on  that node corresponding to  participant Id  0 through N-1.  

The default ports used by the SEDP built-in  Endpoints  match those used by the SPDP. If a node chooses not to  use the default ports for the SEDP, it can include the new port numbers as part of the information exchanged  during the SPDP.  

## 9.6.1.2  User traffic  

User traffic is the traffic exchanged between user-defined Endpoints (i.e., non-built-in  Endpoints) . As such, it  pertains to all the traffic that is not related to discovery. By default, user-defined  Endpoints  use the port number  expressions listed in Table 9.9.  
Table 9.9 - Ports used by user-defined Endpoints  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/6334b9ceb34d87acaf382a4efc53005675cdb10db4ae42c26296a9dd63766d85.jpg)  

User-defined Endpoints may choose to not use the default ports. In that case, remote Endpoints obtain the port  number as part of the information exchanged during the Simple Endpoint Discovery Protocol.  

## 9.6.1.3  Default Port Numbers  

The port number express s ions use the following parameters:  

$\begin{array}{r l}{\mathbb{D}G}&{{}=}\end{array}$   DomainId Gain   $\begin{array}{r l}{\mathbb{P}G}&{{}=}\end{array}$   Participant Id Gain    $\begin{array}{r l}{\mathtt{P B}}&{{}=}\end{array}$   Port Base number  d0, d1, d2,  $\mathrm{d}\,\boldsymbol{\mathfrak{I}}\ =$  additional offsets  

Implementations must expose these parameters so they can be customized by the user.  

In order to enable out-of-the-box interoperability, the following default values must be used:  

$$
\begin{array}{r l}{\tt P B}&{=\ 740}\\ {\tt D G}&{=\ 250}\\ {\tt P G}&{=\ 2}\\ {\tt d0}&{=\ 0}\\ {\tt d1}&{=\ 10}\\ {\tt d2}&{=\ 1}\\ {\tt d3}&{=\ 11}\end{array}
$$  

Given UDP port numbers are limited to 64K, the above defaults enable the use of about 230 domains with up to  120  Participants  per node per domain.  

## 9.6.1.4  Default Settings for the Simple Participant Discovery Protocol  

When using the SPDP, each  Participant  sends announcements to a pre-configured list of locators. What ports to  use when configuring these locators is discussed above. This sub clause describes any remaining settings that  are required to enable plug-and-play interoperability.  

##   Default multicast address  

In order to enable plug-and-play interoperability, the default pre-configured list of locators must include the  following multicast locator (assuming UDPv4):  

Default Multi cast Locator   $=$   {LOCATOR_KIND_UDPv4, “239.255.0.1”, PB + DG \*  domainId + d0}  

All  Participants  must announce and listen on this multicast address.  

S PDP built in Participant Writer.reader Locator s CONTAINS Default Multi cast Locator  S PDP built in Participant Reader.multi cast Locator List CONTAINS  Default Multi cast Locator  

##   Default announcement rate  

The default rate by which SPDP periodic announcements are sent equals 30 seconds.  
## 9.6.2  Data representation for the built-in Endpoints  

## 9.6.2.1  Data Representation for the Participant Message Data Built-in Endpoints  

The Behavior module within the PIM (8.4) defines the DataType  Participant Message Data . This type is the  logical content of the  Built in Participant Message Writer  and  Built in Participant Message Reader  built-in  Endpoints.  

The PSM  maps the  Participant Message Data  type into the following IDL:  

typedef octet Octet Array 4[4];   typedef sequence<octet> OctetSeq;   struct Participant Message Data {  Gui d Prefix t  participant Gui d Prefix;   Octet Array 4  kind;  OctetSeq  data;  };  

The following values for the kind field are reserved by RTPS:  

#define PARTICIPANT MESSAGE DATA KIND UNKNOWN {  $0\!\times\!00$  ,   $0\!\times\!0\,0$  ,   $0\!\times\!00$  ,   $0\!\times\!0\,0$  }  #define PARTICIPANT MESSAGE DATA KIND AUTOMATIC LIVELINESS UPDATE { $0\!\times\!0\,0$ ,  $0\!\times\!0\,0$ ,  $0\!\times\!0\,0$ ,  $0\!\times\!0\,\bot$ } #define PARTICIPANT MESSAGE DATA KIND MANUAL LIVELINESS UPDATE {  $0\!\times\!0\,0$  ,   $0\!\times\!0\,0$  ,   $0\!\times\!0\,0$  ,   $0\!\times\!0\,Z$  }  

RTPS also reserves for future use all values of the kind field where the most significant bit is not set. Therefore:  

kind.value[0] &   $0\times80\;\;==\;\;0$   // reserved by RTPS   kind.value[0] &   $0\times80\;\;==\;\;1$   // vendor specific kind  

Implementations can decide the upper length of the data field but must be able to support at least 128 bytes.  Following the CDR encoding, the wire representation of the  Participant Message Data  structure is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/38e7676df9c06ead006714a08a62808f2e918308ad75b39dfe677a2d4a20e4cb.jpg)  

## 9.6.2.2  Simple Discovery Protocol built-in Endpoints  

The Discovery Module within the PIM (8.5) defines the DataTypes  S PDP discovered Participant Data ,  Discovered Writer Data ,  Discovered Reader Data , and  Discovered Topic Data . These types define the logical  contents of the data sent between the RTPS built-in Endpoints.  
The PSM maps these types into the following IDL:  

struct S PDP discovered Participant Data {       DDS::Participant Built in Topic Data dd s Participant Data;       Participant Proxy participant Proxy;      Duration_t lease Duration;  };  struct Discovered Writer Data {       DDS::Publication Built in Topic Data dd s Publication Data;       Writer Proxy m Writer Proxy;  };  struct Discovered Reader Data {       DDS::Subscription Built in Topic Data dd s Subscription Data;       Reader Proxy m Reader Proxy;  Content Filter Property t content Filter;  };  struct Discovered Topic Data {       DDS::Topic Built in Topic Data dd s Topic Data;  };  

where each DDS built-in topic data type is defined by the DDS specification.  

The discovery data is sent using standard  Data  Sub messages. In order to allow for QoS extensibility while  preserving interoperability between versions of the protocol, the wire-representation of the  Serialized Data  within the  Data  Submessage uses the format of a  Parameter List  Sub message Element. That is, the  Serialized Data  contains each QoS and other information within a separate parameter identified by a  Parameter Id. Within each parameter, the parameter value is represented using CDR.  

For example, in order to add a vendor-specific Endpoint Discovery Protocol (EDP) in the  S PDP discovered Participant Data , a vendor could define a vendor-specific parameter Id and use it to add a new  parameter to the  Parameter List  contained in  S PDP discovered Participant Data . The presence of this  parameter Id would denote support for the corresponding EDP. As this is a vendor-specific parameter Id, other  vendors’ implementations would simply ignore the parameter and the information it contains. The parameter  itself would contain any additional data required by the vendor-specific EDP represented using CDR.  

For optimization, implementations of the protocol shall not include a parameter in the Data submessage if it  contains information that is redundant with other parameters already present in that same Data submessage. As a  result of this optimization an implementation shall omit the serialization of the parameters listed in    Table  9.10.  

The key-only messages for the built-in topics are defined as follows. In the case of a DATA submessage  containing the  S PDP discovered Participant Data  with KeyFlag  ${=}1$  , the only parameter Id present within the  Parameter List  shall be the PID PARTICIPANT GUI D. In the case of a DATA submessage containing  one of  SED P discovered Publication Data ,  SED P discovered Subscription Data , or  SED P discovered Topic Data   with KeyFlag  $\scriptstyle{=1}$  , the only parameter Id present within the  Parameter List  shall be the  PID ENDPOINT GUI D.  

Table 9.10 - Omitted Builtin Endpoint Parameters  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/64362915abcfabd802c0c0a7c1d9d191c3b556a90dfbca30fc88f6c2bfbf6485.jpg)  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/4a96e898f98d3e1d6edef3c1bda34b2fb4923f9d30167e788ddbaa80120ef2db.jpg)  

For example, an implementation of the protocol sending DATA message containing the  S PDP discovered Participant Data, SED P discovered Publication Data, or SED P discovered Subscription Data shall  omit the parameter that contains the guidPrefix. The implementation of the protocol in the receiver side shall  derive this value from the “key” parameter which is one of the following: “Participant Built in Topic Data::key”,  “Subscription Built in Topic Data::key”, or “Publication Built in Topic Data::key”.  

##   Parameter Id space  

As described in 9.4.2.11, the Parameter Id space is 16 bits wide. In order to accomodate vendor specific options  and future extensions to the protocol, the Parameter Id space is partitioned into multiple subspaces. The  Parameter Id subspaces are listed in Table 9.11.  

Table 9.11 - Parameter Id subspaces  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/c438f7d66ecc6b7a37a805e292a6baeb2e00dc87e51f04d5a72969a513bc72d0.jpg)  

The first subspace division enables vendor-specific Parameter Ids. Future minor versions of the RTPS protocol  can add new parameters up to a maximum Parameter Id of 0x7fff. The range  $0x8000$   to 0xffff is reserved for  vendor-specific options and will not be used by any future versions of the protocol. Other specifications may  reserve portions of the protocol-specific range of Parameter Ids. Table 9.12 lists the Parameter Ids reserved for  use by other specifications and future revisions thereof. Other specifications may reserve portions of the  protocol-specific range of Parameter Ids. Table 9.12 lists the Parameter Ids reserved for use by other  specifications and future revisions thereof.  

Table 9.12 - Parameter Ids Reserved by other Specifications (all ranges are inclusive)  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/d4629806bbb399545f0c5675e6d51da84db6d51b3705ef9bbba5abb70d6f657e.jpg)  

For backwards compatibility, both subspaces are subdivided again. If a Parameter Id is expected, but not present,  the protocol will assume the default value. Similarly, if a Parameter Id is present but not recognized, the protocol  will either skip and ignore the parameter or treat the parameter as an incompatible QoS. The actual behavior  depends on the Parameter Id value, see Table 9.11.  

##   Parameter ID values  

Table 9.13 summarizes the list of Parameter Ids used for the data for the built-in Entities. Table 9.14 lists the  
Entities to which each parameter ID applies and its default value.  

Table 9.13 - Parameter Id Values  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/77696a79a3f3b340ef7100f813883bb7d9f62f0780880e2b27fdd427ba7755a7.jpg)  
Table 9.14 - Parameter Id mapping and default values  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/bd9ee91a6bf53e4bfac8a30279824fafb434ad113a1ef451a2a7e6e563cac134.jpg)  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/5d8c6d45dde8a138d4baf8096d2ab813d6230a813052f9041be7a75bb76791aa.jpg)  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/c1d152fa9a1a79a218b5c3d729c01fc925b48b313d57267e0c1568ad59b52b4e.jpg)  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/60bc6fcd65a2dae8d1b0a326983fc611a023221d653066fa189a43c4edc241e6.jpg)  

## 9.6.3  Parameter Id Definitions used to Represent In-line QoS  

The Messages module within the PIM (8.3) provides the means for the  Data  (8.3.7.2) and  DataFrag  (8.3.7.3) Sub messages to include QoS policies in-line with the Submessage. The QoS policies are contained  using a  Parameter List .  

Sub clause 8.7.2.1 defines the complete set of parameters that can appear within the inlineQos  Sub message Element. The corresponding set of parameter Ids is listed in Table 9.15.  

Table 9.15 - Inline QoS parameters  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/dcdc2e9b6770201a59a382d12aac7f0828d6890abbd8b609eac496f32b428f93.jpg)  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/519e96dd75fa3892d7c92d3d77d332958d149f5354b2dd9ff65011a6c2cb0dd1.jpg)  

The policies that can appear in-line include a subset of the DataWriter QoS policies (Parameter Id defined in  9.6.2) and some additional QoS (for which a new Parameter Id is defined).  

The following sub clauses describe these additional QoS in more detail.  

## 9.6.3.1  Content filter info (PID CONTENT FILTER INFO)  

Following the CDR encoding, the wire representation of the  Content Filter Info t  (see 9.3.2) in-line QoS is:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/18c4f060ba9d977404ed98abf827c4d6258232ab29d7c1a7a558c3e2a156f30b.jpg)  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/c5b757b1d91c43d8de54d79a43f117d5c8fd56e51149ef7330fd3c1471155d32.jpg)  

The  filter Result  member is encoded as a bitmap. Bit 0 (MSB) corresponds to the first filter signature, bit 1 to the  second filter signature, and so on. The content filter info in-line QoS is invalid unless  

The bitmap is interpreted as follows:  

Table 9.16 - Interpretation of filter Result member in content filter info in-line QoS  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/944e12bf3cff26e49565b72d2848ba6c9b0a6b5c5aa089dbce765f1e19b79206.jpg)  

A filter’s signature is calculated as the 128-bit MD5 checksum of all strings in the filter's  Content Filter Property t . More precisely, all strings are combined into the following character array:  

[ content Filtered Topic Name related Topic Name filter Class Name filter Expression  expression Parameters[0] expression Parameters[1] ...  expression Parameters[numParams - 1] ]  

where each individual string includes its NULL termination character. The filter signature is calculated by  taking the MD5 checksum of the above character sequence.  

## 9.6.3.2  Coherent set (PID COHERENT SET)  

The coherent set in-line QoS parameter uses the CDR encoding for Sequence Number t .  

As defined in 8.7.5, all  Data  and  DataFrag  Sub messages that belong to the same coherent set must contain  the coherent set in-line QoS parameter with value equal to the sequence number of the first sample in the set.  

For example, assume a coherent set contains sample updates with sequence numbers 3, 4, 5 and 6 from a given  Writer . Samples in this coherent set are identified by including the coherent set in-line QoS parameter with  value 3. Some example  Data  sub messages that the  Writer  can use to denote the end of this coherent set are  listed in Table 9.17.  

Table 9.17 - Example Data Sub messages to denote the end of a coherent set  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/f5fc3dd540038a2bf95bcbd3df49caf9b848c8377ce21bdc300e01ce9cf938fe.jpg)  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/8f116020b9c0b7c31dddc61525a9cad49e6a9098a361771cbd39f6df826a3ee9.jpg)  

## 9.6.3.3  Group Coherent Set (PID GROUP COHERENT SET)  

The group coherent set in-line QoS parameter uses the CDR encoding for Sequence Number t.  

As defined in 8.7.6, all  Data  sub messages and the first  DataFrag  submessage belonging to a sample must  contain the group coherent set in-line QoS parameter with value equal to the group sequence number of the first  sample in the set.  

For example, assume a group coherent set contains samples with group sequence numbers 11, 12, and 13 from  two  Writers . Samples in the coherent set are identified by including coherent set in-line QoS parameters and  group coherent set in-line QoS parameters, among others. Example  Data  Sub messages are listed in Table 9.18  - Example Data Sub messages in a GROUP coherent setTable 9.18 .  

Table 9.18 - Example Data Sub messages in a GROUP coherent set  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/1cf50a340fd17478c976d64640a2a04e2eb83874b67c0918d8ed31f88df363a4.jpg)  

## 9.6.3.4  Group Sequence Number (PID GROUP SEQ NUM)  

The group sequence number in-line QoS parameter uses the CDR encoding for  Sequence Number t .  

As defined in 8.7.5, all  Data  sub messages and the first  DataFrag  submessage sent by  Data Writers  belonging  to a  Publisher  with Presentation access scope   $G R O U P$  must contain the group sequence number in-line QoS  parameter with value equal to the group sequence number.  
## 9.6.3.5  Publisher Writer Info (PID WRITER GROUP INFO)  

The publisher writer info in-line QoS parameter uses the CDR encoding for  Writer Group Info t . See clause  8.7.5.  

As defined in 8.7.5, for  Data Writers  belonging to a  Publisher  with Presentation  access scope   GROUP , the  Data  sub messages and the first  DataFrag  submessage of each sample shall contain the publisher writer info  in-line QoS parameter.  

The  End Coherent Set   Data  submessage (see clause 8.7.6) for those  Data Writers  shall also contain the  publisher writer info in-line QoS parameter.  

## 9.6.3.6  Secure Publisher Writer Info (PID SECURE WRITER GROUP INFO)  

The secure publisher writer info in-line QoS parameter uses the CDR encoding for  Writer Group Info t . See  clause 8.7.5.  

The secure publisher writer info in-line QoS is reserved for DDS Security. In the cases when it is used it shall be  added anywhere that the  PID WRITER GROUP INFO  in-line QoS is required.  

## 9.6.3.7  Original Writer Info (PID ORIGINAL WRITER INFO)  

Following the CDR encoding, the wire representation of the  Original Writer Info t  (see 9.3.2) in-line QoS shall  be:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/67e610bfcfa22d9c65c6484c09918ba8f4a440e270b1dd72a0bb61145c75b673.jpg)  

The original writer info parameter may appear in the  Data  or in the  DataFrag  sub messages.  

## 9.6.3.8  KeyHash (PID KEY HASH)  

The key hash inline parameter contains the CDR encoding of the  KeyHash_t . The  KeyHash_t  is defined as a 16- Byte octet array (see 9.3.2) therefore the key hash inline parameter just copies those 16 Bytes.  

The  KeyHash_t  is computed from the Data as follows using one of two algorithms depending on whether the  Data type is such that the maximum size of the sequential CDR encapsulation of all the key fields is less than or  equal to 128 bits (the size of the  KeyHash_t ).  

•   If the maximum size of the sequential CDR representation of all the key fields is less than or  equal to 128 bits, then the  KeyHash_t  shall be computed as the CDR Big-Endian representation  
of all the Key fields in sequence. Any unfilled bits in the  KeyHash_t  shall be set to zero.  

•   Otherwise the  KeyHash_t  shall be computed as a 128-bit MD5 Digest (IETF RFC 1321) applied  to the CDR Big- Endian representation of all the Key fields in sequence.  

Note that the choice of the algorithm to use depends on the data-type, not on any particular data value.  

Example 1 . Assume the following IDL-described type:  

struct Type With Short Key {      long id;     $/\star$   assume defined as a key field \*/       string name  $<\!6\!>$  ;   $/\star$   assume defined as a key field \*/      /\* other non-key fields \*/  };  

Then we know that the maximum size for the CDR representation of the key fields is 15 Bytes (4 for the ‘id’  field, plus 4 for the length of the string ‘name’ plus at most 7 Bytes for the string (includes extra byte for  terminating NUL).  

In this example the  KeyHash_  $t$    shall be computed as:  

Where CDR(x) represents the big-endian CDR representation of that field.  

A concrete data value of this type such as {32, “hello”, ...} would be represented as:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/a00294969044cdc10ca61bdd75c9dd59bde8c81ee7ec44ab75c165eaea440208.jpg)  

Note that for clarity use a notation where each byte can be represented either as a hexadecimal number (e.g.,   $0\mathbf{x}20$   or as a character (e.g., ‘h’);  

Example 2 : Assume the following IDL-described type:  

struct Type With Short Key {      long id;     $/\star$   assume defined as a key field \*/       string name  ${<}8{>}$  ;   $/\star$   assume defined as a key field \*/        /\* other non-key fields \*/   };  

Then we know that the maximum size for the CDR representation of the key fields is 17 Bytes (4 for the ‘id’  field, plus 4 for the length of the string ‘name’ plus at most 9 Bytes for the string (includes extra byte for  terminating NUL).  

In this example the  KeyHash_t  shall be computed as:  

## 9.6.3.9  Status Info t (PID STATUS INFO)  

The status info parameter contains the CDR encoding of the  Status Info t . The  Status Info t  is defined as a 4- Byte octet array (see 9.3.2) therefore the status info inline parameter just copies those 4 Bytes.  

The status info parameter may appear in the  Data  or in the  DataFrag  sub messages.  
The Status Info t shall be interpreted as a 32-bit worth of flags with the layout shown below:  

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/d1e5af16c1894d0c9b7b322185e4615bf088f022305414f737195f0b40ac87e1.jpg)  

The flags represented with the literal ‘X’ are unused by this version of the protocol and should be set to zero by  the writer and not interpreted by the reader so that they may be used in future versions of the protocol without  breaking interoperability.  

The flags in the status info provide information on the status of the data-object to which the submessage refers.  Specifically, the status info is used to communicate changes to the Lifecycle State of a data-object instance.  

The current version of the protocol defines the  Disposed Flag , the  Unregistered Flag , the  Filtered Flag .  

The  Dispose ed Flag  is represented with the literal ‘D.’  

$\scriptstyle{\mathrm{D}}=1$   indicates that the DDS DataWriter has disposed the instance of the data-object whose Key appears in the  submessage.  

The  Unregistered Flag  is represented with the literal ‘U.’  

$\mathrm{U}{=}1$   indicates that the DDS DataWriter has unregistered the instance of the data-object whose Key appears in  the submessage.  

The  Filtered Flag  is represented with the literal ‘F.’  

$\mathrm{F}{=}1$   indicates that the DDS DataWriter has written as sample for the instance of the data-object whose Key  appears in the submessage but the sample did not pass the content filter specified by the DDS DataReader.  

If both Disposed Flag  $\==0$   and Unregistered Flag  $\mathord{=}0$  , then the data-object whose Key appears in the Submessage  has Instance State ALIVE in the DDS DataWriter. In this case the value of the Filtered Flag indicates whether the  sample that was written for that data-object instance passed the reader-specified filter: Filtered Flag  $\scriptstyle{\mathfrak{f}}=0$   indicates  the sample passed the filter and Filtered Flag  $\scriptstyle{\stackrel{=}{1}}$   indicates it did not pass the filter.  

Note that the protocol does not require that the DDS DataWriter propagates the "register" operation. Therefore,  the DDS DataWriter can implement 'register' as a local operation. Since the DDS DataWriter register operation  does not provide a data value propagating the register operation would be of limited use to the DataReader.  

## 9.6.4  Parameter Ids Deprecated by the Protocol  

The Parameter Ids shown in Table 9.19 have been deprecated by the versions indicated in the table. These  parameters should not be used by versions of the protocol equal or newer than the deprecated version unless  they are used with the same meaning as in versions prior to the deprecated version. Implementations that wish  to interoperate with earlier versions should send and process the parameters in Table 9.18.  

Table 9.19 – Deprecated Parameter Id Values  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/a80d2e3a15e10ef81d3421854c16705c26504255cdaece18c2f12e81dd6ac110.jpg)  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/0c1cd9551f3e45b45953e6ef3e61790c63f3c0c6effb53ddba80e7fafc27f21a.jpg)  
This page intentionally left blank.
