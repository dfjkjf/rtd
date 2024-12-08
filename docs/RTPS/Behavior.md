---
layout: default
title: Behavior Module
nav_order: 4
---

# 8.4 Behavior Module

This module describes the dynamic behavior of the RTPS entities. It describes the valid sequences of message exchanges between RTPS Writer endpoints and RTPS Reader endpoints and the timing constraints of those messages.

## 8.4.1 Overview

Once an RTPS Writer has been matched with an RTPS Reader , they are both responsible for ensuring that Cache Change changes that exist in the Writer ’s History Cache are propagated to the Reader ’s History Cache .

The Behavior Module describes how the matching RTPS Writer and Reader pair must behave in order to propagate Cache Change changes. The behavior is defined in terms of message exchanges using the RTPS Messages defined in 8.3. The Behavior Module is organized as follows:
- 8.4.2 lists what requirements all implementations of the RTPS protocol must satisfy in terms of behavior. An implementation that satisfies these requirements is considered compliant and will be interoperable with other compliant implementations.
- As implied above, it is possible for multiple implementations to satisfy the minimum requirements, where each implementation may choose a different trade-off between memory requirements, bandwidth usage, s cal ability, and efficiency. The RTPS specification does not mandate a single implementation with corresponding behavior. Instead, it defines the minimum requirements for interoperability and then provides two Reference Implementations, the Stateless and Stateful Reference Implementations, described in 8.4.3.
- The protocol behavior depends on such settings as the RELIABILITY QoS. 8.4.4 discusses the possible combinations.
- 8.4.5 and 8.4.6 define notational conventions and define any new types used in this module.
- 8.4.7 through 8.4.12 model the two Reference Implementations.
- 8.4.13 describes the Writer Liveliness Protocol that is used by Participants to assert the liveliness of their contained Writers.
- 8.4.14 discusses some optional behavior, including support for fragmented data.
- Finally, 8.4.15 provides guidelines for actual implementations.

Note that, as discussed earlier in 8.2.9, the Behavior Module does not model the interactions between DDS 
Entities and their corresponding RTPS entities. For example, it simply assumes a DDS DataWriter adds and removes Cache Change changes to and from its RTPS Writer ’s History Cache . Changes are added by the DDS DataWriter as part of its write operation and removed when no longer needed. It is important to realize the DDS DataWriter may remove a Cache Change before it has been propagated to one or more of the matched RTPS Reader endpoints. The RTPS Writer is not in control of when a Cache Change is removed from the Writer ’s History Cache . It is the responsibility of the DDS DataWriter to only remove those Cache Change changes that can be removed based on the communication status and the DDS DataWriter’s QoS. For example, the HISTORY QoS setting of KEEP_LAST with a depth of 1 allows a DataWriter to remove a Cache Change if a more recent change replaces the value of the same data-object.

## 8.4.1.1 Example Behavior

The contents of this sub clause are not part of the formal specification of the protocol. The purpose of this sub clause is to provide an intuitive understanding of the protocol.

A typical sequence illustrating the exchanges between an RTPS Writer and a matched RTPS Reader is shown in Figure 8.14. The example sequence in this case uses the Stateful Reference Implementation.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/e38b73201eff085d5980dd3a57445137fbcd373e2c44358e899dee8934f31823.jpg)
Figure 8.14 – Example Behavior

The individual interactions are described below:

1. The DDS user writes data by invoking the write operation on the DDS DataWriter .2. The DDS DataWriter invokes the new_change operation on the RTPS Writer to create a new Cache Change . Each Cache Change is identified uniquely by a Sequence Number .3. The new_change operation returns.4. The DDS DataWriter uses the add_change operation to store the Cache Change into the RTPS Writer ’s History Cache .
5. The add_change operation returns.

6. The write operation returns, the user has completed the action of writing Data.7. The RTPS Writer sends the contents of the Cache Change changes to the RTPS Reader using the Data Submessage and requests an acknowledgment by also sending a Heartbeat Submessage.8. The RTPS Reader receives the Data message and, assuming that the resource limits allow that, places the Cache Change into the reader’s History Cache using the add_change operation.9. The add_change operation returns. The Cache Change is visible to the DDS DataReader and the DDS user. The conditions for this depend on the reliability Level attribute of the RTPS Reader .a. For a RELIABLE DDS DataReader , changes in its RTPS Reader ’s History Cache are made visible to the user application only when all previous changes (i.e., changes with smaller sequence numbers) are also visible.b. For a BEST EFFORT DDS DataReader, changes in its RTPS Reader ’s History Cache are made visible to the user only if no future changes have already been made visible (i.e., if there are no changes in the RTPS Receiver’s History Cache with a higher sequence number).10. The DDS user is notified by one of the mechanisms described in the DDS Specification (e.g., by means of a listener or a WaitSet)
 and initiates reading of the data by calling the take operation on the DDS DataReader .11. The DDS DataReader accesses the change using the get_change operation on the History Cache .12. The get_change operation returns the Cache Change to the DataReader.13. The take operation returns the data to the DDS user.14. The RTPS Reader sends an AckNack message indicating that the Cache Change was placed into the Reader’s History Cache . The AckNack message contains the GUID of the RTPS Reader and the Sequence Number of the change. This action is independent from the notification to the DDS user and the reading of the data by the DDS user. It could have occurred before or concurrently with that.15. The State ful Writer records that the RTPS Reader has received the Cache Change and adds it to the set of a cke d changes maintained by the Reader Proxy using the a cke d changes set operation.16. The DDS user invokes the return loan operation on the DataReader to indicate that it is no longer using the data it retrieved by means of the previous take operation. This action is independent from the actions on the writer side as it is initiated by the DDS user.17. The DDS DataReader uses the remove change operation to remove the data from the History Cache.18. The remove change operation returns.19. The return loan operation returns.20. The DDS DataWriter uses the operation is a cke d by all to determine which Cache Changes have been received by all the RTPS Reader endpoints matched with the State ful Writer .21. The is a cke d by all returns and indicates that the change with the specified ‘seq_num’ Sequence Number has been acknowledged by all RTPS Reader endpoints.22. The DDS DataWriter uses the operation remove change to remove the change associated with ‘seq_num’ from the RTPS Writer’s History Cache . In doing this, the DDS DataWriter also takes into account other DDS QoS such as DURABILITY.23. The operation remove change returns.

The description above did not model some of the interactions between the DDS DataReader and the RTPS Reader ; for example, the mechanism used by the RTPS Reader to alert to the DataReader that it should call read or take to check whether new changes have been received (i.e., what causes step 10 to be taken).

Also unmodeled are some interactions between the DDS DataWriter and the RTPS Writer ; such as the mechanism used by the RTPS Writer to alert to the DataWriter that it should check whether a particular change 
has been fully acknowledged such that it can be removed from the History Cache (i.e., what causes step 20 above to be initiated).

The aforementioned interactions are not modeled because they are internal to the implementation of the middleware and have no effect on the RTPS protocol.

## 8.4.2 Behavior Required for Interoperability

This sub clause describes the requirements that all implementations of the RTPS protocol must satisfy in order to be:
- compliant with the protocol specification
- interoperable with other implementations

The scope of these requirements is limited to message exchanges between RTPS implementations by different vendors. For message exchanges between implementations by the same vendor, vendors may opt for a noncompliant implementation or may use a proprietary protocol instead.

## 8.4.2.1 General Requirements

The following requirements apply to all RTPS Entities.

##  All communications must take place using RTPS Messages

No other messages can be used than the RTPS Messages defined in 8.3. The required contents, validity and interpretation of each Message is defined by the RTPS specification.

Vendors may extend Messages for vendor specific needs using the extension mechanisms provided by the protocol (see 8.6). This does not affect interoperability.

##  All implementations must implement the RTPS Message Receiver

Implementations must implement the rules followed by the RTPS Message Receiver , as introduced in 8.3.4, to interpret Sub messages within the RTPS Message and maintain the state of the Message Receiver .

This requirement also includes proper Message formatting by preceding Entity Sub messages with Interpreter Sub messages when required for proper interpretation of the former, as defined in 8.3.7 .

The timing characteristics of all implementations must be tunable

Depending on the application requirements, deployment configuration and underlying transports, the end-user may want to tune the timing characteristics of the RTPS protocol.

Therefore, where the requirements on the protocol behavior allow delayed responses or specify periodic events, implementations must allow the end-user to tune those timing characteristics.

##  Implementations must implement the Simple Participant and Endpoint Discovery Protocols

Implementations must implement the Simple Participant and Endpoint Discovery Protocols to enable the discovery of remote Endpoints (see 8.5).

RTPS allows the use of different Participant and Endpoint Discovery Protocols, depending on the deployment needs of the application. For the purpose of interoperability, implementations must implement at least the Simple Participant Discovery Protocol and Simple Endpoint Discovery Protocol (see 8.5.1).

## 8.4.2.2 Required RTPS Writer Behavior

The following requirements apply to RTPS Writers only. Unless indicated, the requirements apply to both reliable and best-effort Writers .
Writers must not send data out-of-order

A Writer must send out data samples in the order they were added to its History Cache .

Writers must include in-line QoS values if requested by a Reader

A Writer must honor a Reader’s request to receive data messages with in-line QoS.

Writers must send periodic HEARTBEAT Messages (reliable only)

A Writer must periodically inform each matching reliable Reader of the availability of a data sample by sending a periodic HEARTBEAT Message that includes the sequence number of the available sample. If no samples are available, no HEARTBEAT Message needs to be sent.

For strict reliable communication, the Writer must continue to send HEARTBEAT Messages to a Reader until the Reader has either acknowledged receiving all available samples or has disappeared. In all other cases, the number of HEARTBEAT Messages sent can be implementation specific and may be finite.

##  Writers must eventually respond to a negative acknowledgment (reliable only)

When receiving an ACKNACK Message indicating a Reader is missing some data samples, the Writer must respond by either sending the missing data samples, sending a GAP message when the sample is not relevant, or sending a HEARTBEAT message when the sample is no longer available.

The Writer may respond immediately or choose to schedule the response for a certain time in the future. It can also coalesce related responses so there need not be a one-to-one correspondence between an ACKNACK Message and the Writer’s response. These decisions and the timing characteristics are implementation specific.

Sending Heartbeats and Gaps with Writer Group Information

A Writer belonging to a Group shall send HEARTBEAT or GAP Sub messages to its matched Readers even if the Reader has acknowledged all of that Writer’s samples. This is necessary for the Subscriber to detect the group sequence numbers that are not available in that Writer . The exception to this rule is when the Writer has sent DATA or DATA_FRAG Sub messages that contain the same information.

## 8.4.2.3 Required RTPS Reader Behavior

A best-effort Reader is completely passive as it only receives data and does not send messages itself. Therefore, the requirements below only apply to reliable Readers .

Readers must respond eventually after receiving a HEARTBEAT with final flag not set

Upon receiving a HEARTBEAT Message with final flag not set, the Reader must respond with an ACKNACK Message. The ACKNACK Message may acknowledge having received all the data samples or may indicate that some data samples are missing.

The response may be delayed to avoid message storms.

Readers must respond eventually after receiving a HEARTBEAT that indicates a sample is missing

Upon receiving a HEARTBEAT Message, a Reader that is missing some data samples must respond with an ACKNACK Message indicating which data samples are missing. This requirement only applies if the Reader can accomodate these missing samples in its cache and is independent of the setting of the final flag in the HEARTBEAT Message.

The response may be delayed to avoid message storms.

The response is not required when a liveliness HEARTBEAT has both liveliness and final flags set to indicate it is a liveliness-only message.
##  Once acknowledged, always acknowledged

Once a Reader has positively acknowledged receiving a sample using an ACKNACK Message, it can no longer negatively acknowledge that same sample at a later point.

Once a Writer has received positive acknowledgement from all Readers , the Writer can reclaim any associated resources. However, if a Writer receives a negative acknowledgement to a previously positively acknowledged sample, and the Writer can still service the request, the Writer should send the sample.

##  Readers can only send an ACKNACK Message in response to a HEARTBEAT Message

In steady state, an ACKNACK Message can only be sent as a response to a HEARTBEAT Message from a Writer . ACKNACK Messages can be sent from a Reader when it first discovers a Writer as an optimization.Writers are not required to respond to these pre-emptive ACKNACK Messages.

