## Audio Driver Design
### Design
The audio driver is a crucial component for audio streaming in the system, as it facilitates the transmission of user-parsed .WAV data to the Audio CODEC for audio playback. This driver does this using the Hardware Abstraction Layer (HAL) library, the high-performance H7 DMA (Direct Memory Access), and the Serial Audio Interface (SAI) FIFO.

The driver has two main functionalities: the initialization of all HAL objects and the management and transferring of buffered and parsed audio data to the DMA. During initialization, the driver uses the HAL library to initialize Block A (RX) and B (TX) of the SAI FIFO with the desired user bit depth and sample rate. It automatically starts the SAI FIFO *MCLK* and initializes and links the DMA to the TX SAI FIFO. While originally, only one block of SAI FIFO was wanted as no RX operations are expected for this board, through experimentation it was determined that the Daisy devboard relies on the initialization of the RX block to start the *MCLK*, which is needed to send any audio data. While the RX block is not being used at the moment, this design adds the benefit of being able to expand the board to handle microphones and other inputs in the future. The below diagram covers the high-level initialization operation of the audio driver.

![alt text](init-audio-driver-activity.png)

When the user requests a transfer, the driver transfers the data to a dedicated DMA queue, from which the SAI FIFO autonomously retrieves samples. The driver polls the DMA interrupt status to make sure the DMA is not busy when the transmission is requested, and will block until the DMA is ready. This functionality was originally designed to block the thread, but not wait the entire CPU. However, due to timing constraints, FreeRTOS was not successfully integrated into the system, and thus the polling method had to be used. While less efficient than thread sleeping and waking on interrupt, the large input buffer sizes (256KB) gives long periods between any polling needs to be performed, giving the CPU more free time. Once transferred, the SAI FIFO Block B requests these samples from the DMA, and sends them to the CODEC for interpretation. 

![alt text](transmit-audio-driver-activity.png)

The designed data flow to get audio from the user level to the codec is highlighted below.

![alt text](data-flow.png)

The decision to incorporate DMA into the audio driver architecture stems from the desire to enable asynchronous streaming capabilities. Technically, it is possible to send audio data directly to the SAI FIFO for each block, but the SAI FIFO only has an 8 sample depth, and thus the CPU would constantly need to feed the FIFO to keep it from running empty, which would apply to any buffer size. By offloading the data transfer task from the CPU to dedicated DMA channels, we alleviate CPU stress and unlock the capability for larger input sample buffer sizes. This not only enhances the efficiency of data transmission but also ensures smoother audio playback and less overall CPU utilization.

### Implementation
The audio driver is encapsulated within the `SAIDriver` class, which serves as the central component responsible for managing audio data transmission and interaction with the hardware components. Organizing the driver into a class offers several benefits, including improved modularity, code organization, and ease of maintenance. Additionally, by encapsulating the driver in a class, the creation of an audio driver object is fully managed and encapsulated, increasing code organization and easing user parsing level interaction.


