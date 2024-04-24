#include "sai-driver.h"
#include "Legacy/stm32_hal_legacy.h"
#include "stm32h750xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_cortex.h"
#include "stm32h7xx_hal_def.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_sai.h"
#include <cstdint>

DMA_HandleTypeDef hdma;

/**
 * @brief Constructs a new SAIDriver object.
 *
 * This constructor initializes a new instance of the SAIDriver class.
 * SAI1_Block_A will be initialized first to start MCLK which will drive the
 * SAI1_Block_B TX FIFO. All TX data will be sent to SAI1_Block_B.
 *
 * @param stereo A boolean value indicating whether to use stereo or mono audio.
 * Default is stereo.
 * @param bitDepth The bit depth of the audio data. Default is 32-bit.
 * @param sampleRate The sample rate of the audio data. Default is 48KHz.
 * @return A new instance of the SAIDriver class.
 */
SAIDriver::SAIDriver(bool stereo, BitDepth bitDepth, SampleRate sampleRate) {
  uint8_t bitDepthValue;
  uint32_t protocolValue;

  // Initialize block A as master
  // (Necessary to have with the Daisy)
  hsaiA.Instance = SAI1_Block_A;
  hsaiA.Init.AudioMode = SAI_MODEMASTER_TX;
  hsaiA.Init.Synchro = SAI_ASYNCHRONOUS; // Master mode should be asynchronous

  // Initialize block B as slave
  hsaiB.Instance = SAI1_Block_B;
  hsaiB.Init.AudioMode = SAI_MODESLAVE_TX;
  hsaiB.Init.Synchro = SAI_SYNCHRONOUS;

  // Set the bit depth
  switch (bitDepth) {
  case BitDepth::BIT_DEPTH_16:
    bitDepthValue = SAI_PROTOCOL_DATASIZE_16BIT;
    protocolValue = SAI_I2S_STANDARD;
    this->bitDepth = 16;
    break;
  case BitDepth::BIT_DEPTH_16_EXTENDED:
    bitDepthValue = SAI_PROTOCOL_DATASIZE_16BITEXTENDED;
    protocolValue = SAI_I2S_STANDARD;
    this->bitDepth = 16;
    break;
  case BitDepth::BIT_DEPTH_24:
    bitDepthValue = SAI_PROTOCOL_DATASIZE_24BIT;
    // TODO: CHECK IF 24 BIT CAN BE STD ON DAISY
    protocolValue = SAI_I2S_STANDARD;
    this->bitDepth = 24;
    break;
  case BitDepth::BIT_DEPTH_32:
    bitDepthValue = SAI_PROTOCOL_DATASIZE_32BIT;
    protocolValue = SAI_I2S_STANDARD;
    this->bitDepth = 32;
    break;
  default:
    bitDepthValue = SAI_PROTOCOL_DATASIZE_32BIT;
    protocolValue = SAI_I2S_STANDARD;
    this->bitDepth = 32;
    break;
  }
  hsaiA.Init.DataSize = bitDepthValue;
  hsaiB.Init.DataSize = bitDepthValue;
  // Set publicly available buffer size
  this->dmaBufferWordSize = DMA_RAM_BUFFER_SIZE / (this->bitDepth / 8);

  // Set the sample rate
  switch (sampleRate) {
  case SampleRate::SAMPLE_RATE_8K:
    hsaiA.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_8K;
    hsaiB.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_8K;
    this->sampleRate = 8000; // 8k
    break;
  case SampleRate::SAMPLE_RATE_11K:
    hsaiA.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_11K;
    hsaiB.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_11K;
    this->sampleRate = 11025; // 11k
    break;
  case SampleRate::SAMPLE_RATE_16K:
    hsaiA.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_16K;
    hsaiB.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_16K;
    this->sampleRate = 16000; // 16k
    break;
  case SampleRate::SAMPLE_RATE_22K:
    hsaiA.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_22K;
    hsaiB.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_22K;
    this->sampleRate = 22050; // 22k
    break;
  case SampleRate::SAMPLE_RATE_32K:
    hsaiA.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_32K;
    hsaiB.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_32K;
    this->sampleRate = 32000; // 32k
    break;
  case SampleRate::SAMPLE_RATE_44K:
    hsaiA.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_44K;
    hsaiB.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_44K;
    this->sampleRate = 44100; // 44k
    break;
  case SampleRate::SAMPLE_RATE_48K:
    hsaiA.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
    hsaiB.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
    this->sampleRate = 48000; // 48k
    break;
  case SampleRate::SAMPLE_RATE_96K:
    hsaiA.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_96K;
    hsaiB.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_96K;
    this->sampleRate = 96000; // 96k
    break;
  case SampleRate::SAMPLE_RATE_192K:
    hsaiA.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_192K;
    hsaiB.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_192K;
    this->sampleRate = 192000; // 192k
    break;
  }

  // Set up generic API values for init protocol
  genericHSAISetup(&hsaiA);
  genericHSAISetup(&hsaiB);

  // Initialize HSAI A
  if (HAL_SAI_InitProtocol(&hsaiA, protocolValue, bitDepthValue, 2) != HAL_OK) {
    __asm__ __volatile__("bkpt #0");
  }
  // Send a single word to the RX FIFO to start MCLK
  uint32_t dummyData[1] = {
      0x00000000}; // Fix: Use initializer list for array initialization
  uint8_t *pData = (uint8_t *)dummyData;
  SAINATransmit(pData, 1, 2000);

  // Initialize HSAI B
  if (HAL_SAI_InitProtocol(&hsaiB, protocolValue, bitDepthValue, 2) != HAL_OK) {
    __asm__ __volatile__("bkpt #1");
  }
}

