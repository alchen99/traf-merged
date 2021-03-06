;// @@@ START COPYRIGHT @@@
;//
;//  Copyright 1998
;//  Compaq Computer Corporation
;//     Protected as an unpublished work.
;//        All rights reserved.
;//
;//  The computer program listings, specifications, and documentation
;//  herein are the property of Compaq Computer Corporation or a
;//  third party supplier and shall not be reproduced, copied,
;//  disclosed, or used in whole or in part for any reason without
;//  the prior express written permission of Compaq Computer
;//  Corporation.
;//
;// @@@ END COPYRIGHT @@@ 
;//****************************************************************************
;//*
;//*  Header of the Messages file
;//*
;//****************************************************************************

;//* MessageIdTypeDef = A symbolic name that is output as the typedef name 
;//* for each numeric MessageId value
MessageIdTypedef=DWORD	

;//* SeverityNames = Two most significant bits in the MessageId which define 
;//* the type of message
SeverityNames=	(Success=0x0 
				Informational=0x1 
				Warning=0x2 
				Error=0x3)

;//* FacilityNames = Defines the set of names that are allowed as the value 
;//* of the Facility keyword
FacilityNames=	(System=0x0FF 
;//			Application=0xFFF
			AssociationServer=0x001
			ConfigurationServer=0x002
			ODBCDriver=0x003
			ODBCServer=0x004) 

;//* LanguageNames = Defines the set of names that are allowed as the value 
;//* of the language keyword.  It maps to the message file for that language
;//* LanguageNames=(English=009:tdm_odbcDrvMsg_009)

;//**** end of header section; what follows are the message definitions *****
;//*
;//*  All error message definitions start with the keyword "MessageId"
;//*  if no value is specified, the number will be the last number used for the
;//*  facility plus one.  Instead of providing a number, we can provide +NUMBER
;//*  where number is the offset to be added to the last number used in 
;//*  the facility
;//*  MessageId numbers are limited to 16 bit values
;//*  
;//*  Severity and Facility if not specified will use the last option selected
;//*  the names used must match the names defined in the header
;//*  
;//*  SymbolicName is a symbols used to associate a C symbolic ocnstant name
;//*  with the final 32-bit message code that is the result of ORing together
;//*  the MessageId | Severity | Facility bits.  The constant definition is
;//*  output in the generated .h file
;//*
;//*  After the message definition keywords comes one or more message text
;//*  definitions.  Each message text definition starts with the "Language"
;//*  keyword that identifies which binary output file the message text is 
;//*  to be output to.  The message text is terminated by a line containing
;//*  a single period at the beginning of the line, immediately followed by
;//*  a new line.  No spaces allowed around keyword.  Within the message text,
;//*  blank lines and white space are preserved as part of the message.
;//*
;//*  Escape sequences supported in the message text:
;//*    %0 - terminates a message text line without a trailing new line 
;//*        (for prompts)
;//*    %n!printf format string! - Identifies an insert (parameter); 
;//*         n can be between 1 and 99; defaults to !s!
;//*  Inserts refer to parameters used with FormatMessage API call, 
;//*    if not passed, an error is generated
;//*    %% - a single percent sign
;//*    %n - a hard line break
;//*    %r - output a hard carriage return
;//*    %b - space in the formatted message text
;//*    %t - output a tab
;//*    %. - a single period
;//*    %! - a single exclamation point
;//*
;//*
;//*  Values are 32 bit values layed out as follows:
;//*
;//*   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
;//*   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
;//*  +---+-+-+-----------------------+-------------------------------+
;//*  |Sev|C|R|     Facility          |               Code            |
;//*  +---+-+-+-----------------------+-------------------------------+
;//*
;//*  where
;//*
;//*      Sev - is the severity code
;//*
;//*          00 - Success
;//*          01 - Informational
;//*          10 - Warning
;//*          11 - Error
;//*
;//*      C - is the Customer code flag
;//*
;//*      R - is a reserved bit
;//*
;//*      Facility - is the facility code
;//*
;//*      Code - is the facility's status code
;//*
;//*************************************************************************
;//*
;//*      Actual Messages follow this line and have the following 
;//*      structure:
;//*          Characters 1 to 5  of the Msg Text will contain SQLState
;//*          Characters 7 to 10 of the Msg Text will contain a Help ID number
;//*          Characters from 11 to the end of the Msg Text will contain the text
;//*
;//*************************************************************************


