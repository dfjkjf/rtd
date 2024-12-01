---
layout: default
title: QoS
nav_order: 6
---

# QoS

## 8.6  Versioning and Extensibility  

Implementations of this version of the RTPS protocol should be able to process RTPS Messages not only with  the same major version but possibly higher minor versions.  

### 8.6.1  Allowed Extensions within this major Version  

Within this major version, future minor versions of the protocol can augment the protocol in the following  ways:  

•   Additional Sub messages with other  sub message Ids  can be introduced and used anywhere in an  RTPS Message. An implementation should skip over unknown Sub messages using the  sub message Length  field in the Sub message Header.  •   Additional fields can be added to the end of a Submessage that was already defined in the current  minor version. An implementation should skip over additional fields using the  sub message Length  field in the Sub message Header.  •   Additional built-in Endpoints with new IDs can be added. An implementation should ignore any  unknown built-in Endpoints.Additional parameters with new  parameter Ids  can be added. An  implementation should ignore any unknown parameters.  

All such changes require an increase of the minor version number.  

### 8.6.2  What cannot change within this major Version  

The following items cannot be changed within the same major version:  

•   A Submessage cannot be deleted.  •   A Submessage cannot be modified except as described in 8.6.1.  •   The meaning of  sub message Ids  cannot be modfied.  

All such changes require an increase in the major version number.  

## 8.7  Implementing DDS QoS and advanced DDS features using RTPS  

The RTPS protocol and its extension mechanisms provide the core functionality required to implement DDS.  This sub clause defines how to use RTPS to implement the DDS QoS parameters.  

In addition, this sub clause defines the RTPS protocol extensions required for implementing the following  advanced DDS features:  •   Content-filtered Topics, see 8.7.3  •   Instance State Changes 8.7.4  •   Group Ordered Access, see 8.7.5  •   Coherent Sets, see 8.7.6  

All extensions are based on the standard extension mechanisms provided by RTPS.  

This sub clause forms a normative part of the specification for the purpose of interoperability.  

### 8.7.1  Adding in-line Parameters to Data Sub messages  

Data  and  DataFrag  Sub messages optionally contain a  Parameter List  Sub message Element for storing  in-line QoS parameters and other information.  
In case a  Reader  does not keep a list of matching remote  Writers  or the QoS parameters they were configured  with (i.e., is a stateless  Reader) , a  Data  Submessage with in-line QoS parameters contains all the information  needed to enable the  Reader  to apply all  Writer -specific QoS parameters.  

A stateless  Reader’s  need for receiving in-line QoS to get information on remote  Writers  is the justification for  requiring a  Writer  to send in-line QoS if the  Reader  requests them (8.4.2.2.2).  

For immutable QoS, all RxO QoS are sent in-line to allow a stateless  Reader  to reject samples in case of  incompatible QoS. Mutable QoS relevant to the  Reader  are sent in-line so they may take effect immediately,  regardless of the amount of state kept on the  Reader . Note that a stateful  Reader  has the option of relying on its  cached information of remote  Writers  rather than the received in-line QoS.  

A stateless  Reader  uses the discovery protocol to announce to remote  Writers  that it expects to receive QoS  parameters in-line, as discussed in the Discovery Module (8.5). If in-line QoS parameters are expected,  implementations must also include the topic name as an in-line parameter. This ensures that on the receiving  side, the Submessage can be passed to all  Readers  for that topic, including the stateless  Readers .  

Independent of whether  Readers  expect in-line QoS parameters, a  Data  Submessage may also contain in-line  parameters related to coherent sets and content-filtered topics. This is described in more detail in the sub clauses  that follow.  

For improved performance, stateful implementations may ignore in-line QoS and instead rely solely on cached  values obtained through Discovery. Note that not parsing in-line QoS may delay the point in time when a new  QoS takes effect, as it first must be propagated through Discovery.  

### 8.7.2  DDS QoS Parameters  

Table 8.80 provides an overview of which QoS parameters affect the RTPS wire protocol and which can appear  as in-line QoS. The parameters that affect the wire protocol are discussed in more detail in the subsub clauses  below.  

