---
layout: default
title: Discovery Module
nav_order: 5
---

# 8.5 Discovery Module 

The RTPS Behavior Module assumes RTPS Endpoints are properly configured and paired up with matching remote Endpoints. It does not make any assumptions on how this configuration took place and only defines how to exchange data between these Endpoints.

In order to be able to configure Endpoints, implementations must obtain information on the presence of remote Endpoints and their properties. How to obtain this information is the subject of the Discovery Module.
The Discovery Module defines the RTPS discovery protocol. The purpose of the discovery protocol is to allow each RTPS Participant to discover other relevant Participants and their Endpoints . Once remote Endpoints have been discovered, implementations can configure local Endpoints accordingly to establish communication.

The DDS specification equally relies on the use of a discovery mechanism to establish communication between matched Data Writers and Data Readers. DDS implementations must automatically discover the presence of remote entities, both when they join and leave the network. This discovery information is made accessible to the user through DDS built-in topics.

The RTPS discovery protocol defined in this Module provides the required discovery mechanism for DDS.

## 8.5.1 Overview 

The RTPS specification splits up the discovery protocol into two independent protocols: 1. Participant Discovery Protocol 2. Endpoint Discovery Protocol 

A Participant Discovery Protocol (PDP) specifies how Participants discover each other in the network. Once two Participants have discovered each other, they exchange information on the Endpoints they contain using an Endpoint Discovery Protocol (EDP). Apart from this causality relationship, both protocols can be considered independent.

Implementations may choose to support multiple PDPs and EDPs, possibly vendor-specific. As long as two Participants have at least one PDP and EDP in common, they can exchange the required discovery information.For the purpose of interoperability, all RTPS implementations must provide at least the following discovery protocols: 

1. Simple Participant Discovery Protocol (SPDP) 2. Simple Endpoint Discovery Protocol (SEDP)

Both are basic discovery protocols that suffice for small to medium scale networks. Additional PDPs and EDPs that are geared towards larger networks may be added to future versions of the specification.

Finally, the role of a discovery protocol is to provide information on discovered remote Endpoints . How this information is used by a Participant to configure its local Endpoints depends on the actual implementation of the RTPS protocol and is not part of the discovery protocol specification. For example, for the reference implementations introduced in 8.4.7, the information obtained on the remote Endpoints allows the implementation to configure: 
- The RTPS Reader Locator objects that are associated with each RTPS Stateless Writer .
- The RTPS Reader Proxy objects associated with each RTPS State ful Writer . 
- The RTPS Writer Proxy objects associated with each RTPS State ful Reader .

The Discovery Module is organized as follows: 
- The SPDP and SEDP rely on pre-defined RTPS built-in Writer and Reader Endpoints to exchange discovery information. 8.5.2 introduces these RTPS built-in Endpoints.
- The SPDP is discussed in 8.5.3.
- The SEDP is discussed in 8.5.4.

## 8.5.2 RTPS Built-in Discovery Endpoints 

The DDS specification specifies that discovery takes place using “built-in” DDS Data Readers and Data Writers with pre- defined Topics and QoS.

There are four pre-defined built-in Topics: “DC PS Participant,” “DC PS Subscription,” “DC PS Publication,” and “DCPSTopic.” The DataTypes associated with these Topics are also specified by the DDS specification and mainly contain Entity QoS values.
For each of the built-in Topics, there exists a corresponding DDS built-in DataWriter and DDS built-in DataReader. The built-in Data Writers are used to announce the presence and QoS of the local DDS Participant and the DDS Entities it contains (Data Readers, Data Writers and Topics) to the rest of the network. Likewise, the built-in Data Readers collect this information from remote Participants, which is then used by the DDS implementation to identify matching remote Entities. The built-in Data Readers act as regular DDS Data Readers and can also be accessed by the user through the DDS API.

The approach taken by the RTPS Simple Discovery Protocols (SPDP and SEDP) is analogous to the built-in Entity concept. RTPS maps each built-in DDS DataWriter or DataReader to an associated built-in RTPS Endpoint . These built- in Endpoints act as regular Writer and Reader Endpoints and provide the means to exchange the required discovery information between Participants using the regular RTPS protocol defined in the Behavior Module.

The SPDP, which concerns itself with how Participants discover each other, maps the DDS built-in Entities for the “DC PS Participant” Topic. The SEDP, which specifies how to exchange discovery information on local Topics, Data Writers and Data Readers, maps the DDS built-in Entities for the “DC PS Subscription,” “DC PS Publication” and “DCPSTopic” Topics.