;//************************************************************************
;//*
;//*           DRIVER Error messages from the string table
;//*
;//************************************************************************
MessageID = 1
SymbolicName = IDS_01_002
Severity=Error
Facility=ODBCDriver
Language=English
01002 01001 Disconnect error. Transaction rolled back.%0
.

MessageID = 2
SymbolicName = IDS_01_004
Language=English
01004 01004 Data truncated.%0
.

MessageID = 3
SymbolicName = IDS_01_006
Language=English
01006 01006 Privilege not revoked.%0
.

MessageID = 4
SymbolicName = IDS_07_001
Language=English
07001 01008 Wrong number of parameters.%0
.

MessageID = 5
SymbolicName = IDS_07_003
Language=English
07003 01010 Dynamic SQL error.  Cursor specification cannot be executed.%0
.

MessageID = 6
SymbolicName = IDS_07_005
Language=English
07005 01012 Dynamic SQL error.  Prepared statement is not a cursor specification.%0
.

MessageID = 7
SymbolicName = IDS_07_006
Language=English
07006 01014 Restricted data type attribute violation.%0
.

MessageID = 8
SymbolicName = IDS_07_008
Language=English
07008 01016 Dynamic SQL error.  Wrong number of bound columns.%0
.

MessageID = 9
SymbolicName = IDS_08_001
Language=English
08001 01018 Unable to connect to data source. %0
.

MessageID = 10
SymbolicName = IDS_08_001_01
Language=English
08001 01018 Unable to connect to data source. The Association Service entry is not found or empty.%0
.

MessageID = 11
SymbolicName = IDS_08_001_02
Language=English
08001 01018 Unable to connect to data source. The Driver Entry is not found or empty.%0
.

MessageID = 12
SymbolicName = IDS_08_002
Language=English
08002 01020 Connection in use.%0
.

MessageID = 13
SymbolicName = IDS_08_003
Language=English
08003 01022 Connection not open.%0
.

MessageID = 14
SymbolicName = IDS_08_004
Language=English
08004 01024 Data source rejected establishment of connection for implementation defined reasons.%0
.

MessageID = 15
SymbolicName = IDS_08_005
Language=English
08005 01026 Communication failure.%0
.

MessageID = 16
SymbolicName = IDS_08_006
Language=English
08006 01028 Transaction rolledback.%0
.

MessageID = 17
SymbolicName = IDS_08_007
Language=English
08007 01030 Connection failure during transaction.%0
.

MessageID = 18
SymbolicName = IDS_08_S01
Language=English
08S01 01032 Communication link failure. The server timed out or disappeared%0
.

MessageID = 19
SymbolicName = IDS_21_001
Language=English
21001 01034 Cardinality violation; insert value list does not match column list.%0
.

MessageID = 20
SymbolicName = IDS_21_002
Language=English
21002 01036 Cardinality violation; parameter value list does not match column list.%0
.

MessageID = 21
SymbolicName = IDS_21_S01
Language=English
21S01 01038 Cardinality violation; insertion value list does not match column list.%0
.

MessageID = 22
SymbolicName = IDS_21_S02
Language=English
21S02 01040 Cardinality violation; parameter list does not match column list.%0
.

MessageID = 23
SymbolicName = IDS_22_001
Language=English
22001 01042 String data right truncation.%0
.

MessageID = 24
SymbolicName = IDS_22_003
Language=English
22003 01044 Numeric value out of range.%0
.

MessageID = 25
SymbolicName = IDS_22_005
Language=English
22005 01046 Error in assignment.%0
.

MessageID = 26
SymbolicName = IDS_22_005_01
Language=English
22005 Precision or scale out of range.%0
.

MessageID = 27
SymbolicName = IDS_22_012
Language=English
22012 01050 Division by zero.%0
.

MessageID = 28
SymbolicName = IDS_23_000
Language=English
23000 01052 Integrity constraint violation.%0
.

MessageID = 29
SymbolicName = IDS_24_000
Language=English
24000 01054 Invalid cursor state.%0
.

MessageID = 30
SymbolicName = IDS_25_000
Language=English
25000 01056 Invalid transaction state.%0
.

MessageID = 31
SymbolicName = IDS_26_000
Language=English
26000 01058 Invalid SQL statement identifier.%0
.

