---
layout: default
title: Structure Module
nav_order: 2
---

# 8.2 Structure Module

This sub clause describes the structure of the RTPS entities that are the communication actors. The main classes used by the RTPS protocol are shown in Figure 8.1.

## 8.2.1 Overview

RTPS entities are the protocol-level endpoints used by the application-visible DDS entities in order to communicate with each other.

Each RTPS Entity is in a one-to-one correspondence with a DDS Entity. The History Cache forms the interface between the DDS Entities and their corresponding RTPS Entities. For example, each write operation on a DDS DataWriter adds a Cache Change to the History Cache of its corresponding RTPS Writer . The RTPS Writer subsequently transfers the Cache Change to the History Cache of all matching RTPS Readers . On the receiving side, the DDS DataReader is notified by the RTPS Reader that a new Cache Change has arrived in the History Cache , at which point the DDS DataReader may choose to access it using the DDS read or take API.
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/ea9773bfc9aabcabf0154b982a5ab70cc2790d8aaa8ed4241e00e60958723620.jpg)
Figure 8.1 - RTPS Structure Module

This sub clause provides an overview of the main classes used by the RTPS virtual machine and the types used to describe their attributes. Subsequent sub clauses describe each class in detail.

## 8.2.1.1 Summary of the classes used by the RTPS virtual machine

All RTPS entities derive from the RTPS Entity class. Table 8.1 lists the classes used by the RTPS virtual machine.

Table 8.1 - Overview of RTPS Entities and Classes
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/388dc17f853e441891b560af2be87cf8ff5d3c5fa0eda5c02d469641fd56cc37.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/9c08581bbae95c15585cade85d5fd323e9380cdf32619f0abb65aff1e1764b9c.jpg)

## 8.2.1.2 Summary of the types used to describe RTPS Entities and Classes

The Entities and Classes used by the virtual machine each contain a set of attributes. The types of the attributes are summarized in Table 8.2.

Table 8.2 - Types of the attributes that appear in the RTPS Entities and Classes
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/e3c4e53350675a4a5dd8338b7d90bdb983a85f289b262f7f68e346d31cafee2f.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/7f1478daaf42c9a8cd30cf7e2a90de46e1c5d02fe8e765d74db1291dbe229bb6.jpg)

## 8.2.1.3 Configuration attributes of the RTPS Entities

RTPS entities are configured by a set of attributes. Some of these attributes map to the QoS policies set on the corresponding DDS entities. Other attributes represent parameters that allow tuning the behavior of the protocol to specific transport and deployment situations. Additional attributes encode the state of the RTPS Entity and are not used to configure the behavior.
The attributes used to configure a subset of the RTPS Entities are shown in Figure 8.2. The attributes to configure Writer and Reader Entities are closely tied to the protocol behavior and will be introduced in 8.4.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/415b92350278dcb5748aab719db3c90eefa044f606a17252c3807348c774b20f.jpg)
Figure 8.2 - Attributes used to configure the main RTPS Entities

The remainder of this sub clause describes each of the RTPS entities in more detail.

## 8.2.2 The RTPS History Cache

The History Cache is part of the interface between DDS and RTPS and plays different roles on the reader and the writer side.

On the writer side, the History Cache contains the partial history of changes to data-objects made by the corresponding DDS Writer that are needed to service existing and future matched RTPS Reader endpoints. The partial history needed depends on the DDS Qos and the state of the communications with the matched RTPS Reader endpoints.

On the reader side, it contains the partial superposition of changes to data-objects made by all the matched RTPS Writer endpoints.

The word “partial” is used to indicate that it is not necessary that the full history of all changes ever made is maintained. Rather what is needed is the subset of the history needed to meet the behavioral needs of the RTPS protocol and the QoS needs of the related DDS entities. The rules that define this subset are defined by the RTPS protocol and depend both on the state of the communications protocol and on the QoS of the related DDS entities.

The History Cache is part of the interface between DDS and RTPS. In other words, both the RTPS entities and their related DDS entities are able to invoke the operations on their associated History Cache .
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/884bbbe25ab0eb763efaf12915c53ff986a6d932d9457900d2a0041a4b3ec624.jpg)