## 8.4.3 Implementing the RTPS Protocol

The RTPS specification states that a compliant implementation of the protocol need only satisfy the requirements presented in 8.4.2. Therefore, the behavior of actual implementations may differ as a function of the design trade-offs made by each implementation.

The Behavior Module of the RTPS specification defines two reference implementations:

Stateless Reference Implementation: The Stateless Reference Implementation is optimized for s cal ability. It keeps virtually no state on remote entities and therefore scales very well with large systems. This involves a trade-off, as improved s cal ability and reduced memory usage may require additional bandwith usage. The Stateless Reference Implementation is ideally suited for best-effort communication over multicast.

Stateful Reference Implementation: The Stateful Reference Implementation maintains full state on remote entities. This approach minimizes bandwidth usage, but requires more memory and may imply reduced s cal ability. In contrast to the Stateless Reference Implementation, it can guarantee strict reliable communication and is able to apply QoS-based or content-based filtering on the Writer side.

Both reference implementations are described in detail in the sub clauses that follow.

Actual implementations need not necessarily follow the reference implementations. Depending on how much state is maintained, implementations may be a combination of the reference implementations.

For example, the Stateless Reference Implementation maintains minimal info and state on remote Entities. As such, it is not able to perform time-based filtering on the Writer side as this requires keeping track of each remote Reader and its properties. It is also not able to drop out-of-order samples on the Reader side as this requires keeping track of the largest sequence number received from each remote Writer . Some implementations may mimic the Stateless Reference Implementation, but choose to store enough additional state to be able to avoid some of the above limitations. The required additional information can be stored in a permanent fashion, in which case the implementation approaches the Stateful Reference Implementation, or can be slowly aged and kept around on an as needed basis to approximate, to the extent possible, the behavior that would result if the state were maintained.

Regardless of the actual implementation, in order to guarantee interoperability, it is important that all implementations, including both reference implementations, satisfy the requirements presented in 8.4.2.

## 8.4.4 The Behavior of a Writer with respect to each matched Reader

The behavior of an RTPS Writer with respect to each matched Reader depends on the setting of the reliability Level attribute in the RTPS Writer and RTPS Reader . This controls whether a best-effort or a reliable protocol is used.

Not all possible combinations of the reliability Level are possible. An RTPS Writer cannot be matched to an RTPS Reader unless either the RTPS Writer has the reliability Level set to RELIABLE, or else both the RTPS 
Writer and RTPS Reader have the reliability Level set to BEST EFFORT. This is because the DDS specification states that a BEST EFFORT DDS DataWriter can only be matched with a BEST EFFORT DDS DataReader and a RELIABLE DDS DataWriter can be matched with both a RELIABLE and a BEST EFFORT DDS DataReader.

As mentioned in 8.4.3, whether a Writer can be matched to a Reader does not depend on whether both use the same implementation of the RTPS protocol. That is, a Stateful Writer is able to communicate with a Stateless Reader and vice versa.

## 8.4.5 Notational Conventions

The reference implementations are described using UML sequence charts and state-diagrams. These diagrams use some abbreviations to refer to the RTPS Entities. The abbreviations used are listed in Table 8.45.

Table 8.45 - Abbreviations used in the sequence charts and state diagrams of the Behavior Module
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/47b6517fcae31ddd34d3f2852b583958f2c7c7a5e3b580014ec9c92cdf3ccc6d.jpg)

## 8.4.6 Type Definitions

The Behavior Module introduces the following additional types.

Table 8.46 - Types definitions for the Behavior Module
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/937fc254fe7379e9308b6bf4b6de50823da5a917b7d7fa6dc05e78a6000b20f2.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/4fa735d6a7804603f081e98780b918fc6be1860a91e781c3822899609526cb29.jpg)

## 8.4.7 RTPS Writer Reference Implementations

The RTPS Writer Reference Implementations are based on specializations of the RTPS Writer class, first introduced in 8.2. This sub clause describes the RTPS Writer and all additional classes used to model the RTPS Writer Reference Implementations. The actual behavior is described in 8.4.8 and 8.4.9.

## 8.4.7.1 RTPS Writer

RTPS Writer specializes RTPS Endpoint and represents the actor that sends Cache Change messages to the matched RTPS Reader endpoints. The Reference Implementations Stateless Writer and State ful Writer specialize RTPS Writer and differ in the knowledge they maintain about the matched Reader endpoints.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/fd3479d2e03e081f41561b1df94460cf7df5e0cc9d764a3ba2d54481565e7db2.jpg)
Figure 8.15 - RTPS Writer Endpoints

Table 8.47 describes the attributes of the RTPS Writer .
Table 8.47 - RTPS Writer Attributes
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/11cc223131a2eab49e093ef5f509a18877d35c9142ee8c082455ef7586c502c8.jpg)
The attributes of the RTPS Writer allow for fine-tuning of the protocol behavior. The operations of the RTPS Writer are described in Table 8.48.
Table 8.48 - RTPS Writer operations
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/7937d4557ae70cdbbae7c72a542f8dca6f7b86d87b9cb6758741e7935f65c967.jpg)

##  Default Timing-Related Values

The following timing-related values are used as the defaults in order to facilitate ‘out-of-the-box’ interoperability between implementations.

nac k Response Delay.sec  $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$ ; nac k Response Delay.nanosec = 200 \* 1000 \* 1000; //200 milliseconds nac k Suppression Duration.sec  $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$ ; nac k Suppression Duration.nanosec $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$ ;

##  new

This operation creates a new RTPS Writer .

The newly-created writer ‘this’ is initialized as follows:

this.guid : $=$  <as specified in the constructor>;  this.uni cast Locator List : $=\zeta+\alpha s$  specified in the constructor>; this.multi cast Locator List : $=\zeta+\alpha s$  specified in the constructor>; this.reliability Level : $=\zeta+\alpha s$  specified in the constructor>;  this.topicKind : $=\zeta+\alpha s$  specified in the constructor>;  this.pushMode : $=$  <as specified in the constructor>;  this.heartbeat Period : $=\zeta+\alpha s$  specified in the constructor>; this.nac k Response Delay : $=\zeta+\alpha s$  specified in the constructor>; this.nac k Suppression Duration : $=\zeta+\alpha s$  specified in the constructor>; this.last Change Sequence Number : $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$ ; this.writer cache : $=$  new History Cache;

##  new_change

This operation creates a new Cache Change to be appended to the RTPS Writer ’s History Cache . The sequence number of the Cache Change is automatically set to be the sequence Number of the previous change plus one.

This operation returns the new change.

This operation performs the following logical steps:

++this.last Change Sequence Number; a_change : $=$  new Cache Change(kind, this.guid, this.last Change Sequence Number,
## 8.4.7.2 RTPS Stateless Writer

Specialization of RTPS Writer used for the Stateless Reference Implementation. The RTPS Stateless Writer has no knowledge of the number of matched readers, nor does it maintain any state for each matched RTPS Reader endpoint. The RTPS Stateless Writer maintains only the RTPS Reader Locator list that should be used to send information to the matched readers.

Table 8.49 - RTPS Stateless Writer attributes
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/b41cec1ef0912c3756a916b5ae2cb33c0b4def411b9cddcd1226642b38aad7e9.jpg)

The RTPS Stateless Writer is useful for situations where (a) the writer’s History Cache is small, or (b) the communication is best-effort, or (c) the writer is communicating via multicast to a large number of readers.

The virtual machine interacts with the Stateless Writer using the operations in Table 8.50

Table 8.50 - Stateless Writer operations
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/e724958c12f9c635addaa2432bd2911d2061dfb1ba678db27c37b3d6a94d73eb.jpg)

##  new

This operation creates a new RTPS Stateless Writer .

In addition to the initialization performed on the RTPS Writer super class (8.4.7.1.2), the newly-created Stateless Writer ‘this’ is initialized as follows:

##  reader locator add

This operation adds the Reader Locator a_locator to the Stateless Writer::reader locator s.
ADD a_locator TO {this.reader locator s};

##  reader locator remove

This operation removes the Reader Locator a_locator from the Stateless Writer::reader locator s.

##  unsent changes reset

This operation modifies the set of ‘unsent changes’ for all the Reader Locator s in the Stateless Writer::reader locator s. The list of unsent changes is reset to match the complete list of changes available in the writer’s History Cache . This operation is useful when called periodically to cause the Stateless Writer to keep re-sending all available changes in its History Cache .

FOREACH reader Locator in {this.reader locator s} DO       reader Locator.unsent changes : $=$  {this.writer cache.changes}

## 8.4.7.3 RTPS Reader Locator

Valuetype used by the RTPS Stateless Writer to keep track of the locators of all matching remote Readers .

Table 8.51 - RTPS Reader Locator attributes

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/727fec7e0eaaf39fc479eea9cf0a6e9ac58d667c8b62b0462ecbffd71d5b0f48.jpg)

The virtual machine interacts with the Reader Locator using the operations in Table 8.52

Table 8.52 - RTPS Reader Locator operations

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/fae21ba536a355ee3956c896f0d59b3900a5b722177de22b4a08102306888110.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/02edfa8e43c8c0c99d3a5000d6c176495115ef7a876590b07ea770cc1fe3a29b.jpg)

##  new

This operation creates a new RTPS Reader Locator . The newly-created Reader Locator ‘this’ is initialized as follows:

this.requested changes : $=$  <empty>; this.unsent changes : $=$  RTPS::Writer.writer cache.changes; this.locator : $=\zeta+\alpha s$  specified in the constructor>; this.expects Inline Qo s : $=$  <as specified in the constructor;

##  next requested change

This operation returns the Cache Change for the Reader Locator that has the lowest sequence number of the requested changes. This represents the next repair packet that should be sent to the RTPS Reader located at this Reader Locator in response to a previous AckNack message (see 8.3.7.1) from the Reader .

next seq num : $=~\mathrm{{MANN}}$  {change.sequence Number           SUCH-THAT change IN this.requested changes()};  return change IN this.requested changes()           SUCH-THAT (change.sequence Number  $==$  next seq num);

##  next unsent change

This operation returns the Cache Change for the Reader Locator that has the lowest sequence number of unsent changes. This represents the next change that should be sent to the RTPS Reader located at this Reader Locator .

next seq num : $=$  MIN { change.sequence Number           SUCH-THAT change IN this.unsent changes()};  return change IN this.unsent changes()      SUCH-THAT (change.sequence Number $==$  next seq num);

##  requested changes

This operation returns the list requested changes for this Reader Locator . This list represents the set of changes that were requested by the RTPS Readers at this Reader Locator using an ACKNACK Message.

##  requested changes set

This operation adds the set of changes with sequence numbers ‘re q seq num set’ to the requested changes list.

FOR_EACH seq_num IN re q seq num set DO FIND cache change IN RTPS::Writer.writer cache.changes SUCH-THAT (cache change.sequence Number $\==$ seq_num) ADD cache change TO this.requested changes; END

##  unsent changes

This operation returns the list unsent changes for this Reader Locator . This list represents the set of changes in the writer’s History Cache that have not been sent yet to this Reader Locator .
## 8.4.7.4 RTPS State ful Writer

Specialization of RTPS Writer used for the Stateful Reference Implementation. The RTPS State ful Writer is configured with the knowledge of all matched RTPS Reader endpoints and maintains state on each matched RTPS Reader endpoint.

By maintaining state on each matched RTPS Reader endpoint, the RTPS State ful Writer can determine whether all matched RTPS Reader endpoints have received a particular Cache Change and can be optimal in its use of network bandwidth by avoiding to send announcements to readers that have received all the changes in the writer’s History Cache . The information it maintains also simplifies QoS-based filtering on the Writer side. The attributes specific to the State ful Writer are described in Table 8.53.

Table 8.53 - RTPS State ful Writer Attributes
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/0ae9eb46a5f3ae10a7c382e643a8ab8bad9715344ad3ac94087955919576770a.jpg)

Table 8.54 - State ful Writer Operations
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/039ecd4b124696213713c61919f3ab4707db4a6f1ab217a793d6e78fdcf46527.jpg)

##  new