MessageID = 32
SymbolicName = IDS_28_000
Language=English
28000 01060 Invalid authorization specification.%0
.

MessageID = 33
SymbolicName = IDS_28_000_00
Language=English
28000 01062 Invalid authorization specification; Access to selected database has been denied.%0
.

MessageID = 34
SymbolicName = IDS_34_000
Language=English
34000 01064 Invalid cursor name.%0
.

MessageID = 35
SymbolicName = IDS_37_000
Language=English
37000 01066 Syntax error in SQL dynamic statement.%0
.

MessageID = 36
SymbolicName = IDS_3C_000
Language=English
3C000 01068 Duplicate cursor name.%0
.

MessageID = 37
SymbolicName = IDS_40_001
Language=English
40001 01070 Attempt to initiate new SQL Server operation with data pending.%0
.

MessageID = 38
SymbolicName = IDS_42_000
Language=English
42000 01072 Syntax error or access rule violation.%0
.

MessageID = 39
SymbolicName = IDS_70_100
Language=English
70100 01074 Operation aborted (server did not process cancel request).%0
.

MessageID = 40
SymbolicName = IDS_S0_001
Language=English
S0001 01076 Invalid table name; base table or view already exists.%0
.

MessageID = 41
SymbolicName = IDS_S0_002
Language=English
S0002 01078 Invalid table name; table or view not found.%0
.

MessageID = 42
SymbolicName = IDS_S0_011
Language=English
S0011 01080 Invalid index name; index already exists.%0
.

MessageID = 43
SymbolicName = IDS_S0_012
Language=English
S0012 01082 Invalid index name; index not found.%0
.

MessageID = 44
SymbolicName = IDS_S0_021
Language=English
S0021 01084 Invalid column name; column already exists.%0
.

MessageID = 45
SymbolicName = IDS_S0_022
Language=English
S0022 01086 Invalid column name; column not found.%0
.

MessageID = 46
SymbolicName = IDS_S1_000
Language=English
S1000 01088 General error.%0
.

MessageID = 47
SymbolicName = IDS_S1_000_00
Language=English
S1000 01090 General error: Ongoing transaction has been commited.%0
.

MessageID = 48
SymbolicName = IDS_S1_000_01
Language=English
S1000 01092 The stored procedure required to complete this operation could not be found on the server (they were supplied with the ODBC setup disk for the SQL Server driver). Contact your service provider.%0
.

MessageID = 49
SymbolicName = IDS_S1_000_02
Language=English
S1000 01094 Unknown token received from SQL Server%0
.

MessageID = 50
SymbolicName = IDS_S1_000_03
Language=English
S1000 01096 The ODBC catalog stored procedures installed on server %s are version %s; version %02d.%02d.%4.4d is required to ensure proper operation.  Please contact your system administrator.%0
.

MessageID = 51
SymbolicName = IDS_S1_000_04
Language=English
S1000 01098 Unable to load communication module.  Driver has not been correctly installed.%0
.

MessageID = 52
SymbolicName = IDS_S1_000_05
Language=English
S1000 01100 Communication module is not valid.  Driver has not been correctly installed.%0
.

MessageID = 53
SymbolicName = IDS_S1_000_06
Language=English
S1000 01102 Data type mismatch.%0
.

MessageID = 54
SymbolicName = IDS_S1_001
Language=English
S1001 01104 Memory allocation error.%0
.

MessageID = 55
SymbolicName = IDS_S1_002
Language=English
S1002 01106 Invalid column number.%0
.

MessageID = 56
SymbolicName = IDS_S1_003
Language=English
S1003 01108 Program type out of range.%0
.

MessageID = 57
SymbolicName = IDS_S1_004
Language=English
S1004 01110 SQL data type out of range.%0
.

MessageID = 58
SymbolicName = IDS_S1_005
Language=English
S1005 01112 Parameter number out of range.%0
.

MessageID = 59
SymbolicName = IDS_S1_006
Language=English
S1006 01114 Invalid conversion specified.%0
.

MessageID = 60
SymbolicName = IDS_S1_007
Language=English
S1007 01116 Row count not available from the data source.%0
.

MessageID = 61
SymbolicName = IDS_S1_008
Language=English
S1008 01118 Operation cancelled.%0
.

MessageID = 62
SymbolicName = IDS_S1_009
Language=English
S1009 01120 Invalid argument value.%0
.