## 8.5.3 The Simple Participant Discovery Protocol 

The purpose of a PDP is to discover the presence of other Participants on the network and their properties.

A Participant may support multiple PDPs, but for the purpose of interoperability, all implementations must support at least the Simple Participant Discovery Protocol.

## 8.5.3.1 General Approach 

The RTPS Simple Participant Discovery Protocol (SPDP) uses a simple approach to announce and detect the presence of Participants in a domain.

For each Participant, the SPDP creates two RTPS built-in Endpoints: the S PDP built in Participant Writer and the S PDP built in Participant Reader.

The S PDP built in Participant Writer is an RTPS Best-Effort Stateless Writer . The History Cache of the S PDP built in Participant Writer contains a single data-object of type S PDP discovered Participant Data . The value of this data-object is set from the attributes in the Participant . If the attributes change, the data-object is replaced.

The S PDP built in Participant Writer periodically sends this data-object to a pre-configured list of locators to announce the Participant’s presence on the network. This is achieved by periodically calling Stateless Writer::unsent changes reset, which causes the Stateless Writer to resend all changes present in its History Cache to all locators. The periodic rate at which the S PDP built in Participant Writer sends out the S PDP discovered Participant Data defaults to a PSM specified value. This period should be smaller than the lease Duration specified in the S PDP discovered Participant Data (see also 8.5.3.3.2).

The pre-configured list of locators may include both unicast and multicast locators. Port numbers are defined by each PSM. These locators simply represent possible remote Participants in the network, no Participant need actually be present. By sending the S PDP discovered Participant Data periodically, Participants can join the network in any order.

The S PDP built in Participant Reader receives the S PDP discovered Participant Data announcements from the remote Participants. The contained information includes what Endpoint Discovery Protocols the remote Participant supports. The proper Endpoint Discovery Protocol is then used for exchanging Endpoint information with the remote Participant.

Implementations can minimize any start-up delays by sending an additional S PDP discovered Participant Data in response to receiving this data-object from a previously unknown Participant, but this behavior is optional.Implementations may also enable the user to choose whether to automatically extend the pre-configured list of locators with new locators from newly discovered Participants. This enables asymmetric locator lists. These last two features are optional and not required for the purpose of interoperability.
## 8.5.3.2 S PDP discovered Participant Data 

The S PDP discovered Participant Data defines the data exchanged as part of the SPDP.

Figure 8.27 illustrates the contents of the S PDP discovered Participant Data . As shown in the figure, the S PDP discovered Participant Data specializes the Participant Proxy and therefore includes all the information necessary to configure a discovered Participant . The S PDP discovered Participant Data also specializes the DDS-defined DDS::Participant Built in Topic Data providing the information the corresponding DDS built-in DataReader needs.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/c0570d46bf72e9a60ee670f77b131d47beadd8444bf14ad12cc926c6d6b0f60d.jpg)
Figure 8.27 - S PDP discovered Participant Data 

The attributes of the S PDP discovered Participant Data and their interpretation are described in Table 8.73.

Table 8.73 - RTPS S PDP discovered Participant Data attributes 
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/c05a04b37e0e12b8a52ed22b63c980879a07990395e6355e96b8ebf562f8a6dc.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/3884600959cca905e116ad0e070b2d379277b7e6e5f797b638d15cd9eb46ff9b.jpg)
As mentioned in 8.5.3.1, the S PDP discovered Participant Data lists the Endpoint Discovery Protocols supported by the Participant . The attributes shown in Table 8.73 only reflect the mandatory SEDP. There are currently no other Endpoint Discovery Protocols defined by the RTPS specification. In order to extend S PDP discovered Participant Data to include additional EDPs, the standard RTPS extension mechanisms can be used. Please refer to 9.6.2 for additional information.

## 8.5.3.3 The built-in Endpoints used by the Simple Participant Discovery Protocol 

Figure 8 . 28 illustrates the built-in Endpoints introduced by the Simple Participant Discovery Protocol.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/4c34bdb042f25afba8fe21aaa1521da1ba015beb7971ed421d6b36d40f96de32.jpg)
Figure 8 . 28 - The built-in Endpoints used by the Simple Participant Discovery Protocol 