This operation creates a new RTPS State ful Writer . In addition to the initialization performed on the RTPS Writer super class (8.4.7.1.2), the newly-created State ful Writer ‘this’ is initialized as follows:

this.matched readers : $=$  <empty>;
##  is a cke d by all

This operation takes a Cache Change a_change as a parameter and determines whether all the Reader Proxy have acknowledged the Cache Change. The operation will return true if all Reader Proxy have acknowledged the corresponding Cache Change and false otherwise.

return true IF and only IF   FOREACH proxy IN this.matched readers     IF change IN proxy.changes for reader THEN       change.is relevant  $==$  TRUE AND change.status $==$  ACKNOWLEDGED

##  matched reader add

This operation adds the Reader Proxy a reader proxy to the set State ful Writer::matched readers.

##  matched reader remove

This operation removes the Reader Proxy a reader proxy from the set State ful Writer::matched readers.

REMOVE a reader proxy FROM {this.matched readers};  delete proxy;

##  matched reader lookup

This operation finds the Reader Proxy with GUID_t a reader gui d from the set State ful Writer::matched readers.

FIND proxy IN this.matched readers      SUCH-THAT (proxy.remote Reader Gui d $==$  a reader gui d); return proxy;

## 8.4.7.5 RTPS Reader Proxy

The RTPS Reader Proxy class represents the information an RTPS State ful Writer maintains on each matched RTPS Reader . The attributes of the RTPS Reader Proxy are described in Table 8.55.

Table 8.55 - RTPS Reader Proxy Attributes

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/1e3c6e0a66c5fdf10a609f5b455622e8762d5c5e7c9eb896cfb53a1a6c49109e.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/7982dafce6fb5b1b343905d378f7318c303bb5390bbe4226f39c9cb707721a57.jpg)

The matching of an RTPS State ful Writer with an RTPS Reader means that the RTPS State ful Writer will send the Cache Change changes in the writer’s History Cache to the matched RTPS Reader represented by the Reader Proxy . The matching is a consequence of the match of the corresponding DDS entities. That is, the DDS DataWriter matches a DDS DataReader by Topic, has compatible QoS, and is not being explicitly ignored by the application that uses DDS.

The virtual machine interacts with the Reader Proxy using the operations in Table 8.56.

Table 8.56 - Reader Proxy Operations
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/50ae726422cb087179e58d8005a28bec543850d7c107ebdebf76a0a0b3061608.jpg)

##  new

This operation creates a new RTPS Reader Proxy . The newly-created reader proxy ‘this’ is initialized as follows:

this.attributes : $=\zeta+\mathrm{a}\,s$  specified in the constructor>;  this.changes for reader : $=$  RTPS::Writer.writer cache.changes; 
FOR_EACH change IN (this.changes for reader) DO { IF ( DDS_FILTER(this, change))
 THEN change.is relevant : $=$  FALSE;  ELSE change.is relevant : $=$  TRUE;  IF ( RTPS::Writer.pushMode  $==$  true) THEN change.status : $=$  UNSENT;  ELSE change.status : $=$  UNACKNOWLEDGED; }

The above logic indicates that the newly-created Reader Proxy initializes its set of ‘changes for reader’ to contain all the Cache Changes in the Writer’s History Cache.

The change is marked as ‘irrelevant’ if the application of any of the DDS-DataReader filters indicates the change is not relevant to that particular reader. The DDS specification indicates that a DataReader may provide a time-based filter as well as a content-based filter. These filters should be applied in a manner consistent with the DDS specification to select any changes that are irrelevant to the DataReader.

The status is set depending on the value of the RTPS Writer attribute ‘ pushMode .’

##  a cke d changes set

This operation changes the Change For Reader status of a set of changes for the reader represented by Reader Proxy ‘the reader proxy.’ The set of changes with sequence number smaller than or equal to the value ‘committed seq num’ have their status changed to ACKNOWLEDGED.

FOR_EACH change in this.changes for reader SUCH-THAT (change.sequence Number  $<=$  committed seq num) DO change.status : $=$  ACKNOWLEDGED;

##  next requested change

This operation returns the Change For Reader for the Reader Proxy that has the lowest sequence number among the changes with status ‘REQUESTED.’ This represents the next repair packet that should be sent to the RTPS Reader represented by the Reader Proxy in response to a previous AckNack message (see 8.3.7.1) from the Reader .

next seq num : $=$  MIN {change.sequence Number     SUCH-THAT change IN this.requested changes()}  return change IN this.requested changes()       SUCH-THAT (change.sequence Number $==$  next seq num);

##  next unsent change

This operation returns the Cache Change for the Reader Proxy that has the lowest sequence number among the changes with status ‘UNSENT.’ This represents the next change that should be sent to the RTPS Reader represented by the Reader Proxy .

next seq num : $=$  MIN { change.sequence Number       SUCH-THAT change IN this.unsent changes() };  return change IN this.unsent changes()       SUCH-THAT (change.sequence Number  $==$  next seq num);

##  requested changes

This operation returns the subset of changes for the Reader Proxy that have status ‘REQUESTED.’ This represents the set  of changes that were requested by the RTPS Reader represented by the Reader Proxy using an ACKNACK Message.

return change IN this.changes for reader       SUCH-THAT (change.status $==$  REQUESTED); 
This operation modifies the Change For Reader status of a set of changes for the RTPS Reader represented by Reader Proxy ‘this.’ The set of changes with sequence numbers ‘re q seq num set’ have their status changed to REQUESTED.

FOR_EACH seq_num IN re q seq num set DO FIND change for reader IN this.changes for reader SUCH-THAT (change for reader.sequence Number $\widetilde{\cdot}\overline{{=}}\overline{{=}}$ seq_num) change for reader.status : $=$  REQUESTED; END

##  unsent changes

This operation returns the subset of changes for the Reader Proxy the have status ‘UNSENT.’ This represents the set of changes that have not been sent to the RTPS Reader represented by the Reader Proxy .

return change IN this.changes for reader SUCH-THAT (change.status  $==$  UNSENT);

##  una cke d changes

This operation returns the subset of changes for the Reader Proxy that have status ‘UNACKNOWLEDGED.’ This represents the set of changes that have not been acknowledged yet by the RTPS Reader represented by the Reader Proxy .

return change IN this.changes for reader       SUCH-THAT (change.status  $==$  UNACKNOWLEDGED);

## 8.4.7.6 RTPS Change For Reader

The RTPS Change For Reader is an association class that maintains information of a Cache Change in the RTPS Writer History Cache as it pertains to the RTPS Reader represented by the Reader Proxy . The attributes of the RTPS Change For Reader are described in Table 8.57.

Table 8.57 - RTPS Change For Reader Attributes
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/ff8551c3d1c9607ef856ee57899090582fdbdd9b685289c41948ac7128a53219.jpg)

## 8.4.8 RTPS Stateless Writer Behavior

## 8.4.8.1 Best-Effort Stateless Writer Behavior

The behavior of the Best-Effort RTPS Stateless Writer with respect to each Reader Locator is described in Figure 8.16.
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/a9bef505c620ce2b588a9203e4b0338ec3eb1a033570a53b1b83842dfdddc80d.jpg)
Figure 8.16 - Behavior of the Best-Effort Stateless Writer with respect to each Reader Locator

The state-machine transitions are listed in Table 8.58.

Table 8.58 - Transitions for Best-effort Stateless Writer behavior with respect to each Reader Locator

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/c169a307bc1268046ce5862e2f9939c65a700bcf1b33870f5db1928c83d8a100.jpg)

##  Transition T1

This transition is triggered by the configuration of an RTPS Best-Effort Stateless Writer ‘the rtp s writer’ with an RTPS Reader Locator . This configuration is done by the Discovery protocol (8.5) as a consequence of the discovery of a DDS DataReader that matches the DDS DataWriter that is related to ‘the rtp s writer.’

The discovery protocol supplies the values for the Reader Locator constructor parameters. The transition performs the following logical actions in the virtual machine:

a_locator : $=$  new Reader Locator( locator, expects Inline Qo s)
; the rtp s writer.reader locator add( a_locator)
;

##  Transition T2

This transition is triggered by the guard condition [RL::unsent changes( $)\!\!\stackrel{\textstyle>}{=}\!\!<\!\mathrm{emptyset}\!\!>]$  indicating that there are some changes in the RTPS Writer History Cache that have not been sent to the RTPS Reader Locator .

The transition performs no logical actions in the virtual machine.
##  Transition T3

This transition is triggered by the guard condition [RL::unsent changes $()\mathrm{==}\mathit{<}\mathrm{emptyset}\mathit{>}]$  indicating that all changes in the RTPS Writer History Cache have been sent to the RTPS Reader Locator . Note that this does not indicate that the changes have been received, only that an attempt was made to send them.

The transition performs no logical actions in the virtual machine.

##  Transition T4

This transition is triggered by the guard condition [RL::can_send() $=$ true] indicating that the RTPS Writer ‘the_writer’ has the resources needed to send a change to the RTPS Reader Locator ‘the reader locator.’

The transition performs the following logical actions in the virtual machine:

a_change : $=$  the reader locator.next unsent change(); IF a_change IN the_writer.writer cache.changes { DATA $=$ new DATA(a_change); IF (the reader locator.expects Inline Qo s) { DATA.inlineQos : $=$  the_writer.related dd s writer.qos; DATA.inlineQos $+=$ a_change.inlineQos; } DATA.readerId : $=$  ENTITY ID UNKNOWN; sendto the reader locator.locator, DATA; } ELSE { GAP $=$ new GAP(a_change.sequence Number); GAP.readerId : $=$  ENTITY ID UNKNOWN; sendto the reader locator.locator, GAP; }

After the transition, the following post-conditions hold: ( a_change BELONGS-TO the reader locator.unsent changes())
  $==$  FALSE

##  Transition T5

This transition is triggered by the configuration of an RTPS Writer ‘the rtp s writer’ to no longer send to the RTPS Reader Locator ‘the reader locator.’ This configuration is done by the Discovery protocol (8.5) as a consequence of breaking a pre-existing match of a DDS DataReader with the DDS DataWriter related to ‘the rtp s writer.’

The transition performs the following logical actions in the virtual machine:

the rtp s writer.reader locator remove(the reader locator);  delete the reader locator;

## 8.4.8.2 Reliable Stateless Writer Behavior

The behavior of the reliable RTPS Stateless Writer with respect to each Reader Locator is described in Figure 8.17.
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/43fdc7484889d1423a42b58bd5d825f809c6eb69ac4437e3ba7981fd1943b421.jpg)
Figure 8.17 - Behavior of the Reliable Stateless Writer with respect to each Reader Locator

The state-machine transitions are listed in Table 8.59.

Table 8.59 - Transitions for the Reliable Stateless Writer behavior with respect to each Reader Locator

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/4ccd711d72b0e0b70e3a483390f3e2eb67f0e46a56ee34cf6a49c931f77a997e.jpg)
##  Transition T1

This transition is triggered by the configuration of an RTPS Reliable Stateless Writer ‘the rtp s writer’ with an RTPS Reader Locator . This configuration is done by the Discovery protocol (8.5, ‘Discovery Module’) as a consequence of the discovery of a DDS DataReader that matches the DDS DataWriter that is related to ‘the rtp s writer.’

The discovery protocol supplies the values for the Reader Locator constructor parameters. The transition performs the following logical actions in the virtual machine:

a_locator : $=$  new Reader Locator( locator, expects Inline Qo s)
; the rtp s writer.reader locator add( a_locator)
;

##  Transition T2

This transition is triggered by the guard condition [RL::unsent changes $()\stackrel{!=}{\quad}<\!\mathrm{emptyset}\!\!>]$  indicating that there are some changes in the RTPS Writer History Cache that have not been sent to the Reader Locator . The transition performs no logical actions in the virtual machine.

##  Transition T3

This transition is triggered by the guard condition [RL::unsent changes $==<\!\mathrm{emptyset}\!y\!>]$  indicating that all changes in the RTPS Writer History Cache have been sent to the Reader Locator . Note that this does not indicate that the changes have been received, only that there has been an attempt made to send them. The transition performs no logical actions in the virtual machine.

##  Transition T4

