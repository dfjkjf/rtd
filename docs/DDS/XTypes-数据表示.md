---
layout: default
title: XTypes规范-数据表示
nav_order: 4
---

## 7.4 数据表示

数据表示模块规定了将给定类型的数据对象进行外部化处理的方式，以便将其存储在文件中或通过网络进行传输。这通常也被称为“数据序列化”或“数据编组”。

数据表示有多种用途，例如：

- 将数据表示为“字节流”，以便通过网络发送

- 将数据表示为“字节流”，以便存储在文件中

- 将数据表示为人类可读的形式，以便向用户显示

- 为用户提供一种语言，以便向工具输入数据值或在文件中指定这些值

<img src="https://cdn.noedgeai.com/0196d72e-acdc-7687-8b04-f51e81c0a388_1.jpg?x=206&y=444&w=1381&h=935&r=0"/>

图 23 - 数据表示概念模型

本规范引入了多种数据表示方式。定义多种类型表示的原因是，每种表示方式都更适合或针对特定目的进行了优化。这些表示方式大多是等效的。因此，除了便利性或性能之外，选择使用一种表示方式而非另一种的理由很少。

替代表示方式总结在表 30 中。

表 30 – 替代数据表示方式

<table><tbody><tr><td>数据表示</td><td>使用它的原因</td><td>缺点</td></tr><tr><td>扩展CDR（通用数据表示，Common Data Representation），涵盖“传统”CDR和参数化CDR</td><td>紧凑高效的二进制表示。最大限度减少CPU和带宽使用。支持类型演变。是现有的国际对象管理组织（OMG）标准。（来自公共对象请求代理体系结构（CORBA）的传统CDR [CDR]；来自实时发布订阅协议（RTPS）的参数化CDR [RTPS]。）已在数据分发服务（DDS）互操作性协议中使用。</td><td>不可供人类阅读。</td></tr><tr><td>可扩展标记语言（XML）</td><td>可供人类阅读 可使用标准工具轻松解析和转换</td><td>耗费大量CPU 占用的空间比CDR多10到20倍</td></tr></tbody></table>

<!-- Media -->

### 7.4.1 扩展CDR表示法（编码版本1）

本规范定义了对象管理组织（OMG）CDR表示法 [CDR] 的扩展，该扩展能够同时处理可选成员以及可追加/可变类型。这些扩展产生了两种编码格式：PLAIN_CDR和PL_CDR。

对于所有基本类型和不可变构造类型（其中“传统”CDR表示法已有明确定义），这两种编码格式均采用OMG CDR表示法：

- PLAIN_CDR对CDR进行了扩展，以处理可选成员、位掩码和映射。

- PL_CDR利用实时发布订阅协议（RTPS）参数列表编码来处理可变类型。

#### 7.4.1.1 PLAIN_CDR编码

PLAIN_CDR编码应用于最终类型和可追加类型，包括（显然）基本类型。它还应用于所有字符串、序列和映射类型。声明为可变的聚合类型应使用第7.4.1.2节中描述的PL_CDR编码。

PLAIN_CDR编码基于传统的CDR表示格式 [CDR]，并进行了以下所述的最小扩展，以处理本规范引入的新类型和概念。

[RTPS] 规范规定，在序列化数据子消息元素之后，应添加填充字节，以便下一个子消息从相对于RTPS消息起始位置的4字节偏移处开始。本XTYPES规范进一步要求，在序列化数据末尾添加的任何填充字节都应设置为零。

#### 7.4.1.1.1 基本类型

基本类型的PLAIN_CDR编码应与“传统”CDR [CDR] 中的编码相同。具体而言：

- 序列化数据应在与基本类型大小对齐的偏移处进行编码。

- 如果本地系统的字节序与当前XCDR流（XCDR.cendian）中配置的字节序不同，则应进行字节序字节交换。

<!-- Media -->

下表31总结了各种基本类型的序列化。

表31 - 版本1编码中基本类型的序列化

<table><tbody><tr><td>基本类型</td><td>编码大小</td><td>对齐方式（版本1）</td><td>字节表示</td></tr><tr><td>字节</td><td>1</td><td>1</td><td>字节值</td></tr><tr><td>布尔型</td><td>1</td><td>1</td><td>0表示假，1表示真</td></tr><tr><td>8位字符类型（Char8）</td><td>1</td><td>1</td><td>按照7.2.2.2.1.2中描述进行编码的字符值</td></tr><tr><td>16位字符类型（Char16）</td><td>2</td><td>2</td><td>按照7.2.2.2.1.2中描述进行编码的字符值</td></tr><tr><td>8位有符号整数（Int8） 8位无符号整数（UInt8）</td><td>1</td><td>1</td><td>使用补码表示法的整数值</td></tr><tr><td>16位有符号整数（Int16） 16位无符号整数（UInt16）</td><td>2</td><td>2</td><td>使用补码表示法的整数值</td></tr><tr><td>32位有符号整数（Int32） 32位无符号整数（UInt32）</td><td>4</td><td>4</td><td>使用补码表示法的整数值</td></tr><tr><td>64位有符号整数（Int64） 64位无符号整数（UInt64）</td><td>8</td><td>8</td><td>使用补码表示法的整数值</td></tr><tr><td>32位浮点数（Float32）</td><td>4</td><td>4</td><td>标准化单精度浮点数的IEEE标准 [IEEE - 748]</td></tr><tr><td>64位浮点数（Float64）</td><td>8</td><td>8</td><td>标准化双精度浮点数的IEEE标准 [IEEE - 748]</td></tr><tr><td>128位浮点数（Float128）</td><td>16</td><td>8</td><td>标准化四精度浮点数的IEEE标准 [IEEE - 748]</td></tr></tbody></table>

<!-- Media -->

#### 7.4.1.1.2 字符数据

Char8 类型的对象不应被解释为具有特定编码，并且应像字节（Byte）基本类型一样按原样序列化。

String<Char8> 类型的对象应使用 UTF - 8 字符编码表示。String<Char8> 类型对象的序列化长度应为 CDR 缓冲区中 String<Char8> 字符所占用的字节数，包括终止空字符（NUL）。序列化长度可能与 Unicode 字符数不同，因为使用 UTF - 8 编码的单个 Unicode 字符可能占用一到四个字节。

String<Char16> 类型的对象应使用 UTF - 16 字符编码表示。String<Char16> 类型对象的序列化长度应为 CDR 缓冲区中 String<Char16> 字符所占用的字节数。这是字符串中字符数的两倍，因为使用 UTF - 16 编码的单个字符（在基本多文种平面中）序列化需要 2 个字节。

String<Char16> 类型对象的 UTF - 16 表示不应包含字节顺序标记（Byte Order Mark，BOM）。该表示也不应包含任何终止空字符。

原理：通过将序列化长度设置为表示形式所支持的字节数，可以发送基本多文种平面（BMP）之外的 UTF - 16 编码的 Unicode 字符（映射到两个 UTF - 16 单元）。在这种情况下，序列化长度仍将指示到字符串末尾的字节数。UTF - 16 表示所使用的字节顺序可以从 RTPS 封装标识符中已有的字节顺序推断得出（见第 7.6.3.1.2 条），因此不需要 BOM。最后，使用空字符终止 UTF - 16 编码的字符串不被认为是最佳实践，并且 OMG CDR 的最新版本也不这样做。

#### 7.4.1.1.3 枚举类型

#### 7.4.1.1.3.1 枚举类型

枚举类型的对象应序列化为整数，其大小取决于其关联类型的“位边界”。

<!-- Media -->

表 32 - 枚举类型的序列化

<table><tbody><tr><td>对应的基本类型</td><td>位边界</td></tr><tr><td>8位有符号整数（Int8）</td><td>1-8</td></tr><tr><td>16位有符号整数（Int16）</td><td>9-16</td></tr><tr><td>32位有符号整数（Int32）</td><td>17 - 32（32位是默认大小，并且对应此规范之前的所有枚举类型）</td></tr></tbody></table>

