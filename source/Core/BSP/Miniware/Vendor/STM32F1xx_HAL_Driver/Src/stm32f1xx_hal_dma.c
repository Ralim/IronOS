/**
  ******************************************************************************
  * @file    stm32f1xx_hal_dma.c
  * @author  MCD Application Team
  * @brief   DMA HAL module driver.
  *         This file provides firmware functions to manage the following
  *         functionalities of the Direct Memory Access (DMA) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral State and errors functions
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
   (#) Enable and configure the peripheral to be connected to the DMA Channel
       (except for internal SRAM / FLASH memories: no initialization is
       necessary). Please refer to the Reference manual for connection between peripherals
       and DMA requests.

   (#) For a given Channel, program the required configuration through the following parameters:
       Channel request, Transfer Direction, Source and Destination data formats,
       Circular or Normal mode, Channel Priority level, Source and Destination Increment mode
       using HAL_DMA_Init() function.

   (#) Use HAL_DMA_GetState() function to return the DMA state and HAL_DMA_GetError() in case of error
       detection.

   (#) Use HAL_DMA_Abort() function to abort the current transfer

     -@-   In Memory-to-Memory transfer mode, Circular mode is not allowed.
     *** Polling mode IO operation ***
     =================================
    [..]
          (+) Use HAL_DMA_Start() to start DMA transfer after the configuration of Source
              address and destination address and the Length of data to be transferred
          (+) Use HAL_DMA_PollForTransfer() to poll for the end of current transfer, in this
              case a fixed Timeout can be configured by User depending from his application.

     *** Interrupt mode IO operation ***
     ===================================
    [..]
          (+) Configure the DMA interrupt priority using HAL_NVIC_SetPriority()
          (+) Enable the DMA IRQ handler using HAL_NVIC_EnableIRQ()
          (+) Use HAL_DMA_Start_IT() to start DMA transfer after the configuration of
              Source address and destination address and the Length of data to be transferred.
              In this case the DMA interrupt is configured
          (+) Use HAL_DMA_IRQHandler() called under DMA_IRQHandler() Interrupt subroutine
          (+) At the end of data transfer HAL_DMA_IRQHandler() function is executed and user can
              add his own function by customization of function pointer XferCpltCallback and
              XferErrorCallback (i.e. a member of DMA handle structure).

     *** DMA HAL driver macros list ***
     =============================================
      [..]
       Below the list of most used macros in DMA HAL driver.

       (+) __HAL_DMA_ENABLE: Enable the specified DMA Channel.
       (+) __HAL_DMA_DISABLE: Disable the specified DMA Channel.
       (+) __HAL_DMA_GET_FLAG: Get the DMA Channel pending flags.
       (+) __HAL_DMA_CLEAR_FLAG: Clear the DMA Channel pending flags.
       (+) __HAL_DMA_ENABLE_IT: Enable the specified DMA Channel interrupts.
       (+) __HAL_DMA_DISABLE_IT: Disable the specified DMA Channel interrupts.
       (+) __HAL_DMA_GET_IT_SOURCE: Check whether the specified DMA Channel interrupt has occurred or not.

     [..]
      (@) You can refer to the DMA HAL driver header file for more useful macros

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/** @addtogroup STM32F1xx_HAL_Driver
 * @{
 */

/** @defgroup DMA DMA
 * @brief DMA HAL module driver
 * @{
 */