The Protocol reserves the following values of the EntityId_t for the SPDP built-in Endpoints: ENTITY ID S PDP BUILT IN PARTICIPANT WRITER ENTITY ID S PDP BUILT IN PARTICIPANT READER 

##  S PDP built in Participant Writer 

The relevant attribute values for configuring the S PDP built in Participant Writer are shown in Table 8.74.

Table 8.74 - Attributes of the RTPS Stateless Writer used by the SPDP 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/b3bb785af8a36eaef3b37cba53e5c37b049b2efe557939c8feaa32e34b8f11ef.jpg)
##  S PDP built in Participant Reader 

The S PDP built in Participant Reader is configured with the attribute values shown in Table 8.75.

Table 8.75 - Attributes of the RTPS Stateless Reader used by the SPDP 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/f976bdf0bb17f1dba51b24dc6d9bf2149240388a0d6bef3438455704f86adb1b.jpg)

The History Cache of the S PDP built in Participant Reader contains information on all active discovered participants; the key used to identify each data-object corresponds to the Participant GUID.

Each time information on a participant is received by the S PDP built in Participant Reader, the SPDP examines the History Cache looking for an entry with a key that matches the Participant GUID. If an entry with a matching key is not there, a new entry is added keyed by the GUID of the Participant.

Periodically, the SPDP examines the S PDP built in Participant Reader History Cache looking for stale entries defined as those that have not been refreshed for a period longer than their specified lease Duration. Stale entries are removed.

## 8.5.3.4 Logical ports used by the Simple Participant Discovery Protocol 

As mentioned above, each S PDP built in Participant Writer uses a pre-configured list of locators to announce a Participant’s presence on the network.

In order to enable plug-and-play interoperability, the pre-configured list of locators must use the following wellknown logical ports: 

Table 8.76 - Logical ports used by the Simple Participant Discovery Protocol 
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/0b1f99802bbefd524318c113af43795abf770a68f4b782f4b6b52cc46a3bd0ea.jpg)
## 8.5.4 The Simple Endpoint Discovery Protocol 

An Endpoint Discovery Protocol defines the required information exchange between two Participants in order to discover each other’s Writer and Reader Endpoints.

A Participant may support multiple EDPs, but for the purpose of interoperability, all implementations must support at least the Simple Endpoint Discovery Protocol .

## 8.5.4.1 General Approach 

Similar to the SPDP, the Simple Endpoint Discovery Protocol uses pre-defined built-in Endpoints. The use of pre-defined built-in Endpoints means that once a Participant knows of the presence of another Participant , it can assume the presence of the built-in Endpoints made available by the remote participant and establish the association with the locally-matching built-in Endpoints.

The protocol used to communicate between built-in Endpoints is the same as used for application-defined Endpoints. Therefore, by reading the built-in Reader Endpoints, the protocol virtual machine can discover the presence and QoS of the DDS Entities that belong to any remote Participants . Similarly, by writing the built-in Writer Endpoints a Participant can inform the other Participants of the existence and QoS of local DDS Entities.

The use of built-in topics in the SEDP therefore reduces the scope of the overall discovery protocol to the determination of which Participants are present in the system and the attribute values for the Reader Proxy and Writer Proxy objects that correspond to the built-in Endpoints of these Participants . Once that is known, everything else results from the application of the RTPS protocol to the communication between the built-in RTPS Readers and Writers .

## 8.5.4.2 The built-in Endpoints used by the Simple Endpoint Discovery Protocol 

The SEDP maps the DDS built-in Entities for the “DC PS Subscription,” “DC PS Publication,” and “DCPSTopic” Topics. According to the DDS specification, the reliability QoS for these built-in Entities is set to ‘reliable.’ The SEDP therefore maps each corresponding built-in DDS DataWriter or DataReader into corresponding reliable RTPS Writer and Reader Endpoints.

For example, as illustrated in Figure 8.29, the DDS built-in Data Writers for the “DC PS Subscription,” “DC PS Publication,” and “DCPSTopic” Topics can be mapped to reliable RTPS State ful Writers and the corresponding DDS built-in Data Readers to reliable RTPS State ful Readers . Actual implementations need not use the stateful reference implementation. For the purpose of interoperability, it is sufficient that an implementation provides the required built-in Endpoints and reliable communication that satisfies the general requirements listed in 8.4.2.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/f37c4500d3acf6fff28089022f47b2fff9c3d77ccc65d5c8cfa0b3494a364dbf.jpg)
Figure 8.29 - Example mapping of the DDS Built-in Entities to corresponding RTPS built-in Endpoints 