<!-- Media -->

#### 7.4.1.1.3.2 位掩码类型

位掩码类型的对象应根据位掩码的边界，以与以下基本类型相同的方式进行序列化：

<!-- Media -->

表 33 - 位掩码类型的序列化

<table><tbody><tr><td>边界</td><td>对应的基本类型</td></tr><tr><td>[1..8]</td><td>无符号8位整数（UInt8）</td></tr><tr><td>[9..16]</td><td>无符号16位整数（UInt16）</td></tr></tbody></table>

<table><tbody><tr><td>[17..32]</td><td>32位无符号整数（UInt32）</td></tr><tr><td>[33..64]</td><td>64位无符号整数（UInt64）</td></tr></tbody></table>

<!-- Media -->

位索引从位掩码全字节大小的最低有效位开始，从0开始计数。在位掩码的边界小于相应基本类型的位数的情况下，剩余序列化位的状态未指定，并且这些位不被视为位掩码的一部分。

#### 7.4.1.1.4 映射类型

映射类型的对象应根据以下等效的接口描述语言（IDL）进行表示：

```c
struct MapEntry_<key_type>_<value_type>[_<bound>] {
	<key_type> key;
	<value_type> value;
};

typedef sequence<MapEntry_<key_type>_<value_type>[_<bound>][, <bound>]>
Map_<key_type>_<value_type>[_<bound>];
```
<key_type> 和 <value_type> 名称的定义与类型系统中的定义相同。另请参阅第7.2.2.4.3节，该节定义了集合类型的隐式名称。

例如，以下IDL映射类型的对象：

```c
map<long, float>
```
...应按照它们是以下IDL序列类型的对象进行序列化：

```c
struct MapEntry_Int32_Float32 {
	long key;
	float value;
};

typedef sequence<MapEntry_Int32_Float32> Map_Int32_Float32;
```

#### 7.4.1.1.5 结构体

结构体类型的对象应按照CDR规范 [CDR] 所定义的方式表示，并按以下描述进行扩充。

#### 7.4.1.1.5.1 继承

如果存在基类型定义的成员，则这些成员应在其派生类型的成员之前进行序列化。其表示方式应完全等同于所有成员都按相同顺序在最派生类型中定义的情况。

#### 7.4.1.1.5.2 可选成员

标记为可选的结构成员之前应加上一个参数头，具体描述见下面的7.4.1.2节“参数化CDR”。

#### 7.4.1.2 参数化CDR编码

参数化CDR编码基于 [RTPS] 中定义的RTPS参数列表CDR编码。

参数列表数据结构中的每个元素或参数只是一个CDR封装的数据块。每个元素之前都有一个参数头，该参数头由一个两字节的参数ID和一个两字节的参数长度组成。一个参数接着一个参数，直到遇到列表终止标记。

与 [RTPS] 子条款9.4.2.11“参数列表”中所述不同，参数长度的值是序列化成员的精确长度。它不考虑序列化成员之后可能存在的任何填充字节。为了使下一个参数ID相对于前一个参数ID从4字节偏移处开始，可以添加填充字节。

这种数据表示使用参数列表数据结构的元素有两个目的：

- 任何可变聚合类型的对象都应序列化为一个参数列表。其每个成员应对应于该列表中的一个参数。

- 最终或可追加结构的任何可选成员之前都应有一个描述该成员的参数头。如果该成员在特定对象中没有取值，则参数头指示的数据长度应为零。对参数头数据结构的这种重用并不构成一个完整的参数列表：可选成员之后不应跟有列表终止标记。

#### 7.4.1.2.1 参数ID值的解释

如RTPS规范的9.6.2.2.1节“参数ID空间”所述，16位宽的参数ID范围可以解释为一个2位宽的位掩码，后面跟着一个14位宽的无符号整数。

- 位掩码的第一位（即整个16位宽参数ID的最高有效位）指示该参数是否具有特定于实现的解释。本规范将此位称为FLAG_IMPL_EXTENSION。

- 位掩码的第二位表示，如果使用该参数的实现无法识别该参数的ID，是可以简单地忽略该参数，还是会导致整个数据样本被丢弃。本规范将此位称为FLAG_MUST_UNDERSTAND（必须理解标志）。当且仅当被封装成员的must_understand（必须理解）属性设置为true时，才应设置此位。

在参数ID的14位宽整数区域内，本规范进一步保留最大的255个值（从16,129 (0x3F01) 到16,383 (0x3FFF)），供对象管理组织（OMG）在本规范和未来规范中使用。下表34列出了保留的参数ID值。要使一个参数被识别为表34中已知的值之一，FLAG_IMPL_EXTENSION（实现扩展标志）位必须设置为零。有关FLAG_MUST_UNDERSTAND（必须理解标志）位的值，请参考表34。

<!-- Media -->

表34 – 保留的参数ID值

<table><tbody><tr><td>名称</td><td>14位十六进制值</td><td>是否设置了MUST_UNDERSTAND标志？</td><td>描述</td></tr><tr><td>PID_EXTENDED</td><td>0x3F01</td><td>是</td><td>允许指定大的成员ID和/或数据长度值；详见下文</td></tr><tr><td>PID_LIST_END</td><td>0x3F02</td><td>是</td><td>表示参数列表数据结构的结束。实时发布订阅协议（RTPS）规定，参数ID值1应用于终止数据分发服务（DDS）内置主题数据类型中的参数列表。为避免为所有类型保留此参数ID从而使该数据表示的所有生产者和消费者的成员ID到参数ID的映射规则复杂化，简单发现类型应遵循一项特殊限制：不得使用成员ID 1，并且参数ID 1应终止参数列表以提供向后兼容性。实现还应能稳健处理接收到参数ID 0x3F02以指示列表结束的情况。这些类型包括内置主题数据类型以及那些将它们作为成员包含在内的其他类型，如[RTPS]中所定义。</td></tr><tr><td>PID_IGNORE</td><td>0x3F03</td><td>否</td><td>此数据表示的所有消费者应忽略具有此ID的参数。</td></tr><tr><td>由对象管理组织（OMG）保留</td><td>0x3F04 - 0x3FFF</td><td>不适用</td><td>由对象管理组织（OMG）保留</td></tr></tbody></table>

在写入数据时，本规范的实现应按照表34的描述设置FLAG_MUST_UNDERSTAND位。在读取数据时，本规范的实现应能适应FLAG_MUST_UNDERSTAND位的任何设置，并仍然接受该参数。

<!-- Media -->

---

<!-- Footnote -->

\( PID_IGNORE  \) 设计原理（非规范性）：实时发布订阅协议（RTPS）使用参数ID（PID）0（“PID_PAD”，对应成员ID 0）作为填充字段。PID_IGNORE将此概念应用于使用此数据表示的所有数据类型。额外保留PID 0并非必要：因为RTPS定义的类型不使用成员ID 0，这些类型的使用者自然会忽略他们遇到的与其对应的PID的任何情况。

<!-- Footnote -->

---

本规范扩展了参数列表数据结构，以允许使用32位的参数ID和长达4GB的数据长度。此扩展使用保留的必须理解的16位参数ID PID_EXTENDED来表明某个成员的参数ID和/或长度需要32位。成员ID（长成员ID）和成员长度（长成员长度）紧跟在PID_EXTENDED参数ID和附带的16位长度之后的8个字节中。

设置了必须理解标志的PID_EXTENDED的值为0x7F01（即0x4000 + 0x3F01）。

在PID_EXTENDED和长度之后的四个字节应为一个序列化的32位无符号整数（UINT32）值“eMemberHeader”，它是通过将四个1位标志与28位成员ID组合而成的。这些标志占据UINT32值的4个最高有效位。标志与成员ID的组合如下所示：

---

标志1（FLAG_1） = 0x80000000