This transition is triggered by the guard condition [RL::can_send() $=$ true] indicating that the RTPS Writer ‘the_writer’ has the resources needed to send a change to the RTPS Reader Locator ‘the reader locator.’

The transition performs the following logical actions in the virtual machine:

a_change : $=$ the reader locator.next unsent change(); DATA $=$ new DATA(a_change); IF (the reader locator.expects Inline Qo s) { DATA.inlineQos : $=$  the_writer.related dd s writer.qos; }  DATA.readerId : $=$  ENTITY ID UNKNOWN; sendto the reader locator.locator, DATA;

After the transition the following post-conditions hold:

( a_change BELONGS-TO the reader locator.unsent changes())
 $==$  FALSE

##  Transition T5

This transition is triggered by the firing of a periodic timer configured to fire each W::heartbeat Period.

The transition performs the following logical actions in the virtual machine for the Writer ‘the rtp s writer’ and  Reader Locator ‘the reader locator.’

seq num min : $=$  the rtp s writer.writer cache.get seq num min();  seq num max : $=$  the rtp s writer.writer cache.get seq num max(); HEARTBEAT : $=$  new HEARTBEAT(the rtp s writer.writerGuid, seq num min,                                                  seq_num_max);  HEARTBEAT.FinalFlag : $=$  SET; HEARTBEAT.readerId : $=$  ENTITY ID UNKNOWN; sendto the reader locator, HEARTBEAT;

##  Transition T6

This transition is triggered by the reception of an ACKNACK message destined to the RTPS Stateless Writer  ‘the rtp s writer’ originating from some RTPS Reader .
The transition performs the following logical actions in the virtual machine:

FOREACH reply locator t IN { Receiver.uni cast Reply Locator List,  Receiver.multi cast Reply Locator List }  reader locator : $=$  the rtp s writer.reader locator lookup(reply locator t); reader locator.requested changes set(ACKNACK.readerS N State.set);

Note that the processing of this message uses the reply locators in the RTPS Receiver . This is the only source of information for the Stateless Writer to determine where to send the reply to. Proper functioning of the protocol requires that the RTPS Reader inserts an InfoReply Submessage ahead of the AckNack such that these fields are properly set.

##  Transition T7

This transition is triggered by the guard condition [RL::requested changes( $)\stackrel{!=}{=}<\!\!\mathrm{emptyset}\!\!>]$  indicating that there are changes that have been requested by some RTPS Reader reachable at the RTPS Reader Locator . The transition performs no logical actions in the virtual machine.

##  Transition T8

This transition is triggered by the reception of an ACKNACK message destined to the RTPS Stateless Writer ‘the rtp s writer’ originating from some RTPS Reader . The transition performs the same logical actions performed by Transition T6 (8.4.8.2.6).

##  Transition T9

This transition is triggered by the firing of a timer indicating that the duration of W::nac k Response Delay has elapsed since the state must repair was entered. The transition performs no logical actions in the virtual machine.

## Transition T10

This transition is triggered by the guard condition [RL::can_send() $=$ true] indicating that the RTPS Writer ‘the_writer’ has the resources needed to send a change to the RTPS Reader Locator ‘the reader locator.’ The transition performs the following logical actions in the virtual machine.

a_change : $=$  the reader locator.next requested change(); IF a_change IN the_writer.writer cache.changes { DATA $=$ new DATA(a_change); IF (the reader locator.expects Inline Qo s) { DATA.inlineQos : $=$  the_writer.related dd s writer.qos;  DATA.inlineQos $+=$ a_change.inlineQos; }  DATA.readerId : $=$  ENTITY ID UNKNOWN; sendto the reader locator.locator, DATA; }  ELSE {  $\mathtt{G A P}\ =\ \mathtt{n e w}$  GAP(a_change.sequence Number); GAP.readerId : $=$  ENTITY ID UNKNOWN; sendto the reader locator.locator, GAP; }

After the transition the following post-conditions hold:

( a_change BELONGS-TO the reader locator.requested changes())
 $==$  FALSE

Note that it is possible that the requested change had already been removed from the History Cache by the DDS  DataWriter . In that case, the Stateless Writer sends a GAP Message.
## Transition T11

This transition is triggered by the guard condition [RL::requested changes( $)\!==\!<\!\mathrm{emptyset}\!>]$  indicating that there are no further changes requested by an RTPS Reader reachable at the RTPS Reader Locator . The transition performs no logical actions in the virtual machine.

## Transition T12

This transition is triggered by the configuration of an RTPS Writer ‘the rtp s writer’ to no longer send to the RTPS Reader Locator ‘the reader locator.’ This configuration is done by the Discovery protocol (8.5) as a consequence of breaking a pre-existing match of a DDS DataReader with the DDS DataWriter related to ‘the rtp s writer.’

The transition performs the following logical actions in the virtual machine:

the rtp s writer.reader locator remove(the reader locator);  delete the reader locator;

## 8.4.9 RTPS State ful Writer Behavior

## 8.4.9.1 Best-Effort State ful Writer Behavior

The behavior of the Best-Effort RTPS State ful Writer with respect to each matched RTPS Reader is described in Figure 8.18.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/16c81eba1a757571f21f62c5344486007ddba681f8320215cc24fd0e78c80424.jpg)
Figure 8.18 - Behavior of Best-Effort State ful Writer with respect to each matched Reader

The state-machine transitions are listed in Table 8.60.

Table 8.60 - Transitions for Best-effort Stateful Writer behavior with respect to each matched Reader

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/12e4082fdfb2d92a1dcaae43f503c9b092d0a5683aa6f39bdc386272a4467720.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/89704bb60ff1d33d583692f4080562cf9edde190cb8adf92800d68d14756c113.jpg)

##  Transition T1

This transition is triggered by the configuration of an RTPS Writer ‘the rtp s writer’ with a matching RTPS Reader . This configuration is done by the Discovery protocol (8.5) as a consequence of the discovery of a DDS DataReader that matches the DDS DataWriter that is related to ‘the rtp s writer.’

The discovery protocol supplies the values for the Reader Proxy constructor parameters. The transition performs the following logical actions in the virtual machine:

a reader proxy : $=$  new Reader Proxy( remote Reader Gui d, remote Group Entity Id, expects Inline Qo s, uni cast Locator List, multi cast Locator List); the rtp s writer.matched reader add(a reader proxy);

The Reader Proxy ‘a reader proxy’ is initialized as discussed in 8.4.7.5. This includes initializing the set of unsent changes and applying DDS_FILTER to each of the changes.

##  Transition T2

This transition is triggered by the guard condition [RP::unsent changes()  $!=$ <empty>] indicating that there are some changes in the RTPS Writer History Cache that have not been sent to the RTPS Reader represented by the Reader Proxy .

Note that for a Best-Effort Writer , W::pushMode $==$ true, as there are no acknowledgements. Therefore, the Writer always pushes out data as it becomes available.

The transition performs no logical actions in the virtual machine.

##  Transition T3

This transition is triggered by the guard condition [RP::unsent changes $()\mathrm{==}\mathit{<}\mathrm{empty>}]$  indicating that all changes in the RTPS Writer History Cache have been sent to the RTPS Reader represented by the Reader Proxy . Note that this does not indicate that the changes have been received, only that there has been an attempt made to send them.

The transition performs no logical actions in the virtual machine.

##  Transition T4

This transition is triggered by the guard condition [RP::can_send() $==$  true] indicating that the RTPS Writer ‘the rtp s writer’ has the resources needed to send a change to the RTPS Reader represented by the Reader Proxy ‘the reader proxy.’

The transition performs the following logical actions in the virtual machine:

a_change : $=$ the reader proxy.next unsent change(); a_change.status : $=$  UNDERWAY; if (a_change.is relevant) { DATA $=$ new DATA(a_change); IF (the reader proxy.expects Inline Qo s) { DATA.inlineQos : $=$  the rtp s writer.related dd s writer.qos; DATA.inlineQos $+=$ a_change.inlineQos; } 
DATA.readerId : $=$  ENTITY ID UNKNOWN; send DATA; }  else { GAP $=$ new GAP(a_change.sequence Number); GAP.readerId : $=$  ENTITY ID UNKNOWN; Send GAP; }

The above logic is not meant to imply that each DATA Submessage is sent in a separate RTPS Message. Rather multiple Sub messages can be combined into a single RTPS message.

After the transition, the following post-conditions hold:

( a_change BELONGS-TO the reader proxy.unsent changes())
 $==$ FALSE

##  Transition T5

This transition is triggered by the addition of a new Cache Change ‘a_change’ to the History Cache of the RTPS Writer ‘the rtp s writer’ by the corresponding DDS DataWriter. Whether the change is relevant to the RTPS Reader represented by the Reader Proxy ‘the reader proxy’ is determined by the DDS_FILTER.

The transition performs the following logical actions in the virtual machine:

ADD a_change TO the reader proxy.changes for reader;  IF (DDS_FILTER(the reader proxy, change)) THEN change.is relevant : $=$  FALSE; ELSE change.is relevant : $=$  TRUE;  IF (the rtp s writer.pushMode $==$  true) THEN change.status : $=$  UNSENT;    ELSE change.status : $=$  UNACKNOWLEDGED;

##  Transition T6

This transition is triggered by the configuration of an RTPS Writer ‘the rtp s writer’ to no longer be matched with the RTPS Reader represented by the Reader Proxy ‘the reader proxy’. This configuration is done by the Discovery protocol (8.5) as a consequence of breaking a pre-existing match of a DDS DataReader with the DDS DataWriter related to ‘the rtp s writer.’

The transition performs the following logical actions in the virtual machine:

the rtp s writer.matched reader remove(the reader proxy);  delete the reader proxy;

## 8.4.9.2 Reliable State ful Writer Behavior

The behavior of the Reliable RTPS State ful Writer with respect to each matched RTPS Reader is described in Figure 8.19.
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/cd856c11ad686c1b21c52393892b80b72c4d73ddbbd6b23c1ddc581616020c0a.jpg)
Figure 8.19 - Behavior of Reliable State ful Writer with respect to each matched Reader

The state-machine transitions are listed in Table 8.61.

Table 8.61 - Transitions for Reliable State ful Writer behavior with respect to each matched Reader

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/ad93c2e9c93a2d791c2b1ef38af8751f577c0479fae3727f54a886ef1408bb1b.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/9e2a79943a87f49c1aa95574e2062187b8663bb40744d1c65caf14b8bad7cb0b.jpg)

##  Transition T1

This transition is triggered by the configuration of an RTPS Reliable State ful Writer ‘the rtp s writer’ with a matching RTPS Reader . This configuration is done by the Discovery protocol (8.5) as a consequence of the discovery of a DDS DataReader that matches the DDS DataWriter that is related to ‘the rtp s writer.’

The discovery protocol supplies the values for the Reader Proxy constructor parameters. The transition performs the following logical actions in the virtual machine:

a reader proxy : $=$  new Reader Proxy( remote Reader Gui d,  remote Group Entity Id, expects Inline Qo s,  uni cast Locator List, multi cast Locator List); the rtp s writer.matched reader add(a reader proxy);

The Reader Proxy ‘a reader proxy’ is initialized as discussed in 8.4.7.5. This includes initializing the set of unsent changes and applying a filter to each of the changes.

##  Transition T2

This transition is triggered by the guard condition [RP::unsent changes()  $!=$ <empty>] indicating that there are some changes in the RTPS Writer History Cache that have not been sent to the RTPS Reader represented by the Reader Proxy .

The transition performs no logical actions in the virtual machine.

##  Transition T3

This transition is triggered by the guard condition [RP::unsent changes $()\mathrm{=<empty>}]$  indicating that all changes in the RTPS Writer History Cache have been sent to the RTPS Reader represented by the Reader Proxy . Note that this does not indicate that the changes have been received, only that there has been an attempt made to send them.

The transition performs no logical actions in the virtual machine.

##  Transition T4

This transition is triggered by the guard condition [RP::can_send() $==$  true] indicating that the RTPS Writer ‘the rtp s writer’ has the resources needed to send a change to the RTPS Reader represented by the Reader Proxy ‘the reader proxy.’ 
The transition performs the following logical actions in the virtual machine:

a_change : $=$ the reader proxy.next unsent change(); a_change.status : $=$  UNDERWAY; if (a_change.is relevant) { DATA $=$ new DATA(a_change); IF (the reader proxy.expects Inline Qo s) { DATA.inlineQos : $=$  the rtp s writer.related dd s writer.qos; DATA.inlineQos $+=$ a_change.inlineQos; }  DATA.readerId : $=$  ENTITY ID UNKNOWN; send DATA; }  else { GAP $=$ new GAP(a_change.sequence Number); GAP.readerId : $=$  ENTITY ID UNKNOWN; send GAP; }

The above logic is not meant to imply that each DATA or GAP Submessage is sent in a separate RTPS Message. Rather multiple Sub messages can be combined into a single RTPS message.

The above illustrates the simplified case where a GAP Submessage includes a single sequence number. This would result in potentially many Sub messages in cases where many sequence numbers in close proximity refer to changes that are not relevant to the Reader. Efficient implementations will combine multiple ‘irrelevant’ sequence numbers as much as possible into a single GAP message.

After the transition, the following post-conditions hold: ( a_change BELONGS-TO the reader proxy.unsent changes())
 $==$ FALSE

##  Transition T5

This transition is triggered by the guard condition [RP::una cke d changes $\begin{array}{r c l}{(\mathbf{\Delta})}&{==}&{<\tt e m p t y>]}\end{array}$  indicating that all changes in the RTPS Writer History Cache have been acknowledged by the RTPS Reader represented by the Reader Proxy .

The transition performs no logical actions in the virtual machine.

##  Transition T6

This transition is triggered by the guard condition [RP::una cke d changes()  $!=\tt<\tt e m p t y>]$  indicating that there are changes in the RTPS Writer History Cache have not been acknowledged by the RTPS Reader represented by the Reader Proxy .

The transition performs no logical actions in the virtual machine.

##  Transition T7

This transition is triggered by the firing of a periodic timer configured to fire each W::heartbeat Period.

The transition performs the following logical actions for the State ful Writer ‘the rtp s writer’ in the virtual machine:

seq num min : $=$  the rtp s writer.writer cache.get seq num min();  seq num max : $=$  the rtp s writer.writer cache.get seq num max(); HEARTBEAT : $=$  new HEARTBEAT(the rtp s writer.writerGuid,               seq_num_min, seq_num_max);  HEARTBEAT.FinalFlag : $=$  NOT_SET; HEARTBEAT.readerId : $=$  ENTITY ID UNKNOWN;  send HEARTBEAT;

##  Transition T8

This transition is triggered by the reception of an ACKNACK Message destined to the RTPS State ful Writer 
‘the rtp s writer’ originating from the RTPS Reader represented by the Reader Proxy ‘the reader proxy.’ The transition performs the following logical actions in the virtual machine:

the rtp s writer.a cke d changes set(ACKNACK.readerS N State.base - 1); the reader proxy.requested changes set(ACKNACK.readerS N State.set);

After the transition the following post-conditions hold:

MIN { change.sequence Number IN the reader proxy.una cke d changes() }  $>=$                          ACKNACK.readerSNState.base - 1

##  Transition T9

This transition is triggered by the guard condition [RP::requested changes()  $!=$  <empty>] indicating that there are changes that have been requested by the RTPS Reader represented by the Reader Proxy .

The transition performs no logical actions in the virtual machine.

## Transition T10

This transition is triggered by the reception of an ACKNACK message destined to the RTPS State ful Writer ‘the_writer’ originating from the RTPS Reader represented by the Reader Proxy ‘the reader proxy.’

The transition performs the same logical actions as Transition T8 (8.4.9.2.8).

## Transition T11

This transition is triggered by the firing of a timer indicating that the duration of W::nac k Response Delay has elapsed since the state must repair was entered.

The transition performs no logical actions in the virtual machine.

## Transition T12

This transition is triggered by the guard condition [RP::can_send() $==$  true] indicating that the RTPS Writer ‘the rtp s writer’ has the resources needed to send a change to the RTPS Reader represented by the Reader Proxy ‘the reader proxy.’

The transition performs the following logical actions in the virtual machine:

a_change : $=$ the reader proxy.next requested change(); a_change.status : $=$  UNDERWAY; if (a_change.is relevant) { DATA $=$ new DATA(a_change, the reader proxy.remote Reader Gui d); IF (the reader proxy.expects Inline Qo s) { DATA.inlineQos : $=$  the rtp s writer.related dd s writer.qos; DATA.inlineQos $+=$ a_change.inlineQos; }  send DATA; }  else { GAP $=$ new GAP(a_change.sequence Number, the reader proxy.remote Reader Gui d); send GAP; }

The above logic is not meant to imply that each DATA or GAP Submessage is sent in a separate RTPS message. Rather multiple Sub messages can be combined into a single RTPS message.

The above illustrates the simplified case where a GAP Submessage includes a single sequence number. This would result in potentially many Sub messages in cases where many sequence numbers in close proximity refer to changes that are not relevant to the Reader. Efficient implementations will combine multiple ‘irrelevant’ sequence numbers as much as possible into a single GAP message.
After the transition the following post-condition holds:

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/6c8689c126ad95adb615e522f89a8c947295dfa7acc2be8c595a8d6aaf715c4e.jpg)

## Transition T13

This transition is triggered by the guard condition [RP::requested changes $()==<\!\mathrm{emptyset}\!y\!>]$  indicating that there are no more changes requested by the RTPS Reader represented by the Reader Proxy .

The transition performs no logical actions in the virtual machine.

## Transition T14

This transition is triggered by the addition of a new Cache Change ‘a_change’ to the History Cache of the RTPS Writer ‘the rtp s writer’ by the corresponding DDS DataWriter. Whether the change is relevant to the RTPS Reader represented by the Reader Proxy ‘the reader proxy’ is determined by the DDS_FILTER.

The transition performs the following logical actions in the virtual machine:

ADD a_change TO the reader proxy.changes for reader; IF (DDS_FILTER(the reader proxy, change)) THEN a_change.is relevant : $=$  FALSE; ELSE a_change.is relevant : $=$  TRUE; IF (the rtp s writer.pushMode  $==$  true) THEN a_change.status : $=$  UNSENT;    ELSE a_change.status : $=$  UNACKNOWLEDGED;

## Transition T15

This transition is triggered by the removal of a Cache Change ‘a_change’ from the History Cache of the RTPS Writer ‘the rtp s writer’ by the corresponding DDS DataWriter. For example, when using HISTORY QoS set to KEEP_LAST with depth $==1$ , a new change will cause the DDS DataWriter to remove the previous change from the History Cache .

The transition performs the following logical actions in the virtual machine:

## Transition T16

This transition is triggered by the configuration of an RTPS Writer ‘the rtp s writer’ to no longer be matched with the RTPS Reader represented by the Reader Proxy ‘the reader proxy.’ This configuration is done by the Discovery protocol (8.5) as a consequence of breaking a pre-existing match of a DDS DataReader with the DDS DataWriter related to ‘the rtp s writer.’

The transition performs the following logical actions in the virtual machine:

## 8.4.9.3 Change For Reader illustrated

The Change For Reader keeps track of the communication status (attribute status)
 and relevance (attribute is relevant)
 of each Cache Change with respect to a specific remote RTPS Reader , identified by the corresponding Reader Proxy .

The attribute is relevant is initialized to TRUE or FALSE when the Change For Reader is created, depending on the DDS QoS and Filters that may apply. A Change For Reader that initially has is relevant set to TRUE may have the setting modified to FALSE when the corresponding Cache Change has become irrelevant for the RTPS Reader because of a later Cache Change . This can happen, for example, when the DDS QoS of the related DDS DataWriter specifies a HISTORY kind KEEP_LAST and a later Cache Change modifies the value of the same data-object (identified by the instance Handle attribute of the Cache Change)
 making the previous Cache Change irrelevant.
The behavior of the RTPS State ful Writer described in Figure 8.20 and Figure 8.21 modifies each Change For Reader as a side-effect of the operation of the protocol. To further define the protocol, it is illustrative to examine the Finite State Machine representing the value of the status attribute for any given Change For Reader . This is shown in Figure 8.22 below for a Reliable State ful Writer . A Best-Effort State ful Writer uses only a subset of the state-diagram.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/2a8fde9cd648c29aa53f702f1052309bd75fe96ffc4df38a8af33d63781b3596.jpg)

Figure 8.20 - Changes in the value of the status attribute of each Change For Reader

The states have the following meanings:
- <New> a Cache Change with Sequence Number t ‘seq_num’ is available in the History Cache of the RTPS State ful Writer but this has not been announced yet or sent to the RTPS Reader represented by the Reader Proxy .
- <Unsent> the State ful Writer has never sent a DATA or GAP with this seq_num to the RTPS Reader and it intends to do so in the future.
- <Requested> the RTPS Reader has requested via an ACKNACK message that the change is sent again. The State ful Writer intends to send the change again in the future.
- <Underway> the Cache Change has been sent and the State ful Writer will ignore new requests for this Cache Change .
- <Unacknowledged> the Cache Change should be received by the RTPS Reader , but this has not been acknowledged by the RTPS Reader . As the message could have been lost, the RTPS Reader may request the Cache Change to be sent again.
- <Acknowledged> the RTPS State ful Writer knows that the RTPS Reader has received the Cache Change with Sequence Number t ‘seq_num.’

The following describes the main events that trigger transitions in the State Machine. Note that this statemachine just keeps track of the ‘ status ’ attribute of a particular Change For Reader and does not perform any specific actions nor send any messages.
- new Change For Reader (seq_num): The Reader Proxy has created a Change For Reader association class to track the state of a Cache Change with Sequence Number t seq_num.
- [W::pushMode $=$ true]: The setting of the State ful Writer ’s attribute W::pushMode determines whether the status is changed to <Unsent> or else is changed to <Unacknowledged>. A BestEffort Writer always uses W::pushMode $==$  true.
- received NACK(seq_num): The State ful Writer has received an ACKNACK message where seq_num belongs to the ACKNACK.readerS N State, indicating the RTPS Reader has not received the Cache Change and wants the State ful Writer to send it again.
- sent DATA(seq_num) $:$  The State ful Writer has sent a DATA message containing the 
Cache Change with Sequence Number t seq_num.
- sent GAP(seq_num) $:$  The State ful Writer has sent a GAP where seq_num is in the GAP’s irrelevant sequence number list, which means that the seq_num is irrelevant to the RTPS Reader .
- received ACK(seq_num) $:$ The Writer has received an ACKNACK with ACKNACK.readerS N State.base $>$  seq_num. This means the Cache Change with sequence number seq_num has been received by the RTPS Reader .

## 8.4.10 RTPS Reader Reference Implementations

The RTPS Reader Reference Implementations are based on specializations of the RTPS Reader class, first introduced in 8.2. This sub clause describes the RTPS Reader and all additional classes used to model the RTPS Reader Reference Implementations. The actual behavior is described in 8.4.11and 8.4.12.

## 8.4.10.1 RTPS Reader

RTPS Reader specializes RTPS Endpoint and represents the actor that receives Cache Change messages from one or more RTPS Writer endpoints. The Reference Implementations Stateless Reader and State ful Reader specialize RTPS Reader and differ in the knowledge they maintain about the matched Writer endpoints.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/9658b9f472bbb177c534dd7980374ccd6eb79628a59dfb70c828cf8daee5a770.jpg)
Figure 8.21 - RTPS Reader endpoints

The configuration attributes of the RTPS Reader are listed in Table 8.62 and allow for fine-tuning of the protocol behavior. The operations on an RTPS Reader are listed in Table 8.63.
Table 8.62 - RTPS Reader configuration attributes
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/cc2d5ae77f414b630a3cbdb077ea595b5be3d5fedf7b51ef8de685d707e85539.jpg)

Table 8.63 - RTPS Reader operations
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/1d8799a0fd587c43ffd79fb69e4fae88c576449a03b5b443b5ab43885e415990.jpg)

## Default Timing-Related Values

The following timing-related values are used as the defaults in order to facilitate ‘out-of-the-box’ interoperability between implementations.

heartbeat Response Delay.sec $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$ ; heartbeat Response Delay.nanosec  $=\;\;500\;\;\star\;\;1000\;\;\star\;\;1000\,;\;\;//\;\;50$ 0 milliseconds heartbeat Suppression Duration.sec  $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$ ; heartbeat Suppression Duration.nanosec  $\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\qquad\$ ;