Table 8.80 - Implementing DDS QoS Parameters using the RTPS Wire Protocol  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/42bff268aab0ae4e99902c280cd27979f1853254fb831c675d55dcbe21d2305d.jpg)  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/c4ff0b4ca7a0235ac55021e9f4b0be6e43e49adf4abb20204051642e34e7aa1b.jpg)  

#### 8.7.2.1  In-line DDS QoS Parameters  

Table 8.80 lists the standard DDS QoS parameters that may appear in-line.  

If a  Reader  expects to receive in-line QoS parameters and any of these QoS parameters are missing, it will  assume the default value for that QoS parameter, where the default is defined by DDS.  

In-line parameters are added to data sub messages to make them self-describing. In order to achieve selfdescribing messages, not only the parameters defined in Table 8.80 have to be sent with the submessage, but  also a parameter TOPIC_NAME. This parameter contains the name of the topic that the submessage belongs to.  

## 8.7.2.2  DDS QoS Parameters that affect the wire protocol  

##   DURABILITY  

While volatile and transient-local durability do not affect the RTPS protocol, support for transient and persistent  durability may. This is not covered in the current version of the specification.  

##   PRESENTATION  

Sub clause 8.7.5 defines how to implement the GROUP ordered access policy of the PRESENTATION QoS.  

Sub clause 8.7.6 defines how to implement the coherent access policy of the PRESENTATION QoS. The other  aspects of this QoS do not affect the RTPS protocol.  

##  LIVELINESS  

Implementations must follow the approaches below:  

•   DD S AUTOMATIC LIVELINESS QO S  $:$   liveliness is maintained through the  Built in Participant Message Writer . For a given  Participant , in order to maintain the liveliness of  its  Writer  Entities with LIVELINESS QoS set to AUTOMATIC, implementations must refresh  the  Participant’s  liveliness (i.e., send the  Participant Message Data,  see (8.4.13.5) at a rate faster  than the smallest lease duration among the  Writer s.  •   DD S MANUAL BY PARTICIPANT LIVELINESS QO S  $:$   liveliness is maintained through  the  Built in Participant Message Writer . If the  Participant  has any  MANUAL BY PARTICIPANT  Writers , implementations must check periodically to see if  write(), assert liveliness(), dispose(),  or  un register instance()  was called for any of them. The  period for this check equals the smallest lease duration among the  Writers . If any of the  operations were called, implementations must refresh the  Participant’s  liveliness (i.e., send the  Participant Message Data , see 8.4.13.5).  •   DD S MANUAL BY TOPIC LIVELINESS QO S  $:$   liveliness is maintained by sending data or  an explicit  Heartbeat  message with liveliness flag set. The standard RTPS Messages that  result from calling  write(), dispose(),  or  un register instance()  on a  Writer  Entity suffice to assert  
the liveliness of a  Writer  with LIVELINESS QoS set to MANUAL BY TOPIC .  When  assert liveliness()  is called, the  Writer  must send a  Heartbeat  Message with final flag and  liveliness flag set.  

##   TIME BASED FILTER  

Implementations may optimize bandwith usage by applying a time-based filter on the  Writer  side. That way,  data that would be dropped on the  Reader  side is never sent.  

When one or more data updates are filtered out on the  Writer  side, implementations must send a  Gap  Submessage instead, indicating which samples were filtered out. This Submessage must be sent before the next  update and notifies the Reader the missing updates were filtered out and not simply lost.  

##   RELIABILITY  

Implementations must meet the reliable RTPS protocol requirements for interoperability, defined in 8.4.2.  

##   DESTINATION ORDER  

In order to implement the DD S BY SOURCE TIMESTAMP DESTINATION ORDER QO S policy,  implementations must include an  Info Timestamp  Submessage with every update from a  Writer .  

##   WRITER DATA LIFECYCLE  

If  auto dispose unregistered instances  is enabled,  Data  Messages that unregister an instance must also  dispose it. This restricts the allowable values of the Disposed Flag and Unregistered Flag flags.  

## 8.7.3  Content-filtered Topics  

Content-filtered topics make it possible for a DDS DataReader to request the middleware to filter out data  samples based on their contents.  

When filtering on the Reader side only, samples which do not pass the filter are simply dropped by the  middleware. In this case, no further extensions to RTPS are needed.  

In many cases, implementations will benefit from filtering on the Writer side, in addition to filtering on the  Reader side. When filtering on the Writer side, a sample that does not pass a Reader side filter may sometimes  not be sent to that  Reader . This conserves bandwidth.  

In order to support Writer side filtering, standard RTPS extension mechanisms are used to:  •   Include Reader filter information during the Endpoint discovery phase.  •   Include filter results with each data sample.  

The  Writer  may indicate to a  Reader  that a Sample has been filtered due to the application of the readerspecified content filter by sending a directed  Data  message that includes only the key information  (DataFlag  $\mathord{=}0$  ), indicating in the Inline Qos that the instance state is ALIVE FILTERED. See 8.7.3.2. The  Reader  may use this information to transition the specified instance to Instance State ALIVE FILTERED.  

The  Writer  may indicate to a  Reader  that it has applied a set of filters to a Sample and the corresponding result  by including the  Content Filtered Info t  into the  Data  message, see 8.7.3.3. Readers can use  Content Filtered Info t  to determine whether their filter has been already applied by the  Writer  and avoid having  to apply the filter again.  

Alternatively, the  Writer  may not send a  Data  message at all. This is only allowed if the previous sample for  that Instance was already filtered for that  Reader , see 8.7.4.  

## 8.7.3.1  Exchanging filter information using the built-in Endpoints  

Content-filtered topics are defined on the Reader side. In order to implement Writer side filtering, information  
on the filter used by a given Reader must be propagated to matching remote Writers. This requires extending the  data type associated with RTPS built-in Endpoints.  

As illustrated in Figure 8.31, the data types associated with RTPS built-in Endpoints extend the DDS built-in  topic data types, which include all relevant QoS. Since DDS does not define content-filtered topics as a Reader  QoS policy (instead, DDS defines separate Content-filtered Topics), RTPS adds an additional  Content Filter Property t  field to Discovered Reader Data, defined in Table 8.81.  

Table 8.81 - Content filter property  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/d303c09b51b0779bcd1456bfa6bd1509a1725e4900aa366c1f9e7285be8be37d.jpg)  