标志2（FLAG_2） = 0x40000000

标志3（FLAG_3） = 0x20000000

标志4（FLAG_4） = 0x10000000

成员头（eMemberHeader） = 标志1（FLAG_1） & 实现扩展标志（FLAG_IMPL_EXTENSION）

+ 标志2（FLAG_2） & 必须理解标志（FLAG_MUST_UNDERSTAND）

+ 标志3（FLAG_3） & 未指定标志1（FLAG_UNSPECIFIED1）

+ 标志4（FLAG_4） & 未指定标志2（FLAG_UNSPECIFIED2）

+ 成员ID（memberId）

---

如上述公式所示，标志1（FLAG_1）编码实现扩展标志，标志2（FLAG_2）编码必须理解标志，标志3（FLAG_3）和标志4（FLAG_4）留作未来扩展使用。

在PID_EXTENDED和长度之后的第二个四个字节应被解释为一个32位无符号整数（llength），它包含序列化成员的长度。请注意，llength是序列化成员的确切长度，不包括成员之后可能存在的任何填充。

与PID_EXTENDED（slength）关联的16位长度值应等于8。

序列化成员应紧接着长成员长度（11ength）开始。即从PID_EXTENDED参数的起始位置算起正好12个字节处。有关使用PID_EXTENDED的CDR缓冲区布局示例，请参见图24。


大端表示

<img src="https://cdn.noedgeai.com/0196d72e-acdc-7687-8b04-f51e81c0a388_8.jpg?x=192&y=1838&w=1359&h=264&r=0"/>

<img src="https://cdn.noedgeai.com/0196d72e-acdc-7687-8b04-f51e81c0a388_9.jpg?x=194&y=187&w=1407&h=1084&r=0"/>

图24 - CDR缓冲区中PID_EXTENDED的使用

16位参数ID中FLAG_IMPL_EXTENSION和FLAG_MUST_UNDERSTAND位的设置不应被解释为也适用于扩展参数。相反，扩展参数头内四位标志掩码的最高有效位应表示数据成员的FLAG_IMPL_EXTENSION值。次高有效位应表示数据成员的FLAG_MUST_UNDERSTAND值。除非其他OMG规范另有规定，否则其余两位应设置为零。

基于PID_EXTENDED的这些扩展参数头，在用于序列化可变聚合类型对象的参数列表数据结构中应是合法的。如上文所述，在最终或可追加结构的可选成员之前使用时，它们也应是合法的。

扩展参数的对齐规则应与非扩展参数的对齐规则相同，后者在[RTPS]第9.4.2.11条中定义。

#### 7.4.1.2.2 成员ID到参数ID的映射

成员ID到参数的映射应如下所示：

- 从0到16,128（0x3F00）（含）的成员ID应精确表示在参数ID的低14位中。

- 所有其他成员ID必须使用扩展参数头格式表示。

- 几乎任何参数都可以合法地使用扩展参数头表示。并不要求可以用RTPS规范定义的较短头描述的参数必须以这种方式描述；如果一个参数可以使用短参数头或扩展头描述，则该头的短表示和扩展表示应被视为完全等效。这种映射确保用户定义数据类型的成员永远不会设置FLAG_IMPL_EXTENSION位。目前，FLAG_IMPL_EXTENSION位仅用于RTPS发现定义的数据类型，这些数据类型可能有也可能没有RTPS规范本身定义的位掩码。

#### 7.4.1.2.3 聚合类型成员的省略和重新排序

因为每个参数（在这种情况下是类型成员）都被明确标识，并且可变结构成员的标识是基于这些参数的ID进行的，所以可变结构的成员可以以任何顺序出现。此外，从接收方的角度来看，任何可变结构成员的值都可以省略。对于非可选成员，这可能是因为发送方具有不包含该成员的兼容类型。在这种情况下，如果接收方的成员不是可选的，则它在逻辑上采用其默认值。如果该成员是可选的，则它根本不取值。

使用参数化CDR编码数据时，编码数据应包含所有非可选成员，即使成员值与该成员的默认值匹配。这是因为数据表示的解码可能是针对不同（可赋值）的数据类型进行的。因此，在解码数据时可能会假设省略成员的默认值不同。

最终或可追加结构的对象不会作为完整的参数列表进行序列化，即使某些成员是可选的。因此，这些类型的成员不能省略或重新排序。

由于联合成员是基于判别器值来识别的，因此在序列化当前非判别器成员的值之前，必须先序列化判别器成员的值。判别器值不得省略。

#### 7.4.1.2.4 嵌套对象

在聚合可变类型的对象包含另一个聚合可变类型的对象的情况下，一个参数列表将包含另一个参数列表。在这种情况下，参数 ID 是相对于最内层的类型定义来解释的。（例如，类型 `Foo` 可能包含类型 `Bar` 的一个实例。`Foo` 和 `Bar` 都可能定义 ID 为 5 的成员。在对应于 `Bar` 对象的参数列表中，参数 ID 5 的出现应被视为引用 `Bar` 的成员 5，而不是 `Foo` 的成员 5。）

同样，PID_LIST_END 的出现表示最内层参数列表的结束。

### 7.4.2 扩展 CDR 表示法（编码版本 2）

本规范定义了三种与编码版本 2 一起使用的编码格式：PLAIN_CDR2、DELIMITED_CDR 和 PL_CDR2。

这三种编码格式利用了 PLAIN_CDR 编码。它们对版本 1 中使用的编码进行了增强，以提高类型可赋值性并减小序列化数据的大小。

- PLAIN_CDR2 应用于所有基本类型、字符串类型和枚举类型。它也用于可扩展性类型为 FINAL 的任何类型。该编码与 PLAIN_CDR 类似，不同之处在于 INT64、UINT64、FLOAT64 和 FLOAT128 被序列化到 CDR 缓冲区时的偏移量是按 4 字节对齐的，而不是像 PLAIN_CDR 那样按 8 字节对齐。

- DELIMITED_CDR 应用于可扩展性类型为 APPENDABLE 的类型。在使用 PLAIN_CDR2 序列化对象之前，它会先序列化一个 UINT32 分隔符头（DHEADER）。该分隔符对后续序列化对象的字节序和长度进行编码。

- PL_CDR2 应用于可扩展性类型为 MUTABLE 的聚合类型。与 DELIMITED_CDR 类似，它在序列化对象之前也会序列化一个 DHEADER。此外，它会在每个序列化成员之前序列化一个成员头（EMHEADER）。成员头对成员 ID、必须理解标志和后续序列化成员的长度进行编码。

### 7.4.3 扩展 CDR 编码虚拟机

编码格式是使用一个作用于 XCDR 流对象的虚拟机来指定的。XCDR 流保存了数据对象逐步序列化到流中所产生的字节。

XCDR 流模型由以下部分组成：

- 一个线性字节缓冲区，用于放置序列化的对象。

- 一组内部状态变量，这些变量可能会影响后续序列化到流中的对象的序列化。参见表 36。

- 一组对流进行的操作，这些操作会修改状态变量。参见表 37。

- 一种“流插入”操作，它以一种取决于对象类型、其组成以及状态变量值的格式将对象序列化到流中。追加操作使用运算符“<<”表示。参见表37。

#### 7.4.3.1 编码版本和格式

编码格式由编码版本和被序列化对象的可扩展性类型决定。表35规定了每种情况下应使用的格式。

<!-- Media -->

表35 - 要使用的序列化格式。

<table><tbody><tr><td>可扩展性类型</td><td>编码版本</td><td>网络传输的编码格式</td></tr><tr><td>最终版</td><td>1</td><td>普通紧凑数据表示（PLAIN_CDR）</td></tr><tr><td>最终版</td><td>2</td><td>普通紧凑数据表示2（PLAIN_CDR2）</td></tr><tr><td>可追加</td><td>1</td><td>普通紧凑数据表示（PLAIN_CDR）</td></tr><tr><td>可追加</td><td>2</td><td>定界紧凑数据表示（DELIMITED_CDR）</td></tr><tr><td>可变</td><td>1</td><td>PL紧凑数据表示（PL_CDR）</td></tr><tr><td>可变</td><td>2</td><td>PL紧凑数据表示2（PL_CDR2）</td></tr></tbody></table>

