## MIDI Driver Design
### Design
To encourage more musical and generally more interesting usage of the sample player, samples can be triggered and played through the means of some sort of controller. To allow an external controller to interface with out system, we developed a MIDI handler to translate incoming MIDI (UART) data into usable information. This driver handles the reception and storage of UART data, parsing of data into MIDI events, and providing stored MIDI events as needed by the system. The MIDI driver does not explicitly handle the UART peripheral initialization, though it will be mentioned in this report to emcompass the information in one place.

As the actual processing of MIDI events is left up to the system, the MIDI driver only has one real job: converting incoming UART MIDI into usable MIDI events. To do this, the handler needs to be able to be able to take in incoming MIDI data via UART and store it to make sure no data is lost or overwritten when higher priority processes are occuring. These bytes of data can the be processed into MIDI events, which stores information about the target channel, messgae type, and any other pertinent information to the event. This in turn allows for easy processing by the system, especially when the work of dealing with more complex messages (like system real time and system exclusive messages) can be left to the handler.

While MIDI data is is most commonly transfered over UART (the original standard completed in 1983) and over USB (developed in 1999), this driver is specifically designed for use with MIDI over UART in mind. This can be handled using the STM32H7's plethora of UART/USART peripherals in a variety of modes, which can be managed by the system designer. Due to the infrequent and variable length nature of MIDI, some form of interupt or DMA-based handling of UART reception is recommended (how our system was handled will be convered more in implementation, though the current design of the driver allows for the user to manage the UART peripheral as desired). MIDI specifications require transmission to occur at a baud rate of 31250 and a word length of 8 bits.

### Implementation
The codec driver is encapsulated within the `midiHandler` class, which serves as the central component responsible for parsing MIDI events, storage of MIDI data and parsed MIDI events, and data I/O. The class was created as a template in order to allow for the sizes of the two emcompassed circular buffers to be varied if multiple handlers are created (as a note, this concept will be revisited in the future due to unneeded complexities being added, and the use case for having multiple MIDI handlers is few and far between).

Upon creation, the `midiHandler` object initializes it's two circular buffers and it's `midiParser` object, both of which have been designed for use with the `midiHandler`. The circular objects are initialized with the specified type and size (provided as a constant value in the template class instantiation), with the `midiRecieveBuffer` being of type uint8_t and the `midiEventBuffer` being of custom type `MidiEvent`.

The `MidiEvent` type is used to improve readability and general handling of parsed MIDI events. Defined in it's respective header file, a `MidiEvent` structure consists of a `MidiMessageType` that defines the core type of the event, the relevant channel the event is for, a uint8_t data array of size two to store the up to two bytes of data some events require, and additional message type information used for System Common, System Exclusive, and Channel Mode messages.

The `midiParser` object is used to make the `midiHandler` more intuitive and user friendly, hiding much of the underlying processing of recieved bytes from the handler itself. Other than it's intializer, the `midiParser` class only has one function: `parse`. This function takes in a byte of data and a pointer to `MidiEvent` variable, which is used to store and "build" an event over various incoming bytes until the event has been created. As data is parsed, the `midiParser` keeps track of the event in a state machine-like way, handling data until the event has been "built" or an invalid input causes the process to restart. A visual representation of the `parse` function's process is shown below.

![alt text](image.png)

Post construction, the `midiHandler` provides a few public functions to the user. To handle the raw UART data, the `enqueueByte`, `dequeueByte`, and `midiRecieveCheckEmpty` are used to manage the MIDI data circular buffer. Similarly, `dequeueEvent`, and `midiEventCheckEmpty` are used to manage the MIDI event circular buffer. Unlike the data buffer, the MIDI event buffer does not have any way to enqueue events into the buffer. This is because the `parse` function (which makes use of the class' parser object) will automatically enqueue the event into the corresponding buffer, without the need for the user to handle the data. Lastly, the `parse` function takes in a byte of MIDI data and provides it to the `midiParser` object. Once a MIDI event is completed via parsing, the event is automatically enqueued to the `midiEventBuffer`.

In order to recieve incoming MIDI data from a controller, the system needs to be able to reliably receieve incoming UART data. This was done by utilizing the USART1 peripheral on the STM32H7 as an asyncronus UART reciever. **Note: the driver is not responsible for handling of UART peripheral, it is up to the user to do so. The following information describes how our implimentation of UART for the system was accomplished.** The UART peripheral was configured to MIDI specifications (a baud rate of 31250 and a word length of 8 bits). To handle the incoming bytes, we opted to utilize the UART peripheral in interupt mode to avoid consistently polling while audio playback was occuring. Upon an interupt and the reception of one byte of data, the recieved data is instantly enqueued into the `midiHandler`. This allows us to ignore the issues of unknown message lengths. While this works fine since we are not dealing with too many system processes and the MIDI specification is relatively slow, this design can be bottlenecked later on, leading to missed messages. A later improvement would be making use of DMA transfer of larger amounts of data at once, with the addition of idle line detection to handle the issue of unknown message lengths.

### Usage Definition
In order to use the MIDI driver, an instance of the class must first be created. This can be done by defining a new MIDI handler class as shown below. 

```c++
midiHandler<<data buffer size>, <event buffer size>> <MIDI handle object name>
```

The data and event buffer size must be defined at compile time through the use of a contant value or expression. It cannot be defined at run time as to avoid run time memory allocation.

With the class object created, data can now be processed by the system. Reception of incoming data is done byte by byte using the `enqueueByte` function, which enqueues a provided byte of data into the corresponding circular buffer. This can be done like shown below.

```c++
<MIDI handle object name>.enqueueByte(<data byte>);
```

Similarly, `dequeueByte` and `dequeueEvent` return the next piece of data from the MIDI data buffer and MIDI event buffer respectively. `dequeueByte` will normally be used to get the next byt of data to be processed by the parser, and `dequeueEvent` will normally be used to return the next MIDI event to be processed by the system.

The `parse` function takes in a byte of MIDI data and provides it to the parser. Once the parser has a complete and valid MIDI event, the event will automatically be enqueued into its respective buffer. A valid way to combine the `parse` and `dequeueByte` functgions is shown below.

```c++
<MIDI handle object name>.parse(<MIDI handle object name>.dequeueByte());
```

The two public functions `midiRecieveCheckEmpty` and `midiEventCheckEmpty` can be useful in knowning when data needs to be parsed or events need to be processed. An example of using the `midiRecieveCheckEmpty` to manage parsing is shown below.

```c++
if (midi_handler.midiRecieveCheckEmpty() == false) {
      midi_handler.parse(midi_handler.dequeueByte());
    }
```