Figure 8.3 - RTPS History Cache

The History Cache attributes are listed in Table 8.3.

Table 8.3 – RTPS History Cache Attributes

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/a7251efbce5460b947b95a31c2d947b1e4f449dbd20aebe82d531677469c726d.jpg)

The RTPS entities and the related DDS entities interact with the History Cache using the operations in Table 8.4.

Table 8.4 - RTPS History Cache operations
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/b94355af0ef9cdee30295924dac2610120ddf60dfd00bd9ca05d635c506c0db8.jpg)
The following sub clauses provide details on the operations.

## 8.2.2.1 new

This operation creates a new RTPS History Cache . The newly-created history cache is initialized with an empty list of changes.

## 8.2.2.2 add_change

This operation inserts the Cache Change a_change into the History Cache .

This operation will only fail if there are not enough resources to add the change to the History Cache . It is the responsibility of the DDS service implementation to configure the History Cache in a manner consistent with the DDS Entity RESOURCE LIMITS QoS and to propagate any errors to the DDS-user in the manner specified by the DDS specification.

This operation performs the following logical steps:

## 8.2.2.3 remove change

This operation indicates that a previously-added Cache Change has become irrelevant and the details regarding the Cache Change need not be maintained in the History Cache . The determination of irrelevance is made based on the QoS associated with the related DDS entity and on the acknowledgment status of the Cache Change . This is described in 8.4.1.

This operation performs the following logical steps:

## 8.2.2.4 get seq num min

This operation retrieves the smallest value of the Cache Change::sequence Number attribute among the Cache Change stored in the History Cache . This operation performs the following logical steps:

min seq num : $=$  MIN { change.sequence Number WHERE (change IN this.changes) }  return min seq num;

## 8.2.2.5 get seq num max

This operation retrieves the largest value of the Cache Change::sequence Number attribute among the Cache Change stored in the History Cache .

This operation performs the following logical steps:

## 8.2.3 The RTPS Cache Change

Class used to represent each change added to the History Cache . The Cache Change attributes are listed in Table 8.5.
Table 8.5 - RTPS Cache Change attributes
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/c2fcc88e5be314cf4c6a1a16dc58ac817370966558c0c9e56c074f2cf7db9b9a.jpg)

## 8.2.4 The RTPS Entity

RTPS Entity is the base class for all RTPS entities and maps to a DDS Entity. The Entity configuration attributes are listed in Table 8.6

Table 8.6 - RTPS Entity Attribues
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/050dcfc7a9c7f5fbad728ab8e5e503a91c220cb7b591a3d234d4b17732250725.jpg)

## 8.2.4.1 Identifying RTPS entities: The GUID

The GUID (Globally Unique Identifier) is an attribute of all RTPS Entities and uniquely identifies the Entity within a DDS Domain.

The GUID is built as a tuple <prefix, entityId> combining a Gui d Prefix t prefix and an EntityId_t entityId .
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/9f269255dfb092267edeef2e71fc3623774edd3a20c0c46b456944c2610d3d05.jpg)
Figure 8.4 - RTPS GUID_t uniquely identifies Entities and is composed of a prefix and a suffix

Table 8.7 - Structure of the GUID_t

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/8a4d7f278dd4d169180b7bedf0525a93d709c5d95161da05a8a6e34f66410fc3.jpg)

## 8.2.4.2 The GUIDs of RTPS Participants

Every Participant has GUID <prefix, ENTITY ID PARTICIPANT>, where the constant ENTITY ID PARTICIPANT is a special value defined by the RTPS protocol. Its actual value depends on the PSM.

The implementation is free to choose the prefix , as long as every Participant in the Domain has a unique GUID.

## 8.2.4.3 The GUIDs of the RTPS Endpoints within a Participant

The Endpoints contained by a Participant with GUID <participant Prefix, ENTITY ID PARTICIPANT $\textgreater$  have the GUID <participant Prefix, entityId >. The entityId is the unique identification of the Endpoint relative to the Participant . This has several consequences:
- The GUIDs of all the Endpoints within a Participant have the same prefix .

 .Once the GUID of an Endpoint is known, the GUID of the Participant that contains the endpoint is also known.

 .The GUID of any endpoint can be deduced from the GUID of the Participant to which it belongs and its entityId . The selection of entityId for each RTPS Entity depends on the PSM.