<!-- Media -->

#### 7.4.3.2 XCDR流状态

#### 7.4.3.2.1 XCDR流状态变量

XCDR流的状态由表36中定义的变量值（XCDR状态变量）描述。

<!-- Media -->

表36 - XCDR流模型中的状态变量和常量

<table><tbody><tr><td>XCDR状态变量</td><td>含义</td></tr><tr><td>本地字节序（NENDIAN）</td><td>表示系统使用的本地字节序的常量。它取决于处理器架构、编译器和操作系统。有两种可能的值：小端字节序（LITTLE ENDIAN）和大端字节序（BIG ENDIAN）</td></tr><tr><td>当前字节序（cendian）</td><td>表示当前字节序的选择变量。这是用于将后续对象序列化到流中的字节序。它会影响整数类型、浮点类型、枚举类型和Char16类型。</td></tr><tr><td>偏移量</td><td>表示字节流中下一序列化字节将放置位置的偏移量的整数变量。XCDR.offset是相对于流的起始位置计算的，因此XCDR.offset统计当前已序列化到流中的字节数。每有一个字节被序列化到流中，XCDR.offset就会递增。</td></tr><tr><td>起始位置</td><td>表示用于对齐操作的“流的逻辑起始位置”的流偏移量的整数状态变量。每种类型“T”都有一个默认对齐方式（T.dalignment）。这是该类型的对象序列化到流中时默认使用的对齐方式。类型为T的对象O应在满足以下条件的偏移量处进行序列化：((XCDR.offset - XCDR.origin) % T.dalignment) == 0如果当前的XCDR.offset不满足上述条件，则序列化过程应插入使XCDR.offset推进所需的最少“填充字节”，以满足该条件。</td></tr><tr><td>eversion</td><td>用于标识对数据流进行序列化时所使用的编码规则版本的八位字节状态变量。预定义的值如下：{0x00} -- 无版本（VERSION_NONE） {0x01} -- 版本1 {0x02} -- 版本2</td></tr><tr><td>最大对齐</td><td>表示将用于未来序列化到流中的对象的对齐方式的最大值的整数状态变量。此值会覆盖正在序列化的对象所需的对齐方式，因此任何类型为O.type的对象 \( \mathrm{O} \) 的对齐条件变为：((XCDR.offset - XCDR.origin)% MALIGN(O))== 0 其中MALIGN(O) = MIN(O.type.alignment, XCDR.maxalign) 此值会根据XCDR.eversion自动设置。XCDR.maxalign \( = \) MAXALIGN( XCDR.eversion )</td></tr></tbody></table>

<!-- Media -->

#### 7.4.3.2.2 更改XCDR流状态的操作

由于将数据对象序列化到流中，XCDR流状态会被修改。执行表37中所示的操作也可能会修改该状态。

<!-- Media -->

表37 - XCDR流模型中的流操作

<table><tbody><tr><td>XCDR流操作</td><td>含义</td></tr><tr><td>INIT(V1=<nv1>,V2=<nv2>,...)</nv2></nv1></td><td>初始化（构造）XCDR流，并按指定设置状态变量V1、V2等。符号 <\?> 表示该值可由实现方选择。</td></tr><tr><td>PUSH(VARIABLE=<newvalue>)</newvalue></td><td>将指定的XCDR流变量VARIABLE压入栈中，并将当前值设置为 <newvalue>。符号 <!--?--> 表示新值可由实现方选择。此操作可通过POP(   )操作撤销。</newvalue></td></tr><tr><td>PUSH(V1=<nv1>,V2=<nv2>,...)</nv2></nv1></td><td>对列出的变量和新值多次调用PUSH(   )的快捷方式。</td></tr>
<tr><td>弹出(变量)</td><td>用在最后一次“压入( )”操作中压入的该变量的值替换XCDR流变量“变量”，并将其从栈中移除。</td></tr><tr><td>弹出(变量1, 变量2,...)</td><td>对列出的变量多次调用“弹出( )”操作的快捷方式。</td></tr><tr><td>最大对齐(<eversion>)</eversion></td><td>此操作返回给定编码版本所使用的最大对齐方式：最大对齐 \( \left( \text{VERSION2}\right)  = 4 \) 最大对齐(版本1) = 8 最大对齐(无版本) = 8</td></tr><tr><td>对齐(数值)</td><td>此操作用于推进XCDR流，以实现XCDR偏移量的所需对齐。通过向流中插入“填充字节”来推进XCDR偏移量。填充字节的值未作具体规定。实际推进的字节数不仅取决于“数值”，还取决于XCDR最大对齐值。具体而言，流将对齐到所需对齐值：所需对齐值 = 最小值(数值, XCDR最大对齐值) 操作执行后，以下条件应成立：(XCDR偏移量 - XCDR起始位置) % 所需对齐值 == 0</td></tr><tr><td>XCDR \( <  < \{ \mathrm{O} : \mathrm{T}\} \)</td><td>“追加”流操作。从XCDR偏移量开始，将类型为“类型”的对象“对象”（使用扩展CDR表示法）序列化到XCDR流中。</td></tr></tbody></table>

#### 7.4.3.2.3 XCDR流初始化

XCDR流应使用空缓冲区进行初始化。

字节序应根据实现的需求进行设置，不过为实现最佳性能，常见的设置是使用本地系统字节序（NENDIAN）。

编码版本（eversion）应设置为DataWriter上配置的值。在本版本的DDS - XTypes规范中，它可以设置为1或2。

XCDR流中的前2个八位字节应为封装头（ENC_HEADER），用于指示顶级类型的字节序、编码版本和编码算法。参见表39。这是与DataWriter关联的类型。

#### 7.4.3.3 类型和字节转换

序列化虚拟机的操作使用一组辅助类型和字节缓冲区转换。

类型转换将一种类型转换为另一种类型，通常会修改其可扩展性类型。

字节缓冲区转换在字节数组中执行字节交换，或者允许将基本类型的对象重新解释为字节数组。

这些转换用于将一种类型的序列化分解为一组已描述过的其他类型的序列化。

表38定义了类型和字节转换。

<!-- Media -->

表38 - 序列化虚拟机中使用的类型和字节转换

<table><tbody><tr><td>类型或对象转换</td><td>含义</td></tr><tr><td>任意类型T的AsFinal(T)</td><td>此转换仅影响聚合类型。对于其他类型，AsFinal(T) 返回 T。对于受影响的类型，AsFinal(T) 是一种新类型，其声明与 T 相同，只是其可扩展性类型为 FINAL（最终）。</td></tr><tr><td>任意类型T的AsNested(T)</td><td>出于序列化目的，此转换将该类型视为嵌套类型。</td></tr><tr><td>基本类型（PRIMITIVE TYPE）的任意对象O的AsBytes(O)</td><td>此转换将基本对象重新解释为字节数组。生成的字节按照系统使用的本机字节序（NENDIAN）在处理器内存中的显示顺序排列。</td></tr><tr><td>ESWAP(B, <doit>) 其中 \( \mathrm{B} \) 是 1、2、4 或 8 字节的流</doit></td><td>根据当前 XCDR 字节序（XCDR.cendian）是否与本机字节序（NENDIAN）匹配，有条件地交换输入流 B 上的字节。如果输入是单字节或者 XCDR.cendian == NENDIAN，则此操作返回相同的输入流。否则，该操作会生成一个与输入长度相同的新字节流，并根据输入流的长度执行（字节序）字节交换：长度为 2 时：\( \{ \mathrm{B}\left\lbrack  1\right\rbrack  ,\mathrm{B}\left\lbrack  0\right\rbrack  \} \) 长度为 4 时：\( \{ \mathrm{B}\left\lbrack  3\right\rbrack  ,\mathrm{B}\left\lbrack  2\right\rbrack  ,\mathrm{B}\left\lbrack  1\right\rbrack  ,\mathrm{B}\left\lbrack  0\right\rbrack  \} \) 长度为 8 时：{ B[7], B[6], B[5], B[4], B[3], B[2], B[1], B[0] }</td></tr></tbody></table>