## new

This operation creates a new RTPS Reader .

The newly-created reader ‘this’ is initialized as follows:

this.guid : $=$  <as specified in the constructor>;  this.uni cast Locator List : $=\zeta+\alpha s$  specified in the constructor>; this.multi cast Locator List : $=\zeta+\alpha s$  specified in the constructor>; this.reliability Level : $=\zeta+\alpha s$  specified in the constructor>;  this.topicKind : $=\zeta+\alpha s$  specified in the constructor>; 
this.expects Inline Qo s : $=$  <as specified in the constructor>; this.heartbeat Response Delay : $=\zeta+\alpha s$  specified in the constructor>; this.reader cache : $=$  new History Cache;

## 8.4.10.2 RTPS Stateless Reader

Specialization of RTPS Reader . The RTPS Stateless Reader has no knowledge of the number of matched writers, nor does it maintain any state for each matched RTPS Writer .

In the current Reference Implementation, the Stateless Reader does not add any configuration attributes or operations to those inherited from the Reader super class. Both classes are therefore identical. The virtual machine interacts with the Stateless Reader using the operations in Table 8.64.

Table 8.64 - Stateless Reader operations
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/81a3e3ff1bf5f5a90f1d59fde34a5472a289a330788eae68f0897be942edc924.jpg)

## new

This operation creates a new RTPS Stateless Reader . The initialization is performed as on the RTPS Reader super class (8.4.10.1.2).

## 8.4.10.3 RTPS State ful Reader

Specialization of RTPS Reader . The RTPS State ful Reader keeps state on each matched RTPS Writer . The state kept on each writer is maintained in the RTPS Writer Proxy class.

Table 8.65 - RTPS State ful Reader Attributes
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/4c9dfd527c59cb2c6f9912f04d351c8dbd896e929518cbdf7ff1f200bb078716.jpg)

The virtual machine interacts with the State ful Reader using the operations in Table 8.66.

Table 8.66 - State ful Reader Operations

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/258a89476e13046c6527ad7c416d8d2c1660bb4e899af5e0d88aae1fbc18681a.jpg)
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/a0f593948852befd5cf4a5e22a4c4e3af5d17c4e5d101b9346dd9c1b4ac0380d.jpg)

## new

This operation creates a new RTPS State ful Reader . The newly-created stateful reader ‘this’ is initialized as follows:

this.attributes : $=$  <as specified in the constructor>;  this.matched writers : $=$  <empty>;

## matched writer add

This operation adds the Writer Proxy a writer proxy to the State ful Reader::matched writers.

## matched writer remove

This operation removes the Writer Proxy a writer proxy from the set State ful Reader::matched writers.

## matched writer lookup

This operation finds the Writer Proxy with GUID_t a writer gui d from the set State ful Reader::matched writers.

FIND proxy IN this.matched writers         SUCH-THAT (proxy.remote Writer Gui d $==$  a writer gui d);  return proxy;

## 8.4.10.4 RTPS Writer Proxy

The RTPS Writer Proxy represents the information an RTPS State ful Reader maintains on each matched RTPS Writer . The attributes of the RTPS Writer Proxy are described in Table 8.67.

The association is a consequence of the matching of the corresponding DDS Entities as defined by the DDS specification, that is the DDS DataReader matching a DDS DataWriter by Topic, having compatible QoS, belonging to a common partition, and not being explicitly ignored by the application that uses DDS.

Table 8.67 - RTPS Writer Proxy Attributes

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/17b35ee0555c9aa2f3fd813cac3cd55fd3baf0ff62758ea8e537c10b781f907d.jpg)
The virtual machine interacts with the Writer Proxy using the operations in Table 8.68.
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/ebefab61d19393b73fb6dfbd09053439cc80cbf31c3747a0efef6077afe0d2bf.jpg)

Table 8.68 - Writer Proxy Operations
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/2f5cf1373394c3fb60a029ffd98604477829df90e956d31b8c4f394195c6aa5f.jpg)
## new

This operation creates a new RTPS Writer Proxy .

The newly-created writer proxy ‘this’ is initialized as follows:

this.attributes : $=\zeta+\alpha s$  specified in the constructor>;  this.changes from writer : $=\ \varsigma a1.1$  past and future samples from the writer>;

The changes from writer of the newly-created Writer Proxy is initialized to contain all past and future samples from the Writer represented by the Writer Proxy . This is a conceptual representation only, used to describe the Stateful Reference Implementation. The Change From Writer status of each Cache Change in changes from writer is initialized to UNKNOWN, indicating the State ful Reader initially does not know whether any of these changes actually already exist. As discussed in 8.4.12.3, the status will change to RECEIVED or MISSING as the State ful Reader receives the actual changes or is informed about their existence via a HEARTBEAT message.

## available changes max

This operation returns the maximum Sequence Number t among the changes from writer changes in the RTPS Writer Proxy that are available for access by the DDS DataReader.

The condition to make any Cache Change ‘a_change’ available for ‘access’ by the DDS DataReader is that there are no changes from the RTPS Writer with Sequence Number t smaller than or equal to a_change.sequence Number that have status MISSING or UNKNOWN. In other words, the available changes max and all previous changes are either RECEIVED or LOST.

Logical action in the virtual machine:

seq_num : $=$  MAX { change.sequence Number SUCH-THAT ( change IN this.changes from writer    AND ( change.status $==$  RECEIVED  OR change.status  $==$  LOST))
 }; return seq_num;

## irrelevant change set

This operation modifies the status of a Change From Writer to indicate that the Cache Change with the Sequence Number t ‘a_seq_num’ is irrelevant to the

RTPS Reader . Logical action in the virtual machine:

FIND change FROM this.changes from writer SUCH-THAT (change.sequence Number  $==$  a_seq_num); change.status : $=$  RECEIVED; change.is relevant : $=$  FALSE;

## lost changes update

This operation modifies the status stored in Change From Writer for any changes in the Writer Proxy whose status is MISSING or UNKNOWN and have sequence numbers lower than ‘first available seq num.’ The status of those changes  is modified to LOST indicating that the changes are no longer available in the Writer History Cache of the RTPS Writer represented by the RTPS Writer Proxy .

Logical action in the virtual machine:

FOREACH change IN this.changes from writer SUCH-THAT ( change.status $==$  UNKNOWN OR change.status  $==$  MISSING    AND seq_num  $<$  first available seq num)
 DO { change.status : $=$  LOST; }
## missing changes

This operation returns the subset of changes for the Writer Proxy that have status ‘MISSING.’ The changes with status ‘MISSING’ represent the set of changes available in the History Cache of the RTPS Writer represented by the RTPS Writer Proxy that have not been received by the RTPS Reader .

return { change IN this.changes from writer SUCH-THAT change.status  $==$  MISSING};

## missing changes update

This operation modifies the status stored in Change From Writer for any changes in the Writer Proxy whose status is UNKNOWN and have sequence numbers smaller or equal to ‘last available seq num.’ The status of those changes is modified from UNKNOWN to MISSING indicating that the changes are available at the Writer History Cache of the RTPS Writer represented by the RTPS Writer Proxy but have not been received by the RTPS Reader .

Logical action in the virtual machine:

FOREACH change IN this.changes from writer     SUCH-THAT ( change.status $==$  UNKNOWN AND seq_num  $<=$  last available seq num)
 DO {  change.status : $=$  MISSING; }

## received change set

This operation modifies the status of the Change From Writer that refers to the Cache Change with the

Sequence Number t ‘a_seq_num.’ The status of the change is set to ‘RECEIVED,’ indicating it has been received. Logical action in the virtual machine:

FIND change FROM this.cha;nge s from writer      SUCH-THAT change.sequence Number $==$  a_seq_num;  change.status : $=$  RECEIVED

## 8.4.10.5 RTPS Change From Writer

The RTPS Change From Writer is an association class that maintains information of a Cache Change in the RTPS Reader History Cache as it pertains to the RTPS Writer represented by the Writer Proxy .

The attributes of the RTPS Change From Writer are described in Table 8.69.

Table 8.69 - RTPS Change From Writer Attributes

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/3fa3952719ba1513a093164e01583c18d6dfb9f201d7a3b393b556b4bcaf06c0.jpg)
## 8.4.11 RTPS Stateless Reader Behavior

## 8.4.11.1 Best-Effort Stateless Reader Behavior

The behavior of the Best-Effort RTPS Stateless Reader is independent of any writers and is described in Figure 8.22.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/b9a3ade5b71fcb1e46614a7eaa22d26bb06da86f4069f2786325162ec2e47431.jpg)
Figure 8.22 - Behavior of the Best-Effort Stateless Reader

The state-machine transitions are listed in Table 8.70.

Table 8.70 - Transitions for Best-effort Stateless Reader behavior

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/ff8c80c59f893b8a24a45549976320dc96767fe7edce37683678d3eece47f868.jpg)

## Transition T1

This transition is triggered by the creation of an RTPS Stateless Reader ‘the rtp s reader.’ This is the result of the creation of a DDS DataReader as described in 8.2.9.

The transition performs no logical actions in the virtual machine.

## Transition T2

This transition is triggered by the reception of a DATA message by the RTPS Reader ‘the rtp s reader.’ The DATA message contains the change ‘a_change.’ The representation is described in 8.3.7.2.

The stateless nature of the Stateless Reader prevents it from maintaining the information required to determine the highest sequence number received so far from the originating RTPS Writer . The consequence is that in those cases the corresponding DDS DataReader may be presented duplicate or out-of order changes. Note that if the DDS DataReader is configured to order data by ‘source timestamp,’ any available data will still be presented inorder when accessing the data through the DDS DataReader.

As mentioned in 8.4.3, actual stateless implementations may try to avoid this limitation and maintain this information in non-permanent fashion (using for example a cache that expires information after a certain time) to approximate, to the extent possible, the behavior that would result if the state were maintained.

The transition performs the following logical actions in the virtual machine:

a_change : $=$ new Cache Change(DATA); the rtp s reader.reader cache.add_change(a_change);

## Transition T3

This transition is triggered by the destruction of an RTPS Reader ‘the rtp s reader.’ This is the result of the 
destruction of a DDS DataReader as described in 8.2.9.

The transition performs no logical actions in the virtual machine.

## 8.4.11.2 Reliable Stateless Reader Behavior

This combination is not supported by the RTPS protocol. In order to implement the reliable protocol, the RTPS Reader must keep some state on each matched RTPS Writer .

## 8.4.12 RTPS State ful Reader Behavior

## 8.4.12.1 Best-Effort State ful Reader Behavior

The behavior of the Best-Effort RTPS State ful Reader with respect to each matched Writer is described in Figure 8.23.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/394324d27561790666719366fa60ed44c1eaa03e609f54652512749193731ee9.jpg)
Figure 8.23 - Behavior of the Best-Effort State ful Reader with respect to each matched Writer

The state-machine transitions are listed in Table 8.71.

Table 8.71 - Transitions for Best-Effort State ful Reader behavior with respect to each matched writer

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/2f601ca6c1509b5fe2a71b3cfdfc26883f0823cc8cec0291b96a80f2b83ebd00.jpg)

## Transition T1

This transition is triggered by the configuration of an RTPS Reader ‘the rtp s reader’ with a matching RTPS Writer . This configuration is done by the Discovery protocol (8.5) as a consequence of the discovery of a DDS DataWriter that matches the DDS DataReader that is related to ‘the rtp s reader.’

The discovery protocol supplies the values for the Writer Proxy constructor parameters. The transition performs the following logical actions in the virtual machine: 
multi cast Locator List); the rtp s reader.matched writer add(a writer proxy);

The Writer Proxy is initialized with all past and future samples from the Writer as discussed in 8.4.10.4.

## Transition T2

This transition is triggered by the reception of a DATA message by the RTPS Reader ‘the rtp s reader.’ The DATA message contains the change ‘a_change.’ The representation is described in 8.3.7.2.

The Best-Effort reader checks that the sequence number associated with the change is strictly greater than the highest sequence number of all changes received in the past from this RTPS Writer (WP::available changes max()). If this check fails, the RTPS Reader discards the change. This ensures that there are no duplicate changes and no out-of-order changes.