## 8.2.4.4 The GUIDs of Endpoint Groups within a Participant

The DDS Specification defines Publisher and Subscriber entities. These two entities have GUIDs that are defined exactly as described for Endpoints in clause 8.2.4.3 above.

## 8.2.5 The RTPS Participant

RTPS Participant is the container of RTPS Endpoint entities and maps to a DDS Domain Participant. In addition, the RTPS Participant facilitates the fact that the RTPS Endpoint entities within a single RTPS Participant are likely to share common properties.
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/e3d836cac4e48659f25d9adf987ed9cb00c326ffaec801d100ce39ffbc7d7e3a.jpg)
Figure 8.5 - RTPS Participant

RTPS Participant contains the attributes shown in Table 8.8.

Table 8.8 - RTPS Participant attributes

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/aac05c356bac41e007578bec60be0341196e82388e3cace1c8ef3eaf9218050b.jpg)

## 8.2.6 The RTPS Endpoint

RTPS Endpoint represents the possible communication endpoints from the point of view of the RTPS protocol.There are two kinds of RTPS Endpoint entities: Writer endpoints and Reader endpoints.

RTPS Writer endpoints send Cache Change messages to RTPS Reader endpoints and potentially receive acknowledgments for the changes they send. RTPS Reader endpoints receive Cache Change and changeavailability announcements from Writer endpoints and potentially acknowledge the changes and/or request missed changes.
Table 8.9 - RTPS Endpoint configuration attribues
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/d6b5a1628015fb65c76144290a7625a6d85ef4864add447baff5a7839f579c0b.jpg)

## 8.2.7 The RTPS Writer

RTPS Writer specializes RTPS Endpoint and represents the actor that sends Cache Change messages to the matched RTPS Reader endpoints. Its role is to transfer all Cache Change changes in its History Cache to the History Cache of the matching remote RTPS Readers .

An RTPS Writer belongs to an RTPS Group .

The attributes to configure an RTPS Writer are closely tied to the protocol behavior and will be introduced in the Behavior Module (8.4).

## 8.2.8 The RTPS Reader

RTPS Reader specializes RTPS Endpoint and represents the actor that receives Cache Change messages from the matched RTPS Writer endpoints.

An RTPS Reader belongs to an RTPS Group .

The attributes to configure an RTPS Reader are closely tied to the protocol behavior and will be introduced in the Behavior Module (8.4).

## 8.2.9 Relation to DDS Entities

As mentioned in 8.2.2, the History Cache forms the interface between DDS Entities and their corresponding 
RTPS Entities. A DDS DataWriter, for example, passes data to its matching RTPS Writer through the common History Cache .

How exactly a DDS Entity interacts with the History Cache however , is implementation specific and not formally modeled by the RTPS protocol. Instead, the Behavior Module of the RTPS protocol only specifies how Cache Change changes are transferred from the History Cache of the RTPS Writer to the History Cache of each matching RTPS Reader .

Despite the fact that it is not part of the RTPS protocol, it is important to know how a DDS Entity may interact with the History Cache to obtain a complete understanding of the protocol. This topic forms the subject of this sub clause.

The interactions are described using UML state diagrams. The abbreviations used to refer to DDS and RTPS Entities are listed in Table 8.10 below.

Table 8.10 - Abbreviations used in the sequence charts and state diagrams
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/56357345a73fba5a5dbcd203e32a9c56d546c8bfd668770a3033345529dfd062.jpg)

## 8.2.9.1 The DDS DataWriter

The write operation on a DDS DataWriter adds Cache Change changes to the History Cache of its associated RTPS Writer. As such, the History Cache contains a history of the most recently written changes. The number of changes is determined by QoS settings on the DDS DataWriter such as the HISTORY and RESOURCE LIMITS QoS.