<!-- Media -->

#### 7.4.3.4 与数据类型和对象相关的函数

序列化虚拟机的操作使用了一组辅助函数，这些函数返回要追加到XCDR流中的字节或数据。其表示法和含义在表39中定义。

<!-- Media -->

表39 - 对对象和类型进行操作的函数

<table><tbody><tr><td>函数</td><td>含义</td></tr><tr><td>适用于任何类型 “T” 的 ENC_HEADER( <e>，<eversion>，T)</eversion></e></td><td>ENC_HEADER 是一个由 2 个八位字节组成的数组，用于标识编码（序列化）类型、编码版本（<eversion>）以及流所使用的字节序（<e>）：\( \{ 0\mathrm{x}{00},0\mathrm{x}{00}\} \) -- 普通 CDR（PLAIN_CDR），大端字节序，{0x00, 0x01} -- 普通 CDR，小端字节序 {0x00, 0x02} -- 简化 CDR（PL_CDR），大端字节序，{0x00, 0x03} -- 简化 CDR，小端字节序，{0x00, 0x10} -- 普通 CDR2（PLAIN_CDR2），大端字节序，\( \{ 0\mathrm{x}{00},0\mathrm{x}{11}\} \) -- 普通 CDR2，小端字节序 {0x00, 0x12} -- 简化 CDR2（PL_CDR2），大端字节序 {0x00, 0x13} -- 简化 CDR2，小端字节序 \( \{ 0\mathrm{x}{00},0\mathrm{x}{14}\} \) -- 定界 CDR（DELIMIT_CDR），大端字节序 \( \{ 0\mathrm{x}{00},0\mathrm{x}{15}\} \) -- 定界 CDR，小端字节序 \( \{ 0\mathrm{x}{01},0\mathrm{x}{00}\} \) - XML</e></eversion></td></tr><tr><td>适用于任何类型 (C) 的 EVERSION(T)</td><td>EVERSION 是一个八位字节，用于标识对流进行序列化时所使用的编码规则的版本。其值如下：0x00 - 未指定版本（理解为版本 1）0x01 - 版本 1 0x02 - 版本 2</td></tr></tbody></table>

<table><tbody><tr><td>任意类型为T的对象O的DHEADER(O)</td><td>一个UInt32类型的头部值，定义为：DHEADER(O) = O.ssize 其中O.ssize是头部之后用于保存\( \mathrm{O} \)的序列化表示所需的字节数。</td></tr><tr><td>EMHEADER1(M) 其中M是结构体的一个成员</td><td>EMHEADER1是增强可变头部（EMHEADER）的前4个字节，由PL_CDR2编码格式使用。它是一个UINT32值，计算方式为：EMHEADER1 \( = \left( {\mathrm{M}\_ \mathrm{{FLAG}} <  < {31}}\right)  + \left( {\mathrm{{LC}} <  < {28}}\right)  + \) M.id 其中：M FLAG是成员\( \mathrm{{LC}} \)的“必须理解”选项的值，是该成员的长度代码的值。</td></tr><tr><td>LC(M) 其中M是结构体的一个成员</td><td>LC是一个3位的长度代码，用于构造EMHEADER1。它决定了EMHEADER头部是否有额外的4个字节（NEXTINT），还用于对后续成员的序列化大小进行编码。</td></tr><tr><td>NEXTINT(M) 其中M是结构体的一个成员</td><td>NEXTINT是增强可变头部（EMHEADER）的第二个4字节。它是一个UInt32值。仅当LC(M) >= 4时才会出现NEXTINT。NEXTINT与LC结合使用，对后续成员的序列化大小进行编码。</td></tr></tbody></table>

<!-- Media -->

#### 7.4.3.4.1 分隔符头部 (DHEADER)

DELIMITED_CDR和PL_CDR编码格式在对象内容序列化之前添加一个32位无符号整数分隔符头部 (DHEADER)。

DHEADER对后续对象的序列化大小进行编码（不包括DHEADER本身）。其定义如下：

---

DHEADER(O) = O.ssize

---

在这个表达式中，O.ssize必须小于4GB（2^32字节）。

DHEADER作为Uint32类型进行序列化时，相对于XCDR.origin强制进行4字节对齐，这可能会在DHEADER之前向流中插入最多3个填充字节。

DHEADER的序列化使用其序列化时XCDR流中激活的字节序（XCDR.cendian）。

#### 7.4.3.4.2 成员头部 (EMHEADER)、长度代码 (LC) 和NEXTINT

PL_CDR2编码格式使用逐个成员的类型 - 长度编码对聚合类型进行序列化。

每个成员的序列化之前都有一个成员头部。成员头部可以是4字节或8字节。

前四个字节是一个名为EMHEADER1的32位无符号整数的序列化表示。EMHEADER1必须使用序列化发生处当前的XCDR流字节序（XCDR.cendian）进行序列化。

如果存在，第二个4字节是一个名为NEXTINT的32位无符号整数的序列化表示。它必须使用与EMHEADER1相同的字节序进行序列化。

EMHEADER1由三部分组成：必须理解标志 (M_FLAG)、长度代码 (LC) 和成员ID。

EMHEADER1 = (M_FLAG << 31) + (LC << 28) + (成员ID & 0x0fffffff)

如果接收方必须理解相应的成员，则必须理解标志 (M_FLAG) 应设置为1，请参阅第7.2.2.4.4.4.6条。否则，应将其设置为零。

长度代码提供了确定成员序列化大小的方法。有八种可能的值，范围从0到7（包括0和7，二进制表示为0b000到0b111）。这些值的解释如下：

- LC值介于0到3之间表示成员头部为4字节。也就是说，没有NEXTINT。LC的值直接编码了序列化成员的长度：

- \( \mathrm{{LC}} = 0 = 0\mathrm{\;b}{000} \) 表示序列化成员的长度为1字节

- \( \mathrm{{LC}} = 1 = 0\mathrm{\;b}{001} \) 表示序列化成员的长度为2字节

- \( \mathrm{{LC}} = 2 = 0\mathrm{\;b}{010} \) 表示序列化成员的长度为4字节

- \( \mathrm{{LC}} = 3 = 0\mathrm{\;b}{011} \) 表示序列化成员的长度为8字节

- LC值介于4到7之间表示成员头部为8字节。也就是说，第二个整数（NEXTINT）紧跟在EMHEADER1之后。LC的值与NEXTINT的值共同编码了序列化成员的长度：

- \( \mathrm{{LC}} = 4 = 0\mathrm{\;b}{100} \) 表示序列化成员的长度为NEXTINT

- \( \mathrm{{LC}} = 5 = 0\mathrm{\;b}{101} \) 表示序列化成员的长度也为NEXTINT

- \( \mathrm{{LC}} = 6 = 0\mathrm{\;b}{110} \) 表示序列化成员的长度为4 * NEXTINT

- \( \mathrm{{LC}} = 7 = 0\mathrm{\;b}{111} \) 表示序列化成员的长度为 \( 8 * \mathrm{{NEXTINT}} \)

LC值为5到7的EMHEADER1还会影响序列化/反序列化虚拟机，因为它们会使NEXTINT也作为序列化成员的一部分被复用。这很有用，因为某些成员的序列化也以一个整数字长开头，该整数字长的值与NEXTINT完全相同。因此，使用长度代码5到7可以在序列化过程中节省4字节。

#### 7.4.3.5 编码（序列化）规则