The  Content Filter Property t  field provides all the required information to enable content filtering on the Writer  side. For example, for the default DDSSQL filter class, a valid filter expression for a data type containing  members a, b and c could be   ${\bf\ddot{\alpha}}({\bf a}<5)$   AND   $({\sf b}==\%0)$  ) AND   $\mathrm{(c>=\%1)}$  )” with expression parameters   $^{\ast5^{\ast}}$   and  “3.” In order for the Writer to apply the filter, it must have been configured to handle filters of the specified  filter class. If not, the Writer will simply ignore the filter information and not filter any data samples.  

DDS allows the user to modify the filter expression parameters at run-time. Each time the parameters are  modified, the updated information is exchanged using the Endpoint discovery protocol. This is identical to  updating a mutable QoS value.  

## 8.7.3.2  Indicating to a Reader that a Sample has been filtered  

There are situations when a  Writer  needs to communicate to a  Reader  that a sample was written but it does not  pass the reader-specified Content Filter. When this happens, the  Writer  can use a Data submessage that does not  contain a Data payload (DataFlag  $\mathord{=}0$  ) and sets Filtered Flag  $\scriptstyle{=1}$  , see 8.3.7.2.2.  
## 8.7.3.3  Including in-line filter results with each data sample  

In general, when applying filtering on the Writer side, a sample is not sent if it does not pass the remote  Reader’s filter. In that case, the  Data  submessage is replaced by a  Gap  submessage. This ensures the sample  is not considered ‘lost’ on the Reader side. This approach matches that of applying a time-based filter on the  Writer side. The remainder of the discussion only refers to  Data  Sub messages, but the same approach is  followed for  DataFrag  Sub messages.  

In some cases, it may still be possible for a Reader to receive a sample that did not pass its filter, for example  when sending data using multicast. Another use case is multiple Readers belonging to the same Participant. In  that case, the Writer need only send a single RTPS message, destined to ENTITY ID UNKNOWN (see  8.4.15.5). Each Reader may use a different filter however, in which case the Writer needs to apply multiple  filters before sending the sample.  

In both use cases, two options exist:  

1.   The sample passes none of the filters for any of the remote Readers. In that case, the  Data  submessage is again replaced by a  Gap  submessage.  2.   The sample passes some or all of the filters. In that case, the sample must still be sent and the  writer must include information with the  Data  submessage on what filters were applied and  the according result.  