#ifdef HAL_DMA_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup DMA_Private_Functions DMA Private Functions
 * @{
 */
static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
/**
 * @}
 */

/* Exported functions ---------------------------------------------------------*/

/** @defgroup DMA_Exported_Functions DMA Exported Functions
 * @{
 */

/** @defgroup DMA_Exported_Functions_Group1 Initialization and de-initialization functions
  *  @brief   Initialization and de-initialization functions
  *
@verbatim
 ===============================================================================
             ##### Initialization and de-initialization functions  #####
 ===============================================================================
    [..]
    This section provides functions allowing to initialize the DMA Channel source
    and destination addresses, incrementation and data sizes, transfer direction,
    circular/normal mode selection, memory-to-memory mode selection and Channel priority value.
    [..]
    The HAL_DMA_Init() function follows the DMA configuration procedures as described in
    reference manual.

@endverbatim
  * @{
  */

/**
 * @brief  Initialize the DMA according to the specified
 *         parameters in the DMA_InitTypeDef and initialize the associated handle.
 * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
 *               the configuration information for the specified DMA Channel.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma) {
  uint32_t tmp = 0U;

  /* Check the DMA handle allocation */
  if (hdma == NULL) {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));
  assert_param(IS_DMA_DIRECTION(hdma->Init.Direction));
  assert_param(IS_DMA_PERIPHERAL_INC_STATE(hdma->Init.PeriphInc));
  assert_param(IS_DMA_MEMORY_INC_STATE(hdma->Init.MemInc));
  assert_param(IS_DMA_PERIPHERAL_DATA_SIZE(hdma->Init.PeriphDataAlignment));
  assert_param(IS_DMA_MEMORY_DATA_SIZE(hdma->Init.MemDataAlignment));
  assert_param(IS_DMA_MODE(hdma->Init.Mode));
  assert_param(IS_DMA_PRIORITY(hdma->Init.Priority));

#if defined(STM32F101xE) || defined(STM32F101xG) || defined(STM32F103xE) || defined(STM32F103xG) || defined(STM32F100xE) || defined(STM32F105xC) || defined(STM32F107xC)
  /* calculation of the channel index */
  if ((uint32_t)(hdma->Instance) < (uint32_t)(DMA2_Channel1)) {
    /* DMA1 */
    hdma->ChannelIndex   = (((uint32_t)hdma->Instance - (uint32_t)DMA1_Channel1) / ((uint32_t)DMA1_Channel2 - (uint32_t)DMA1_Channel1)) << 2;
    hdma->DmaBaseAddress = DMA1;
  } else {
    /* DMA2 */
    hdma->ChannelIndex   = (((uint32_t)hdma->Instance - (uint32_t)DMA2_Channel1) / ((uint32_t)DMA2_Channel2 - (uint32_t)DMA2_Channel1)) << 2;
    hdma->DmaBaseAddress = DMA2;
  }
#else
  /* DMA1 */
  hdma->ChannelIndex   = (((uint32_t)hdma->Instance - (uint32_t)DMA1_Channel1) / ((uint32_t)DMA1_Channel2 - (uint32_t)DMA1_Channel1)) << 2;
  hdma->DmaBaseAddress = DMA1;
#endif /* STM32F101xE || STM32F101xG || STM32F103xE || STM32F103xG || STM32F100xE || STM32F105xC || STM32F107xC */

  /* Change DMA peripheral state */
  hdma->State = HAL_DMA_STATE_BUSY;

  /* Get the CR register value */
  tmp = hdma->Instance->CCR;

  /* Clear PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
  tmp &= ((uint32_t) ~(DMA_CCR_PL | DMA_CCR_MSIZE | DMA_CCR_PSIZE | DMA_CCR_MINC | DMA_CCR_PINC | DMA_CCR_CIRC | DMA_CCR_DIR));

  /* Prepare the DMA Channel configuration */
  tmp |= hdma->Init.Direction | hdma->Init.PeriphInc | hdma->Init.MemInc | hdma->Init.PeriphDataAlignment | hdma->Init.MemDataAlignment | hdma->Init.Mode | hdma->Init.Priority;

  /* Write to DMA Channel CR register */
  hdma->Instance->CCR = tmp;

  /* Initialise the error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

  /* Initialize the DMA state*/
  hdma->State = HAL_DMA_STATE_READY;
  /* Allocate lock resource and initialize it */
  hdma->Lock = HAL_UNLOCKED;

  return HAL_OK;
}

/**
 * @brief  DeInitialize the DMA peripheral.
 * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
 *               the configuration information for the specified DMA Channel.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma) {
  /* Check the DMA handle allocation */
  if (hdma == NULL) {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));

  /* Disable the selected DMA Channelx */
  __HAL_DMA_DISABLE(hdma);

  /* Reset DMA Channel control register */
  hdma->Instance->CCR = 0U;

  /* Reset DMA Channel Number of Data to Transfer register */
  hdma->Instance->CNDTR = 0U;

  /* Reset DMA Channel peripheral address register */
  hdma->Instance->CPAR = 0U;

  /* Reset DMA Channel memory address register */
  hdma->Instance->CMAR = 0U;