By default, all changes in the History Cache are considered relevant for each matching remote RTPS Reader .That is, the Writer should attempt to send all changes in the History Cache to the matching remote Readers .How to do this is the subject of the Behavior Module of the RTPS protocol.

Changes may not be sent to a remote Reader for two reasons:

1. they have been removed from the History Cache by the DDS DataWriter and are no longer available.2. they are considered irrelevant for this Reader .

The DDS DataWriter may decide to remove changes from the History Cache for several reasons. For example, only a limited number of changes may need to be stored based on the HISTORY QoS settings. Alternatively, a sample may have expired due to the LIFESPAN QoS. When using strict reliable communication, a change can only be removed when it has been acknowledged by all readers the change was sent to and which are still active and alive.

Not all changes may be relevant for each matching remote Reader as determined by, for example, the TIME BASED FILTER QoS or through the use of DDS content-filtered topics. Note that whether a change is relevant must be determined on a per Reader basis in this case. Implementations may be able to optimize bandwidth and/or CPU usage by filtering on the Writer side when possible. Whether this is possible depends on whether an implementation keeps track of each individual remote Reader and the QoS and filters that apply to this Reader . The Reader itself will always filter.
QoS or content-based filtering is represented in this document using DDS_FILTER(reader, change) , a notation which reflects that filtering is reader dependent. Depending on what reader specific information is stored by the writer, DDS_FILTER may be a noop. For content-based filtering, the RTPS specification enables sending information with each change that lists what filters have been applied to the change and which filters it passed. If available, this information can then be used by the Reader to filter a change without having to call DDS_FILTER. This approach saves CPU cycles by filtering the sample once on the Writer side, as opposed to filtering on each Reader .

The following state-diagram illustrates how the DDS Data Writer adds a change to the History Cache .

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/a5c245196de67cc5e32dfeff66a21284920fc0bb88c92dd83bf53d9169cca98c.jpg)
Figure 8.6 - DDS DataWriter additions to the History Cache

Table 8.11 - Transitions for DDS DataWriter additions to the History Cache
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/1edd5b3922377dc1dac87a95687964f43910873ddf643a669a696d3e49610c7f.jpg)

##  Transition T1

This transition is triggered by the creation of a DDS DataWriter ‘the dd s writer.’ The transition performs the following logical actions in the virtual machine:

##  Transition T2

This transition is triggered by the act of writing data using a DDS DataWriter ‘the dd s writer.’ The DataWriter::write() operation takes as arguments the ‘data’ and the Instance Handle t ‘handle’ used to differentiate among different data- objects.

The transition performs the following logical actions in the virtual machine: 
the rtp s writer.writer cache.add_change(a_change);

After the transition the following post-conditions hold:

the rtp s writer.writer cache.get seq num max()  $==$  a_change.sequence Number

##  Transition T3

This transition is triggered by the act of disposing a data-object previously written with the DDS DataWriter ‘the dd s writer.’ The DataWriter::dispose() operation takes as parameter the Instance Handle t ‘handle’ used to differentiate among different data-objects.

This operation has no effect if the topicKind $==$ NO_KEY.

The transition performs the following logical actions in the virtual machine:

the rtp s writer : $=$  the dd s writer.related rtp s writer; if (the rtp s writer.topicKind $==$  WITH_KEY) {   a_change : $=$  the rtp s writer.new_change(NOT ALIVE DISPOSED, <nil>,                       inlineQos, handle);   the rtp s writer.writer cache.add_change(a_change); }

After the transition the following post-conditions hold:

if (the rtp s writer.topicKind $==$  WITH_KEY) then   the rtp s writer.writer cache.get seq num max()  $==a$ _change.sequence Number

##  Transition T4

This transition is triggered by the act of un registering a data-object previously written with the DDS DataWriter ‘the dd s writer.’ The DataWriter::unregister() operation takes as arguments the Instance Handle t ‘handle’ used to differentiate among different data-objects.

This operation has no effect if the topicKind $==$ NO_KEY.

The transition performs the following logical actions in the virtual machine:

the rtp s writer : $=$  the dd s writer.related rtp s writer; if (the rtp s writer.topicKind $==$  WITH_KEY) {   a_change : $=$  the rtp s writer.new_change(NOT ALIVE UNREGISTERED, <nil>,                       inlineQos, handle);   the rtp s writer.writer cache.add_change(a_change); }