The  inlineQos  element of the  Data  submessage is used to include the necessary filter information. More  specifically, a new parameter is added, containing the information shown in Table 8.82.  

Table 8.82 - Content filter info associated with a data sample  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/b2ee93c676d6f9e30dd9de806262550abea35b798299423bb853b9964f6bf970.jpg)  

A filter signature  Filter Signature t  uniquely identifies a filter and is based on the filter properties listed in  Table 8.81. How to represent and calculate a filter signature is defined by the PSM. Whether the sample passed  the filters that were applied on the  Writer  side is encoded by the  filter Result t  attribute, again defined by the  PSM.  

Note that a filter signature changes when the filter’s expression parameters change. Until it receives updated  parameter values, a Writer side filter may be using outdated expression parameters, in which case the in-line  filter signature will not match the signature expected by the Reader. As a result, the Reader will ignore the filter  results and instead apply its local filter.  

## 8.7.3.4  Requirements for Interoperability  

Writer side filtering constitutes an optimization and is optional, so it is not required for interoperability. Samples  will always be filtered on the Reader side if:  

•   The Writer side did not apply any filtering.  •   The Writer side did not apply the filter expected by the Reader. As mentioned earlier, this may  occur if the Writer has not yet been informed about updated filter parameters.  •   The Reader side does not support Writer side filtering (and therefore ignores in-line filter  information).  

Likewise, Writers may not filter samples because:  

•   The implementation does not support Content-filtered Topics (in which case the filter properties  
of the Reader are ignored).  

•   The Reader's filter information was rejected (e.g., unrecognized filter class). If an  implementation supports Content-filtered Topics, it must at least recognize the “DDSSQL” filter  class, as mandated by the DDS specification. For all other filter classes, both implementations  must allow the user to register the same custom filter class.  •   Other implementation-specific restrictions, such as a resource limit on the number of remote  readers each writer is able to store filter information for.  

Even if the  Writer  is performing writer-side filtering, the  Writer  must provide enough information for the  Reader  to correctly transition the instance state to ALIVE FILTERED. This means that even if a Sample does  not pass the reader filter, the  Writer  must still send a  Data  submessage unless it the previous sample for that  Instance also did not pass the content filter. See 8.7.3.2.  

This requirement effectively means that a  Writer  needs maintain state per Instance and per “content filtered”  Reader . In this state it must remember whether the last sample written to that Instance passed the reader filter.  

## 8.7.4  Changes in the Instance State  

A DDS DataWriter may register data object instances (operation  register instance ), update their value  (operation  write ), dispose data-object instances (operation  dispose ), and unregister them (operation  un register instance ). When the value of an instance is updated, the new value may not pass the content filter  specified by a subset of the Data Readers.  

Each one of these operations may cause notifications to be dispatched to the matched DDS Data Readers. The  DDS DataReader can determine the nature of the change by inspecting the  Instance State instance state  field in  the  SampleInfo  that is returned on the DDS DataReader  read  or  take  call.  

RTPS uses regular  Data  Sub messages and the in-line QoS parameter extension mechanism to communicate  instance state changes. The serialized information within the inline QoS contains the new  Instance State , that is,  whether the instance has been registered, unregistered, or disposed. The actual details depend on the PSM  (e.g., 9.6.3.4).  

When RTPS sends a  Data  Submessge to communicate instance state changes it may include only the Key of  the Data-Object within the Serialized Payload submessage element (see 8.3.7.2). This is because the Key is  sufficient to uniquely identify the Data-Object instance to which the  Instance State  change applies.  

An implementation of RTPS is not required to propagate registration changes until the DDS DataWriter writes  the first value for that Data-Object instance.  

If a DataWriter updates the value of an instance (operation write), the updated value may not pass the content  filter specified by one (or more) matched Data Readers. In this situation, there are two possibilities:  

1.   If the previous update to the instance passed the filter, then the Writer must send a Data Submessage  that either includes the data value, or else indicates the Intan ce State is ALIVE FILTERED. See  9.6.3.5.  2.   If the previous update to the instance did not pass the filter, then the Writer may omit sending the  Data Submessage to the Reader.  

