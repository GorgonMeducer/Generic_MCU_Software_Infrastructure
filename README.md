# Generic_MCU_Software_Infrastructure

Provide necessary software infrastructure, service, macros to support some high level abstruct concept or paradigm, such as OOPC, FSM, delegate (event-driven) and etc

Currently GMSI provide following services:

- stream2block

    a dedicated stream based service which provide stream interface for block device or IO devices which supports burst transfer mode (such as SPI, USART using DMA). 
    A serial port tempalte is provided to using USART with requirements of Receive complete interrupt and Transfer complete interrupt. Example project demostrates how to use stream2block for usart. 
    
    WHY using stream2block service rather than simple QUEUE + ISR ?
    
    Simple QUEUE + ISR does works for normal applications, but you have to provide atomicity protection for queue. This will hurt real-time response of interrupt significantly if the stream is accessed frequently in super-loop, i.e. enqueue or dequeue. On the other hand, some device might encourage users to use DMA or other method for generate burst transations, in other words, a stream has to be converted into blocks. When stream2block is designed, for a stream, it works like a double buffer system, which means, one block is used for queue access while another block is used for a burst transaction. The burst IO transaction and the queue are connected by a list. The list works like a queue of blocks. So you only need to provide atomicity protection on the list but not the queue. So, frequenly accessing queue in super-loop will not hurt any real-time response of interrupts. Any list access is relatively less freqently comparing with normal QUEUE + ISR scheme.

- multiple_delay
- epool
- scheduler
- crc
- es_simple_frame
- xmodem
- bootloader
- key
- sw_sdio
- mal
- page

...