/**
 * @brief Configures the generic settings of the SAI peripheral.
 *
 * This function sets up the SAI peripheral with generic API config parameters.
 * It configures various settings such as synchronization, output drive, audio
 * frequency, mono/stereo mode, companding mode, tri-state, protocol, and data
 * size.
 *
 * @param hsai Pointer to the SAI handle structure.
 */
void SAIDriver::genericHSAISetup(SAI_HandleTypeDef *hsai) {
  // General HSAI configuration
  hsai->Init.SynchroExt = SAI_SYNCEXT_DISABLE;

  // TODO: CHECK IF NOT DEFINING MCKOUTPUT IS OK
  // hsai->Init.MckOutput = SAI_MCK_OUTPUT_DISABLE;

  hsai->Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLED;
  hsai->Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai->Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  // TODO: CHECK IF NOT DEFINING THIS IS OK
  // hsai->Init.Mckdiv = 0;
  // TODO: OVERSAMPLING

  hsai->Init.MonoStereoMode = SAI_STEREOMODE;
  hsai->Init.CompandingMode = SAI_NOCOMPANDING;
  hsai->Init.TriState = SAI_OUTPUT_NOTRELEASED;
}

/**
 * @brief Initializes the pins for SAI1.
 *
 * This function configures the GPIO pins for SAI1 peripheral. It sets the pins
 * as alternate function with pull-up configuration and medium speed. The pins
 * initialized are SAI1_FS_A, SAI1_MCLK_A, SAI1_SCK_A, SAI1_SA_A, and SAI1_SB_A.
 *
 * @note This function should be called before using the SAI1 peripheral.
 */
void initPins() {
  GPIO_InitTypeDef GPIO_Config;
  // Initialize pins for SAI1
  // GPIO_Config.Pin = gpioPinValues::SAI1_FS_A |
  //                   gpioPinValues::SAI1_MCLK_A |
  //                   gpioPinValues::SAI1_SCK_A |
  //                   gpioPinValues::SAI1_SA_A |
  //                   gpioPinValues::SAI1_SB_A;
  // Configure SAI pins as alternate function pull-up.
  GPIO_Config.Mode = GPIO_MODE_AF_PP;
  GPIO_Config.Pull = GPIO_PULLUP;
  GPIO_Config.Speed =
      GPIO_SPEED_FREQ_MEDIUM; // TODO: CHECK OVER SPEEDS, SEE IF MEDIUM IS OK
  GPIO_Config.Alternate = GPIO_AF6_SAI1;

  // HAL_GPIO_Init(GPIOE, &GPIO_Config);
  // Initialize pin for SAI1_FS_A
  GPIO_Config.Pin = gpioPinValues::SAI1_FS_A;
  HAL_GPIO_Init(GPIOE, &GPIO_Config);

  // Initialize pin for SAI1_MCLK_A
  GPIO_Config.Pin = gpioPinValues::SAI1_MCLK_A;
  HAL_GPIO_Init(GPIOE, &GPIO_Config);

  // Initialize pin for SAI1_SCK_A
  GPIO_Config.Pin = gpioPinValues::SAI1_SCK_A;
  HAL_GPIO_Init(GPIOE, &GPIO_Config);

  // Initialize pin for SAI1_SA_A
  GPIO_Config.Pin = gpioPinValues::SAI1_SA_A;
  HAL_GPIO_Init(GPIOE, &GPIO_Config);

  // Initialize pin for SAI1_SB_A
  GPIO_Config.Pin = gpioPinValues::SAI1_SB_A;
  HAL_GPIO_Init(GPIOE, &GPIO_Config);
}

