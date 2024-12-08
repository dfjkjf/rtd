---
layout: default
title: Messages Module
nav_order: 3
---

# 8.3 Messages Module 

The Messages module describes the overall structure and logical contents of the messages that are exchanged between the RTPS Writer endpoints and RTPS Reader endpoints. RTPS Messages are modular by design and 
can be easily extended to support both standard protocol feature additions as well as vendor-specific extensions.

## 8.3.1 Overview 

The Messages module is organized as follows: 
- 8.3.2 introduces any additional types needed for defining RTPS messages in the subsequent sub clauses.
- 8.3.3 describes the common structure used for all RTPS Messages. All RTPS Messages consist of a Header followed by a series of Sub messages. The number of Sub messages that can be sent in a single RTPS Message is only limited by the maximum message size the underlying transport can support.
- Certain Sub messages may affect how subsequent Sub messages within the same RTPS Message must be interpreted. The context for interpreting Sub messages is maintained by the RTPS Message Receiver and is described in 8.3.4.
- 8.3.5 lists the elementary building blocks for creating Sub messages, also referred to as Sub message Elements. This includes sequence number sets, timestamp, identifiers, etc.
- 8.3.6 describes the structure of the RTPS Header. The fixed size RTPS Header is used to identify an RTPS Message.
- Finally, 8.3.7 introduces all available Sub messages in detail. For each Submessage, the specification defines its contents, when it is considered valid and how it affects the state of the RTPS Message Receiver. The PSM will define the actual mapping of each of these Submessage to bits and bytes on the wire in 9.4.5.

## 8.3.2 Type Definitions 

In addition to the types defined in 8.2.1.2, the Messages module makes use of the types listed in Table 8.13.

Table 8.13 - Types used to define RTPS messages 
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/b39a5a03ac226ab0ea972bd317bcf2b9961af8f0aa6c5578a11b7754e420ffc9.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/bcf84a20d49f44ce67654de1411311c3c09c95e9fff33e08d5390d368390501b.jpg)

## 8.3.3 The Overall Structure of an RTPS Message 

The overall structure of an RTPS Message consists of a fixed-size leading RTPS Header followed by a variable number of RTPS Submessage parts. Each Submessage in turn consists of a Sub message Header and a variable number of Sub message Elements . This is illustrated in Figure 8.8.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/7820755bf8b2fc28887752331ec7d5a07f41caa990e8f5920f51f76dda917bf9.jpg)
Figure 8.8 - Structure of RTPS Messages 

Each message sent by the RTPS protocol has a finite length. This length is not sent explicitly by the RTPS protocol but is part of the underlying transport with which RTPS messages are sent. In the case of a packetoriented transport (like UDP/IP), the length of the message is already provided by the transport headers. A stream-oriented transport (like TCP) would need to insert the length ahead of the message in order to identify the boundary of the RTPS message.

## 8.3.3.1 Header structure 

The RTPS Header must appear at the beginning of every message.
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/4e8ec319a9680a8e328596055a2bb763d4cc33b7e5532a3f848fd50a5a6bef8a.jpg)
Figure 8.9 - Structure of the RTPS Message Header 

The Header identifies the message as belonging to the RTPS protocol. The Header identifies the version of the protocol and the vendor that sent the message. The Header contains the fields listed in Table 8.14.

Table 8.14 - Structure of the Header 
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/5399743340e9212d95876ee3e43bc531be893d9cef61a61508385f564e9e74d1.jpg)

##  protocol 

The protocol identifies the message as an RTPS message. This value is set to PROTOCOL RTP S.

##  version 

The version identifies the version of the RTPS protocol. Implementations following this version of the document implement protocol version 2.4 (major $=2$ , minor $=4$ )
 and have this field set to PROTOCOL VERSION.

##  vendorId 

The vendorId identifies the vendor of the middleware that implemented the RTPS protocol and allows this vendor to add specific extensions to the protocol. The vendorId does not refer to the vendor of the device or product that contains RTPS middleware. The possible values for the vendorId are assigned by the OMG.

The protocol reserves the following value: 

VENDOR ID UNKNOWN 

Vendor IDs can only be reserved by implementers that commit to comply with the current major version of the protocol. To facilitate incremental evolution, the list of vendor IDs is managed separately from this specification. The list is maintained on the OMG DDS website and is accessible at: 
## http://portals.omg.org/dds/omg-dds-standard .

Requests for new vendor IDs should be sent via email to dds $@$ omg.org 

##  guidPrefix 

The guidPrefix defines a default prefix that can be used to reconstruct the Globally Unique Identifiers (GUIDs) that appear within the Sub messages contained in the message. The guidPrefix allows Sub messages to contain only the EntityId part of the GUID and therefore saves from having to repeat the common prefix on every GUID (See 8.2.4.1).

## 8.3.3.2 Submessage structure 

Each RTPS Message consists of a variable number of RTPS Submessage parts. All RTPS Sub messages feature the same identical structure shown in Figure 8.10.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/e22f5cf4b55bb03b0224fd70b6c70f0d730fdefb96cadcd65fd51386ce54501e.jpg)
Figure 8.10 - Structure of the RTPS Sub messages 

All Sub messages start with a Sub message Header part followed by a concatenation of Sub message Element parts. The Sub message Header identifies the kind of Submessage and the optional elements within that Submessage. The Sub message Header contains the fields listed in Table 8.15.

Table 8.15 - Structure of the Sub message Header 
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/9eed0daa790e9b26e62578c6fe1d9bfdc55362ee082f50c9b7cfe1084b0fd185.jpg)
The structure of the RTPS Submessage cannot be changed in this major version (2) of the protocol.