#if defined(STM32F101xE) || defined(STM32F101xG) || defined(STM32F103xE) || defined(STM32F103xG) || defined(STM32F100xE) || defined(STM32F105xC) || defined(STM32F107xC)
  /* calculation of the channel index */
  if ((uint32_t)(hdma->Instance) < (uint32_t)(DMA2_Channel1)) {
    /* DMA1 */
    hdma->ChannelIndex   = (((uint32_t)hdma->Instance - (uint32_t)DMA1_Channel1) / ((uint32_t)DMA1_Channel2 - (uint32_t)DMA1_Channel1)) << 2;
    hdma->DmaBaseAddress = DMA1;
  } else {
    /* DMA2 */
    hdma->ChannelIndex   = (((uint32_t)hdma->Instance - (uint32_t)DMA2_Channel1) / ((uint32_t)DMA2_Channel2 - (uint32_t)DMA2_Channel1)) << 2;
    hdma->DmaBaseAddress = DMA2;
  }
#else
  /* DMA1 */
  hdma->ChannelIndex   = (((uint32_t)hdma->Instance - (uint32_t)DMA1_Channel1) / ((uint32_t)DMA1_Channel2 - (uint32_t)DMA1_Channel1)) << 2;
  hdma->DmaBaseAddress = DMA1;
#endif /* STM32F101xE || STM32F101xG || STM32F103xE || STM32F103xG || STM32F100xE || STM32F105xC || STM32F107xC */

  /* Clear all flags */
  hdma->DmaBaseAddress->IFCR = (DMA_ISR_GIF1 << (hdma->ChannelIndex));

  /* Clean all callbacks */
  hdma->XferCpltCallback     = NULL;
  hdma->XferHalfCpltCallback = NULL;
  hdma->XferErrorCallback    = NULL;
  hdma->XferAbortCallback    = NULL;

  /* Reset the error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

  /* Reset the DMA state */
  hdma->State = HAL_DMA_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hdma);

  return HAL_OK;
}

/**
 * @}
 */

/** @defgroup DMA_Exported_Functions_Group2 Input and Output operation functions
  *  @brief   Input and Output operation functions
  *
@verbatim
 ===============================================================================
                      #####  IO operation functions  #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Configure the source, destination address and data length and Start DMA transfer
      (+) Configure the source, destination address and data length and
          Start DMA transfer with interrupt
      (+) Abort DMA transfer
      (+) Poll for transfer complete
      (+) Handle DMA interrupt request

@endverbatim
  * @{
  */

/**
 * @brief  Start the DMA Transfer.
 * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
 *               the configuration information for the specified DMA Channel.
 * @param  SrcAddress: The source memory Buffer address
 * @param  DstAddress: The destination memory Buffer address
 * @param  DataLength: The length of data to be transferred from source to destination
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_DMA_Start(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength) {
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));

  /* Process locked */
  __HAL_LOCK(hdma);

  if (HAL_DMA_STATE_READY == hdma->State) {
    /* Change DMA peripheral state */
    hdma->State     = HAL_DMA_STATE_BUSY;
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;

    /* Disable the peripheral */
    __HAL_DMA_DISABLE(hdma);

    /* Configure the source, destination address and the data length & clear flags*/
    DMA_SetConfig(hdma, SrcAddress, DstAddress, DataLength);

    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(hdma);
  } else {
    /* Process Unlocked */
    __HAL_UNLOCK(hdma);
    status = HAL_BUSY;
  }
  return status;
}