/**
 * @brief Initializes the clocks for SAI GPIOs and SAI interface.
 *
 * This function enables the clocks for SAI GPIOs (GPIOA, GPIOE, GPIOG) and the
 * SAI interface (SAI1).
 */
void initClks() {
  // Enable clocks for SAI GPIOs
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  // Enable SAI interface clocks
  __HAL_RCC_SAI1_CLK_ENABLE();
}

/**
 * @brief Initializes the DMA for the SAI peripheral.
 *
 * This function initializes the DMA for the SAI peripheral. It configures the
 * DMA stream, sets the request to SAI1_B, sets the direction to memory to
 * peripheral, enables memory increment, sets the data alignment to word, sets
 * the mode to circular, sets the priority to high, and disables the FIFO mode.
 * It is only called for SAI1_Block_B.
 *
 * @param hsai Pointer to the SAI handle structure.
 */
void initDMA(SAI_HandleTypeDef *hsai) {
  // Initialize DMA for SAI
  hdma.Instance = DMA1_Stream0;

  // Perform all init operations
  // TODO: ADD
  hdma.Init.Request =
      DMA_REQUEST_SAI1_B; // Request a DMA with connection to SAI1_A
  hdma.Init.Direction =
      DMA_MEMORY_TO_PERIPH; // TX from DMA to SAI (this will always be TX, even
                            // with other SAI blocks)
  hdma.Init.PeriphInc = DMA_PINC_DISABLE; // Peripheral increment disabled
  // Increment through data in the audio input buffer
  // Set up the DMA to take and output audio words
  hdma.Init.MemInc = DMA_MINC_ENABLE; // Memory increment enabled
  hdma.Init.PeriphDataAlignment =
      DMA_PDATAALIGN_HALFWORD; // Peripheral data alignment is word
  hdma.Init.MemDataAlignment =
      DMA_MDATAALIGN_HALFWORD; // Memory data alignment is word
  hdma.Init.Mode = DMA_NORMAL; // Circular mode (emulate FIFO with more control)
  hdma.Init.Priority = DMA_PRIORITY_HIGH; // High priority
  // Enable built in 8 word FIFO (pre-circular buffer)
  hdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  // Initialize the DMA
  if (HAL_DMA_Init(&hdma) != HAL_OK) {
    __asm__ __volatile__("bkpt #2");
  }

  // hsai->hdmatx = &hdma;

  // Link DMA to SAI
  __HAL_LINKDMA(hsai, hdmatx, hdma);
}

/**
 * @brief Initializes the necessary resources for the SAI peripheral.
 *
 * This function is called by the HAL library to perform any necessary
 * initialization for the SAI peripheral. It enables clocks, configures SAI
 * pins, and initializes DMA for sai block B.
 *
 * @param hsai Pointer to the SAI handle structure.
 */
extern "C" void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai) {
  // Perform any necessary initialization for SAI peripheral
  if (hsai->Instance == SAI1_Block_A || hsai->Instance == SAI1_Block_B) {
    // Enable clocks
    initClks();

    // Configure SAI pins
    initPins();

    // Initialize DMA for block B
    if (hsai->Instance == SAI1_Block_B) {

      // Enable DMA clock
      __HAL_RCC_DMA1_CLK_ENABLE();

      // Enable DMA and sai interrupts
      HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

      // Init DMA
      initDMA(hsai);
    }

    // HAL_NVIC_SetPriority(SAI1_IRQn, 0, 0);
    // HAL_NVIC_EnableIRQ(SAI1_IRQn);
  }
}