##  Sub message Id 

The sub message Id identifies the kind of Submessage . The valid ID’s are enumerated by the possible values of Sub message Kind (see Table 8.13).

The meaning of the Submessage IDs cannot be modified in this major version (2). Additional Sub messages can be added in higher minor versions. In order to maintain inter-oper ability with future versions, Platform Specific Mappings should reserve a range of values intended for protocol extensions and a range of values that are reserved for vendor-specific Sub messages that will never be used by future versions of the RTPS protocol.

##  flags 

The flags in the Submessage header contain 8 boolean values. The first flag, the Endian ness Flag, is present and located in the same position in all Sub messages and represents the endianness used to encode the information in the Submessage . The literal ‘E’ is often used to refer to the Endian ness Flag .

If the Endian ness Flag is set to FALSE, the Submessage is encoded in big-endian format, Endian ness Flag set to TRUE means little-endian.

Other flags have interpretations that depend on the type of Submessage .

##  sub message Length 

Indicates the length of the Submessage (not including the Submessage header). In case sub message Length $>0$ , it is either: 
- The length from the start of the contents of the Submessage until the start of the header of the next Submessage (in case the Submessage is not the last Submessage in the Message)
.
- Or else it is the remaining Message length (in case the Submessage is the last Submessage in the Message)
. An interpreter of the Message can distinguish between these two cases as it knows the total length of the Message .

In case sub message Length $\imath\!=\!\!0$ , the Submessage is the last Submessage in the Message and extends up to the end of the Message . This makes it possible to send Sub messages larger than 64k (the maximum length that can be stored in the sub message Length field), provided they are the last Submessage in the Message .

## 8.3.4 The RTPS Message Receiver 

The interpretation and meaning of a Submessage within a Message may depend on the previous Sub messages contained within that same Message . Therefore, the receiver of a Message must maintain state from previously de serialized Sub messages in the same Message . This state is modeled as the state of an RTPS Receiver that is reset each time a new message is processed and provides context for the interpretation of each Submessage. The RTPS Receiver is shown in Figure 8.11. Table 8.16 lists the attributes used to represent the state of the RTPS Receiver.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/0d5d7f2d0bee290620158ba06ee9219dcfdd896598f6199f23aa585522aa07fc.jpg)
Table 8.16 - Initial State of the Receiver 
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/cab2dd43ac8f9bd155173acd8506c5ecf00fc21d232b6486b6af08f17da3d804.jpg)

## 8.3.4.1 Rules Followed by the Message Receiver 

The following algorithm outlines the rules that a receiver of any Message must follow: 

1. If the full Submessage header cannot be read, the rest of the Message is considered invalid.2. The sub message Length field defines where the next Submessage starts or indicates that the Submessage extends to the end of the Message , as explained in Section 8.3.3.2.3. If this field is invalid, the rest of the Message is invalid.3. A Submessage with an unknown Sub message Id must be ignored and parsing must continue with the next Submessage . Concretely: an implementation of RTPS 2.4 must ignore any Sub messages with IDs that are outside of the Sub message Kind set defined in version 2.4.Sub message Ids in the vendor-specific range coming from a vendorId that is unknown must also be ignored and parsing must continue with the next Submessage .4. Submessage flags. The receiver of a Submessage should ignore unknown flags. An implementation of RTPS 2.4 should skip all flags that are marked as “ X ” (unused) in the protocol.5. A valid sub message Length field must always be used to find the next Submessage , even for Sub messages with known IDs.6. A known but invalid Submessage invalidates the rest of the Message . Sub clause 8.3.7 describes 
each known.

Submessage and when it should be considered invalid. Reception of a valid header and/or Submessage has two effects: 

1. It can change the state of the Receiver; this state influences how the following Sub messages in the Message are interpreted. 8.3.7 discusses how the state changes for each Submessage . In this version of the protocol, only the Header  and the Sub messages InfoSource, InfoReply, Info Destination, and Info Timestamp change the state of the Receiver.2. It can affect the behavior of the Endpoint to which the message is destined. This applies to the basic RTPS messages: Data, DataFrag, HeartBeat, AckNack, Gap, Heartbeat Frag, NackFrag.

Sub clause 8.3.7 describes the detailed interpretation of the Header and every Submessage .

## 8.3.5 RTPS Sub message Elements 

Each RTPS message contains a variable number of RTPS Sub messages. Each RTPS Submessage in turn is built from a set of predefined atomic building blocks called Sub message Elements . RTPS 2.4 defines the following Submessage elements: GuidPrefix, EntityId, Sequence Number, Sequence Number Set, Fragment Number, Fragment Number Set, VendorId, Protocol Version, Locator List, Timestamp, Count, Serialized Data, Parameter List, and Group Digest.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/12c61dabbb51275cb4961fcaeef2478384fa138d6577d9da4301b50314b11f52.jpg)
Figure 8.12 - RTPS Sub message Elements 

## 8.3.5.1 The GuidPrefix, and EntityId 

These Sub message Elements are used to contain the Gui d Prefix t and EntityId_t parts of a GUID_t (defined in 8.2.4.1) within Sub messages.

Table 8.17 - Structure of the GuidPrefix Sub message Element 

Table 8.18 – Structure of the EntityId Sub message Element 
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/77207747daace74aa5d27e1b423478f5366daab7a16e2eb6d432631b10c3d542.jpg)

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/772a9f72ded4fed148184c7e85e54dca636b9b520e001b2ef43248f32852d3df.jpg)
## 8.3.5.2 VendorId 