The transition performs the following logical actions in the virtual machine:

a_change : $=$ new Cache Change(DATA); writer gui d : $=$  {Receiver.Source Gui d Prefix, DATA.writerId};  writer proxy : $=$  the rtp s reader.matched writer lookup(writer gui d); expected seq num : $=$  writer proxy.available changes max()  $+\_1$ ;  if ( a_change.sequence Number $>=$  expected seq num)
 { the rtp s reader.reader cache.add_change(a_change); writer proxy.received change set(a_change.sequence Number);    if ( a_change.sequence Number $>$  expected seq num)
 { writer proxy.lost changes update(a_change.sequence Number); }  }

After the transition the following post-conditions hold:

writer proxy.available changes max()  $>=$  a_change.sequence Number

## Transition T3

This transition is triggered by the configuration of an RTPS Reader ‘the rtp s reader’ to no longer be matched with the RTPS Writer represented by the Writer Proxy ‘ the writer proxy.’ This configuration is done by the Discovery protocol (8.5) as a consequence of breaking a pre-existing match of a DDS DataWriter with the DDS DataReader related to ‘the rtp s reader.’

The transition performs the following logical actions in the virtual machine:

the rtp s reader.matched writer remove(the writer proxy);  delete the writer proxy;

## Transition T4

This transition is triggered by reception of a GAP message destined to the RTPS State ful Reader ‘the_reader’ originating from the RTPS Writer represented by the Writer Proxy ‘the writer proxy’.

The transition performs the following logical actions in the virtual machine:

FOREACH seq_num IN [GAP.gapStart, GAP.gapList.base-1] DO { the writer proxy.irrelevant change set(seq_num); } FOREACH seq_num IN GAP.gapList DO { the writer proxy.irrelevant change set(seq_num); }

## 8.4.12.2 Reliable State ful Reader Behavior

The behavior of the Reliable RTPS State ful Reader with respect to each matched RTPS Writer is described in Figure 8.24.
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/874500eba2236200130bfa9c4cd4b71d4c9a030ed26b0b276ef9ea3f308ef42f.jpg)
Figure 8.24 - Behavior of the Reliable State ful Reader with respect to each matched Writer

The state-machine transitions are listed in Table 8.72.

Table 8.72 - Transitions for Reliable reader behavior with respect to a matched writer
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/f1626a2513b0fc20181c1422e65fc0619e9e7467c272bf7cc4985bdca8d2303f.jpg)
## Transition T1

This transition is triggered by the configuration of an RTPS Reliable State ful Reader ‘the rtp s reader’ with a matching RTPS Writer . This configuration is done by the Discovery protocol (8.5) as a consequence of the discovery of a DDS DataWriter that matches the DDS DataReader that is related to ‘the rtp s reader.’

The discovery protocol supplies the values for the Writer Proxy constructor parameters. The transition performs the following logical actions in the virtual machine:

a writer proxy : $=$  new Writer Proxy(remote Writer Gui d, remote Group Entity Id, uni cast Locator List, multi cast Locator List); the rtp s reader.matched writer add(a writer proxy);

The Writer Proxy is initialized with all past and future samples from the Writer as discussed in 8.4.10.4.

## Transition T2

This transition is triggered by the reception of a HEARTBEAT message destined to the RTPS State ful Reader ‘the_reader’ originating from the RTPS Writer represented by the Writer Proxy ‘the writer proxy.’

The transition performs no logical actions in the virtual machine. Note however that the reception of a HEARTBEAT message causes the concurrent transition T7 (8.4.12.2.7), which performs logical actions.

## Transition T3

This transition is triggered by the guard condition [W::missing changes $)\!=\!<\!\mathrm{emptyset}\!>]$  indicating that all changes known to be in the History Cache of the RTPS Writer represented by the Writer Proxy have been received by the RTPS Reader .

The transition performs no logical actions in the virtual machine.

## Transition T4

This transition is triggered by the guard condition [W::missing changes( $)\!\!=\!\!<\!\!\mathrm{emptyset}\!\!>\!\!]$  indicating that there are some changes known to be in the History Cache of the RTPS Writer represented by the Writer Proxy , which have not been received by the RTPS Reader .

The transition performs no logical actions in the virtual machine.

## Transition T5

This transition is triggered by the firing of a timer indicating that the duration of R::heartbeat Response Delay has elapsed since the state must send a ck was entered.

The transition performs the following logical actions for the Writer Proxy ‘the writer proxy’ in the virtual machine:

missing seq num set.base : $=$  the writer proxy.available changes max() + 1; missing seq num set.set : $=$  <empty>; FOREACH change IN the writer proxy.missing changes() DO    ADD change.sequence Number TO missing seq num set.set; send ACKNACK(missing seq num set);

The above logical action does not express the fact that the PSM mapping of the ACKNACK message will be limited in its capacity to contain sequence numbers. In the case where the ACKNACK message cannot accommodate the complete list of missing sequence numbers it should be constructed such that it contains the subset with smaller value of the sequence number.

## Transition T6

Similar to T1 (8.4.12.2.1), this transition is triggered by the configuration of an RTPS Reliable State ful Reader 
‘the rtp s reader’ with a matching RTPS Writer .

The transition performs no logical actions in the virtual machine.

## Transition T7

This transition is triggered by the reception of a HEARTBEAT message destined to the RTPS State ful Reader ‘the_reader’ originating from the RTPS Writer represented by the Writer Proxy ‘the writer proxy.’

The transition performs the following logical actions in the virtual machine:

the writer proxy.missing changes update(HEARTBEAT.lastSN); the writer proxy.lost changes update(HEARTBEAT.firstSN);

## Transition T8

This transition is triggered by the reception of a DATA message destined to the RTPS State ful Reader ‘the_reader’ originating from the RTPS Writer represented by the Writer Proxy ‘the writer proxy.’

The transition performs the following logical actions in the virtual machine:

a_change : $=$ new Cache Change(DATA); the_reader.reader cache.add_change(a_change); the writer proxy.received change set(a_change.sequence Number);

Any filtering is done when accessing the data using the DDS DataReader read or take operations, as described in 8.2.9.

## Transition T9

This transition is triggered by the reception of a GAP message destined to the RTPS State ful Reader ‘the_reader’ originating from the RTPS Writer represented by the Writer Proxy ‘the writer proxy.’

The transition performs the following logical actions in the virtual machine:

FOREACH seq_num IN [GAP.gapStart, GAP.gapList.base-1] DO { the writer proxy.irrelevant change set(seq_num); }  FOREACH seq_num IN GAP.gapList DO { the writer proxy.irrelevant change set(seq_num); }

##  Transition T10

This transition is triggered by the configuration of an RTPS Reader ‘the rtp s reader’ to no longer be matched with the RTPS Writer represented by the Writer Proxy ‘the writer proxy.’ This configuration is done by the Discovery protocol (8.5) as a consequence of breaking a pre-existing match of a DDS DataWriter with the DDS DataReader related to ‘the rtp s reader.’

The transition performs the following logical actions in the virtual machine:

the rtp s reader.matched writer remove(the writer proxy);  delete the writer proxy;

## 8.4.12.3 Change From Writer illustrated

The Change From Writer keeps track of the communication status (attribute status)
 and relevance (attribute is relevant)
 of each Cache Change with respect to a specific remote RTPS Writer .

The behavior of the RTPS State ful Reader described in Figure 8.24 modifies each Change From Writer as a side-effect of the operation of the protocol. To further define the protocol, it is illustrative to examine the State Machine representing the value of the status attribute for any given Change From Writer . This is shown in Figure 8.25 for a Reliable State ful Reader . A Best-Effort State ful Reader uses only a subset of the statediagram.
![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/46147c9153a586ebe784bf9cbb3bf341a084cbe39a0ab8d45a98a316be264811.jpg)

Figure 8.25 - Changes in the value of the status attribute of each Change From Writer

The states have the following meanings:
- <Unknown>: A Cache Change with Sequence Number t seq_num may or may not be available yet at the RTPS Writer .
- <Missing>: The Cache Change with Sequence Number t seq_num is available in the RTPS Writer and has not been received yet by the RTPS Reader .
- <Requested>: The Cache Change with Sequence Number t seq_num was requested from the RTPS Writer , a response might be pending or underway.
- <Received>: The Cache Change with Sequence Number t seq_num was received: as a DATA if the seq_num is relevant to the RTPS Reader or as a GAP if the seq_num is irrelevant.
- <Lost>: The Cache Change with Sequence Number t seq_num is no longer available at the RTPS Writer . It will not be received.

The following describes the main events that trigger transitions in the State Machine. Note that this statemachine just keeps track of the ‘ status ’ attribute of a particular Change For Reader and does not perform any specific actions nor send any messages.
- new Change From Writer(seq_num): The Writer Proxy has created a Change From Writer association class to track the state of a Cache Change with Sequence Number t seq_num.
- received H $\mathrm{B(firstSN<=sea q\_num<=last N)}$ : The Reader has received a HEARTBEAT with HEARTBEAT.firstSN $<=$  seq_num $<=$  HEARTBEAT.lastSN, indicating a Cache Change with that sequence number is available from the RTPS Writer .
- sent NACK(seq_num) $:$  The Reader has sent an ACKNACK message containing the seq_num inside the ACKNACK.readerS N State, indicating the RTPS Reader has not received the Cache Change and is requesting it is sent again.
- received GAP(seq_num) $:$  The Reader has received a GAP message where seq_num is inside GAP.gapList, which means that the seq_num is irrelevant to the RTPS Reader .
- received DATA(seq_num) $:$  The Reader has received a DATA message with DATA.sequence Number $==$  seq_num.
- received HB(firstSN $>$  seq_num) $:$  The Reader has received a HEARTBEAT with HEARTBEAT.firstSN $>$  seq_num, indicating the Cache Change with that sequence number is no longer available from the RTPS Writer .

## 8.4.13 Writer Liveliness Protocol

The DDS specification requires the presence of a liveliness mechanism. RTPS realizes this requirement with the Writer Liveliness Protocol. The Writer Liveliness Protocol defines the required information exchange between two Participants in order to assert the liveliness of Writers contained by the Participants .

All implementations must support the Writer Liveliness Protocol in order to be interoperable.
## 8.4.13.1 General Approach

The Writer Liveliness Protocol uses pre-defined built-in Endpoints. The use of built-in Endpoints means that once a Participant knows of the presence of another Participant , it can assume the presence of the built-in Endpoints made available by the remote Participant and establish the association with the locally matching built-in Endpoints.

The protocol used to communicate between built-in Endpoints is the same as used for application-defined Endpoints.

## 8.4.13.2 Built-in Endpoints Required by the Writer Liveliness Protocol

The built-in Endpoints required by the Writer Liveliness Protocol are the Built in Participant Message Writer and Built in Participant Message Reader . The names of these Endpoints reflect the fact that they are general-purpose.These Endpoints are used for liveliness but can be used for other data in the future.

The RTPS Protocol reserves the following values of the EntityId_t for these built-in Endpoints: ENTITY ID P 2 P BUILT IN PARTICIPANT MESSAGE WRITER ENTITY ID P 2 P BUILT IN PARTICIPANT MESSAGE READER

The actual value for each of these EntityId_t instances is defined by each PSM.

## 8.4.13.3 Built in Participant Message Writer and Built in Participant Message Reader QoS

For interoperability, both the Built in Participant Message Writer and Built in Participant Message Reader shall use the following QoS values:
- durability.kind $=$ TRANSIENT LOCAL DURABILITY
- history.kind $=$  KEEP LAST HISTORY QO S
- history.depth $=1$

The Built in Participant Message Writer shall use reliability.kind $=$  RELIABLE RELIABILITY QO S.

The Built in Participant Message Reader may be configured to use either RELIABLE RELIABILITY QO S or BEST EFFORT RELIABILITY QO S. If the Built in Participant Message Reader is configured to use BEST EFFORT RELIABILITY QO S then the

BEST EFFORT PARTICIPANT MESSAGE DATA READER flag in Participant Proxy::built in Endpoint Qo s shall be set.