The rules above ensure the Writer provides enough information for the Reader to transition the instance state to  ALIVE FILTERED.  

If a DataWriter disposes an instance (operation dispose) or un registers an instance (operation unregister), there  are several possibilities which dictate whether the Writer must send a  Data  Submessage that indicates the  Instance State  is NOT ALIVE DISPOSED or NOT ALIVE NO WRITERS, respectively. This so called  “dispose/unregister message” shall be sent if any of the following conditions is met:  

1.   The Reader does not have a Content Filter.  2.   The Writer has previously sent a Data message to the Reader for that same instance.  
3.   The Reader has OWNERSHIP QosPolicy kind EXCLUSIVE and the Reader Filter is such that there  could be some values for the Instance that pass the filter.  

In all other cases, the “instance state change” message may be omitted as an optimization.  

These conditions ensure that the Reader is able to determine consistently the ownership and Instance State for  the instance.  

## 8.7.5  Group Ordered Access  

The DDS Specification provides the functionality for  Cache Changes  made by  DataWriter  entities attached to  the same  Publisher  object to be made available to subscribers in the same order they occur.  

In order to support group ordered access, RTPS uses the in-line QoS parameter extension mechanism to include  additional information with each  Cache Change . The additional information denotes ordering within the scope  of the  Publisher , as well as the identity of the  Writers  belonging to the  Publisher .  

The  inlineQos  element of the  Data  submessage is used to include the necessary group sequence number and  publisher writer information. Two new parameters are added to convey this information (see also Table 9.14):  

•   PID GROUP SEQ NUM to contain the group sequence number.  •   PID WRITER GROUP INFO to contain the  Writer Group Info t  defined in Table 8.83.  

Table 8.83 – Group Writer Info associated with a data sample  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/0436971cd2ca516b23493663fdeb4527d91a92926c688111d598b4d2923a8188.jpg)  

When a  Publisher  is configured with access scope  GROUP , all  Data  sub messages and the first  DataFrag   submessage from any  Writer  within the Publisher are accompanied with a  GROUP  sequence number sent as  part of the in-line QoS. The  GROUP  sequence number is a strictly monotonically increasing sequence number  originating from the  Publisher . Each time that a  DataWriter  attached to a  Publisher  makes a  Cache Change   (i.e., increments its own  Writer  sequence number), the  GROUP  sequence number is incremented.  

A  DataReader  attached to a  Subscriber  configured with access scope  GROUP  first orders the samples from a  remote  Writer  as it would in the cases where access scope  GROUP  is not set. Once a sample is ready to be  committed to the DDS  DataReader , it will not commit it. Instead, it will hand it off to a  History Cache  of the  Subscriber  where ordering across remote  Data Writers  belonging to the same  Publisher  occurs. A sample with  GROUP  sequence number  GSN  can be committed to the DDS  DataReader  from the  Subscriber’s  history cache  if any of the following conditions apply:  

•   GSN-1  has been already been committed.  

•   It has been determined that none of the remote  Data Writers  that match reliable  Data Readers  have  GSN-1 . This condition is met when both of the following conditions apply:  

o   The  Subscriber  has received a  Heartbeat  from one of the  Data Writers  with  Heartbeat.currentGSN.value  $>=\mathrm{GSN}$   and the Heartbeat.writerSet (and  Heartbeat.secure Writer Set) matches the set of discovered  Data Writers .  
o   AND for every matched  DataWriter  belonging to the  Publisher  that matches a reliable  DataReader , the  DataWriter  has:  

   Either advanced past the  GSN-1  (by committing a  Data  sample with  Data.inlineQos.group Sequence Number  $>=G S N)$   to the  Subscriber  history cache or  a  Gap  message with  Gap.gapEndGSN.value  $>=G S N–I$       OR announced it does not have the GSN-1 by sending a  Heartbeat  with  Heartbeat.currentGSN.value  $>=\mathrm{GSN}$   and GSN-1  ∉  [_Heartbeat.firstGSN.value_,  _Heartbeat.lastGSN.value_]  

The above rules should only take into consideration  Data Writers  that have not lost their liveliness, see 8.7.2.2.3.  

Implementations could use additional timeout-based rules to limit delays.  

## 8.7.6  Coherent Sets  