The VendorId identifies the vendor of the middleware implementing the RTPS protocol and allows this vendor to add specific extensions to the protocol. The vendor ID does not refer to the vendor of the device or product that contains DDS middleware.

Table 8.19 – Structure of the VendorId Sub message Element 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/849d3bbfc26841920cc748960051ee3636b62c107a8a801a476d2883d01a4cb4.jpg)

The following values are reserved by the protocol: 

VENDOR ID UNKNOWN 

Other values must be assigned by the OMG.

## 8.3.5.3 Protocol Version 

The Protocol Version defines the version of the RTPS protocol.

Table 8.20 - Structure of the Protocol Version Sub message Element 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/142c19866a0fd156abdf27adf14ee81cbc91d8d4da3acdaf1d8b8c65b2fce8b6.jpg)

The RTPS protocol version 2.4 defines the following special values: 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/18095d6f878891499fc6c9444cd192508e437343660243e9f6ef791f78e43197.jpg)

## 8.3.5.4 Sequence Number 

A sequence number is a 64-bit signed integer, that can take values in the range:  $\mathrm-2\mathrm{\`{6}}3\mathrm{\leq}\mathrm{N}\mathrm{\leq}=2\mathrm{\`{6}}3\mathrm{-}1$ . The selection of 64 bits as the representation of a sequence number ensures the sequence numbers never 1  wrap.Sequence numbers begin at 1.

Table 8.21 – Structure of the Sequence Number Sub message Elements 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/62a7cb5642b77ac0d559679386710888f20c6785f98997f7d3a663790c9e4ee3.jpg)

The protocol reserves the following value: 

SEQUENCE NUMBER UNKNOWN 

## 8.3.5.5 Sequence Number Set 

Sequence Number Set Sub message Elements are used as parts of several messages to provide binary information about individual sequence numbers within a range. The sequence numbers represented in the 
Sequence Number Set are limited to belong to an interval with a range no bigger than 256. In other words, a valid Sequence Number Set must verify that: 

maximum(Sequence Number Set) - minimum(Sequence Number Set) < 256 minimum(Sequence Number Set) $>=~\perp$ 

The above restriction allows Sequence Number Set to be represented in an efficient and compact way using bitmaps.

Sequence Number Set Sub message Elements are used for example to selectively request re-sending of a set of sequence numbers.

Table 8.22 – Structure of the Sequence Number Set Sub message Element 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/ac31817c803a341fdfccb4bec0570324d6b0b034ba2ee465dad0d408e6382433.jpg)

## 8.3.5.6 Fragment Number 

A fragment number is a 32-bit unsigned integer and is used by Sub messages to identify a particular fragment in fragmented serialized data. Fragment numbers start at 1.

Table 8.23 - Structure of the Fragment Number Sub message Element 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/13dfde354167fbce2e0f4dc75e2d841011c02fa8487634b25202ba3d450a87c2.jpg)

## 8.3.5.7 Fragment Number Set 

Fragment Number Set Sub message Elements are used to provide binary information about individual fragment numbers within a range. The fragment numbers represented in the Fragment Number Set are limited to belong to an interval with a range no bigger than 256. In other words, a valid Fragment Number Set must verify that: 

The above restriction allows Fragment Number Set to be represented in an efficient and compact way using bitmaps.

Fragment Number Set Sub message Elements are used for example to selectively request re-sending of a set of fragments.

Table 8.24 - Structure of the Fragment Number Set Sub message Element 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/8c8129610c39c484570dfd0cb82cfb6d1f301d2d4b8fdc02b7e9fb5efc57f63f.jpg)

## 8.3.5.8 Timestamp 

Timestamp is used to represent time. The representation should be capable of having a resolution of nano 
seconds or better.

Table 8.25 - Structure of the Timestamp Sub message Element 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/ef9fa8b7681ab507299c48df0c1c067ddfc5687d742c5c36674db0f728a99590.jpg)

There are three special values used by the protocol: 

TIME_ZERO TIME INVALID TIME INFINITE 

## 8.3.5.9 Parameter List 

Parameter List is used as part of several messages to contain QoS parameters that may affect the interpretation of the message. The representation of the parameters follows a mechanism that allows extensions to the QoS without breaking backwards compatibility.

Table 8.26 - Structure of the Parameter List Sub message Element 

Table 8.27 - Structure of each Parameter in a Parameter List Sub message Element 
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/b4edde21548c0b85fd378d2c769eed1b15243ec1de8cf745ee2b65b2e69503b4.jpg)

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/d3abdda4d57c98e339a2201b0df34c4f3e75225ca6604879f35b39a1ae81b3a0.jpg)

The actual representation of the Parameter List is defined for each PSM. However, in order to support interoper ability or bridging between PSMs and allow for extensions that preserve backwards compatibility, the representation used by all PSMs must comply with the following rules: 
- There shall be no more than $2^{\wedge}16$  possible values of the Parameter Id t parameter Id .
- A range of $2^{\wedge}15$  values is reserved for protocol-defined parameters. All the parameter id values defined by the 2.4 version of the protocol and all future revisions of the same major version must use values in this range.
- A range of $2^{\wedge}15$  values is reserved for vendor-defined parameters. The 2.4 version of the protocol and any future revisions of the protocol that correspond to the same major version are not allowed to use values in this range.
- The maximum length of any parameter is limited to $2^{\wedge}16$  octets.

Subject to the above constraints, different PSMs might choose different representations for the Parameter Id t.For example, a PSM could represent parameter Id using short integers while another PSM may use strings.

## 8.3.5.10 Count 

Count is used by several Sub messages and enables a receiver to detect duplicates of the same Submessage.
Table 8.28 - Structure of the Count Sub message Element 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/398e1c87d64df73eaec86d4cce5595bc4653701803981deb227895ca2df23e01.jpg)

## 8.3.5.11 Locator List 

Locator List is used to specify a list of locators.

Table 8.29 - Structure of the Locator List Sub message Element 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/aef87a06e44e04b291db629d625753bb7920f9086ca8beb30f9f5c81e98f4255.jpg)

## 8.3.5.12 Serialized Data 

Serialized Data contains the serialized representation of the value of a data-object. The RTPS protocol does not interpret the serialized data-stream. Therefore, it is represented as opaque data. For additional information see, 10 Serialized Payload Representation.

Table 8.30 – Structure of the Serialized Data Sub message Element 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/90eff666d82d7322d5eaa58a9c800dfe61a37d055cec409091d4c296392673b8.jpg)

## 8.3.5.13 Serialized Data Fragment 

Serialized Data Fragment contains the serialized representation of a data-object that has been fragmented. Like for un fragmented Serialized Data, the RTPS protocol does not interpret the fragmented serialized data-stream. Therefore, it is represented as opaque data. For additional information see, Serialized Payload Representation.

Table 8.31 - Serialized Data Fragment 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/7b6930976679d2c373cbfcf92d003332d0ecbecf0d7b5859b14720a7d4944fcc.jpg)

## 8.3.5.14 Group Digest 

Group Digest is used to communicate a set of EntityId_t in a compact manner.

Table 8.32 - Structure of the Group Digest Sub message Element 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/a7ccfddbba240c1635312e9a0505840929839f8d19c2b3c3bf6576f65e4ebbbd.jpg)

## 8.3.6 The RTPS Header 

As described in 8.3.3, every RTPS Message must start with a Header .
## 8.3.6.1 Purpose 

The Header is used to identify the message as belonging to the RTPS protocol, to identify the version of the RTPS protocol used, and to provide context information that applies to the Sub messages contained within the message.

## 8.3.6.2 Content 

The elements that form the structure of the Header were described in 8.3.3.1. The structure of the Header can only be changed if the major version of the protocol is also changed.

## 8.3.6.3 Validity 

A Header is invalid when any of the following are true:
- The Message has less than the required number of octets to contain a full Header . The number required is defined by the PSM.
- Its protocol value does not match the value of PROTOCOL RTP S 2 .
- The major protocol version is larger than the major protocol version supported by the implementation.

## 8.3.6.4 Change in state of Receiver 

The initial state of the Receiver is described in 8.3.4. This sub clause describes how the Header of a new Message affects the state of the Receiver.

Receiver.source Gui d Prefix  $=$  Header.guidPrefix Receiver.source Version $=$  Header.version Receiver.source Vendor Id  $=$  Header.vendorId Receiver.have Timestamp $=$  false 

## 8.3.6.5 Logical Interpretation 

None 

## 8.3.7 RTPS Sub messages 

The RTPS protocol version 2.4 defines several kinds of Sub messages. They are categorized into two groups: Entity- Sub messages and Interpreter-Sub messages. Entity Sub messages target an RTPS Entity . Interpreter Sub messages modify the RTPS Receiver state and provide context that helps process subsequent Entity Sub messages.

The Entity Sub messages are: 
- Data: Contains information regarding the value of an application Date-object.Data Sub messages are sent by Writers to Readers .
- DataFrag : Equivalent to Data , but only contains a part of the new value (one or more fragments). Allows data to be transmitted as multiple fragments to overcome transport message size limitations.
- Heartbeat : Describes the information that is available in a Writer .Heartbeat messages are sent by a Writer to one or more Readers . 
- Heartbeat Frag : For fragmented data, describes what fragments are available in a Writer .Heartbeat Frag messages are sent by a Writer to one or more Readers .
- Gap : Describes the information that is no longer relevant to Readers. Gap messages are sent by a Writer to one or more Readers .- AckNack : Provides information on the state of a Reader to a Writer .AckNack messages are sent by a Reader to one or more Writers .
- NackFrag : Provides information on the state of a Reader to a Writer, more specifically what fragments the Reader is still missing.NackFrag messages are sent by a Reader to one or more Writers .

The Interpreter Sub messages are: 
- InfoSource: Provides information about the source from which subsequent Entity Sub messages originated. This Submessage is primarily used for relaying RTPS Sub messages. This is not discussed in the current specification.
- Info Destination: Provides information about the final destination of subsequent Entity Sub messages. This Submessage is primarily used for relaying RTPS Sub messages. This is not discussed in the current specification.
- InfoReply: Provides information about where to reply to the entities that appear in subsequent Sub messages.
- Info Timestamp : Provides a source timestamp for subsequent Entity Sub messages.
- Pad : Used to add padding to a Message if needed for memory alignment.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/0128c01a101893b9673b770293f60c68600a4b0a99322f2b848209ba398eb835.jpg)
Figure 8.13 - RTPS Sub messages 

This sub clause describes each of the Sub messages and their interpretation. Each Submessage is described in the same manner under the headings described in Table 8.33.

Table 8.33 – Scheme used to describe each Submessage 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/579dc1d952eae9110bc9a64dc52830947e56123a07099ee3502cc5229f84a44e.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/c6a10ac1071d8b22d5064e414cefde2e2086a5351c4ef05fa094834f5e9e2cb8.jpg)

## 8.3.7.1 AckNack 

##  Purpose 

This Submessage is used to communicate the state of a Reader to a Writer . The Submessage allows the Reader to inform the Writer about the sequence numbers it has received and which ones it is still missing. This Submessage can be used to do both positive and negative acknowledgments.

##  Content 

The elements that form the structure of the AckNack message are described in the table below.

Table 8.34 - Structure of the AckNack Submessage 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/00d6fcaf0f1d5e17fe4076a9bdd0849a11345fe68a5babb8f7313280b1ac51d7.jpg)

##  Validity 

This Submessage is invalid when any of the following is true:
- sub message Length in the Submessage header is too small.
- readerS N State is invalid (as defined in Section 8.3.5.5).
##  Change in state of Receiver 

None 

##  Logical Interpretation 

The Reader sends the AckNack message to the Writer to communicate its state with respect to the sequence numbers used by the Writer .

The Writer is uniquely identified by its GUID. The Writer GUID is obtained using the state of the Receiver: 

writerGUID  $=$  { Receiver.de st Gui d Prefix, AckNack.writerId } 

The Reader is uniquely identified by its GUID. The Reader GUID is obtained using the state of the Receiver: 

readerGUID  $=$  { Receiver.source Gui d Prefix, AckNack.readerId } 

The message serves two purposes simultaneously: 
- The Submessage acknowledges all sequence numbers up to and including the one just before the lowest sequence number in the Sequence Number Set (that is readerS N State.base -1).
- The Submessage negatively-acknowledges (requests) the sequence numbers that appear explicitly in the set.

The mechanism to explicitly represent sequence numbers depends on the PSM. Typically, a compact representation (such as a bitmap) is used.

The FinalFlag indicates whether a Heartbeat by the Writer is expected by the Reader or if the decision is left to the Writer . The use of this flag is described in Section 8.4.

## 8.3.7.2 Data 

This Submessage is sent from an RTPS Writer to an RTPS Reader .

##  Purpose 

The Submessage notifies the RTPS Reader of a change to a data-object belonging to the RTPS Writer . The possible changes include both changes in value as well as changes to the lifecycle of the data-object.

##  Contents 

The elements that form the structure of the Data message are described in the table below.

Table 8.35 - Structure of the Data Submessage 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/498b79f366ec12adb2ae241fe8d44445ffc17d778e839b2dd679a6c0661341e9.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/43d062d2a5c3cb19b71f3a77d5e05308cb0dea4173786312e8ac381583296b61.jpg)

##  Validity 

This Submessage is invalid when any of the following is true: 
- sub message Length in the Submessage header is too small.
- writerSN.value is not strictly positive (1, 2, ...) or is SEQUENCE NUMBER UNKNOWN .
- inlineQos is invalid.

##  Change in state of Receiver 

None 

##  Logical Interpretation 

The RTPS Writer sends the Data Submessage to the RTPS Reader to communicate changes to the dataobjects within the writer. Changes include both changes in value as well as changes to the lifecycle of the dataobject.

Changes to the value are communicated by the presence of the serialized Payload . When present, the serialized Payload is interpreted either as the value of the data-object or as the key that uniquely identifies the data-object from the set of registered objects.
- If the DataFlag is set and the KeyFlag is not set, the serialized Payload element is interpreted as the value of the dtat- object.
- If the KeyFlag is set and the DataFlag is not set, the serialized Payload element is interpreted as the value of the key that identifies the registered instance of the data-object.

If the Inline Qo s Flag is set, the inlineQos element contains QoS values that override those of the RTPS Writer and should be used to process the update. For a complete list of possible in-line QoS parameters, see Table 8.80.

If the NonStandard Payload Flag is set then the serialized Payload element is not formatted according to Section 10. This flag is informational. It indicates that the Serialized Payload has been transformed as described in another specification. For example, this flag should be set when the Serialized Payload is transformed as described in the DDS-Security specification.
The Writer is uniquely identified by its GUID. The Writer GUID is obtained using the state of the Receiver: writerGUID $=$  { Receiver.source Gui d Prefix, Data.writerId } 

The Reader is uniquely identified by its GUID. The Reader GUID is obtained using the state of the Receiver: readerGUID  $=$  { Receiver.de st Gui d Prefix, Data.readerId } 

The Data.readerId can be ENTITY ID UNKNOWN, in which case the Data applies to all Readers of that writerGUID within the Participant identified by the Gui d Prefix t Receiver.de st Gui d Prefix.

## 8.3.7.3 DataFrag 

This Submessage is sent from an RTPS Writer to an RTPS Reader .

##  Purpose 

The DataFrag Submessage extends the Data Submessage by enabling the serialized Data to be fragmented and sent as multiple DataFrag Sub messages. The fragments contained in the DataFrag Sub messages are then re-assembled by the RTPS Reader .

Defining a separate DataFrag Submessage in addition to the Data Submessage, offers the following advantages: 
- It keeps variations in contents and structure of each Submessage to a minimum. This enables more efficient implementations of the protocol as the parsing of network packets is simplified.
- It avoids having to add fragmentation information as in-line QoS parameters in the Data Submessage. This may not only slow down performance, it also makes on-the-wire debugging more difficult, as it is no longer obvious whether data is fragmented or not and which message contains what fragment(s).

##  Contents 

The elements that form the structure of the DataFrag Submessage are described in the table below.

Table 8.36 – Structure of the DataFrag Submessage 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/58bc4355072aa35b96de94bb23d460afd67528f64fffbfd513d03cc15d7991b6.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/3fe5975e550de7569a2d69ffe739b564bb2fda2237c27578b00b307c9918301a.jpg)

##  Validity 

This Submessage is invalid when any of the following is true: 
- sub message Length in the Submessage header is too small.
- writerSN.value is not strictly positive (1, 2, ...) or is SEQUENCE NUMBER UNKNOWN .
- fragment Starting Num.value is not strictly positive (1, 2, ...) or exceeds the total number of fragments (see below).
- fragment Size exceeds dataSize.
- The size of serialized Data exceeds fragments In Sub message \* fragment Size.
- inlineQos is invalid.

##  Change in state of Receiver 

None 

##  Logical Interpretation 

The DataFrag Submessage extends the Data Submessage by enabling the serialized Data to be fragmented and sent as multiple DataFrag Sub messages. Once the serialized Data is re-assembled by the RTPS Reader , the interpretation of the DataFrag Sub messages is identical to that of the Data Submessage.
How to re-assemble serialized Data using the information in the DataFrag Submessage is described below.

The total size of the data to be re-assembled is given by dataSize . Each DataFrag Submessage contains a contiguous segment of this data in its serialized Data element. The size of the segment is determined by the size of the serialized Data element. During re-assembly, the offset of each segment is determined by: 

The data is fully re-assembled when all fragments have been received. The total number of fragments to expect equals: 

Note that each DataFrag Submessage may contain multiple fragments. An RTPS Writer will select fragment Size based on the smallest message size supported across all underlying transports. If some RTPS Readers can be reached across a transport that supports larger messages, the RTPS Writer can pack multiple fragments into a single DataFrag Submessage or may even send a regular Data Submessage if fragmentation is no longer required. For more details, see 8.4.14.1.

When sending inlineQos with DataFrag Sub messages, it is only required to send the inlineQos with the first DataFrag Submessage for a given Writer sequence number. Sending the same inlineQos with every DataFrag Submessage for a given Writer sequence number is redundant.

## 8.3.7.4 Gap 

##  Purpose 

This Submessage is sent from an RTPS Writer to an RTPS Reader and indicates to the RTPS Reader that a range of sequence numbers is no longer relevant. The set may be a contiguous range of sequence numbers or a specific set of sequence numbers.

##  Content 

The elements that form the structure of the Gap message are described in the table below.

Table 8.37 - Structure of the Gap Submessage 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/8d4fb1014d2264ec80d2843f83410a7eac3b6d929e6f1e8fc1bc578b6c2d2158.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/e20f50c46de19ea2552fc85f71e70f7ac14e6ab848b1d75067cfc0bb719d0c6d.jpg)

##  Validity 

This Submessage is invalid when any of the following is true: 
- sub message Length in the Submessage header is too small.
- gapStart is zero or negative.
- gapList is invalid (as defined in 8.3.5.5).

If Group Info Flag is set and: 
- gap Start GSN . value is zero or negative
- gapEndGSN . value is zero or negative
- gapEndGSN . value $<$ gap Start GSN.value-1 

##  Change in state of Receiver 

None 

##  Logical Interpretation 

The RTPS Writer sends the Gap message to the RTPS Reader to communicate that certain sequence numbers are no longer relevant. This is typically caused by Writer-side filtering of the sample (content-filtered topics, time-based filtering). In this scenario, new data-values may replace the old values of the data-objects that were represented by the sequence numbers that appear as irrelevant in the Gap .

The irrelevant sequence numbers communicated by the Gap message are composed of two groups: 

1. All sequence numbers in the range gapStart $<=$  sequence number  $<=$ gapList.base -1 2. All the sequence numbers that appear explicitly listed in the gapList.

This set will be referred to as the Gap ::irrelevant sequence number list.

The Writer is uniquely identified by its GUID. The Writer GUID is obtained using the state of the Receiver: writerGUID  $=$  { Receiver.source Gui d Prefix, Gap.writerId } 

The Reader is uniquely identified by its GUID. The Reader GUID is obtained using the state of the Receiver: readerGUID  $=$  { Receiver.de st Gui d Prefix, Gap.readerId } 

The Writer sets the Group Info Flag to indicate the presence of the gap Start GSN and gapEndGSN elements.These fields provide information related to the Cache Changes of Writers belonging to a Writer  Group . See section 8.7.6 for how DDS uses this feature.

The gapEndGSN can extend past the Group Sequence Number that corresponds to gapList.bitmapBase in situations where those additional Group Sequence Numbers have been written by other Writers .

## 8.3.7.5 Heartbeat 

##  Purpose 

This message is sent from an RTPS Writer to an RTPS Reader to communicate the sequence numbers of 
changes that the Writer has available.

##  Content 

The elements that form the structure of the Heartbeat message are described in the table below.

Table 8.38 - Structure of the Heartbeat Submessage 
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/7715d926122fbbefd483cabf3e491530f594efb0adc94fb32c9e993d9b56c934.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/0774a4544d27b8fee2f81b6ac21daaea1884bb237bc7c28992ccf06347a53124.jpg)

The following examples illustrate how the firstSN.value and lastSN.value are assigned in various scenarios.

Example 1.A Writer that has never written any samples before sending a Heartbeat will send a Heartbeat with firstSN.value $=1$ , lastSN.value $=0$ .

Example 2.A Writer that has only one sample in its cache with sequence number SN will send a Heartbeat  with firstSN.value $=$ lastSN.value $=S\mathrm{N}$ .

Example 3.A Writer that has written 10 samples and still has the last 5 samples in its cache will send a Heartbeat with firstSN.value $=6$ , lastSN.value $=10$ .

Example 4.A Writer that has written 10 samples before sending a Heartbeat but does not have any samples available at the time of the Heartbeat will send a Heartbeat with firstSN.value $=11$ , lastSN.value $=10$ .

##  Validity 

This Submessage is invalid when any of the following is true: 
- sub message Length in the Submessage header is too small
- firstSN.value is zero or negative
- lastSN.value is negative
- lastSN.value $<$  firstSN.value - 1 

If Group Info Flag is set and: 
- currentGSN . value is zero or negative
- firstGSN . value is zero or negative
- lastGSN . value is negative
- lastGSN.value $<$  firstGSN.value - 1
- currentGSN.value $<$  firstGSN.value
- currentGSN.value $<$  lastGSN.value 

##  Change in state of Receiver 

None 

##  Logical Interpretation 

The Heartbeat message serves two purposes: 

1. It informs the Reader of the sequence numbers that are available in the writer’s History Cache so that the Reader may request (using an AckNack)
 any that it has missed.2. It requests the Reader to send an acknowledgement for the Cache Change changes that have been entered into the reader’s History Cache such that the Writer knows the state of the reader.

All Heartbeat messages serve the first purpose. That is, the Reader will always find out the state of the writer’s History Cache and may request what it has missed. Normally, the RTPS Reader would only send an AckNack message if it is missing a Cache Change .

The Writer uses the FinalFlag to request the Reader to send an acknowledgment for the sequence numbers it has received. If the Heartbeat has the FinalFlag set, then the Reader is not required to send an AckNack message back. However, if the FinalFlag is not set, then the Reader must send an AckNack message 
indicating which Cache Change changes it has received, even if the AckNack indicates it has received all Cache Change changes in the writer’s History Cache .

The Writer sets the Liveliness Flag to indicate that the DDS DataWriter associated with the RTPS Writer of the message has manually asserted its liveliness using the appropriate DDS operation (see the DDS Specification).The RTPS Reader should therefore renew the manual liveliness lease of the corresponding remote DDS DataWriter.

The Writer sets the Group Info Flag to indicate the presence of the currentGSN , firstGSN , lastGSN , writerSet , and secure Writer Set elements. These fields provide relate the Cache Changes of Writers belonging to a Writer  Group . See 8.7.6 for how DDS uses this feature.

The Writer is identified uniquely by its GUID. The Writer GUID is obtained using the state of the Receiver: writerGUID  $=$  { Receiver.source Gui d Prefix, Heartbeat.writerId } 

The Reader is identified uniquely by its GUID. The Reader GUID is obtained using the state of the Receiver: readerGUID  $=$  { Receiver.de st Gui d Prefix, Heartbeat.readerId } 

The Heartbeat.readerId can be ENTITY ID UNKNOWN, in which case the Heartbeat applies to all Readers of that writerGUID within the Participant.

## 8.3.7.6 Heartbeat Frag 

##  Purpose 

When fragmenting data and until all fragments are available, the Heartbeat Frag Submessage is sent from an RTPS Writer to an RTPS Reader to communicate which fragments the Writer has available. This enables reliable communication at the fragment level.

Once all fragments are available, a regular Heartbeat message is used.

##  Content 

The elements that form the structure of the Heartbeat Frag message are described in the table below.

Table 8.39 - Structure of the Heartbeat Frag Submessage 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/00e61b27af65e7059aacbb5403e1e248e308c74b58e0b2071cb9bfb20b1124e4.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/b93a1cdfe299f8f35cf33bcb42eb9bd909a14734f2838a94ee788b31bcb818a1.jpg)

##  Validity 

This Submessage is invalid when any of the following is true: 
- sub message Length in the Submessage header is too small
- writerSN.value is zero or negative
- last Fragment Num.value is zero or negative 

##  Change in state of Receiver 

None 

##  Logical Interpretation 

The Heartbeat Frag message serves the same purpose as a regular Heartbeat message, but instead of indicating the availability of a range of sequence numbers, it indicates the availability of a range of fragments for the data change with sequence number WriterSN .

The RTPS Reader will respond by sending a NackFrag message, but only if it is missing any of the available fragments. The Writer is identified uniquely by its GUID. The Writer GUID is obtained using the state of the 

Receiver: 

The Reader is identified uniquely by its GUID. The Reader GUID is obtained using the state of the Receiver: readerGUID  $=$  { Receiver.de st Gui d Prefix, Heartbeat Frag.readerId } 

The Heartbeat Frag.readerId can be ENTITY ID UNKNOWN, in which case the Heartbeat Frag applies to all Readers of that Writer GUID within the Participant .

## 8.3.7.7 Info Destination 

##  Purpose 

This message is sent from an RTPS Writer to an RTPS Reader to modify the GuidPrefix used to interpret the Reader entityIds appearing in the Sub messages that follow it.

##  Content 

The elements that form the structure of the Info Destination message are described in the table below.

Table 8.40 - Structure of the Info Destination Submessage 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/1e891a2bf70014379bf5bca5360d3207d676b6309c409dc4df2a60c75f8fbcbe.jpg)

##  Validity 

This Submessage is invalid when any of the following is true: - sub message Length in the Submessage header is too small.

##  Change in state of Receiver 

if (Info Destination.guidPrefix ! $=$  GUI D PREFIX UNKNOWN) { Receiver.de st Gui d Prefix  $=$  Info Destination.guidPrefix } else { Receiver.de st Gui d Prefix  $=$  <Gui d Prefix t of the Participant receiving               the_message > } 

##  Logical Interpretation 

None 

## 8.3.7.8 InfoReply 

##  Purpose 

This message is sent from an RTPS Reader to an RTPS Writer . It contains explicit information on where to send a reply to the Sub messages that follow it within the same message.

##  Content 

The elements that form the structure of the InfoReply message are described in the table below.

Table 8.41 - Structure of the InfoReply Submessage 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/f09a437a0fc92ad7e87267f9efd01c02ffefc3da7635e5a393e9bd7cb60b5b1a.jpg)

##  Validity 

This Submessage is invalid when any of the following is true:
- sub message Length in the Submessage header is too small.

##  Change in state of Receiver 

Receiver.uni cast Reply Locator List  $=$  InfoReply.uni cast Locator List if ( Multi cast Flag)
 
{ Receiver.multi cast Reply Locator List $=$  InfoReply.multi cast Locator List } else { Receiver.multi cast Reply Locator List $=$  <empty> } 

##  Logical Interpretation 

None 

## 8.3.7.9 InfoSource 

##  Purpose 

This message modifies the logical source of the Sub messages that follow.

##  Content 

The elements that form the structure of the InfoSource message are described in the table below.

Table 8.42 - Structure of the InfoSource Submessage 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/eda1bf2f84a821a48c3b1de82b0a964a590ad5904b8f1d90efb5218947c4215e.jpg)

##  Validity 

This Submessage is invalid when any of the following is true:
- sub message Length in the Submessage header is too small.

##  Change in state of Receiver 

Receiver.source Gui d Prefix  $=$  InfoSource.guidPrefix  Receiver.source Version $=$  InfoSource.protocol Version  Receiver.source Vendor Id $=$  InfoSource.vendorId Receiver.uni cast Reply Locator List $=$  { LOCATOR INVALID } Receiver.multi cast Reply Locator List  $=$  { LOCATOR INVALID }  have Timestamp  $=$  false 

##  Logical Interpretation 

None 

## 8.3.7.10 Info Timestamp 

## Purpose 

This Submessage is used to send a timestamp which applies to the Sub messages that follow within the same message.
## Content 

The elements that form the structure of the Info Timestamp message are described in the table below.

Table 8.43 – Structure of the Info Timestamp Submessage 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/6a9989c65b526354e537df91a9a3ea882989310099438377c30a3e855073234d.jpg)

## Validity 

This Submessage is invalid when the following is true:
- sub message Length in the Submessage header is too small.

## Change in state of Receiver 

if ( !Info Timestamp.Invalidate Flag)
 {   Receiver.have Timestamp  $=$  true   Receiver.timestamp  $=$  Info Timestamp.timestamp } else {   Receiver.have Timestamp  $=$  false } 

## Logical Interpretation 

None 

## 8.3.7.11 NackFrag 

## Purpose 

The NackFrag Submessage is used to communicate the state of a Reader to a Writer . When a data change is sent as a series of fragments, the NackFrag Submessage allows the Reader to inform the Writer about specific fragment numbers it is still missing.

This Submessage can only contain negative acknowledgements. Note this differs from an AckNack Submessage, which includes both positive and negative acknowledgements. The advantages of this approach include: 
- It removes the windowing limitation introduced by the AckNack Submessage. Given the size of a Sequence Number Set is limited to 256, an AckNack Submessage is limited to NACKing only those samples whose sequence number does not not exceed that of the first missing sample by more than 256. Any samples below the first missing samples are acknowledged.NackFrag Sub messages on the other hand can be used to NACK any fragment numbers, even fragments more than 256 apart from those NACKed in an earlier AckNack Submessage. This becomes important when handling samples containing a large number of fragments.
- Fragments can be negatively acknowledged in any order.
## Content 

The elements that form the structure of the NackFrag message are described in the table below.

Table 8.44 - Structure of the NackFrag SubMessage 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/c02590ba872a91e10fc06c4a0262c403958a3f6c9c117a781b1aaa80d5474b6b.jpg)

## Validity 

This Submessage is invalid when any of the following is true: 
- sub message Length in the Submessage header is too small.
- writerSN.value is zero or negative.

fragment Number State is invalid (as defined in 8.3.5.7).

## Change in state of Receiver 

None 

## Logical Interpretation 

The Reader sends the NackFrag message to the Writer to request fragments from the Writer .The Writer is uniquely identified by its GUID. The Writer GUID is obtained using the state of the Receiver: writerGUID  $=$  { Receiver.de st Gui d Prefix, NackFrag.writerId } 

The Reader is identified uniquely by its GUID. The Reader GUID is obtained using the state of the Receiver: readerGUID  $=$  { Receiver.source Gui d Prefix, NackFrag.readerId } 

The sequence number from which fragments are requested is given by writerSN . The mechanism to explicitly represent fragment numbers depends on the PSM. Typically, a compact representation (such as a bitmap) is used.
## 8.3.7.12 Pad 

Purpose 

The purpose of this Submessage is to allow the introduction of any padding necessary to meet any desired memory- alignment requirements. It has no other meaning.

## Content 

This Submessage has no contents. It accomplishes its purposes with only the Submessage header part. The amount of padding is determined by the value of sub message Length .

## Validity 

This Submessage is always valid.

## Change in state of Receiver 

None 

Logical Interpretation 

None 