/**
 * @brief  Start the DMA Transfer with interrupt enabled.
 * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
 *               the configuration information for the specified DMA Channel.
 * @param  SrcAddress: The source memory Buffer address
 * @param  DstAddress: The destination memory Buffer address
 * @param  DataLength: The length of data to be transferred from source to destination
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength) {
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));

  /* Process locked */
  __HAL_LOCK(hdma);

  if (HAL_DMA_STATE_READY == hdma->State) {
    /* Change DMA peripheral state */
    hdma->State     = HAL_DMA_STATE_BUSY;
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;

    /* Disable the peripheral */
    __HAL_DMA_DISABLE(hdma);

    /* Configure the source, destination address and the data length & clear flags*/
    DMA_SetConfig(hdma, SrcAddress, DstAddress, DataLength);

    /* Enable the transfer complete interrupt */
    /* Enable the transfer Error interrupt */
    if (NULL != hdma->XferHalfCpltCallback) {
      /* Enable the Half transfer complete interrupt as well */
      __HAL_DMA_ENABLE_IT(hdma, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));
    } else {
      __HAL_DMA_DISABLE_IT(hdma, DMA_IT_HT);
      __HAL_DMA_ENABLE_IT(hdma, (DMA_IT_TC | DMA_IT_TE));
    }
    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(hdma);
  } else {
    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    /* Remain BUSY */
    status = HAL_BUSY;
  }
  return status;
}

/**
 * @brief  Abort the DMA Transfer.
 * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
 *               the configuration information for the specified DMA Channel.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma) {
  HAL_StatusTypeDef status = HAL_OK;

  /* Disable DMA IT */
  __HAL_DMA_DISABLE_IT(hdma, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));

  /* Disable the channel */
  __HAL_DMA_DISABLE(hdma);

  /* Clear all flags */
  hdma->DmaBaseAddress->IFCR = (DMA_ISR_GIF1 << hdma->ChannelIndex);

  /* Change the DMA state */
  hdma->State = HAL_DMA_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hdma);

  return status;
}

/**
 * @brief  Aborts the DMA Transfer in Interrupt mode.
 * @param  hdma  : pointer to a DMA_HandleTypeDef structure that contains
 *                 the configuration information for the specified DMA Channel.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma) {
  HAL_StatusTypeDef status = HAL_OK;

  if (HAL_DMA_STATE_BUSY != hdma->State) {
    /* no transfer ongoing */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

    status = HAL_ERROR;
  } else {
    /* Disable DMA IT */
    __HAL_DMA_DISABLE_IT(hdma, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));

    /* Disable the channel */
    __HAL_DMA_DISABLE(hdma);

    /* Clear all flags */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_GI_FLAG_INDEX(hdma));

    /* Change the DMA state */
    hdma->State = HAL_DMA_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    /* Call User Abort callback */
    if (hdma->XferAbortCallback != NULL) {
      hdma->XferAbortCallback(hdma);
    }
  }
  return status;
}

/**
 * @brief  Polling for transfer complete.
 * @param  hdma:    pointer to a DMA_HandleTypeDef structure that contains
 *                  the configuration information for the specified DMA Channel.
 * @param  CompleteLevel: Specifies the DMA level complete.
 * @param  Timeout:       Timeout duration.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *hdma, uint32_t CompleteLevel, uint32_t Timeout) {
  uint32_t temp;
  uint32_t tickstart = 0U;

  if (HAL_DMA_STATE_BUSY != hdma->State) {
    /* no transfer ongoing */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;
    __HAL_UNLOCK(hdma);
    return HAL_ERROR;
  }

  /* Polling mode not supported in circular mode */
  if (RESET != (hdma->Instance->CCR & DMA_CCR_CIRC)) {
    hdma->ErrorCode = HAL_DMA_ERROR_NOT_SUPPORTED;
    return HAL_ERROR;
  }

  /* Get the level transfer complete flag */
  if (CompleteLevel == HAL_DMA_FULL_TRANSFER) {
    /* Transfer Complete flag */
    temp = __HAL_DMA_GET_TC_FLAG_INDEX(hdma);
  } else {
    /* Half Transfer Complete flag */
    temp = __HAL_DMA_GET_HT_FLAG_INDEX(hdma);
  }

  /* Get tick */
  tickstart = HAL_GetTick();

  while (__HAL_DMA_GET_FLAG(hdma, temp) == RESET) {
    if ((__HAL_DMA_GET_FLAG(hdma, __HAL_DMA_GET_TE_FLAG_INDEX(hdma)) != RESET)) {
      /* When a DMA transfer error occurs */
      /* A hardware clear of its EN bits is performed */
      /* Clear all flags */
      hdma->DmaBaseAddress->IFCR = (DMA_ISR_GIF1 << hdma->ChannelIndex);

      /* Update error code */
      SET_BIT(hdma->ErrorCode, HAL_DMA_ERROR_TE);

      /* Change the DMA state */
      hdma->State = HAL_DMA_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hdma);

      return HAL_ERROR;
    }
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY) {
      if ((Timeout == 0U) || ((HAL_GetTick() - tickstart) > Timeout)) {
        /* Update error code */
        SET_BIT(hdma->ErrorCode, HAL_DMA_ERROR_TIMEOUT);

        /* Change the DMA state */
        hdma->State = HAL_DMA_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hdma);

        return HAL_ERROR;
      }
    }
  }

  if (CompleteLevel == HAL_DMA_FULL_TRANSFER) {
    /* Clear the transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));

    /* The selected Channelx EN bit is cleared (DMA is disabled and
    all transfers are complete) */
    hdma->State = HAL_DMA_STATE_READY;
  } else {
    /* Clear the half transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_HT_FLAG_INDEX(hdma));
  }

  /* Process unlocked */
  __HAL_UNLOCK(hdma);

  return HAL_OK;
}

