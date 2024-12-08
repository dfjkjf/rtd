---
layout: default
title: Serialized Payload Representation
nav_order: 8
---

# 10 Serialized Payload Representation

## 10.1 Introduction 

The RTPS protocol transfers serialized application data in the Serialized Payload submessage element, see 9.4.2.12. The representation of the serialized application data is not part of the RTPS protocol. The RTPS protocol does not interpret the content of the Serialized Payload . It delivers them as an opaque set of bytes. It is the responsibility of the connectivity layer above the RTPS protocol to serialize and de serialize the application data objects into and from the Serialized Payload .

However, to detect configuration errors, the RTPS protocol provides a mechanism to ensure that the RTPS Writer and Reader have a common understanding of the format used to represent the data in the Serialized Payload . This is defined in Section 10.2.

In the case of DDS using RTPS the responsibility to serialize and de serialize the application data objects into and from the Serialized Payload rests with the DDS DataWriter and DataReader , respectively. In this situation, the content and format of the Serialized Payload is defined in sections 10.3 to 10.5.

## 10.2 Serialized Payload Header and Representation Identifier 

All Serialized Payload shall start with the Serialized Payload Header defined below. The header provides information about the representation of the data that follows.

typedef octet Representation Identifier[2]; typedef octet Representation Options[2]; struct Serialized Payload Header { 

   Representation Identifier representation identifier; 

   Representation Options representation options; 

 }; 

The Serialized Payload Header occupies the first four octets of the Serialized Payload as shown below: 

0...2...........8...............16..............24..............32 

  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 

  |  representation identifier  |  representation options   | 

  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 

  \~                                \~ 

  \~ ... Bytes of data representation using a format that ...  \~ 

  \~ ... depends on the Representation Identifier and options ...\~ 

  \~                                \~ 

  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 

The Representation Identifier is used to identify the data representation used. The Representation Options shall be interpreted in the context of the Representation Identifier , such that each Representation Identifier may define the representation options that it requires.

For alignment purposes, the CDR stream is logically reset at the position that follows the representation options . Therefore, there should be no initial padding before the serialized data is added to the CDR stream 5 .
## 10.3 Serialized Payload for RTPS discovery built-in endpoints 

The Serialized Payload for the data messages associated with built-in discovery endpoints shall use the Representation Identifier values and formats defined in Table 10.1 below.

The current version of the protocol does not use the representation options : The sender shall set the representation options to zero. The receiver shall ignore the value of the representation options .

Table 10.1 - Representation Identifier values for built-in endpoints  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/026e3d2108c0532ca0f010ebf818ff71898dde32830439e37d434b90ec5a47bf.jpg)

## 10.4 Serialized Payload for other RTPS built-in endpoints 

The Serialized Payload for the data messages associated with built-in endpoints other than discovery built-in endpoints shall use one of the Representation Identifier values and formats defined in Table 10.2 below.

Table 10.2 - Representation Identifier values for built-in endpoints other than discovery 
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/6ada6e768b71206f24aeaeb617e656b091a853d1404253e14765a85baf29cc24.jpg)

The definition of each of those builtin Endpoints should indicate the serialized data format and Representation Identifier used.

## 10.5 Serialized Payload for user-defined DDS Topics 

The Serialized Payload for the data messages associated with the user-defined DDS Topics shall use the data representations defined in DDS-XTYPES clause 7.4 (Data Representation). Accordingly, the Representation Identifier values and the corresponding formats shall be as defined in Table 10.3.
Table 10.3 - Represent ion Identifier values for user-defined topic data 
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/b50f4b662fda204dd88433bd8f50b2c08c293f70d97170a52ec297f7ae3fd206.jpg)

Legacy DDS implementations that are not compliant with DDS-XTYPES should minimally support the Representation Identifier values CDR_BE and CDR_LE and the type system elements specified in clause F1 (Type System) in Annex F (Characterizing Legacy DDS Implementations) of the DDS-XTYPES specification.

## 10.6  Example for Built-in Endpoint Data 

Following is the Serialized Payload element used by the SED P built in Subscriptions Writer to declare a DataReader.

The DataReader is for Topic “Square” and type “ShapeType”. The DataReader has the Endpoint GUID c0:a8:02:05:00:00:3a:20:00:00:00:02:80:00:00:07, DESTINATION_ORDER kind 
BY SOURCE TIMESTAMP, and DEADLINE period of 3 seconds. The remaining members have their default values, so they are not serialized into the Serialized Payload .

The representation identifier is PL_LE, indicating little Endian representation.

The corresponding Serialized Payload element has the following layout: 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/102654f66206cdbfa2db31a06cce5eaa0c851e91b0b63863e37882a8e0627b6c.jpg)
The actual bytes of the Serialized Payload element are shown below: 
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/439b2264bf3131aab346836da33f1e9853a265b7c780534c8647f546c774ce56.jpg)

## 10.7  Example for User-defined Topic Data 

Following is the Serialized Payload element used by an application DataWriter to send Data on the Topic “Square” with type “ShapeType” defined by the IDL below. The DataWriter uses PLAIN_CDR representation with encoding version 1 and Little Endian byte order.

@final struct ShapeType { 

   @key string<64> color; 

   long x; 

   long y; 

   long size; 

 }; 
The representation identifier is CDR_LE.

The example uses a data value with color set to “BLUE”, $\mathbf{X}=34$ , $\mathbf{y}=100$ , size $=24$ 

The corresponding Serialized Payload element has the following layout: 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/adf775bfd8b3debaa91d4b0dd41187feea69477e3d7a67f6562e8bf090eade91.jpg)

The actual bytes of the Serialized Payload element are shown below: 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/fd0a083776413b11311bfa72c43167a7939b09dc547c99a38d577f9c29d181bf.jpg)
## A References 

[1] DDS-SECURITY: DDS Security version 1.1 https://www.omg.org/spec/DDS-SECURITY  This page intentionally left blank.