The RTPS Protocol reserves the following values of the EntityId_t for the built-in Endpoints: 

ENTITY ID SED P BUILT IN PUBLICATIONS ANNOUNCER ENTITY ID SED P BUILT IN PUBLICATIONS DETECTOR 
The actual value for the reserved EntityId_t is defined by each PSM.

## 8.5.4.3 Built-in Endpoints required by the Simple Endpoint Discovery Protocol 

Implementations are not required to provide all built-in Endpoints.

As mentioned in the DDS specification, Topic propagation is optional. Therefore, it is not required to implement the SED P built in Topics Reader and SED P built in Topics Writer built-in Endpoints and for the purpose of interoperability, implementations should not rely on their presence in remote Participants.

As far as the remaining built-in Endpoints are concerned, a Participant is only required to provide the built-in Endpoints required for matching up local and remote Endpoints. For example, if a DDS Participant will only contain DDS Data Writers, the only required RTPS built-in Endpoints are the SED P built in Publications Writer and the SED P built in Subscriptions Reader. The SED P built in Publications Reader and the SED P built in Subscriptions Writer built-in Endpoints serve no purpose in this case.

The SPDP specifies how a Participant informs other Participants about what built-in Endpoints it has available.This is discussed in 8.5.3.2.

## 8.5.4.4 Data Types associated with built-in Endpoints used by the Simple Endpoint Discovery Protocol 

Each RTPS Endpoint has a History Cache that stores changes to the data-objects associated with the Endpoint.This also applies to the RTPS built-in Endpoints.Therefore, each RTPS built-in Endpoint depends on some DataType that represents the logical contents of the data written into its History Cache .

Figure 8.30 defines the Discovered Writer Data , Discovered Reader Data , and Discovered Topic Data DataTypes associated with the RTPS built-in Endpoints for the “DC PS Publication,” “DC PS Subscription,” and “DCPSTopic” Topics. The DataType associated with the “DC PS Participant” Topic is defined in 8.5.3.2.

The DataType associated with each RTPS built-in Endpoint contains all the information specified by DDS for the corresponding built-in DDS Entity. For this reason, Discovered Reader Data extends the DDS-defined DDS::Subscription Built in Topic Data, Discovered Writer Data extends DDS::Publication Built in Topic Data, and Discovered Topic Data extends DDS::Topic Built in Topic Data.

In addition to the data needed by the associated built-in DDS Entities, the “Discovered” DataTypes also include all the information that may be needed by an implementation of the protocol to configure the RTPS Endpoints.This information is contained in the RTPS Reader Proxy and Writer Proxy .
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/5a59157360de742a236f33eb87a11ac32151cc8746dfdc48b785577bcc5563b6.jpg)
Figure 8.30 - Data types associated with built-in Endpoints used by the Simple Endpoint Discovery Protocol 

An implementation of the protocol need not necessarily send all information contained in the DataTypes. If any information is not present, the implementation can assume the default values, as defined by the PSM. The PSM also defines how the discovery information is represented on the wire.

The RTPS built-in Endpoints used by the SEDP and their associated DataTypes are shown in Figure 8.31.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/5a5cb45cd69f104871d0df4234382848706ceb20129cd381ce27a728a20e1c0a.jpg)
Figure 8.31 - Built-in Endpoints and the DataType associated with their respective History Cache 

The contents of the History Cache for each built-in Endpoint can be described in terms of the following aspects: DataType, Cardinality, Data-object insertion, Data-object modification, and Data-object deletion.
- DataType. The type of the data stored in the cache. This is partly defined by the DDS specification.
- Cardinality. The number of different data-objects (each with a different key) that can potentially be stored in the cache.
- Data-object insertion. Conditions under which a new data-object is inserted into the cache.- Data-object modification. Conditions under which the value of an existing data-object is modified.
- Data-object deletion. Conditions under which an existing data-object is removed from the cache.It is illustrative to describe the History Cache for each of the built-in Endpoints.

##  SED P built in Publications Writer and SED P built in Publications Reader 

Table 8.77 describes the History Cache for the SED P built in Publications Writer and SED P built in Publications Reader .

Table 8.77 - Contents of the History Cache for the SED P built in Publications Writer and SED P built in Publications Reader 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/166e8e080113c021543c32b1b7810a5bcff53047d345f9c921055d09caf6e016.jpg)