/**
 * @brief  Handles DMA interrupt request.
 * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
 *               the configuration information for the specified DMA Channel.
 * @retval None
 */
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma) {
  uint32_t flag_it   = hdma->DmaBaseAddress->ISR;
  uint32_t source_it = hdma->Instance->CCR;

  /* Half Transfer Complete Interrupt management ******************************/
  if (((flag_it & (DMA_FLAG_HT1 << hdma->ChannelIndex)) != RESET) && ((source_it & DMA_IT_HT) != RESET)) {

    /* Clear the half transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_HT_FLAG_INDEX(hdma));

  }

  /* Transfer Complete Interrupt management ***********************************/
  else if (((flag_it & (DMA_FLAG_TC1 << hdma->ChannelIndex)) != RESET) && ((source_it & DMA_IT_TC) != RESET)) {
    if ((hdma->Instance->CCR & DMA_CCR_CIRC) == 0U) {
      /* Disable the transfer complete and error interrupt */
      __HAL_DMA_DISABLE_IT(hdma, DMA_IT_TE | DMA_IT_TC);

      /* Change the DMA state */
      hdma->State = HAL_DMA_STATE_READY;
    }
    /* Clear the transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    if (hdma->XferCpltCallback != NULL) {
      /* Transfer complete callback */
      hdma->XferCpltCallback(hdma);
    }
  }

  /* Transfer Error Interrupt management **************************************/
  else if ((RESET != (flag_it & (DMA_FLAG_TE1 << hdma->ChannelIndex))) && (RESET != (source_it & DMA_IT_TE))) {
    /* When a DMA transfer error occurs */
    /* A hardware clear of its EN bits is performed */
    /* Disable ALL DMA IT */
    __HAL_DMA_DISABLE_IT(hdma, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));

    /* Clear all flags */
    hdma->DmaBaseAddress->IFCR = (DMA_ISR_GIF1 << hdma->ChannelIndex);

    /* Update error code */
    hdma->ErrorCode = HAL_DMA_ERROR_TE;

    /* Change the DMA state */
    hdma->State = HAL_DMA_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    if (hdma->XferErrorCallback != NULL) {
      /* Transfer error callback */
      hdma->XferErrorCallback(hdma);
    }
  }
  return;
}

/**
 * @brief Register callbacks
 * @param hdma: pointer to a DMA_HandleTypeDef structure that contains
 *              the configuration information for the specified DMA Channel.
 * @param CallbackID: User Callback identifer
 *                    a HAL_DMA_CallbackIDTypeDef ENUM as parameter.
 * @param pCallback: pointer to private callbacsk function which has pointer to
 *                   a DMA_HandleTypeDef structure as parameter.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID, void (*pCallback)(DMA_HandleTypeDef *_hdma)) {
  HAL_StatusTypeDef status = HAL_OK;

  /* Process locked */
  __HAL_LOCK(hdma);

  if (HAL_DMA_STATE_READY == hdma->State) {
    switch (CallbackID) {
    case HAL_DMA_XFER_CPLT_CB_ID:
      hdma->XferCpltCallback = pCallback;
      break;

    case HAL_DMA_XFER_HALFCPLT_CB_ID:
      hdma->XferHalfCpltCallback = pCallback;
      break;

    case HAL_DMA_XFER_ERROR_CB_ID:
      hdma->XferErrorCallback = pCallback;
      break;

    case HAL_DMA_XFER_ABORT_CB_ID:
      hdma->XferAbortCallback = pCallback;
      break;

    default:
      status = HAL_ERROR;
      break;
    }
  } else {
    status = HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hdma);

  return status;
}