The DDS specification provides the functionality to define a set of sample updates as a coherent set. A  DataReader is only notified of the arrival of new updates once all updates in the coherent set have been  received.  

A “Publisher coherent set” is defined as the set of all  Cache Changes  performed by all  Data Writers  in the  Publisher  delimited by the operations  begin coherent changes()  and  

end coherent changes() .  

Resulting from each “Publisher coherent set” there may be one or more “Subscriber coherent sets” defined for  each  Subscriber  in the system. What constitutes a “Subscriber coherent set” depends on the  PRESENTATION   access scope  of the  Subscriber :  

•   If the  Subscriber  has  PRESENTATION   coherent access  $\scriptstyle{=}F A L S E$   then there are no Subscriber  coherent sets. Alternatively, this could be interpreted as if each individual  Cache Change  was an  independent Subscriber coherent set.  •   If the  Subscriber  has  PRESENTATION   access scope  $=$  INSTANCE  or  TOPIC  then there is a separate  “Subscriber” coherent set for each  DataWriter  containing the subset of samples that are written by  each of the  Data Writers  in the Publisher.  •   If the  Subscriber  has  PRESENTATION   access scope  $\smile$  GROUP  then the  Subscriber  coherent set  matches the  Publisher  coherent set.  

A “Subscriber-relevant coherent set” is the subset of changes in the “Subscriber coherent set” that the  Subscriber  must receive in order to consider the coherent set complete. Incomplete coherent sets shall not be  added to the history of the RTPS Data Readers and the corresponding  Cache Changes  shall be discarded by the  Subscriber .  

The “Subscriber-relevant coherent set” is defined as the subset of the “Subscriber coherent change” obtained  after removing the following  Cache Changes :  

•   Changes that belong to  Data Writers  that are not matched with corresponding  Data Readers  in the  Subscriber .  •   Changes that are filtered by content or time.  

Note that samples replaced due to history depth are considered part of the “Subscriber-relevant coherent set” if  any is not received the coherent set is not complete. Likewise, for samples lost due to the use of best-effort  protocol or other reasons.  

In order to support coherent sets, RTPS uses the in-line QoS parameter extension mechanism to include  
additional information in-line with each  Data  Submessage. The additional information denotes membership to  a particular coherent set. The remainder of the discussion only refers to  Data  Sub messages, but the same  approach is followed for  DataFrag  Sub messages.  

For access scope TOPIC, all  Data  Sub messages belonging to the same coherent set have strict monotonically  increasing sequence numbers (as they originated from the same  Writer ). Therefore, a coherent set is uniquely  identified by the sequence number of the first sample update belonging to the coherent set. All sample updates  belonging to the same coherent set contain an in-line QoS parameter with this same sequence number. This  approach also allows the  Reader  to easily determine when the coherent set started.  

The end of a  Writer’s  coherent set is defined by the arrival of one of the following:  

•   A  Data  Submessage from this  Writer  that belongs to a new coherent set.  •   A  Data  Submessage from this  Writer  that does not contain a coherent set in-line QoS parameter  or alternatively, contains a coherent set in-line QoS parameter with value  SEQUENCE NUMBER UNKNOWN.  Both approaches are equivalent.  

Note that a  Data  Submessage need not necessarily contain  serialized Payload . This makes it possible to notify  the  Reader  about the end of a coherent set before the next data is written by the  Writer .  

For access scope  GROUP , all  Data  sub messages and the first  DataFrag  submessage belonging to the same  coherent set have strictly monotonically increasing group sequence numbers (as they originated from the same  Publisher ). Therefore, a group coherent set is uniquely identified by the group sequence number of the first  sample belonging to the coherent set. All  Data  sub messages and the first  DataFrag  submessage belonging to  the same group coherent set shall have three in-line QoS parameters:  

•   The PID GROUP SEQ NUM shall contain the group sequence number.  •   The PID COHERENT SET shall contain the sequence number of the first sample update  belonging to the coherent set from the  Writer .  •   The PID GROUP COHERENT SET shall contain the group sequence number of the first  sample update belonging to the coherent set across all  Writers  within the  Publisher .  

A group’s coherent set is marked as being finished by sending an End Coherent Set (ECS) Data submessage  from all  Writers  within the  Publisher . The ECS  Data  Submessage shall have the following properties:  

