## Lessons Learned

Throughout the project, the team learned several important lessons. Firstly, on the STM32H7 platform, errors often manifest silently, making debugging challenging. Hours spent in Ozone inspecting faults, stepping through code, and looking at registers taught us to expect more time spent debugging than initially thought. Consequently, we adopted better time management skills and a practice of writing code before debugging sessions, which allowed us to allocate more time for debugging, thus ensuring smoother progress.

Another lesson involved recognizing that hardware constraints aren't always within our control. For instance, the Daisy development board's predetermined configurations posed challenges with the SAI FIFO needing the RX buffer to be initialized to a specific block. Hours were spent debugging this when we could have solved the issue much quicker had we understood the hardware operation better. This taught us that understanding hardware connections is crucial for designing efficient workarounds to hardware limitations from the start, and minimizing debugging time.

Furthermore, we realized the benefits of pursuing stretch goals. Despite the added workload, integrating MIDI-based audio sample playback enhanced user experience, underscoring the value of putting in the extra work.

Lastly, we found that it is not only feasible, but beneficial to re-implement daisy library code in a way that is more useful to specific use cases. Tailoring code to our needs facilitated smoother integration of custom hardware and ensured optimal efficiency by focusing solely on essential features, rather than integrating the bloat of the Daisy library.

Reflecting on the project, we wouldn't alter our chosen path significantly. While we encountered our fair share of challenges, they ultimately contributed to our learning and growth, and there were no design decisions that hindered the final product in a significant way.