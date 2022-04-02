#ifndef _USB_DC_H
#define _USB_DC_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * USB endpoint direction and number.
 */
#define USB_EP_DIR_MASK 0x80U
#define USB_EP_DIR_IN   0x80U
#define USB_EP_DIR_OUT  0x00U

/** Get endpoint index (number) from endpoint address */
#define USB_EP_GET_IDX(ep) ((ep) & ~USB_EP_DIR_MASK)
/** Get direction from endpoint address */
#define USB_EP_GET_DIR(ep) ((ep)&USB_EP_DIR_MASK)
/** Get endpoint address from endpoint index and direction */
#define USB_EP_GET_ADDR(idx, dir) ((idx) | ((dir)&USB_EP_DIR_MASK))
/** True if the endpoint is an IN endpoint */
#define USB_EP_DIR_IS_IN(ep) (USB_EP_GET_DIR(ep) == USB_EP_DIR_IN)
/** True if the endpoint is an OUT endpoint */
#define USB_EP_DIR_IS_OUT(ep) (USB_EP_GET_DIR(ep) == USB_EP_DIR_OUT)

/**
 * USB endpoint Transfer Type mask.
 */
#define USBD_EP_TYPE_CTRL 0
#define USBD_EP_TYPE_ISOC 1
#define USBD_EP_TYPE_BULK 2
#define USBD_EP_TYPE_INTR 3
#define USBD_EP_TYPE_MASK 3

/* Default USB control EP, always 0 and 0x80 */
#define USB_CONTROL_OUT_EP0 0
#define USB_CONTROL_IN_EP0  0x80

/**
 * @brief USB Device Controller API
 * @defgroup _usb_device_controller_api USB Device Controller API
 * @{
 */
/**< maximum packet size (MPS) for EP 0 */
#define USB_CTRL_EP_MPS 64

/**
 * @brief USB Endpoint Transfer Type
 */
enum usb_dc_ep_transfer_type {
    /** Control type endpoint */
    USB_DC_EP_CONTROL = 0,
    /** Isochronous type endpoint */
    USB_DC_EP_ISOCHRONOUS,
    /** Bulk type endpoint */
    USB_DC_EP_BULK,
    /** Interrupt type endpoint  */
    USB_DC_EP_INTERRUPT
};

/**
 * @brief USB Endpoint Configuration.
 *
 * Structure containing the USB endpoint configuration.
 */
struct usbd_endpoint_cfg {
    /** The number associated with the EP in the device
     *  configuration structure
     *       IN  EP = 0x80 | \<endpoint number\>
     *       OUT EP = 0x00 | \<endpoint number\>
     */
    uint8_t ep_addr;
    /** Endpoint max packet size */
    uint16_t ep_mps;
    /** Endpoint Transfer Type.
     * May be Bulk, Interrupt, Control or Isochronous
     */
    enum usb_dc_ep_transfer_type ep_type;
};

/**
 * @brief USB Device Core Layer API
 * @defgroup _usb_device_core_api USB Device Core API
 * @{
 */

/**
 * @brief Set USB device address
 *
 * @param[in] addr Device address
 *
 * @return 0 on success, negative errno code on fail.
 */
int usbd_set_address(const uint8_t addr);

/*
 * @brief configure and enable endpoint
 *
 * This function sets endpoint configuration according to one specified in USB
 * endpoint descriptor and then enables it for data transfers.
 *
 * @param [in]  ep_desc Endpoint descriptor byte array
 *
 * @return true if successfully configured and enabled
 */
int usbd_ep_open(const struct usbd_endpoint_cfg *ep_cfg);
/**
 * @brief Disable the selected endpoint
 *
 * Function to disable the selected endpoint. Upon success interrupts are
 * disabled for the corresponding endpoint and the endpoint is no longer able
 * for transmitting/receiving data.
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_close(const uint8_t ep);
/**
 * @brief Set stall condition for the selected endpoint
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_set_stall(const uint8_t ep);
/**
 * @brief Clear stall condition for the selected endpoint
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_clear_stall(const uint8_t ep);
/**
 * @brief Check if the selected endpoint is stalled
 *
 * @param[in]  ep       Endpoint address corresponding to the one
 *                      listed in the device configuration table
 * @param[out] stalled  Endpoint stall status
 *
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_is_stalled(const uint8_t ep, uint8_t *stalled);
/**
 * @brief Write data to the specified endpoint
 *
 * This function is called to write data to the specified endpoint. The
 * supplied usbd_endpoint_callback function will be called when data is transmitted
 * out.
 *
 * @param[in]  ep        Endpoint address corresponding to the one
 *                       listed in the device configuration table
 * @param[in]  data      Pointer to data to write
 * @param[in]  data_len  Length of the data requested to write. This may
 *                       be zero for a zero length status packet.
 * @param[out] ret_bytes Bytes scheduled for transmission. This value
 *                       may be NULL if the application expects all
 *                       bytes to be written
 *
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_write(const uint8_t ep, const uint8_t *data, uint32_t data_len, uint32_t *ret_bytes);
/**
 * @brief Read data from the specified endpoint
 *
 * This is similar to usb_dc_ep_read, the difference being that, it doesn't
 * clear the endpoint NAKs so that the consumer is not bogged down by further
 * upcalls till he is done with the processing of the data. The caller should
 * reactivate ep by setting max_data_len 0 do so.
 *
 * @param[in]  ep           Endpoint address corresponding to the one
 *                          listed in the device configuration table
 * @param[in]  data         Pointer to data buffer to write to
 * @param[in]  max_data_len Max length of data to read
 * @param[out] read_bytes   Number of bytes read. If data is NULL and
 *                          max_data_len is 0 the number of bytes
 *                          available for read should be returned.
 *
 * @return 0 on success, negative errno code on fail.
 */
int usbd_ep_read(const uint8_t ep, uint8_t *data, uint32_t max_data_len, uint32_t *read_bytes);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