##  SED P built in Subscriptions Writer and SED P built in Subscriptions Reader 

Table 8.78 describes the History Cache for the SED P built in Subscriptions Writer and SED P built in Subscriptions Reader.

Table 8.78 - Contents of the History Cache for the SED P built in Subscriptions Writer and SED P built in Subscriptions Reader 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/3247f2d2d0cf4be9d32869870204033b8b3f78698c919ba560a4c314c150d574.jpg)

##  SED P built in Topics Writer and SED P built in Topics Reader 

Table 8.79 describes the History Cache for the SED P built in Topics Writer and built in Topics Reader.
Table 8.79 - Contents of the History Cache for the SED P built in Topics Writer and SED P built in Topics Reader 

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/567c1435076ed25d3201b2418f8d291be54204cb0a8eb0ce1bfae203358c8e46.jpg)

## 8.5.5 Interaction with the RTPS virtual machine 

To further illustrate the SPDP and SEDP, this specification describes how the information provided by the SPDP can be used to configure the SEDP built-in Endpoints in the RTPS virtual machine.

## 8.5.5.1 Discovery of a new remote Participant 

Using the S PDP built in Participant Reader , a local Participant ‘ local participant ’ discovers the existence of another Participant described by the Discovered Participant Data participant data. The discovered Participant uses the SEDP.

The pseudo code below configures the local SEDP built-in Endpoints within local participant to communicate with the corresponding SEDP built-in Endpoints in the discovered Participant .

Note that how the Endpoints are configured depends on the implementation of the protocol. For the stateful reference implementation, this operation performs the following logical steps: 

// Check that the domainId of the discovered participant equals the local one.// If it is not equal then there the local endpoints are not configured to  // communicate with the discovered participant.IF ( participant data.domainId ! $=$  local participant.domainId)
 THEN   RETURN; ENDIF  // Check that the domainTag of the discovered participant equals the local one.// If it is not equal then there the local endpoints are not configured to  // communicate with the discovered participant.IF ( !STRING EQUAL(participant data.domainTag, local participant.domainTag))
 THEN   RETURN; ENDIF IF ( PUBLICATIONS DETECTOR IS_IN participant data.available Endpoints)
 THEN guid $=$  <participant data.guidPrefix, ENTITY ID SED P BUILT IN PUBLICATIONS DETECTOR>; writer  $=$  local participant.SED P built in Publications Writer; proxy  $=$  new Reader Proxy( guid, participant data.meta traffic Uni cast Locator List, 
participant data.meta traffic Multi cast Locator List); writer.matched reader add(proxy); ENDIF 

IF ( PUBLICATIONS ANNOUNCER IS_IN participant data.available Endpoints)
 THEN guid $=$  <participant data.guidPrefix,             ENTITY ID SED P BUILT IN PUBLICATIONS ANNOUNCER>;
reader  $=$  local participant.SED P built in Publications Reader; proxy  $=$  new Writer Proxy( guid, participant data.meta traffic Uni cast Locator List, participant data.meta traffic Multi cast Locator List); reader.matched writer add(proxy); 

ENDIF 

IF ( SUBSCRIPTIONS DETECTOR IS_IN participant data.available Endpoints)
 THEN guid $=$  <participant data.guidPrefix,             ENTITY ID SED P BUILT IN SUBSCRIPTIONS DETECTOR>; writer $=$  local participant.SED P built in Subscriptions Writer; proxy  $=$  new Reader Proxy( guid, participant data.meta traffic Uni cast Locator List, participant data.meta traffic Multi cast Locator List); writer.matched reader add(proxy); 

ENDIF 

IF ( SUBSCRIPTIONS ANNOUNCER IS_IN participant data.available Endpoints)
 THEN  guid  $=$  <participant data.guidPrefix,             ENTITY ID SED P BUILT IN SUBSCRIPTIONS ANNOUNCER>;
reader  $=$  local participant.SED P built in Subscriptions Reader;
proxy  $=$  new Writer Proxy( guid, participant data.meta traffic Uni cast Locator List, participant data.meta traffic Multi cast Locator List); reader.matched writer add(proxy); 

ENDIF 