MessageID = 63
SymbolicName = IDS_S1_010
Language=English
S1010 01122 Function sequence error.%0
.

MessageID = 64
SymbolicName = IDS_S1_012
Language=English
S1012 01124 Invalid transaction operator code specified.%0
.

MessageID = 65
SymbolicName = IDS_HY_015
Language=English
S1015 01130 No cursor name available.%0
.

MessageID = 66
SymbolicName = IDS_S1_090
Language=English
S1090 01132 Invalid string or buffer length.%0
.

MessageID = 67
SymbolicName = IDS_S1_091
Language=English
S1091 01134 Descriptor type out of range.%0
.

MessageID = 68
SymbolicName = IDS_S1_092
Language=English
S1092 01136 Option type out of range.%0
.

MessageID = 69
SymbolicName = IDS_S1_093
Language=English
S1093 01138 Invalid parameter number or missing parameter.%0
.

MessageID = 70
SymbolicName = IDS_S1_094
Language=English
S1094 01140 Invalid scale value.%0
.

MessageID = 71
SymbolicName = IDS_S1_095
Language=English
S1095 01142 Function type out of range.%0
.

MessageID = 72
SymbolicName = IDS_S1_096
Language=English
S1096 01144 Information type out of range.%0
.

MessageID = 73
SymbolicName = IDS_S1_097
Language=English
S1097 01146 Column type out of range.%0
.

MessageID = 74
SymbolicName = IDS_S1_098
Language=English
S1098 01148 Scope type out of range.%0
.

MessageID = 75
SymbolicName = IDS_S1_099
Language=English
S1099 zzzzz Nullable type out of range.%0
.

MessageID = 76
SymbolicName = IDS_S1_100
Language=English
S1100 zzzzz Uniqueness option type out of range.%0
.

MessageID = 77
SymbolicName = IDS_S1_101
Language=English
S1101 zzzzz Accuracy option type out of range.%0
.

MessageID = 78
SymbolicName = IDS_S1_102
Language=English
S1102 zzzzz Table type out of range.%0
.

MessageID = 79
SymbolicName = IDS_S1_103
Language=English
S1103 zzzzz Direction option out of range.%0
.

MessageID = 80
SymbolicName = IDS_S1_106
Language=English
S1106 zzzzz Fetch type out of range.%0
.

MessageID = 81
SymbolicName = IDS_S1_107
Language=English
S1107 zzzzz Row value out of range.%0
.

MessageID = 82
SymbolicName = IDS_S1_108
Language=English
S1108 zzzzz Concurrency option out of range.%0
.

MessageID = 83
SymbolicName = IDS_S1_109
Language=English
S1109 zzzzz Invalid cursor position; no keyset defined.%0
.

MessageID = 84
SymbolicName = IDS_S1_C00
Language=English
S1C00 zzzzz Driver not capable.%0
.

MessageID = 85
SymbolicName = IDS_S1_LD0
Language=English
S1LD0 zzzzz No long data values pending.%0
.

MessageID = 86
SymbolicName = IDS_S1_T00
Language=English
S1T00 zzzzz Timeout expired.%0
.

MessageID = 87
SymbolicName = IDS_IM_001
Language=English
IM001 zzzzz Driver does not support this function.%0
.

MessageID = 88
SymbolicName = IDS_01_000
Language=English
01000 zzzzz General warning.%0
.

MessageID = 89
SymbolicName = IDS_01_S02
Language=English
01S02 zzzzz Option value changed.%0
.

MessageID = 90
SymbolicName = IDS_22_008
Language=English
22008 zzzzz Datetime field overflow.%0
.

MessageID = 91
SymbolicName = IDS_S1_105
Language=English
S1105 zzzzz Invalid parameter type or parameter type not supported.%0
.

MessageID = 92
SymbolicName = IDS_IM_009
Language=English
IM009 zzzzz Unable to load DLL.%0
.

MessageID = 93
SymbolicName = IDS_186_DSTODRV_TRUNC
Language=English
HY721 zzzzz TranslationDLL Truncation - DataSourceToDriver: %0
.

MessageID = 94
SymbolicName = IDS_188_DRVTODS_TRUNC
Language=English
HY721 zzzzz TranslationDLL Truncation - DriverToDataSource: %0
.

MessageID = 95
SymbolicName = IDS_190_DSTODRV_ERROR
Language=English
HY722 zzzzz TranslationDLL Error - DataSourceToDriver: %0
.