虚拟机的逻辑以一组规则的形式表达。每条规则的形式如下：

XCDR[vv] "<<" <匹配条件> "=" XCDR "<<" <序列化操作1>

“<<” <序列化操作2>

“<<” ...

XCDR表示包含对象序列化内容的流。它的状态由其状态变量表示（见第7.4.3.1节），并且它还保存了先前序列化对象的字节。[vv]表示DataWriter使用的编码版本。这是在每个DataWriter上配置的。流在初始化时会设置其编码版本，并且不能修改。

左侧为XCDR[vv]的规则仅在XCDR流使用编码版本vv时适用。左侧为XCDR的规则适用于所有xtypes编码版本。

<匹配条件>表示正在被序列化到XCDR流中的对象。

序列化对象时，将按顺序评估每个规则，并应用第一个具有匹配版本和条件的规则。

规则的应用包括执行每个序列化操作。每个操作可能会更改流的状态变量，或指示应序列化新对象（或对现有对象的修改）。这可能会递归地触发新规则的应用。

规则应一直应用到完成。完成后，XCDR流包含发起序列化的对象的序列化表示。

规则是从构建RTPS序列化数据缓冲区以进行发送的写入器的角度编写的。因此，入口点是所谓的“顶级”类型，它表示可以由DDS数据写入器发布的非嵌套类型。此入口点确保XCDR流包含DDS - RTPS协议所需的序列化数据封装头。如果只是想序列化一个对象而不将其嵌入到RTPS序列化数据中，则可以使用其他入口点。

#### 7.4.3.5.1 匹配条件使用的符号表示法

表40显示了序列化虚拟机使用的符号和表示法。

<!-- Media -->

表40 - 序列化虚拟机中使用的符号和表示法

<table><tbody><tr><td>表示法</td><td>含义</td></tr><tr><td>O : T</td><td>类型为 “T” 的对象 “O” —— O.type 是引用对象类型 “T” 的另一种方式 —— O.ssize 是在 XCDR 流中保存 \( \mathrm{O} \) 的序列化表示所需的字节大小，该 XCDR 流的 XCDR.offset 与 T.dalignment 对齐。</td></tr><tr><td>O : TOP_LEVEL_TYPE</td><td>对象 O 被序列化为顶级主题类型。即由数据写入器直接写入的对象，而非嵌套对象。</td></tr><tr><td>O : PRIMITIVE TYPE</td><td>如 7.2.2.2 中所定义的基本类型的对象 \( \mathrm{O} \)。</td></tr><tr><td>O : STRING_TYPE</td><td>如 7.2.2.4.3 中所定义的具有 Char8 元素的字符串类型的对象 \( \mathrm{O} \)。</td></tr><tr><td>O : WSTRING_TYPE</td><td>如 7.2.2.4.3 中所定义的具有 Char16 元素的字符串类型的对象 \( \mathrm{O} \)。</td></tr><tr><td>O : ENUM_TYPE</td><td>如 7.2.2.4.1 中所定义的枚举类型的对象 “O” —— O.holder 类型根据 @bit_bound 注解的值为 Int8、Int16 或 Int32。 —— O.value 是枚举的（整数）值。</td></tr><tr><td>O : BITMASK TYPE</td><td>如 7.2.2.4.1.2 中所定义的位掩码类型的对象 O —— O.holder 类型根据 @bit_bound 注解的值为 UInt8、UInt16、UInt32 或 UInt64。 —— O.value 是位掩码的（整数）值。</td></tr><tr><td>O : ALIAS_TYPE</td><td>如 7.2.2.4.2 中所定义的别名类型的对象 O —— O.base 类型是等效（别名）类型。</td></tr></tbody></table>

<table><tbody><tr><td>O：数组类型（ARRAY_TYPE）</td><td>一个数组类型的对象“O”，定义见7.2.2.4.3 - O.element_type是元素类型 - O.length是数组中元素的总数（考虑所有维度）。对于一维数组，\( \mathrm{O}\left\lbrack  \mathrm{i}\right\rbrack \)是数组中的“第i个”元素。出于序列化目的，多维数组被视为一个包含所有元素的一维数组，元素的排序方式是第一维的索引变化最慢，最后一维的索引变化最快。</td></tr><tr><td>O：最终数组类型（FARRAY_TYPE）</td><td>与数组类型（ARRAY_TYPE）相同，只是其可扩展性类型为最终（FINAL）。</td></tr><tr><td>O：基本数组类型（PARRAY TYPE）</td><td>一种元素类型为基本类型的数组类型（ARRAY TYPE）。</td></tr><tr><td>O：序列类型（SEQUENCE_TYPE）</td><td>一个序列类型的对象“O”，定义见7.2.2.4.3 - O.element_type是元素类型 - O.length是序列中元素的数量。空序列的O.length == 0。对于非空序列，\( \mathrm{O}\left\lbrack  \mathrm{i}\right\rbrack \)是序列中的“第i个”元素。序列索引从0开始，因此\( \mathrm{O}\left\lbrack  0\right\rbrack \)是序列中的第一个元素，O[O.length - 1]是序列中的最后一个元素。</td></tr><tr><td>O：基本序列类型（PSEQUENCE_TYPE）</td><td>与序列类型（SEQUENCE_TYPE）相同，只是O.element type是基本类型。从某种意义上说，这些序列具有内在的定界性，即CDR表示允许在不遍历每个元素的情况下确定整个序列的序列化大小。</td></tr><tr><td>O：最终序列类型（FSEQUENCE TYPE）</td><td>与序列类型（SEQUENCE TYPE）相同，只是其可扩展性类型为最终（FINAL）。</td></tr></tbody></table>

<table><tbody><tr><td>O：映射类型（MAP_TYPE）</td><td>一个映射类型的对象“O”，如7.2.2.4.3中所定义 - O.key_type是键类型 - O.element_type是元素类型 - O.length是映射中键的数量，这也是映射中元素的数量。对于非空映射，\( \mathrm{O}\left\lbrack  \mathrm{i}\right\rbrack \).key是映射中的第“i”个键，\( \mathrm{O}\left\lbrack  \mathrm{i}\right\rbrack \).element是对应于该键的（值）元素。映射索引从0开始，因此\( \mathrm{O}\left\lbrack  0\right\rbrack \).key是映射中的第一个键，O.key[O.length - 1]是映射中的最后一个键。</td></tr><tr><td>O：最终映射类型（FMAP_TYPE）</td><td>一种可扩展性类型为最终（FINAL）的映射类型（MAP TYPE）。</td></tr><tr><td>O：基本映射类型（PMAP_TYPE）</td><td>一种元素和键均为基本类型的映射类型（MAP TYPE）。</td></tr><tr><td>O：联合类型（UNION_TYPE）</td><td>一个联合类型的对象“O”，如7.2.2.4.4.3中所定义 - O.disc是判别成员。 - O.disc.value是判别成员的值。 - O.disc.type是判别成员的类型。 - O.selected member是根据判别成员的值选择的联合成员。请注意，某些判别值可能不选择任何成员。 - O.selected member.value是所选成员的值（如果有）。 - O.selected member.type是所选成员的类型。</td></tr><tr><td>O：最终联合类型（FUNION_TYPE）</td><td>与联合类型（UNION TYPE）相同，只是其可扩展性类型为最终（FINAL）。</td></tr></tbody></table>