•   It does not contain a  serialized Payload   •   Its group sequence number is equal to one greater than the group sequence number of the final  sample in the group coherent set.  •   It is not filtered by time, content, history, lifespan, etc. It can only be removed from the RTPS  Writer  cache when all data samples belonging to the coherent set are removed.  •   It does not count towards resource limits.  •   It has the  InlineQos  parameters PID GROUP SEQ NUM, PID GROUP COHERENT SET,  PID WRITER GROUP INFO.  •   If required, it may also contain PID SECURE WRITER GROUP INFO. See section 9.6.3.5  for details.  

The ECS  Data  Submessage is sent with in-line QoS parameters:  

•   PID GROUP SEQ NUM: The group sequence number one greater than the group sequence  number of the last sample in the coherent set.  •   PID GROUP COHERENT SET: The group sequence number of the coherent set that it marks  the end of.  •   PID GROUP WRITER INFO: The writer group information encoding which writers were  contained in the  Publisher  during the time that the coherent set was written. Note that Writers  are not allowed to be added or removed from a  Publisher  from the time that a coherent set  begins until after it ends.  

A  DataReader  that receives samples in a group coherent set first waits for the complete coherent set from each  
remote  DataWriter  separately. Once a coherent set from a  DataWriter  is complete, the  DataReader  commits  the entire set to the  History Cache  of the  Subscriber . The  Subscriber  orders these individual coherent sets from  each  DataReader  according to the same rules that are applied for ordered access with scope set to  GROUP . The  group coherent set becomes ready to be committed to the DDS  DataReader  once an ECS sample is committed  to the  Subscriber  and the ECS sample meets the criteria for being committed to the DDS  DataReader .  

Once the group coherent set becomes ready to be committed the  Subscriber  shall determine if the subscriberrelevant coherent set is complete and if so, make it available to the application.  

## 8.7.7  Directed Write  

Direct peer-to-peer communications where a Writer explicitly identifies a Subset of its matched Readers as the  intended destination for a particular sample is useful in some application scenarios.  

RTPS supports directed writes by using the in-line QoS parameter extension mechanism. The serialized  information denotes the GUIDs of the targeted reader(s).  

When a writer sends a directed sample, only recipients with a matching GUID accept the sample; all other  recipients acknowledge but absorb the sample, as if it were a GAP message.  

## 8.7.8  Property Lists  

Property lists are lists of user-definable properties applied to a DDS Entity. An entry in the list is a generic  name-value pair. A user defines a pair to be a property for a DDS Participant, DataWriter, or DataReader. This  extensible list enables non-DDS-specified properties to be applied.  

The RTPS protocol supports Property Lists as in-line parameters. Properties can then be propagated during  Discovery or as in-line QoS.  

## 8.7.9  Original Writer Info  

A service supporting the Transient Local, Transient, or Persistent level of DDS Durability QoS needs to send the  data that has been received and stored on behalf of the persistent writer.  

This service that forwards messages needs to indicate that the forwarded message belongs to the messagestream of another writer, such that if the reader receives the same messages from another source (for example,  another forwarding service or the original writer), it can treat them as duplicates.  

The RTPS protocol suports this forwarding of messages by including information of the original writer.  

When a RTPS Reader receives this information, it will treat it as a normal Cache Change, but once the  Cache Change is ready to be committed to the DDS DataReader, it will not commit it. Instead, it will hand if off  to the History Cache of the RTPS Reader that is communicating with the RTPS Writer indicated in the  ORIGINAL WRITER INFO in-line QoS and treat is as having the sequence number which appears there.  

Table 8.84 - Original writer info  
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/012338c13dccb4b8b6738c6d8885495e9aa302cc98080039a1ad0f2a5cefb92a.jpg)  

## 8.7.10 Key Hash  
The Key Hash provides a hint for the key that uniquely identifies the data-object that is being changed within  the set of objects that have been registered by the DDS DataWriter.  

Nominally the key is part of the serialized data of a data submessage. Using the key hash benefits  implementations by providing a faster alternative than de serializing the full key from the received data-object.  When the key hash is not received by a DataReader, it should be computed from the data itself. If there is no  data in the submessage, then a default zero-valued key hash should be used by the DataReader.  

A Key Hash, if present, shall be computed as described in 9.6.3.3.  