MessageID = 96
SymbolicName = IDS_193_DRVTODS_ERROR
Language=English
HY723 zzzzz TranslationDLL Error - DriverToDataSource: %0
.

MessageID = 97
SymbolicName  = IDS_08_S02
Language=English
08S02 01033 Transport Layer Error.%0
.

MessageID = 98
SymbolicName  = IDS_07_009_02
Language=English
07009 ZZZZZ The value specified for the argument ColumnNumber was greater than the number of columns in the result set.%0
.

MessageID = 99
SymbolicName  = IDS_22_003_02
Language=English
22003 ZZZZZ A negative value cannot be converted to an unsigned numeric datatype.%0
.

MessageID = 150
SymbolicName = IDS_NO_SRVR_AVAILABLE
Language=English
08001 zzzzz No more ODBC servers available to connect.%0
.

MessageID = 151
SymbolicName = IDS_ASSOC_SRVR_NOT_AVAILABLE
Language=English
08001 zzzzz ODBC Services not yet available: %1!s!.%0
.

MessageID = 152
SymbolicName = IDS_DS_NOT_AVAILABLE
Language=English
08001 zzzzz DataSource not yet available or not found: %1!s!.%0
.

MessageID = 200
SymbolicName = IDS_PROGRAM_ERROR
Language=English
S1000 zzzzz Unexpected programming exception has been found: [%1!s!].  Check the server event log on node [%2!s!] for details.%0
.

MessageID = 201
SymbolicName = IDS_PORT_NOT_AVAILABLE
Language=English
08001 zzzzz No more ports available to start ODBC servers.%0
.

MessageID = 202
SymbolicName = IDS_RETRY_ATTEMPT_EXCEEDED
Language=English
08001 zzzzz Retry attempts to connect to the datasource failed, May be ODBC server not able to register to the ODBC service process.%0
.

MessageID = 203
SymbolicName = IDS_01_000_01
Severity=Warning
Language=English
01000 zzzzz General warning. Connected to the default data source: %1!s!.%0
.

MessageID = 204
SymbolicName = IDS_S1_000_07
Language=English
S1000 zzzzz General error. Failed since resource governing policy is hit: %1!s! %0
.

MessageID = 205
SymbolicName = IDS_08_004_01
Language=English
08004 zzzzz Data source rejected establishment of connection since the ODBC Server is connected to 
a different client now %0
.

MessageID = 206
SymbolicName = IDS_RG_LOG_INFO
Language=English
01000 zzzzz Query Estimated cost, %1!s!, exceeds resource policy %2!s!.  Statement written to log.%0
.

MessageID = 210
SymbolicName = IDS_EXCEPTION_MSG
Language=English
S1000 zzzzz General error. Runtime exception [%1!s!] has been detected in function [%2!s!].%0
.

MessageID = 211
SymbolicName = IDS_NO_FUNCTION
Language=English
S1000 zzzzz General error. The called function [%1!s!] is not implemented.  Please contact your service provider.%0
.

MessageID = 212
SymbolicName = IDS_SQL_ERROR
Language=English
S1000 zzzzz SQL error:%0
.

MessageID = 213
SymbolicName = IDS_SQL_WARNING
Language=English
01000 zzzzz SQL warning:%0
.

MessageID = 214
SymbolicName = IDS_UNABLE_TO_CANCEL_BY_SQL
Language=English
S1000 zzzzz Unable to cancel the statement because of a SQL Error.%0
.

MessageID = 215
SymbolicName = IDS_THREAD_ERROR
Language=English
S1000 zzzzz An internal thread error has been detected.  Error message: %1!s!.%0
.

MessageID = 216
SymbolicName = IDS_UNABLE_TO_GETPARAM
Language=English
S1000 zzzzz Unable to get parameter information either because of a NULL pointer or an incomplete parameter list.%0
.

MessageID = 217
SymbolicName = IDS_TRANSACTION_ERROR
Language=English
S1000 zzzzz A TIP transaction error [%1!s!] has been detected.  Check the server event log on Node [%2!s!] for Transaction Error details .%0
.

MessageID = 220
SymbolicName = IDS_NT_ERROR
Language=English
S1000 zzzzz General error. The function [%1!s!] has detected an NT error: [%2!s!].%0
.

MessageID = 221
SymbolicName = IDS_UNABLE_TO_LOAD_LIBRARY
Language=English
S1000 zzzzz Unable to load the HP ODBC driver because of an NT error: %1!s!.%0
.

MessageID = 222
SymbolicName = IDS_UNABLE_TO_GET_VERSION
Language=English
S1000 zzzzz Unable to get the OS version numbers because of an NT error: %1!s!.%0
.

MessageID = 223
SymbolicName = IDS_UNABLE_TO_GET_USER
Language=English
S1000 zzzzz Unable to get the user name because of an NT error: %1!s!.%0
.

MessageID = 224
SymbolicName = IDS_UNABLE_TO_GET_USER_DESC
Language=English
S1000 zzzzz Unable to get the user description because of an NT error: %1!s!.%0
.

MessageID = 225
SymbolicName = IDS_UNABLE_TO_LOGON
Language=English
S1000 zzzzz Unable to authenticate the user because of an NT error: %1!s!.%0
.

MessageID = 226
SymbolicName = IDS_UNABLE_TO_GETODBCINI
Language=English
S1000 zzzzz Unable to retrieve the ODBC.INI string for key [%1!s!] because of an NT error: %2!s!.%0
.

MessageID = 227
SymbolicName = IDS_UNABLE_TO_GETMODULENAME
Language=English
S1000 zzzzz Unable to retrieve the driver's module name because of an NT error: %1!s!.%0
.

MessageID = 228
SymbolicName = IDS_UNABLE_TO_GETFILEVERSION
Language=English
S1000 zzzzz Unable to retrieve the driver's file version because of an NT error: %1!s!.%0
.

MessageID = 300
SymbolicName = IDS_KRYPTON_UNABLE_TO_INITIALIZE
Language=English
S1000 zzzzz Unable to initialize the communication layer because of a network error: %1!s!.%0
.

MessageID = 301
SymbolicName = IDS_KRYPTON_UNABLE_TO_TERMINATE
Language=English
S1000 zzzzz Unable to terminate the dialogue with the MXCS Server because of a network error: %1!s!.%0
.

MessageID = 302
SymbolicName = IDS_KRYPTON_ODBCSRVR_INTERFACE_FAILED
Language=English
S1000 zzzzz Unable to register the MXCS Server interface because of a network error: %1!s!.%0
.

MessageID = 303
SymbolicName = IDS_KRYPTON_ASSRVR_INTERFACE_FAILED
Language=English
S1000 zzzzz Unable to register the Association Server interface because of a network error: %1!s!.%0
.

MessageID = 304
SymbolicName = IDS_KRYPTON_UNABLE_PROXY_CREATE
Language=English
S1000 zzzzz Unable to create the proxy for the MXCS Server because of a network error: %1!s!.%0
.

MessageID = 305
SymbolicName = IDS_KRYPTON_UNABLE_PROXY_DESTROY
Language=English
S1000 zzzzz Unable to destroy the proxy for the MXCS Server because of a network error: %1!s!.%0
.

MessageID = 310
SymbolicName = IDS_KRYPTON_ERROR
Language=English
S1000 zzzzz Unable to perform function [%1!s!] because of a network error: %2!s!.%0
.

MessageID = 311
SymbolicName = IDS_KRYPTON_SRVR_GONE
Language=English
08S01 zzzzz Communication Link failure.  Unable to perform function [%1!s!] because the MXCS Server disappeared.%0
.

MessageID = 312
SymbolicName = IDS_KRYPTON_NO_SRVR
Language=English
S1000 zzzzz Unable to perform function [%1!s!] because the MXCS Server does not exist.%0
.

MessageID = 313
SymbolicName = IDS_DS_NOT_FOUND
Language=English
08001 zzzzz Unable to connect to data source. The Data source Entry is not found.%0
.

MessageID = 314
SymbolicName = IDS_HY_000
Language=English
HY000 01088 General error.%0
.

MessageID = 315
SymbolicName = IDS_HY_001
Language=English
HY001 01089 Memory allocation error.%0
.

MessageID = 316
SymbolicName = IDS_HY_090
Language=English
HY090 01090 Invalid string or buffer length.%0
.

MessageID = 317
SymbolicName = IDS_IM_010
Language=English
IM010 01091 Data source name too long.%0
.

MessageID = 318
SymbolicName = IDS_CEE_THREAD_NOT_AVAILABLE
Language=English
HY000 01092 General error - UnRecoverable Error - The network Component thread has been terminated earlier.%0
.

MessageID = 319
SymbolicName = IDS_HY_092
Language=English
HY092 01093 Invalid attribute/option identifier.%0
.

MessageID = 320
SymbolicName = IDS_HY_C00
Language=English
HYC00 01093 Optional feature not implemented.%0
.

MessageID = 321
SymbolicName = IDS_HY_091
Language=English
HY091 01094 Invalid descriptor field identifier.%0
.

MessageID = 322
SymbolicName = IDS_HY_016
Language=English
HY016 01095 Cannot modify an implementation row descriptor.%0
.

MessageID = 323
SymbolicName = IDS_07_009
Language=English
07009 01096 Invalid descriptor index. RecNumber is 0 (Bookmark is not yet supported).%0
.

MessageID = 324
SymbolicName = IDS_HY_021
Language=English
HY021 01096 Inconsistent descriptor information.%0
.

MessageID = 325
SymbolicName = IDS_HY_003
Language=English
HY003 01097 Invalid application buffer type.%0
.

MessageID = 326
SymbolicName = IDS_HY_013
Language=English
HY013 01098 Memory management error.%0
.

MessageID = 327
SymbolicName = IDS_01_S00
Language=English
01S00 01099 Invalid connection string attribute.%0
.

MessageID = 328
SymbolicName = IDS_IM_007
Language=English
IM007 01100 No data source or driver specified; dialog prohibited.%0
.

MessageID = 329
SymbolicName = IDS_IM_008
Language=English
IM008 01101 Dialog failed.%0
.

MessageID = 330
SymbolicName = IDS_HY_105
Language=English
HY105 01102 Invalid parameter type.%0
.

MessageID = 331
SymbolicName = IDS_HY_018
Language=English
HY018 01103 Server declined cancel request.%0
.

MessageId = 332
SymbolicName = IDS_07_002
Language=English
07002 01104 COUNT field incorrect.%0
.

MessageId = 333
SymbolicName = IDS_07_S01
Language=English
07S01 01105 Invalid use of default parameter SQL_DEFAULT_PARAMETER not supported.%0
.

MessageId = 334
SymbolicName = IDS_HY_010
Language=English
HY010 01106 Function sequence error.%0
.

MessageId = 335
SymbolicName = IDS_HY_020
Language=English
HY020 01107 Attempt to concatenate a null value.%0
.

MessageId = 336
SymbolicName = IDS_HY_019
Language=English
HY019 01108 Non-character and non-binary data sent in pieces.%0
.

MessageId = 337
SymbolicName = IDS_22_002
Language=English
22002 01109 Indicator variable required but not supplied.%0
.

MessageId = 338
SymbolicName = IDS_01_S07
Language=English
01S07 01110 Fractional truncation.%0
.

MessageId = 339
SymbolicName = IDS_HY_107
Language=English
HY107 01111 Row-value out of range.%0
.

MessageId = 340
SymbolicName = IDS_22_018
Language=English
22018 01112 Invalid character value for cast specification.%0
.

MessageId = 341
SymbolicName = IDS_HY_024
Language=English
HY024 01113 Invalid attribute value.%0
.

MessageId = 342
SymbolicName = IDS_HY_110
Language=English
HY110 01114 Invalid driver completion.%0
.

MessageId = 343
SymbolicName = IDS_IM_002
Language=English
IM002 01115 Data source not found or no default driver specification.%0
.

MessageId = 344
SymbolicName = IDS_HY_109
Language=English
HY009 01116 Invalid cursor position.%0
.

MessageId = 345
SymbolicName = IDS_HY_007
Language=English
HY007 01117 Associated statement is not prepared.%0
.

MessageId = 346
SymbolicName = IDS_HY_096
Language=English
HY096 01118 Information type out of range.%0
.

MessageID = 347
SymbolicName = IDS_S1_000_08
Language=English
01000 zzzzz General warning. Resource governing policy is hit: %1!s! %0
.

MessageID = 348
SymbolicName = IDS_HY_004
Language=English
HY004 01119 Invalid SQL data type.%0
.

MessageID = 349
SymbolicName = IDS_22_015
Language=English
22015 zzzzz Interval field overflow.%0
.

MessageID = 350
SymbolicName = IDS_TRANSLATE_ERROR
Language=English
HY000 zzzzz Translate Error: %1!s! %0
.