<table><tbody><tr><td>O ：结构体类型（STRUCT_TYPE）</td><td>一个结构体类型的对象 “O”，定义见 7.2.2.4.4.2 - 若 O.type 继承自另一个结构体，则 O.base_type 是基础结构体的类型。 - O.member_count 是成员的数量。对于非空结构体： - O.member \( \left\lbrack  \mathrm{i}\right\rbrack \) 是结构体中的第 “i” 个成员。它是一个容器，用于存放包含成员值的对象，并包含额外信息。 - 成员索引从 0 开始，因此 \( \mathrm{O}\left\lbrack  0\right\rbrack \) 是第一个成员。参见成员（MEMBER）的定义。</td></tr><tr><td>O ：最终结构体类型（FSTRUCT_TYPE）</td><td>与结构体类型（STRUCT_TYPE）相同，只是其可扩展性类型为最终（FINAL）。</td></tr><tr><td>O ：可变结构体类型（MSTRUCT_TYPE）</td><td>与结构体类型（STRUCT_TYPE）相同，只是其可扩展性类型为可变（MUTABLE）。与最终结构体类型（FSTRUCT_TYPE）不同，O.member[i].id 是 O.member[i] 的成员 ID（MemberId），定义见 7.2.2.4.4.4，它可能与 “i” 不同。</td></tr><tr><td>M ：成员（MEMBER）</td><td>聚合类型的一个成员，见 7.2.2.4.4。 - M.id 是成员 ID。 - M.value 是存放成员值的对象。 - M.value.type 是该对象的类型。 - M.value.ssize 是存放成员值的对象的序列化大小。</td></tr><tr><td>M ：最终成员（FMEMBER）</td><td>可扩展性类型为最终（FINAL）的聚合类型的一个成员（参见成员（MEMBER））。</td></tr><tr><td>M ：可选最终成员（OPT_FMEMBER）</td><td>可扩展性类型为最终（FINAL）的聚合类型的一个可选成员（参见第 7.2.2.4.4.4.7 条）（最终成员（FMEMBER））。</td></tr><tr><td>M ：非可选最终成员（NOPT_FMEMBER）</td><td>可扩展性类型为最终（FINAL）的聚合类型的一个非可选成员（参见第 7.2.2.4.4.4.7 条）（最终成员（FMEMBER））。</td></tr><tr><td>M ：可变成员（MMEMBER）</td><td>可扩展性类型为可变（MUTABLE）的聚合类型的一个成员（参见成员（MEMBER））。</td></tr><tr><td>O ：最终类型（FINAL_TYPE）</td><td>一个可扩展性类型为最终（FINAL）的类型的对象 O。</td></tr><tr><td>O ：可追加类型（APPENDABLE_TYPE）</td><td>一个可扩展性类型为可追加（APPENDABLE）的类型的对象 \( \mathrm{O} \) 这是集合类型和结构体类型的默认类型。</td></tr></tbody></table>

<!-- Media -->

#### 7.4.3.5.2 可选成员的编码

PLAIN_CDR 通过在前面添加一个短成员头（ShortMemberHeader）或一个 12 字节的长成员头（LongMemberHeader）来序列化可选成员。请参阅第 7.4.1.1.5.2 条。如果可选成员不存在，则关联的大小设置为零；如果成员存在，则设置为实际的序列化大小。这些头文件相对于当前流原点（XCDR.origin）以 4 字节偏移量进行序列化，并将对齐原点调整为零，以便对成员本身进行序列化。

PLAIN_CDR2 和 DELIMITED_CDR 首先序列化一个布尔值（<is_present>）来指示成员是否存在，从而对可选成员进行序列化。如果成员不存在，序列化的布尔值应设置为 0；如果存在，则设置为 1。如果成员存在（<is_present> = 1），则应在 <is_present> 布尔值之后进行序列化。如果不存在，则该成员应从序列化中省略。

PL_CDR 和 PL_CDR2 对可选成员的序列化方式与常规成员相同，只是如果可选成员不存在，则相应的成员头和序列化成员将从序列化流中省略。

<!-- Media -->

#### 7.4.3.5.3 完整的序列化规则

(1) XCDR << \{O : 顶级类型（TOP_LEVEL_TYPE）\} =

XCDR

<< 初始化（INIT）( 偏移量（OFFSET）=0, 原点（ORIGIN）=0,

字节序（CENDIAN）=<E>, 版本号（EVERSION）=<eversion> )

<< \{ 编码头（ENC_HEADER）(<E>, <eversion>, O.type) : 字节[2] \}

<< 压入（PUSH）( 版本号（EVERSION） = <eversion> )

<< 压入（PUSH）( 最大对齐（MAXALIGN） = 最大对齐（MAXALIGN）(<eversion>) )

<< 压入（PUSH）( 原点（ORIGIN） = 0 )

<< \{ <选项（OPTIONS）> : 字节[2] \}

\( <  < \{ \) O \( : \) 作为嵌套（AsNested）(O.type) \( \} \)

(2) XCDR << \{O : 基本类型（PRIMITIVE_TYPE）\} =

XCDR

<< 对齐（ALIGN）( O.大小（ssize） )

<< 交换字节序(将对象O转换为字节数组(AsBytes(O)))

<!-- Media -->

(3) XCDR << \{O : 字符串类型(STRING_TYPE)\} =

XCDR

<< \( \{ \) 对象O的大小(O.ssize) \( : \) 32位无符号整数(UInt32) \( \} \; \) // 包含空字符(NUL)

<< \( \{ \mathrm{O}\left\lbrack  \mathrm{i}\right\rbrack \) : 字节(Byte) \( \} {}^{ * }\; \) // 包含空字符(NUL)

(4) XCDR << \{O : 宽字符串类型(WSTRING_TYPE)\} =

XCDR

<< \{ 对象O的大小(O.ssize) : 32位无符号整数(UInt32) \} // 不包含空字符(NUL)

<< \{ 对象O的第i个元素(O[i]) : 16位字符(Char16) \}* // 不包含空字符(NUL)

(5) XCDR << \{O : 枚举类型(ENUM_TYPE)\}

XCDR

<< \{ 对象O的值(O.value) : 对象O的持有者类型(O.holder_type) \}

(6) XCDR << \{O : 位掩码类型(BITMASK_TYPE)\} =

XCDR

<< \{ 对象O的值(O.value) : 对象O的持有者类型(O.holder_type) \}

(7) XCDR << \{O : 别名类型(ALIAS_TYPE)\} =

XCDR

\( <  < \{ O\; : O \) .基类型(base_type) \( \} \)

// 基本元素类型的数组(版本1和2的编码)

(8) XCDR << \{O : 原生数组类型(PARRAY_TYPE)\} =

XCDR

<< \{ 对象O的第i个元素(O[i]) : 对象O的元素类型(O.element_type) \}*

// 使用版本2编码的数组（任意可扩展性）

(9) XCDR[2] << \{O : 数组类型(ARRAY_TYPE)\} =

XCDR

<< \{ 数组O的头部信息(DHEADER(O)) : 32位无符号整数(UINT32) \}

<< \{ 数组O的第i个元素(O[i]) : 数组O的元素类型(O.element_type) \}*

// 使用版本1编码的数组（任意可扩展性）

(10) XCDR[1] << \{O : 数组类型(ARRAY_TYPE)\} =

XCDR

<< \{ 数组O的第i个元素(O[i]) : 数组O的元素类型(O.element_type) \}*

// 具有可扩展性“可追加(APPENDABLE)”的数组使用通用的“可追加”规则：

// (29)-(30)

// 不允许具有可扩展性“可变(MUTABLE)”的数组。将其视为“可追加”。

// 基本元素类型的序列（版本1和2编码）

(11) XCDR << \{ O : 基本元素序列类型(PSEQUENCE_TYPE) \} =

XCDR

\( <  < \{ \) 序列O的长度(O.length) : 32位无符号整数(UInt32) \( \} \)

<< \{ 序列O的第i个元素(O[i]) : 序列O的元素类型(O.element_type) \}*

// 使用版本2编码的序列（任意可扩展性）

(12) XCDR[2] << \{O : 序列类型(SEQUENCE_TYPE)\} =

XCDR

<< \{ 序列O的头部信息(DHEADER(O)) : 32位无符号整数(UINT32) \}

\( <  < \{ \) O的长度 : 无符号32位整数 \( \} \)

<< \{ O[i] : O的元素类型 \}*