/**
 * @brief UnRegister callbacks
 * @param hdma: pointer to a DMA_HandleTypeDef structure that contains
 *              the configuration information for the specified DMA Channel.
 * @param CallbackID: User Callback identifer
 *                    a HAL_DMA_CallbackIDTypeDef ENUM as parameter.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID) {
  HAL_StatusTypeDef status = HAL_OK;

  /* Process locked */
  __HAL_LOCK(hdma);

  if (HAL_DMA_STATE_READY == hdma->State) {
    switch (CallbackID) {
    case HAL_DMA_XFER_CPLT_CB_ID:
      hdma->XferCpltCallback = NULL;
      break;

    case HAL_DMA_XFER_HALFCPLT_CB_ID:
      hdma->XferHalfCpltCallback = NULL;
      break;

    case HAL_DMA_XFER_ERROR_CB_ID:
      hdma->XferErrorCallback = NULL;
      break;

    case HAL_DMA_XFER_ABORT_CB_ID:
      hdma->XferAbortCallback = NULL;
      break;

    case HAL_DMA_XFER_ALL_CB_ID:
      hdma->XferCpltCallback     = NULL;
      hdma->XferHalfCpltCallback = NULL;
      hdma->XferErrorCallback    = NULL;
      hdma->XferAbortCallback    = NULL;
      break;

    default:
      status = HAL_ERROR;
      break;
    }
  } else {
    status = HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hdma);

  return status;
}

/**
 * @}
 */

/** @defgroup DMA_Exported_Functions_Group3 Peripheral State and Errors functions
  *  @brief    Peripheral State and Errors functions
  *
@verbatim
 ===============================================================================
            ##### Peripheral State and Errors functions #####
 ===============================================================================
    [..]
    This subsection provides functions allowing to
      (+) Check the DMA state
      (+) Get error code

@endverbatim
  * @{
  */

/**
 * @brief  Return the DMA hande state.
 * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
 *               the configuration information for the specified DMA Channel.
 * @retval HAL state
 */
HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma) {
  /* Return DMA handle state */
  return hdma->State;
}

/**
 * @brief  Return the DMA error code.
 * @param  hdma : pointer to a DMA_HandleTypeDef structure that contains
 *              the configuration information for the specified DMA Channel.
 * @retval DMA Error Code
 */
uint32_t HAL_DMA_GetError(DMA_HandleTypeDef *hdma) { return hdma->ErrorCode; }

/**
 * @}
 */

/**
 * @}
 */

/** @addtogroup DMA_Private_Functions
 * @{
 */

/**
 * @brief  Sets the DMA Transfer parameter.
 * @param  hdma:       pointer to a DMA_HandleTypeDef structure that contains
 *                     the configuration information for the specified DMA Channel.
 * @param  SrcAddress: The source memory Buffer address
 * @param  DstAddress: The destination memory Buffer address
 * @param  DataLength: The length of data to be transferred from source to destination
 * @retval HAL status
 */
static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength) {
  /* Clear all flags */
  hdma->DmaBaseAddress->IFCR = (DMA_ISR_GIF1 << hdma->ChannelIndex);

  /* Configure DMA Channel data length */
  hdma->Instance->CNDTR = DataLength;

  /* Memory to Peripheral */
  if ((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH) {
    /* Configure DMA Channel destination address */
    hdma->Instance->CPAR = DstAddress;

    /* Configure DMA Channel source address */
    hdma->Instance->CMAR = SrcAddress;
  }
  /* Peripheral to Memory */
  else {
    /* Configure DMA Channel source address */
    hdma->Instance->CPAR = SrcAddress;

    /* Configure DMA Channel destination address */
    hdma->Instance->CMAR = DstAddress;
  }
}

/**
 * @}
 */

#endif /* HAL_DMA_MODULE_ENABLED */
/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