bool SAIDriver::returnDMABusy() {
  return this->hsaiB.hdmatx->State == HAL_DMA_STATE_BUSY;
}

/**
 * @brief Transmits data to DMA which then transmits SAI to codec.
 *
 * This function transmits the provided data over SAI using DMA. It waits for
 * the DMA transfer to complete before returning.
 *
 * @param pData Pointer to the data buffer.
 * @param Size Size of the data buffer in bytes.
 * @param Timeout Timeout value in milliseconds.
 * @return 0 if transmit successful, 1 if DMA queue is full.
 */
int SAIDriver::txTransmit(uint8_t *pData, uint32_t Size, uint32_t Timeout) {
  // Ensure the size passed in is less than the size of the DMA buffer
  if (Size > this->dmaBufferWordSize) {
    return 1;
  }
  // Block transmission while the DMA is transferring
  while (returnDMABusy()) {
    // Polling block
    // HAL_Delay(1); // TODO: OPTIMIZE THIS DELAY
  }

  // Transmit data through DMA to SAI
  if (HAL_SAI_Transmit_DMA(&hsaiB, pData, Size) != HAL_OK) {
    __asm__ __volatile__("bkpt #0");
    return 1;
  }

  return 0;
}

/**
 * @brief Transmits data through the SAI peripheral.
 *
 * This function transmits the specified data directly through the SAI
 * peripheral.
 *
 * @param pData Pointer to the data buffer.
 * @param Size Size of the data buffer in bytes.
 * @param Timeout Timeout duration in milliseconds.
 */
void SAIDriver::SAINATransmit(uint8_t *pData, uint16_t Size, uint32_t Timeout) {
  if (HAL_SAI_Transmit(&hsaiA, pData, Size, Timeout) != HAL_OK) {
    __asm__ __volatile__("bkpt #3");
  }
}

/**
 * @brief DMA1 Stream0 interrupt handler.
 *
 * This function is the interrupt handler for DMA1 Stream0. It is called when
 * the DMA transfer associated with Stream0 completes or encounters an error. In
 * this function, the HAL_DMA_IRQHandler function is called to handle the
 * interrupt.
 */
extern "C" void DMA1_Stream0_IRQHandler() {
  HAL_DMA_IRQHandler(&hdma);

  // // Check if transfer is complete
  // if(__HAL_DMA_GET_FLAG(&hdma, DMA_FLAG_TCIF0_4)) {
  //     // Set the queue flag to false
  //     SAIDriver::dmaQueueFull = false;
  // }
}

/**
 * @brief Deinitializes the DMA for the SAI peripheral.
 *
 * This function deinitializes the DMA associated with the SAI peripheral.
 *
 * @param hsai Pointer to the SAI handle structure.
 */
void deInitDMA(SAI_HandleTypeDef *hsai) {
  // Deinitialize DMA
  HAL_DMA_DeInit(hsai->hdmatx);
}

/**
 * @brief Destructor for the SAIDriver class.
 *
 * This destructor performs necessary deinitialization for the SAI peripheral,
 * including:
 * - Deinitializing GPIO pins used by the SAI peripheral
 * - Disabling the SAI1 clock
 * - Disabling the GPIOE, GPIOG, GPIOA, and DMA1 clocks
 * - Disabling the SAI1 interrupt
 * - Deinitializing the SAI handles hsaiA and hsaiB
 */
SAIDriver::~SAIDriver() {
  // Perform any necessary deinitialization for SAI peripheral
  HAL_GPIO_DeInit(GPIOE, SAI1_SA_A);
  HAL_GPIO_DeInit(GPIOE, SAI1_SB_A);
  HAL_GPIO_DeInit(GPIOE, SAI1_FS_A);
  HAL_GPIO_DeInit(GPIOE, SAI1_SCK_A);
  HAL_GPIO_DeInit(GPIOE, SAI1_MCLK_A);
  __HAL_RCC_SAI1_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_DMA1_CLK_DISABLE();
  HAL_NVIC_DisableIRQ(SAI1_IRQn);
  // TODO: ADD DMA DEINIT
  HAL_SAI_DeInit(&hsaiA);
  deInitDMA(&hsaiB);
  HAL_SAI_DeInit(&hsaiB);
}