// 使用版本1编码的序列（任意可扩展性）

(13) 扩展公共数据表示[1] << \{O : 序列类型\} =

## 扩展公共数据表示

<< \{ O的长度 : 无符号32位整数 \}

<< \{ O[i] : O的元素类型 \}*

// 具有可扩展性“可追加”的序列使用通用的“可追加”规则：

// (29)-(30)

// 不允许具有可扩展性“可变”的序列。视为

// 可追加。

// 基本键和元素类型的映射（版本1和2编码）

(14) 扩展公共数据表示 << \{O : 基本映射类型\} =

XCDR

<< \{ O的长度 : 无符号32位整数 \}

<< \{ (O[i]的键 : O的键类型),

(O[i]的元素 : O的元素类型) \}*

// 使用版本2编码的映射（任意可扩展性）

(15) 扩展通用数据记录[2]（XCDR[2]） << \{ O : 映射类型（MAP_TYPE） \} =

XCDR

<< \{ 数据头(O)（DHEADER(O)） : 32位无符号整数（UINT32） \}

<< \{ (O[i].键 : O.键类型),

(O[i].元素 : O.元素类型) \}*

// 使用版本1编码的映射（任何可扩展性）

(16) 扩展通用数据记录[1]（XCDR[1]） << \{O : 映射类型（MAP_TYPE）\} =

XCDR

\( <  < \{ \) O.长度 : 32位无符号整数（UInt32） \( \} \)

<< \{ (O[i].键 : O.键类型),

(O[i].元素 : O.元素类型) \}*

## // 具有可扩展性“可追加”（APPENDABLE）的映射使用通用的“可追加”规则：

// (29)-(30)

// 不允许具有可扩展性“可变”（MUTABLE）的映射。将其视为“可追加”（APPENDABLE）。

// 具有可扩展性“最终”（FINAL）的结构（版本1和2编码）

// 固定成员（FMMEBER）可以是“非可选固定成员”（NOPT_FMEMBER (18)）或“可选固定成员”（OPT_FMEMBER (19)）

(17) 扩展通用数据记录（XCDR） << \{O : 固定结构类型（FSTRUCT_TYPE）\} =

XCDR

<< \{ O.成员[i] : 固定成员（FMEMBER） \}*

// 最终聚合类型（结构、联合）的非可选成员

(18) XCDR << \{M : 非可选最终成员(NOPT_FMEMBER)\} =

XCDR

<< \{ M值 : M值的类型 \}

// 最终聚合类型（结构体、联合体）的可选成员，版本1

// 有关MMEMBER序列化，请参见(26)和(27)

(19) XCDR[1] << \{M : 可选最终成员(OPT_FMEMBER)\} =

XCDR

\( <  < \{ M : {MMEMBER}\} \)

// 最终聚合类型（结构体、联合体）的可选成员，版本2

(20) XCDR[2] << \{M : 可选最终成员(OPT_FMEMBER)\} =

XCDR

<< \{ <是否存在> : 布尔值 \}

<< 如果 (<是否存在>) \{ M值 : M值的类型 \}

// 结构体可扩展性“可追加”由通用“可追加”规则处理：

// (29)-(30) // 具有可扩展性“可变”的结构体，版本2编码

(21) XCDR[2] << \{O : 可变结构体类型(MSTRUCT_TYPE)\} =

XCDR

<< \{ 数据头(DHEADER(O)) : 32位无符号整数 \}

<< \{ O的成员[i] : 可变成员(MMEMBER) \}*

// 可变聚合类型（结构体、联合体）的成员，版本2编码

(22) XCDR[2] << \{M : 可变成员(MMEMBER)\} =

XCDR

<< { EMHEADER1(M) : 无符号32位整数 }>

<< 如果 (LC(M) >= 4) { NEXTINT(M) : 无符号32位整数 }>

<< 如果 (LC(M) >= 5) XCDR偏移量 = XCDR偏移量 - 4>

<< { M值 : M值类型 }>

// 具有可扩展性MUTABLE的结构，版本1编码

(23) XCDR[1] << {O : 可变结构类型} =

XCDR

<< { O成员[i] : 成员 }*>

<< { 进程ID哨兵 : 无符号16位整数 }>

<< \( \{ \) 长度 \( = 0 : \) 无符号16位整数 \( \} \)>

// 可变聚合类型（结构、联合）的成员，版本1编码 // 当M.id <= 2^14且M.value.ssize <= 2^16时使用短PL编码 (24) XCDR[1] << {M : 成员} =

XCDR

<< 对齐(4)>

<< { 标志I + 标志M + M.id : 无符号16位整数 }>

<< { M值的子大小 : 无符号16位整数 }>

<< 压入( 原点 = 0 )>

<< { M值 : M值类型 }>

// 可变聚合类型（结构、联合）的成员，版本1编码

// 使用长PL编码

(25) XCDR[1] << \{M : 成员类型(MMEMBER)\} =

XCDR

<< 对齐(4)

<< \{ 标志I(FLAG_I) + 标志M(FLAG_M) + 扩展PID(PID_EXTENDED) : 无符号16位整数(UInt16) \}

<< \{ 长度为8(slength=8) : 无符号16位整数(UInt16) \}

<< \{ M的ID(M.id) : 无符号32位整数(UInt32) \}

<< \{ M的值的大小(M.value.ssize) : 无符号32位整数(UInt32) \}

<< 压入(起始位置=0)(PUSH( ORIGIN=0 ))

<< \{ M的值(M.value) : M的值的类型(M.value.type) \} // 具有可扩展性的联合类型，最终版本（版本1和2编码） // 有关可选成员类型(NOPT_FMEMBER)和成员类型(FMEMBER)的序列化，请参阅(18)至(20) (26) XCDR << \{O : 函数联合类型(FUNION_TYPE)\} = XCDR << \{ O的判别式(O.disc) : 可选成员类型(NOPT_FMEMBER) \} << \{ O选择的成员(O.selected_member) : 成员类型(FMEMBER) \}? // 具有可扩展性的联合类型，可追加，由通用可追加规则处理： // (29)-(30)

// 具有可扩展性的联合类型，可变，版本2编码

// 有关使用版本2编码对成员类型(MMEMBER)进行序列化的信息，请参阅(22)

(27) XCDR[2] << \{O : 可变联合类型(MUNION_TYPE)\} =

XCDR

<< \{ 数据头(DHEADER(O)) : 无符号32位整数(UInt32) \}

\( <  < \{ \) O的判别式(O.disc) \( : \) 成员类型(MMEMBER) \( \} \)

<< \{ O选择的成员(O.selected_member) : 成员类型(MMEMBER) \}? // 具有可扩展性的联合类型，可变，版本1编码 // 有关使用版本1编码对成员类型(MMEMBER)进行序列化的信息，请参阅(25)-(26) (28) XCDR[1] << \{O : 可变联合类型(MUNION_TYPE)\} = XCDR << \{ O的判别式(O.disc) : 成员类型(MMEMBER) \} << \{ O选择的成员(O.selected_member) : 成员类型(MMEMBER) \}? << \{ 哨兵PID(PID_SENTINEL) : 无符号16位整数(UInt16) \} << \( \{ \) 长度(length) \( = 0 : \) 无符号16位整数(UInt16) \( \} \)

## // 可扩展性，可追加（集合或聚合类型），版本1

// 编码

(29) XCDR[1] << \{O : 可追加类型（APPENDABLE_TYPE）\} =

XCDR

\( <  < \{ \) O : 作为最终类型（AsFinal）(O.type) \( \} \)

// 可扩展性 可追加（集合或聚合类型），版本2

// 编码

(30) XCDR[2] << \{O : 可追加类型（APPENDABLE_TYPE）\} =

XCDR

<< \{ 数据头（DHEADER）(O) : 32位无符号整数（UInt32） \}

\( <  < \{ \) O : 作为最终类型（AsFinal）(O.type) \( \} \)