If the Participant Proxy::built in Endpoint Qo s is included in the S PDP discovered Participant Data, then the Built in Participant Message Writer shall treat the Built in Participant Message Reader as indicated by the flags. If the Participant Proxy::built in Endpoint Qo s is not included then the Built in Participant Message Writer shall treat the Built in Participant Message Reader as if it is configured with RELIABLE RELIABILITY QO S.

## 8.4.13.4 Data Types Associated with Built-in Endpoints used by Writer Liveliness Protocol

Each RTPS Endpoint has a History Cache that stores changes to the data-objects associated with the Endpoint .This is also true for the RTPS built-in Endpoints . Therefore, each RTPS built-in Endpoint depends on some DataType that represents the logical contents of the data written into its History Cache .

Figure 8.26 defines the Participant Message Data datatype associated with the RTPS built-in Endpoint for the DC PS Participant Message Topic.

![](https://cdn-mineru.openxlab.org.cn/model-mineru/prod/b4353e268968c38605265b2d765c56beda74c0076ff8df1e6d9e10bd20c175b6.jpg)
## 8.4.13.5 Implementing Writer Liveliness Protocol Using the Built in Participant Message Writer and Builtin- Participant Message Reader

The liveliness of a subset of Writers belonging to a Participant is asserted by writing a sample to the Built in Participant Message Writer . If the Participant contains one or more Writers with a liveliness of AUTOMATIC LIVELINESS QO S, then one sample is written at a rate faster than the smallest lease duration among the Writers sharing this QoS. Similarly, a separate sample is written if the Participant contains one or more Writers with a liveliness of MANUAL BY PARTICIPANT LIVELINESS QO S at a rate faster than the smallest lease duration among these Writers . The two instances are orthogonal in purpose so that if a Participant contains Writers of each of the two liveliness kinds described, two separate instances must be periodically written. The instances are distinguished using their DDS key, which is comprised of the participant Gui d Prefix and kind fields. Each of the two types of liveliness QoS handled through this protocol will result in a unique kind field and therefore form two distinct instances in the History Cache .

In both liveliness cases the participant Gui d Prefix field contains the Gui d Prefix t of the Participant that is writing the data (and therefore asserting the liveliness of its Writers)
.

The DDS liveliness kind MANUAL BY TOPIC LIVELINESS QO S is not implemented using the  Built in Participant Message Writer and Built in Participant Message Reader . It is discussed in 8.7.2.2.3.

## 8.4.14 Optional Behavior

This sub clause describes optional features of the RTPS protocol. Optional features may not be supported by all RTPS implementations. An optional feature does not affect basic interoperability, but is only available if all implementations involved support it.

## 8.4.14.1 Large Data

As described in 7.6, RTPS poses very few requirements on the underlying transport. It is sufficient that the transport offers a connection less service capable of sending packets best-effort.

That said, a transport may impose its own limitations. For example, it may limit the maximum packet size (e.g., 64K for UDP) and hence the maximum RTPS Submessage size. This mainly affects the Data Submessage, as it limits the maximum size of the serialized Data or also, the maximum serialized size of the data type used.

In order to address this limitation, 8.3.7 introduces the following Sub messages to enable fragmenting large data:
- DataFrag
- Heartbeat Frag
- NackFrag

The following sub clauses list the corresponding behavior required for interoperability.

## How to select the fragment size

The fragment size is determined by the Writer and must meet the following requirements:
- All transports available to the Writer must be able to accommodate DataFrag Sub messages containing at least one fragment. This means the transport with the smallest maximum message size determines the fragment size.
- The fragment size must be fixed for a given Writer and is identical for all remote Readers . By fixing the fragment size, the data a fragment number refers to does not depend on a particular remote Reader .This simplifies processing negative acknowledgements ( NackFrag)
 from a Reader .
- The fragment size must satisfy: fragment size $<=65536$  bytes.

Note the fragment size is determined by all  transports available to the Writer , not simply the subset of transports required to reach all currently known Readers . This ensures newly discovered Readers, regardless of the 
transport they can be reached on, can be accommodated without having to change the fragment size, which would violate the above requirements.

## How to send fragments

If fragmentation is required, a Data Submessage is replaced by a sequence of DataFrag Sub messages. The protocol behavior for sending DataFrag Sub messages matches that for sending regular Data Sub messages with the following additional requirements:
- DataFrag Sub messages are sent in order, where ordering is defined by increasing fragment numbers.Note this does not guarantee in order arrival.
- Data must only be fragmented if required. If multiple transports are available to the Writer and some transports do not require fragmentation, a regular Data Submessage must be sent on those transports instead. Likewise, for variable size data types, a regular Data Submessage must be used if fragmentation is not required for a particular sequence number.
- For a given sequence number, if in-line QoS parameters are used, they must be included with the first DataFrag Submessage (containing the fragment with fragment number equal to 1). They may also be included with subsequent DataFrag sub messages for this sequence number, but this is not required.

If a transport can accommodate multiple fragments of the given fragment size, it is recommended that implementations concatenate as many fragments as possible into a single DataFrag message.

When sending multiple DataFrag messages, flow control may be required to avoid flooding the network.Possible approaches include a leaky bucket or token bucket flow control scheme. This is not part of the RTPS specification.

## How to re-assemble fragments

DataFrag Sub messages contain all required information to re-assemble the serialized data. Once all fragments have been received, the same protocol behavior applies as for a regular Data Submessage.

Note that implementations must be able to handle out-of-order arrival of DataFrag sub messages.

## Reliable Communication

The protocol behavior for reliably sending DataFrag Sub messages matches that for sending regular Data Sub messages with the following additional requirements:
- The semantics for a Heartbeat Submessage remains unchanged: A Heartbeat message must only include those sequence numbers for which all fragments are available.
- The semantics for an AckNack Submessage remain unchanged: an AckNack message must only positively acknowledge a sequence number when all fragments were received for that sequence number. Likewise, a sequence number must be negatively acknowledged only when all fragments are missing.
- In order to negatively acknowledge a subset of fragments for a given sequence number, a NackFrag Submessage must be used. When data is fragmented, a Heartbeat may trigger both AckNack and NackFrag Sub messages.

Additional considerations:
- As mentioned above, a Heartbeat Submessage can only include a sequence number once all fragments for that sequence number are available. If a Writer wants to inform a Reader on the partial availability of fragments for a given sequence number, a Heartbeat Frag Submessage can be used - A NackFrag Submessage can only be sent in response to a Heartbeat or Heartbeat Frag submessage.

## 8.4.15 Implementation Guidelines

The contents of this sub clause are not part of the formal specification of the protocol. The purpose of this sub clause is to provide guidelines for high-performance implementations of the protocol.

## 8.4.15.1 Implementation of Reader Proxy and Writer Proxy

The PIM models the Reader Proxy as maintaining an association with each Cache Change in the Writer’s History Cache . This association is modeled as being mediated by the association class Change For Reader . The direct implementation of this model would result in a lot of information being maintained for each Reader Proxy . In practice, what is required is that the Reader Proxy is able to implement the operations used by the protocol and this does not require the use of explicit associations.

For example, the operations unsent changes() and next unsent change() can be implemented by having the Reader Proxy maintain a single sequence number ‘ highest Seq Num Sent . ’ The highest Seq Num Sent would record the highest value of the sequence number of any Cache Change sent to the Reader Proxy . Using this the operation unsent changes() could be implemented by looking up all changes in the History Cache and selecting the ones with sequence Number greater than highest Seq Num Sent . The implementation of next unsent change() would also look at the History Cache and return the Cache Change that has the next-highest sequence number greater than highest Seq Num Sent . These operations could be done efficiently if the History Cache maintains an index by sequence Number .

The same techniques can be used to implement, requested changes (), requested changes set (), and next requested change (). In this case, the implementation can maintain a sliding window of sequence numbers (which can be efficiently represented by a Sequence Number t lowest Requested Change and a fixed-length bitmap) to store whether a particular sequence number is currently requested. Requests that do not fit in the window can be ignored as they correspond to sequence numbers higher than the ones in the window and the reader can be relied on resending the request later if it is still missing the change.

Similar techniques can be used to implement a cke d changes set () and una cke d changes ().

## 8.4.15.2 Efficient use of Gap and AckNack Sub messages

Both Gap and AckNack Sub messages are designed such that they can contain information about a set of sequence numbers. For simplicity, the virtual machine used in the protocol description did not always attempt to fully use these Sub messages to store all the sequence numbers for which they would apply. The result would be that sometimes multiple Gap or AckNack messages would be sent when, a more efficient implementation, would have combined these Sub messages into a single one. All these implementations are compliant with the protocol and interoperable. However, implementations that combine multiple Gap and AckNack Sub messages and take advantage of the ability of these Sub messages to contain a set of sequence number will be more efficient in both bandwidth and CPU usage.

## 8.4.15.3 Coalescing multiple Data Sub messages

The RTPS protocol allows multiple Sub messages to be coalesced into a single RTPS message. This means that they will all share a single RTPS Header and be sent in a single ‘network-transport transaction.’ Most networktransports have a relatively-large fixed overhead compared with the extra cost of additional bytes in the message. Therefore, implementations that combine Sub messages into a single RTPS message will in general make better utilization of CPU and bandwidth.

A particularly common case is the coalescing of multiple Data Sub messages into a single RTPS message. The need for this can occur in a response to an AckNack requesting multiple changes or as a result of multiple changes made on the writer side that have not yet been propagated to the reader. In all these cases, it is generally beneficial to coalesce the Sub messages into fewer RTPS messages.
Note that the coalescing of Data Sub messages is not restricted to Sub messages originating from the same RTPS Writer. It is also possible to coalesce Sub messages originating from multiple RTPS Writer entities. RTPS Writer entities that correspond to DDS DataWriter entities belonging to the same DDS Publisher are prime candidates for this.

## 8.4.15.4 Piggybacking HeartBeat Sub messages

The RTPS protocol allows Sub messages of different kinds to be coalesced into a single RTPS message. A particularly useful case is the piggybacking of HeartBeat Sub messages following Data Sub messages. This allows the RTPS Writer to explicitly request an acknowledgment of the changes it sent without the additional traffic needed to send a separate HeartBeat .

## 8.4.15.5 Sending to unknown readerId

As described in the Messages Module, it is possible to send RTPS Messages where the readerId is left unspecified (ENTITY ID UNKNOWN). This is required when sending these Messages over Multicast, but also allows to send a single Message over unicast to reach multiple Readers within the same Participant .Implementations are encouraged to use this feature to minimize bandwidth usage.

## 8.4.15.6 Reclaiming Finite Resources from Unresponsive Readers

An implementation likely has finite resources to work with. For a Writer , reclaiming queue resources should happen when all Readers have acknowledged a sample in the queue and resources limits dictate that the old sample entry is to be used for a new sample.

There may be scenarios where an alive Reader becomes unresponsive and will never acknowledge the Writer .Instead of blocking on the unresponsive Reader , the Writer should be allowed to deem the Reader as ‘Inactive’ and proceed in updating its queue. The state of a Reader is either Active or Inactive. Active Readers have sent ACKNACKs that have been recently received. The Writer should determine the inactivity of a Reader by using a mechanism based on the rate and number of ACKNACKs received. Then samples that have been acknowledged by all Active Readers can be freed, and the Writer can reclaim those resources if necessary. Note that strict reliability is not guaranteed when a Reader becomes Inactive.

## 8.4.15.7 Setting Count in Heartbeat, Heartbeat Frag, AckNack, and NackFrag sub messages

The Count element of a HEARTBEAT differentiates between logical HEARTBEATs. A received HEARTBEAT with the same Count as a previously received HEARTBEAT can be ignored to prevent triggering a duplicate repair session. So, an implementation should ensure that same logical HEARTBEATs are tagged with the same Count.

The HEARTBEATS received by a Reader should have Counts greater than all older HEARTBEATs from the same Writer . Otherwise they can be discarded. As long as this requirement is met, it is up to the implementation to decide whether a Writer keeps a Count specific to each Reader or the Count is shared among all of its matching Readers . The same logic applies for Counts of ACKNACKs. It is up to the implementation to decide whether a Reader keeps a Count specific to each Writer or if it is shared among all of its matching Writers .

The Count element should be incremented and compared according to modular arithmetic rules in order to accommodate the integer overflow.