After the transition the following post-conditions hold:

if (the rtp s writer.topicKind $==$  WITH_KEY) then   the rtp s writer.writer cache.get seq num max()  $==\ \acute{c}$ _change.sequence Number

##  Transition T5

This transition is triggered by the destruction of a DDS DataWriter ‘the dd s writer.’ The transition performs the following logical actions in the virtual machine:
## 8.2.9.2 The DDS DataReader

The DDS DataReader gets its data from the History Cache of the corresponding RTPS Reader . The number of changes stored in the History Cache is determined by QoS settings such as the HISTORY and RESOURCE LIMITS QoS.

Each matching Writer will attempt to transfer all relevant samples from its History Cache to the History Cache of the Reader . The implementation of the read or take call on the DDS DataReader accesses the History Cache .The changes returned to the user are those in the History Cache that pass all Reader specific filters, if any.

A Reader filter is equally represented by DDS_FILTER(reader, change) . As mentioned above, implementations may be able to perform most of the filtering on the Writer side. In that case, samples are either never sent (and therefore not present in the History Cache of the Reader)
 or contain information on what filters where applied and the corresponding outcome (for content-based filtering).

A DDS DataReader may also decide to remove changes from the History Cache in order to satisfy such QoS as TIME BASED FILTER. This exact behavior is again implementation specific and is not modeled by the RTPS protocol.

The following state-diagram illustrates how the DDS Data Reader accesses changes in the History Cache .

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/77d4cc0a064829a0fcf1acec361388c3816de89d8eb20ece1add93cb5d8fee9a.jpg)

Table 8.12 - Transitions for DDS DataReader access to the History Cache

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/87ad9b682d5baf8eaf48c4ebccd532fd8816754eb52a5e80f15f4a1372536235.jpg)

##  Transition T1

This transition is triggered by the creation of a DDS DataReader ‘the dd s reader.’ The transition performs the following logical actions in the virtual machine:

the rtp s reader  $=$  new RTPS::Reader;

##  Transition T2

This transition is triggered by the act of reading data from the DDS DataReader ‘the dd s reader’ by means of the ‘read’ operation. Changes returned to the application remain in the RTPS Reader’s History Cache such that subsequent read or take operations can find them again.
The transition performs the following logical actions in the virtual machine:

the rtp s reader : $=$  the dd s reader.related rtp s reader; a change list : $=$ new(); FOREACH change IN the rtp s reader.reader cache.changes { if DDS_FILTER(the rtp s reader, change)    ADD change TO a change list; }  RETURN a change list;

The DDS_FILTER() operation reflects the capabilities of the DDS DataReader API to select a subset of changes based on Cache Change::kind , QoS, content-filters and other mechanisms. Note that the logical actions above only reflect the behavior and not necessarily the actual implementation of the protocol.

##  Transition T3

This transition is triggered by the act of reading data from the DDS DataReader ‘the dd s reader’ by means of the ‘take’ operation. Changes returned to the application are removed from the RTPS Reader’s History Cache such that subsequent read or take operations do not find the same change.

The transition performs the following logical actions in the virtual machine:

the rtp s reader : $=$  the dd s reader.related rtp s reader; a change list : $=$ new(); FOREACH change IN the rtp s reader.reader cache.changes {   if DDS_FILTER(the rtp s reader, change) {     ADD change TO a change list;   }   the rtp s reader.reader cache.remove change(a_change); } RETURN a change list;

The DDS_FILTER() operation reflects the capabilities of the DDS DataReader API to select a subset of changes based on Cache Change::kind , QoS, content-filters and other mechanisms. Note that the logical actions above only reflect the behavior and not necessarily the actual implementation of the protocol.

After the transition the following post-conditions hold:

FOREACH change IN a change list   change BELONGS_TO the rtp s reader.reader cache.changes  $==$  FALSE

##  Transition T4

This transition is triggered by the destruction of a DDS DataReader ‘the dd s reader.’ The transition performs the following logical actions in the virtual machine:
