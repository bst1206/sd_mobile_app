/**
@mainpage Zigbee Network Processor Library
@brief READ ME FIRST
@author Derek Smith

@section intro Introduction
The Zigbee Network Processor (ZNP) allows you to easily create Zigbee applications without having to make 
extensive changes to the Zigbee Stack. This page is a general overview of the examples we've provided 
to interface with the ZNP. See documentation on each file for more infomation. 
Methods are commented in the .c files. Header files list public (externally accessible) methods only.

There are two interfaces to the ZNP: the Simple API and the Application Framework / Zigbee Device Objects (AFZDO).
Simple API is a little easier, as it is only ~10 methods to learn, but AFZDO is more powerful.

The examples included demonstrate basic functionality to help you get up and running quickly. 
There are four categories of examples:

 @section sys System Interface
 These are the generic ZNP configuration methods and can be used with Simple API or AFZDO.
 These require a hardware interface file (znp_interface_spi.c).
- znp_commands.c
- znp_interface.c
- znpRfTestInterface.c Note: this is only needed for RF testing

 @section api Application Programming Interface
 This enables your application to send/receive data using the ZNP. 
 Use either Simple API or AFZDO but not both.
- simple_api.c: Implements the Simple API
- af_zdo.c: Implements the AF/ZDO

@section znphw ZNP Hardware Interface
This file supports the SPI interface
- znp_interface_spi.c

@section utils Miscellaneous ZNP Utilities
Methods or defines used by one or more of the previous files.
- znp_commands.c: defines all the commands used by the ZNP
- application_configuration.c: utility method used in registerApplication() in Simple API or AFZDO

@note Projects compiled with IAR Embedded Workbench for IAR version 4.21.2
<hr>
@page about About this Documentation
This HTML output generated by Doxygen, a free documentation generator tool 
that uses special tags in the code to generate pretty HTML output that you see here. 
It is very similar to javadoc if you have experience with that, in fact many of the tags are the same. 
Documentation generators like this are highly recommended, especially if you're writing code that someone else will need to read.
@see www.doxygen.org




*/