IF ( TOPICS DETECTOR IS_IN participant data.available Endpoints)
 THEN guid  $=$  <participant data.guidPrefix,             ENTITY ID SED P BUILT IN TOPICS DETECTOR>; writer  $=$  local participant.SED P built in Topics Writer; proxy $=$  new Reader Proxy( guid, participant data.meta traffic Uni cast Locator List, participant data.meta traffic Multi cast Locator List); writer.matched reader add(proxy); 

ENDIF 

IF ( TOPICS ANNOUNCER IS_IN participant data.available Endpoints)
 THEN guid  $=$  <participant data.guidPrefix,             ENTITY ID SED P BUILT IN TOPICS ANNOUNCER>; reader  $=$  local participant.SED P built in Topics Reader; proxy $=$  new Writer Proxy( guid, participant data.meta traffic Uni cast Locator List, participant data.meta traffic Multi cast Locator List); reader.matched writer add(proxy); 

ENDIF 

## 8.5.5.2 Removal of a previously discovered Participant 

Based on the remote Participant’s lease Duration , a local Participant ‘ local participant ’ concludes that a previously discovered Participant with GUID_t participant gui d is no longer present. The Participant ‘ local participant ’ must reconfigure any local Endpoints that were communicating with Endpoints in the 
Participant identified by the GUID_t participant gui d .

For the stateful reference implementation, this operation performs the following logical steps: 

guid $=$  <participant gui d.guidPrefix,              ENTITY ID SED P BUILT IN PUBLICATIONS DETECTOR>;
writer  $=$  local participant.SED P built in Publications Writer; proxy  $=$  writer.matched reader lookup(guid); writer.matched reader remove(proxy);
guid $=$  <participant gui d.guidPrefix,              ENTITY ID SED P BUILT IN PUBLICATIONS ANNOUNCER>;
reader  $=$  local participant.SED P built in Publications Reader; proxy  $=$  reader.matched writer lookup(guid); reader.matched writer remove(proxy); guid $=$  <participant gui d.guidPrefix,              ENTITY ID SED P BUILT IN SUBSCRIPTIONS DETECTOR>;
writer  $=$  local participant.SED P built in Subscriptions Writer; proxy  $=$  writer.matched reader lookup(guid); writer.matched reader remove(proxy); guid $=$  <participant gui d.guidPrefix,              ENTITY ID SED P BUILT IN SUBSCRIPTIONS ANNOUNCER>;
reader  $=$  local participant.SED P built in Subscriptions Reader; proxy  $=$  reader.matched writer lookup(guid); reader.matched writer remove(proxy); guid $=$  <participant gui d.guidPrefix, ENTITY ID SED P BUILT IN TOPICS DETECTOR>; writer $=$  local participant.SED P built in Topics Writer; proxy  $=$  writer.matched reader lookup(guid); writer.matched reader remove(proxy); guid $=$  <participant gui d.guidPrefix, ENTITY ID SED P BUILT IN TOPICS ANNOUNCER>; reader  $=$  local participant.SED P built in Topics Reader; proxy  $=$  reader.matched writer lookup(guid); reader.matched writer remove(proxy); 

## 8.5.6 Supporting Alternative Discovery Protocols 

The requirements on the Participant and Endpoint Discovery Protocols may vary depending on the deployment scenario. For example, a protocol optimized for speed and simplicity (such as a protocol that would be deployed in embedded devices on a LAN) may not scale well to large systems in a WAN environment.

For this reason, the RTPS specification allows implementations to support multiple PDPs and EDPs. There are many possible approaches to implementing a Discovery Protocol including the use of static discovery, filebased discovery, a central look-up service, etc. The only requirement imposed by RTPS for the purpose of interoperability is that all RTPS implementations support at least the SPDP and SEDP. It is expected that over time, a collection of interoperable Discovery Protocols will be developed to address specific deployment needs.

If an implementation supports multiple PDPs, each PDP may be initialized differently and discover a different set of remote Participants. Remote Participants using a different vendor’s RTPS implementation must be contacted using at least the SPDP to ensure interoperability. There is no such requirement when the remote Participant uses the same RTPS implementation.

Even when the SPDP is used by all Participants, remote Participants may still use different EDPs. Which EDPs a Participant supports is included in the information exchanged by the SPDP. All Participants must support at least the SEDP, so they always have at least one EDP in common. However, if two Participants both support 
another EDP, this alternative protocol can be used instead. In that case, there is no need to create the SEDP built-in Endpoints, or if they already exist, no need to configure them to match the new remote Participant. This approach enables a vendor to customize the EDP if desired without compromising interoperability.
