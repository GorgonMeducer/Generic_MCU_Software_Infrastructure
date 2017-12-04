# Generic_MCU_Software_Infrastructure

Provides necessary software infrastructures, services, macros to support some high level abstruct concepts and/or paradigm, such as OOPC, FSM, delegate (event-driven) and etc

Currently GMSI provides following services:

- epool

    a common pool infrastructure, which could be considered as an abstract class for pool (heap). The letter "e" here represents "embedded".  


- block
    
    a building block class which provides the basis of block based buffer services. block_t will be used by other services, such as block_queue, stream2block, es_simple_frame and etc.
    Both dynamic(heap) and static memory allocation are supported. 
    
    
- block_queue

    A queue of block_t which implemented as a block list. 
    
    
- stream2block

    a dedicated stream-based service which provides stream interface for block devices, i.e. IO devices which support burst transfer mode (such as SPI, USART using DMA). 
    A serial port tempalte is provided to use USART with the minimal requirments of Data-Receiving-Complete interrupt and Data-Transfering-Complete interrupt. Example project demostrates how to use stream2block with usart. 
    
    WHY using stream2block service rather than simple QUEUE + ISR ?
    
    Simple QUEUE + ISR does works for normal applications, but you have to provide atomicity protection for queues. This will hurt real-time response of interrupt significantly if the stream is accessed frequently in super-loop, i.e. enqueue or dequeue. On the other hand, some device might encourage users to use DMA or other methods which can generate burst transations, in other words, a stream has to be converted into blocks. When stream2block is designed, it works like a double-bufferred system, so when one block is used for queue access, another block is used for a burst transaction. The burst IO transactiond and the queue are connected via a list, which works like a queue of blocks. You only need to provide atomicity protection on the list but not the queue. So, frequenly accessing queue in the super-loop will not hurt any real-time response, as list accesses are relatively less freqently comparing those accesses in normal QUEUE + ISR scheme.


- es_simple_frame
    
    A communication service with provides encoding and decoding functions based on a simple frame structure, which only contains a 8bit header, a 16bit length, an optional data field and a 16bits CRC checksum. This service can be configured working with exchangable buffer ( block_t ) or static memory buffer.

- multiple_delay

    A dedicated temporal module which can provide delay monitoring service for finite state machine or tasks in RTOS. A 1ms timer event handler is required to drive a 32bit counter. 

- scheduler

    A dedicated non-preeptive finite state machine scheduler. 
    
- crc

    Various CRC functions. 

- xmodem
   
    A dedicated module which implements Xmodem communication protocal.

- bootloader

    A bootloader logic template. You just need to 1) implement the memories access driver, 2)describe the supported memories in the target device, and 3) provide a stream interface (i_byte_pipe_t) for communication. 

- key
    A dedicated module which provides general key-board services, such as signal filtering, key event detection and key buffer. 

- sw_sdio

    A dedicated module which provides software-implemented sdio driver. 

- mal

    Memory Abstruct Layer, provides unified, random access API for all kinds of page-based and/or non-page-based memories. 

- page

    A dedicated service which provide random accesses for page-based memories. 

    

...
