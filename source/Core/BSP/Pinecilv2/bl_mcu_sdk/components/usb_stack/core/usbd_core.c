/**
 * @file usbd_core.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "usbd_core.h"
#include "usbd_winusb.h"

#define USBD_EP_CALLBACK_LIST_SEARCH   0
#define USBD_EP_CALLBACK_ARR_SEARCH    1
#define USBD_EP_CALLBACK_SEARCH_METHOD USBD_EP_CALLBACK_ARR_SEARCH

/* general descriptor field offsets */
#define DESC_bLength         0 /** Length offset */
#define DESC_bDescriptorType 1 /** Descriptor type offset */

/* config descriptor field offsets */
#define CONF_DESC_wTotalLength        2 /** Total length offset */
#define CONF_DESC_bConfigurationValue 5 /** Configuration value offset */
#define CONF_DESC_bmAttributes        7 /** configuration characteristics */

/* interface descriptor field offsets */
#define INTF_DESC_bInterfaceNumber  2 /** Interface number offset */
#define INTF_DESC_bAlternateSetting 3 /** Alternate setting offset */

#define USB_REQUEST_BUFFER_SIZE 256
#define USB_EP_OUT_NUM          8
#define USB_EP_IN_NUM           8

static struct usbd_core_cfg_priv {
    /** Setup packet */
    struct usb_setup_packet setup;
    /** Pointer to data buffer */
    uint8_t *ep0_data_buf;
    /** Remaining bytes in buffer */
    uint32_t ep0_data_buf_residue;
    /** Total length of control transfer */
    uint32_t ep0_data_buf_len;
    /** Zero length packet flag of control transfer */
    bool zlp_flag;
    /** Pointer to registered descriptors */
    const uint8_t *descriptors;
    /* Buffer used for storing standard, class and vendor request data */
    uint8_t req_data[USB_REQUEST_BUFFER_SIZE];

#if USBD_EP_CALLBACK_SEARCH_METHOD == 1
    usbd_endpoint_callback in_ep_cb[USB_EP_IN_NUM];
    usbd_endpoint_callback out_ep_cb[USB_EP_OUT_NUM];
#endif
    /** Variable to check whether the usb has been enabled */
    bool enabled;
    /** Variable to check whether the usb has been configured */
    bool configured;
    /** Currently selected configuration */
    uint8_t configuration;
    /** Remote wakeup feature status */
    uint16_t remote_wakeup;
    uint8_t reserved;
} usbd_core_cfg;

static usb_slist_t usbd_class_head = USB_SLIST_OBJECT_INIT(usbd_class_head);
static struct usb_msosv1_descriptor *msosv1_desc;
static struct usb_msosv2_descriptor *msosv2_desc;
static struct usb_bos_descriptor *bos_desc;

/**
 * @brief print the contents of a setup packet
 *
 * @param [in] setup The setup packet
 *
 */
static void usbd_print_setup(struct usb_setup_packet *setup)
{
    USBD_LOG_INFO("Setup: "
                  "bmRequestType 0x%02x, bRequest 0x%02x, wValue 0x%04x, wIndex 0x%04x, wLength 0x%04x\r\n",
                  setup->bmRequestType,
                  setup->bRequest,
                  setup->wValue,
                  setup->wIndex,
                  setup->wLength);
}

/**
 * @brief Check if the device is in Configured state
 *
 * @return true if Configured, false otherwise.
 */
static bool is_device_configured(void)
{
    return (usbd_core_cfg.configuration != 0);
}
/**
 * @brief Check if the interface of given number is valid
 *
 * @param [in] interface Number of the addressed interface
 *
 * This function searches through descriptor and checks
 * is the Host has addressed valid interface.
 *
 * @return true if interface exists - valid
 */
static bool is_interface_valid(uint8_t interface)
{
    const uint8_t *p = (uint8_t *)usbd_core_cfg.descriptors;
    const struct usb_configuration_descriptor *cfg_descr;

    /* Search through descriptor for matching interface */
    while (p[DESC_bLength] != 0U) {
        if (p[DESC_bDescriptorType] == USB_DESCRIPTOR_TYPE_CONFIGURATION) {
            cfg_descr = (const struct usb_configuration_descriptor *)p;

            if (interface < cfg_descr->bNumInterfaces) {
                return true;
            }
        }

        p += p[DESC_bLength];
    }

    return false;
}
/**
 * @brief Check if the endpoint of given address is valid
 *
 * @param [in] ep Address of the Endpoint
 *
 * This function checks if the Endpoint of given address
 * is valid for the configured device. Valid Endpoint is
 * either Control Endpoint or one used by the device.
 *
 * @return true if endpoint exists - valid
 */
static bool is_ep_valid(uint8_t ep)
{
    /* Check if its Endpoint 0 */
    if ((ep & 0x7f) == 0) {
        return true;
    }

    return true;
}
#if USBD_EP_CALLBACK_SEARCH_METHOD == 1
static void usbd_ep_callback_register(void)
{
    usb_slist_t *i, *j, *k;
    usb_slist_for_each(i, &usbd_class_head)
    {
        usbd_class_t *class = usb_slist_entry(i, struct usbd_class, list);

        usb_slist_for_each(j, &class->intf_list)
        {
            usbd_interface_t *intf = usb_slist_entry(j, struct usbd_interface, list);

            usb_slist_for_each(k, &intf->ep_list)
            {
                usbd_endpoint_t *ept = usb_slist_entry(k, struct usbd_endpoint, list);

                if (ept->ep_cb) {
                    if (ept->ep_addr & 0x80) {
                        usbd_core_cfg.in_ep_cb[ept->ep_addr & 0x7f] = ept->ep_cb;
                    } else {
                        usbd_core_cfg.out_ep_cb[ept->ep_addr & 0x7f] = ept->ep_cb;
                    }
                }
            }
        }
    }
}
#endif
/**
 * @brief configure and enable endpoint
 *
 * This function sets endpoint configuration according to one specified in USB
 * endpoint descriptor and then enables it for data transfers.
 *
 * @param [in]  ep_desc Endpoint descriptor byte array
 *
 * @return true if successfully configured and enabled
 */
static bool usbd_set_endpoint(const struct usb_endpoint_descriptor *ep_desc)
{
    struct usbd_endpoint_cfg ep_cfg;

    ep_cfg.ep_addr = ep_desc->bEndpointAddress;
    ep_cfg.ep_mps = ep_desc->wMaxPacketSize;
    ep_cfg.ep_type = ep_desc->bmAttributes & USBD_EP_TYPE_MASK;

    USBD_LOG_INFO("Open endpoint:0x%x type:%u mps:%u\r\n",
                  ep_cfg.ep_addr, ep_cfg.ep_type, ep_cfg.ep_mps);

    usbd_ep_open(&ep_cfg);
    usbd_core_cfg.configured = true;

    return true;
}
/**
 * @brief Disable endpoint for transferring data
 *
 * This function cancels transfers that are associated with endpoint and
 * disabled endpoint itself.
 *
 * @param [in]  ep_desc Endpoint descriptor byte array
 *
 * @return true if successfully deconfigured and disabled
 */
static bool usbd_reset_endpoint(const struct usb_endpoint_descriptor *ep_desc)
{
    struct usbd_endpoint_cfg ep_cfg;

    ep_cfg.ep_addr = ep_desc->bEndpointAddress;
    ep_cfg.ep_mps = ep_desc->wMaxPacketSize;
    ep_cfg.ep_type = ep_desc->bmAttributes & USBD_EP_TYPE_MASK;

    USBD_LOG_INFO("Close endpoint:0x%x type:%u\r\n",
                  ep_cfg.ep_addr, ep_cfg.ep_type);

    usbd_ep_close(ep_cfg.ep_addr);

    return true;
}

/**
 * @brief get specified USB descriptor
 *
 * This function parses the list of installed USB descriptors and attempts
 * to find the specified USB descriptor.
 *
 * @param [in]  type_index Type and index of the descriptor
 * @param [in]  lang_id    Language ID of the descriptor (currently unused)
 * @param [out] len        Descriptor length
 * @param [out] data       Descriptor data
 *
 * @return true if the descriptor was found, false otherwise
 */
static bool usbd_get_descriptor(uint16_t type_index, uint8_t **data, uint32_t *len)
{
    uint8_t type = 0U;
    uint8_t index = 0U;
    uint8_t *p = NULL;
    uint32_t cur_index = 0U;
    bool found = false;

    type = GET_DESC_TYPE(type_index);
    index = GET_DESC_INDEX(type_index);

    if ((type == USB_DESCRIPTOR_TYPE_STRING) && (index == USB_OSDESC_STRING_DESC_INDEX)) {
        USBD_LOG_INFO("read MS OS 2.0 descriptor string\r\n");

        if (!msosv1_desc) {
            return false;
        }

        *data = (uint8_t *)msosv1_desc->string;
        *len = msosv1_desc->string_len;

        return true;
    } else if (type == USB_DESCRIPTOR_TYPE_BINARY_OBJECT_STORE) {
        USBD_LOG_INFO("read BOS descriptor string\r\n");

        if (!bos_desc) {
            return false;
        }

        *data = bos_desc->string;
        *len = bos_desc->string_len;
        return true;
    }
    /*
     * Invalid types of descriptors,
     * see USB Spec. Revision 2.0, 9.4.3 Get Descriptor
     */
    else if ((type == USB_DESCRIPTOR_TYPE_INTERFACE) || (type == USB_DESCRIPTOR_TYPE_ENDPOINT) ||
#ifndef CONFIG_USB_HS
             (type > USB_DESCRIPTOR_TYPE_ENDPOINT)) {
#else
             (type > USB_DESCRIPTOR_TYPE_OTHER_SPEED)) {
#endif
        return false;
    }

    p = (uint8_t *)usbd_core_cfg.descriptors;

    cur_index = 0U;

    while (p[DESC_bLength] != 0U) {
        if (p[DESC_bDescriptorType] == type) {
            if (cur_index == index) {
                found = true;
                break;
            }

            cur_index++;
        }

        /* skip to next descriptor */
        p += p[DESC_bLength];
    }

    if (found) {
        /* set data pointer */
        *data = p;

        /* get length from structure */
        if (type == USB_DESCRIPTOR_TYPE_CONFIGURATION) {
            /* configuration descriptor is an
             * exception, length is at offset
             * 2 and 3
             */
            *len = (p[CONF_DESC_wTotalLength]) |
                   (p[CONF_DESC_wTotalLength + 1] << 8);
        } else {
            /* normally length is at offset 0 */
            *len = p[DESC_bLength];
        }
    } else {
        /* nothing found */
        USBD_LOG_ERR("descriptor <type:%x,index:%x> not found!\r\n", type, index);
    }

    return found;
}

/**
 * @brief set USB configuration
 *
 * This function configures the device according to the specified configuration
 * index and alternate setting by parsing the installed USB descriptor list.
 * A configuration index of 0 unconfigures the device.
 *
 * @param [in] config_index Configuration index
 * @param [in] alt_setting  Alternate setting number
 *
 * @return true if successfully configured false if error or unconfigured
 */
static bool usbd_set_configuration(uint8_t config_index, uint8_t alt_setting)
{
    uint8_t *p = (uint8_t *)usbd_core_cfg.descriptors;
    uint8_t cur_alt_setting = 0xFF;
    uint8_t cur_config = 0xFF;
    bool found = false;

    if (config_index == 0U) {
        /* TODO: unconfigure device */
        USBD_LOG_ERR("Device not configured - invalid configuration\r\n");
        return true;
    }

    /* configure endpoints for this configuration/altsetting */
    while (p[DESC_bLength] != 0U) {
        switch (p[DESC_bDescriptorType]) {
            case USB_DESCRIPTOR_TYPE_CONFIGURATION:
                /* remember current configuration index */
                cur_config = p[CONF_DESC_bConfigurationValue];

                if (cur_config == config_index) {
                    found = true;
                }

                break;

            case USB_DESCRIPTOR_TYPE_INTERFACE:
                /* remember current alternate setting */
                cur_alt_setting =
                    p[INTF_DESC_bAlternateSetting];
                break;

            case USB_DESCRIPTOR_TYPE_ENDPOINT:
                if ((cur_config != config_index) ||
                    (cur_alt_setting != alt_setting)) {
                    break;
                }

                found = usbd_set_endpoint((struct usb_endpoint_descriptor *)p);
                break;

            default:
                break;
        }

        /* skip to next descriptor */
        p += p[DESC_bLength];
    }

    return found;
}

/**
 * @brief set USB interface
 *
 * @param [in] iface Interface index
 * @param [in] alt_setting  Alternate setting number
 *
 * @return true if successfully configured false if error or unconfigured
 */
static bool usbd_set_interface(uint8_t iface, uint8_t alt_setting)
{
    const uint8_t *p = usbd_core_cfg.descriptors;
    const uint8_t *if_desc = NULL;
    struct usb_endpoint_descriptor *ep_desc;
    uint8_t cur_alt_setting = 0xFF;
    uint8_t cur_iface = 0xFF;
    bool ret = false;

    USBD_LOG_DBG("iface %u alt_setting %u\r\n", iface, alt_setting);

    while (p[DESC_bLength] != 0U) {
        switch (p[DESC_bDescriptorType]) {
            case USB_DESCRIPTOR_TYPE_INTERFACE:
                /* remember current alternate setting */
                cur_alt_setting = p[INTF_DESC_bAlternateSetting];
                cur_iface = p[INTF_DESC_bInterfaceNumber];

                if (cur_iface == iface &&
                    cur_alt_setting == alt_setting) {
                    if_desc = (void *)p;
                }

                USBD_LOG_DBG("Current iface %u alt setting %u",
                             cur_iface, cur_alt_setting);
                break;

            case USB_DESCRIPTOR_TYPE_ENDPOINT:
                if (cur_iface == iface) {
                    ep_desc = (struct usb_endpoint_descriptor *)p;

                    if (cur_alt_setting != alt_setting) {
                        ret = usbd_reset_endpoint(ep_desc);
                    } else {
                        ret = usbd_set_endpoint(ep_desc);
                    }
                }

                break;

            default:
                break;
        }

        /* skip to next descriptor */
        p += p[DESC_bLength];
    }

    usbd_event_notify_handler(USB_EVENT_SET_INTERFACE, (void *)if_desc);

    return ret;
}

/**
 * @brief handle a standard device request
 *
 * @param [in]     setup    The setup packet
 * @param [in,out] len      Pointer to data length
 * @param [in,out] ep0_data_buf Data buffer
 *
 * @return true if the request was handled successfully
 */
static bool usbd_std_device_req_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    uint16_t value = setup->wValue;
    // uint16_t index = setup->wIndex;
    bool ret = true;

    switch (setup->bRequest) {
        case USB_REQUEST_GET_STATUS:
            USBD_LOG_DBG("REQ_GET_STATUS\r\n");
            /* bit 0: self-powered */
            /* bit 1: remote wakeup */
            *data = (uint8_t *)&usbd_core_cfg.remote_wakeup;

            *len = 2;
            break;

        case USB_REQUEST_CLEAR_FEATURE:
            USBD_LOG_DBG("REQ_CLEAR_FEATURE\r\n");
            ret = false;

            if (value == USB_FEATURE_REMOTE_WAKEUP) {
                usbd_core_cfg.remote_wakeup = 0;
                usbd_event_notify_handler(USB_EVENT_CLEAR_REMOTE_WAKEUP, NULL);
                ret = true;
            }

            break;

        case USB_REQUEST_SET_FEATURE:
            USBD_LOG_DBG("REQ_SET_FEATURE\r\n");
            ret = false;

            if (value == USB_FEATURE_REMOTE_WAKEUP) {
                usbd_core_cfg.remote_wakeup = 1;
                usbd_event_notify_handler(USB_EVENT_SET_REMOTE_WAKEUP, NULL);
                ret = true;
            }

            if (value == USB_FEATURE_TEST_MODE) {
                /* put TEST_MODE code here */
            }

            break;

        case USB_REQUEST_SET_ADDRESS:
            USBD_LOG_DBG("REQ_SET_ADDRESS, addr 0x%x\r\n", value);
            usbd_set_address(value);
            break;

        case USB_REQUEST_GET_DESCRIPTOR:
            USBD_LOG_DBG("REQ_GET_DESCRIPTOR\r\n");
            ret = usbd_get_descriptor(value, data, len);
            break;

        case USB_REQUEST_SET_DESCRIPTOR:
            USBD_LOG_DBG("Device req 0x%02x not implemented\r\n", setup->bRequest);
            ret = false;
            break;

        case USB_REQUEST_GET_CONFIGURATION:
            USBD_LOG_DBG("REQ_GET_CONFIGURATION\r\n");
            /* indicate if we are configured */
            *data = (uint8_t *)&usbd_core_cfg.configuration;
            *len = 1;
            break;

        case USB_REQUEST_SET_CONFIGURATION:
            value &= 0xFF;
            USBD_LOG_DBG("REQ_SET_CONFIGURATION, conf 0x%x\r\n", value);

            if (!usbd_set_configuration(value, 0)) {
                USBD_LOG_DBG("USB Set Configuration failed\r\n");
                ret = false;
            } else {
                /* configuration successful,
                 * update current configuration
                 */
                usbd_core_cfg.configuration = value;
                usbd_event_notify_handler(USB_EVENT_CONFIGURED, NULL);
            }

            break;

        case USB_REQUEST_GET_INTERFACE:
            break;

        case USB_REQUEST_SET_INTERFACE:
            break;

        default:
            USBD_LOG_ERR("Illegal device req 0x%02x\r\n", setup->bRequest);
            ret = false;
            break;
    }

    return ret;
}

/**
 * @brief handle a standard interface request
 *
 * @param [in]     setup    The setup packet
 * @param [in,out] len      Pointer to data length
 * @param [in]     ep0_data_buf Data buffer
 *
 * @return true if the request was handled successfully
 */
static bool usbd_std_interface_req_handler(struct usb_setup_packet *setup,
                                           uint8_t **data, uint32_t *len)
{
    /** The device must be configured to accept standard interface
     * requests and the addressed Interface must be valid.
     */
    if (!is_device_configured() ||
        (!is_interface_valid((uint8_t)setup->wIndex))) {
        return false;
    }

    switch (setup->bRequest) {
        case USB_REQUEST_GET_STATUS:
            /* no bits specified */
            *data = (uint8_t *)&usbd_core_cfg.remote_wakeup;

            *len = 2;
            break;

        case USB_REQUEST_CLEAR_FEATURE:
        case USB_REQUEST_SET_FEATURE:
            /* not defined for interface */
            return false;

        case USB_REQUEST_GET_INTERFACE:
            /** This handler is called for classes that does not support
             * alternate Interfaces so always return 0. Classes that
             * support alternative interfaces handles GET_INTERFACE
             * in custom_handler.
             */
            *data = (uint8_t *)&usbd_core_cfg.reserved;

            *len = 1;
            break;

        case USB_REQUEST_SET_INTERFACE:
            USBD_LOG_DBG("REQ_SET_INTERFACE\r\n");
            usbd_set_interface(setup->wIndex, setup->wValue);
            break;

        default:
            USBD_LOG_ERR("Illegal interface req 0x%02x\r\n", setup->bRequest);
            return false;
    }

    return true;
}

/**
 * @brief handle a standard endpoint request
 *
 * @param [in]     setup    The setup packet
 * @param [in,out] len      Pointer to data length
 * @param [in]     ep0_data_buf Data buffer
 *
 * @return true if the request was handled successfully
 */
static bool usbd_std_endpoint_req_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    uint8_t ep = (uint8_t)setup->wIndex;

    /* Check if request addresses valid Endpoint */
    if (!is_ep_valid(ep)) {
        return false;
    }

    switch (setup->bRequest) {
        case USB_REQUEST_GET_STATUS:

            /** This request is valid for Control Endpoints when
             * the device is not yet configured. For other
             * Endpoints the device must be configured.
             * Firstly check if addressed ep is Control Endpoint.
             * If no then the device must be in Configured state
             * to accept the request.
             */
            if (((ep & 0x7f) == 0) || is_device_configured()) {
                /* bit 0 - Endpoint halted or not */
                usbd_ep_is_stalled(ep, (uint8_t *)&usbd_core_cfg.remote_wakeup);
                *data = (uint8_t *)&usbd_core_cfg.remote_wakeup;

                *len = 2;
                break;
            }

            return false;

        case USB_REQUEST_CLEAR_FEATURE:
            if (setup->wValue == USB_FEATURE_ENDPOINT_STALL) {
                /** This request is valid for Control Endpoints when
                 * the device is not yet configured. For other
                 * Endpoints the device must be configured.
                 * Firstly check if addressed ep is Control Endpoint.
                 * If no then the device must be in Configured state
                 * to accept the request.
                 */
                if (((ep & 0x7f) == 0) || is_device_configured()) {
                    USBD_LOG_ERR("ep:%x clear halt\r\n", ep);
                    usbd_ep_clear_stall(ep);
                    usbd_event_notify_handler(USB_EVENT_CLEAR_HALT, NULL);
                    break;
                }
            }

            /* only ENDPOINT_HALT defined for endpoints */
            return false;

        case USB_REQUEST_SET_FEATURE:
            if (setup->wValue == USB_FEATURE_ENDPOINT_STALL) {
                /** This request is valid for Control Endpoints when
                 * the device is not yet configured. For other
                 * Endpoints the device must be configured.
                 * Firstly check if addressed ep is Control Endpoint.
                 * If no then the device must be in Configured state
                 * to accept the request.
                 */
                if (((ep & 0x7f) == 0) || is_device_configured()) {
                    /* set HALT by stalling */
                    USBD_LOG_ERR("ep:%x set halt\r\n", ep);
                    usbd_ep_set_stall(ep);
                    usbd_event_notify_handler(USB_EVENT_SET_HALT, NULL);
                    break;
                }
            }

            /* only ENDPOINT_HALT defined for endpoints */
            return false;

        case USB_REQUEST_SYNCH_FRAME:

            /* For Synch Frame request the device must be configured */
            if (is_device_configured()) {
                /* Not supported, return false anyway */
                USBD_LOG_DBG("ep req 0x%02x not implemented\r\n", setup->bRequest);
            }

            return false;

        default:
            USBD_LOG_ERR("Illegal ep req 0x%02x\r\n", setup->bRequest);
            return false;
    }

    return true;
}

/**
 * @brief default handler for standard ('chapter 9') requests
 *
 * If a custom request handler was installed, this handler is called first.
 *
 * @param [in]     setup    The setup packet
 * @param [in]     ep0_data_buf Data buffer
 * @param [in,out] len      Pointer to data length
 *
 * @return true if the request was handled successfully
 */
static int usbd_standard_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    int rc = 0;

    switch (setup->bmRequestType_b.Recipient) {
        case USB_REQUEST_TO_DEVICE:
            if (usbd_std_device_req_handler(setup, data, len) == false) {
                rc = -1;
            }

            break;

        case USB_REQUEST_TO_INTERFACE:
            if (usbd_std_interface_req_handler(setup, data, len) == false) {
                rc = -1;
            }

            break;

        case USB_REQUEST_TO_ENDPOINT:
            if (usbd_std_endpoint_req_handler(setup, data, len) == false) {
                rc = -1;
            }

            break;

        default:
            rc = -1;
    }

    return rc;
}

/*
 * The functions usbd_class_request_handler(), usbd_custom_request_handler() and usbd_vendor_request_handler()
 * go through the interfaces one after the other and compare the
 * bInterfaceNumber with the wIndex and and then call the appropriate
 * callback of the USB function.
 * Note, a USB function can have more than one interface and the
 * request does not have to be directed to the first interface (unlikely).
 * These functions can be simplified and moved to usb_handle_request()
 * when legacy initialization throgh the usb_set_config() and
 * usb_enable() is no longer needed.
 */
static int usbd_class_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG_DBG("bRequest 0x%02x, wIndex 0x%04x\r\n", setup->bRequest, setup->wIndex);

    if (setup->bmRequestType_b.Recipient != USB_REQUEST_TO_INTERFACE) {
        return -1;
    }

    usb_slist_t *i, *j;
    usb_slist_for_each(i, &usbd_class_head)
    {
        usbd_class_t *class = usb_slist_entry(i, struct usbd_class, list);

        usb_slist_for_each(j, &class->intf_list)
        {
            usbd_interface_t *intf = usb_slist_entry(j, struct usbd_interface, list);

            if (intf->class_handler && (intf->intf_num == (setup->wIndex & 0xFF))) {
                return intf->class_handler(setup, data, len);
            }
        }
    }
    return -1;
}

static int usbd_vendor_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG_DBG("bRequest 0x%02x, wValue0x%04x, wIndex 0x%04x\r\n", setup->bRequest, setup->wValue, setup->wIndex);

    // if(setup->bmRequestType_b.Recipient != USB_REQUEST_TO_DEVICE)
    // {
    //     return -1;
    // }

    if (msosv1_desc) {
        if (setup->bRequest == msosv1_desc->vendor_code) {
            switch (setup->wIndex) {
                case 0x04:
                    USBD_LOG_INFO("get Compat ID\r\n");
                    *data = (uint8_t *)msosv1_desc->compat_id;
                    *len = msosv1_desc->compat_id_len;

                    return 0;
                case 0x05:
                    USBD_LOG_INFO("get Compat id properties\r\n");
                    *data = (uint8_t *)msosv1_desc->comp_id_property;
                    *len = msosv1_desc->comp_id_property_len;

                    return 0;
                default:
                    USBD_LOG_ERR("unknown vendor code\r\n");
                    return -1;
            }
        }
    } else if (msosv2_desc) {
        if (setup->bRequest == msosv2_desc->vendor_code) {
            switch (setup->wIndex) {
                case WINUSB_REQUEST_GET_DESCRIPTOR_SET:
                    USBD_LOG_INFO("GET MS OS 2.0 Descriptor\r\n");
                    *data = (uint8_t *)msosv2_desc->compat_id;
                    *len = msosv2_desc->compat_id_len;
                    return 0;
                default:
                    USBD_LOG_ERR("unknown vendor code\r\n");
                    return -1;
            }
        }
    }

    usb_slist_t *i, *j;

    usb_slist_for_each(i, &usbd_class_head)
    {
        usbd_class_t *class = usb_slist_entry(i, struct usbd_class, list);

        usb_slist_for_each(j, &class->intf_list)
        {
            usbd_interface_t *intf = usb_slist_entry(j, struct usbd_interface, list);

            if (intf->vendor_handler && !intf->vendor_handler(setup, data, len)) {
                return 0;
            }
        }
    }

    return -1;
}

static int usbd_custom_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG_DBG("bRequest 0x%02x, wIndex 0x%04x\r\n", setup->bRequest, setup->wIndex);

    if (setup->bmRequestType_b.Recipient != USB_REQUEST_TO_INTERFACE) {
        return -1;
    }

    usb_slist_t *i, *j;
    usb_slist_for_each(i, &usbd_class_head)
    {
        usbd_class_t *class = usb_slist_entry(i, struct usbd_class, list);

        usb_slist_for_each(j, &class->intf_list)
        {
            usbd_interface_t *intf = usb_slist_entry(j, struct usbd_interface, list);

            if (intf->custom_handler && (intf->intf_num == (setup->wIndex & 0xFF))) {
                return intf->custom_handler(setup, data, len);
            }
        }
    }

    return -1;
}

/**
 * @brief handle a request by calling one of the installed request handlers
 *
 * Local function to handle a request by calling one of the installed request
 * handlers. In case of data going from host to device, the data is at *ppbData.
 * In case of data going from device to host, the handler can either choose to
 * write its data at *ppbData or update the data pointer.
 *
 * @param [in]     setup The setup packet
 * @param [in,out] data  Data buffer
 * @param [in,out] len   Pointer to data length
 *
 * @return true if the request was handles successfully
 */
static bool usbd_setup_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    uint8_t type = setup->bmRequestType_b.Type;

    if (type == USB_REQUEST_STANDARD) {
        if (!usbd_custom_request_handler(setup, data, len)) {
            return true;
        }

        if (usbd_standard_request_handler(setup, data, len) < 0) {
            USBD_LOG_ERR("Handler Error %d\r\n", type);
            usbd_print_setup(setup);
            return false;
        }
    } else if (type == USB_REQUEST_CLASS) {
        if (usbd_class_request_handler(setup, data, len) < 0) {
            USBD_LOG_ERR("Handler Error %d\r\n", type);
            usbd_print_setup(setup);
            return false;
        }
    } else if (type == USB_REQUEST_VENDOR) {
        if (usbd_vendor_request_handler(setup, data, len) < 0) {
            USBD_LOG_ERR("Handler Error %d\r\n", type);
            usbd_print_setup(setup);
            return false;
        }
    } else {
        return false;
    }

    return true;
}
/**
 * @brief send data or status to host
 *
 * @return N/A
 */
static void usbd_send_to_host(uint16_t len)
{
    uint32_t chunk = 0U;

    if (usbd_core_cfg.zlp_flag == false) {
        chunk = usbd_core_cfg.ep0_data_buf_residue;

        if (usbd_ep_write(USB_CONTROL_IN_EP0, usbd_core_cfg.ep0_data_buf, usbd_core_cfg.ep0_data_buf_residue, &chunk) < 0) {
            USBD_LOG_ERR("USB write data failed\r\n");
            return;
        }

        usbd_core_cfg.ep0_data_buf += chunk;
        usbd_core_cfg.ep0_data_buf_residue -= chunk;

        /*
         * Set ZLP flag when host asks for a bigger length and the
         * last chunk is wMaxPacketSize long, to indicate the last
         * packet.
         */
        if ((!usbd_core_cfg.ep0_data_buf_residue) && (usbd_core_cfg.ep0_data_buf_len >= USB_CTRL_EP_MPS) && !(usbd_core_cfg.ep0_data_buf_len % USB_CTRL_EP_MPS)) {
            /* Transfers a zero-length packet next*/
            usbd_core_cfg.zlp_flag = true;
        }
    } else {
        usbd_core_cfg.zlp_flag = false;
        USBD_LOG_DBG("send zlp\r\n");
        if (usbd_ep_write(USB_CONTROL_IN_EP0, NULL, 0, NULL) < 0) {
            USBD_LOG_ERR("USB write zlp failed\r\n");
            return;
        }
    }
}

static void usbd_ep0_setup_handler(void)
{
    struct usb_setup_packet *setup = &usbd_core_cfg.setup;

    /*
    * OUT transfer, Setup packet,
    * reset request message state machine
    */
    if (usbd_ep_read(USB_CONTROL_OUT_EP0, (uint8_t *)setup,
                     sizeof(struct usb_setup_packet), NULL) < 0) {
        USBD_LOG_ERR("Read Setup Packet failed\r\n");
        usbd_ep_set_stall(USB_CONTROL_IN_EP0);
        return;
    }

    //usbd_print_setup(setup);

    if (setup->wLength > USB_REQUEST_BUFFER_SIZE) {
        if (setup->bmRequestType_b.Dir != USB_REQUEST_DEVICE_TO_HOST) {
            USBD_LOG_ERR("Request buffer too small\r\n");
            usbd_ep_set_stall(USB_CONTROL_IN_EP0);
            return;
        }
    }

    // usbd_core_cfg.ep0_data_buf = usbd_core_cfg.req_data;
    usbd_core_cfg.ep0_data_buf_residue = setup->wLength;
    usbd_core_cfg.ep0_data_buf_len = setup->wLength;
    usbd_core_cfg.zlp_flag = false;

    /* this maybe set code in class request code  */
    if (setup->wLength &&
        setup->bmRequestType_b.Dir == USB_REQUEST_HOST_TO_DEVICE) {
        USBD_LOG_DBG("prepare to out data\r\n");
        return;
    }

    /* Ask installed handler to process request */
    if (!usbd_setup_request_handler(setup, &usbd_core_cfg.ep0_data_buf, &usbd_core_cfg.ep0_data_buf_len)) {
        USBD_LOG_ERR("usbd_setup_request_handler failed\r\n");
        usbd_ep_set_stall(USB_CONTROL_IN_EP0);
        return;
    }

    /* Send smallest of requested and offered length */
    usbd_core_cfg.ep0_data_buf_residue = MIN(usbd_core_cfg.ep0_data_buf_len,
                                             setup->wLength);
    /*Send data or status to host*/
    usbd_send_to_host(setup->wLength);
}

static void usbd_ep0_out_handler(void)
{
    uint32_t chunk = 0U;
    struct usb_setup_packet *setup = &usbd_core_cfg.setup;

    /* OUT transfer, status packets */
    if (usbd_core_cfg.ep0_data_buf_residue == 0) {
        /* absorb zero-length status message */
        USBD_LOG_DBG("recv status\r\n");

        if (usbd_ep_read(USB_CONTROL_OUT_EP0,
                         NULL,
                         0, NULL) < 0) {
            USBD_LOG_ERR("Read DATA Packet failed\r\n");
            usbd_ep_set_stall(USB_CONTROL_IN_EP0);
        }

        return;
    }

    usbd_core_cfg.ep0_data_buf = usbd_core_cfg.req_data;

    /* OUT transfer, data packets */
    if (usbd_ep_read(USB_CONTROL_OUT_EP0,
                     usbd_core_cfg.ep0_data_buf,
                     usbd_core_cfg.ep0_data_buf_residue, &chunk) < 0) {
        USBD_LOG_ERR("Read DATA Packet failed\r\n");
        usbd_ep_set_stall(USB_CONTROL_IN_EP0);
        return;
    }

    usbd_core_cfg.ep0_data_buf += chunk;
    usbd_core_cfg.ep0_data_buf_residue -= chunk;

    if (usbd_core_cfg.ep0_data_buf_residue == 0) {
        /* Received all, send data to handler */
        usbd_core_cfg.ep0_data_buf = usbd_core_cfg.req_data;

        if (!usbd_setup_request_handler(setup, &usbd_core_cfg.ep0_data_buf, &usbd_core_cfg.ep0_data_buf_len)) {
            USBD_LOG_ERR("usbd_setup_request_handler1 failed\r\n");
            usbd_ep_set_stall(USB_CONTROL_IN_EP0);
            return;
        }

        /*Send status to host*/
        usbd_send_to_host(setup->wLength);
    } else {
        USBD_LOG_ERR("ep0_data_buf_residue is not zero\r\n");
    }
}

static void usbd_ep0_in_handler(void)
{
    struct usb_setup_packet *setup = &usbd_core_cfg.setup;

    /* Send more data if available */
    if (usbd_core_cfg.ep0_data_buf_residue != 0 || usbd_core_cfg.zlp_flag == true) {
        usbd_send_to_host(setup->wLength);
    } else {
        /*ep0 tx has completed,and no data to send,so do nothing*/
    }
}

static void usbd_ep_out_handler(uint8_t ep)
{
#if USBD_EP_CALLBACK_SEARCH_METHOD == 0
    usb_slist_t *i, *j, *k;
    usb_slist_for_each(i, &usbd_class_head)
    {
        usbd_class_t *class = usb_slist_entry(i, struct usbd_class, list);

        usb_slist_for_each(j, &class->intf_list)
        {
            usbd_interface_t *intf = usb_slist_entry(j, struct usbd_interface, list);

            usb_slist_for_each(k, &intf->ep_list)
            {
                usbd_endpoint_t *ept = usb_slist_entry(k, struct usbd_endpoint, list);

                if ((ept->ep_addr == ep) && ept->ep_cb) {
                    ept->ep_cb(ep);
                }
            }
        }
    }
#else

    if (usbd_core_cfg.out_ep_cb[ep & 0x7f]) {
        usbd_core_cfg.out_ep_cb[ep & 0x7f](ep);
    }

#endif
}

static void usbd_ep_in_handler(uint8_t ep)
{
#if USBD_EP_CALLBACK_SEARCH_METHOD == 0
    usb_slist_t *i, *j, *k;
    usb_slist_for_each(i, &usbd_class_head)
    {
        usbd_class_t *class = usb_slist_entry(i, struct usbd_class, list);

        usb_slist_for_each(j, &class->intf_list)
        {
            usbd_interface_t *intf = usb_slist_entry(j, struct usbd_interface, list);

            usb_slist_for_each(k, &intf->ep_list)
            {
                usbd_endpoint_t *ept = usb_slist_entry(k, struct usbd_endpoint, list);

                if ((ept->ep_addr == ep) && ept->ep_cb) {
                    ept->ep_cb(ep);
                }
            }
        }
    }
#else

    if (usbd_core_cfg.in_ep_cb[ep & 0x7f]) {
        usbd_core_cfg.in_ep_cb[ep & 0x7f](ep);
    }

#endif
}

static void usbd_class_event_notify_handler(uint8_t event, void *arg)
{
    usb_slist_t *i, *j;
    usb_slist_for_each(i, &usbd_class_head)
    {
        usbd_class_t *class = usb_slist_entry(i, struct usbd_class, list);

        usb_slist_for_each(j, &class->intf_list)
        {
            usbd_interface_t *intf = usb_slist_entry(j, struct usbd_interface, list);

            if (intf->notify_handler) {
                intf->notify_handler(event, arg);
            }
        }
    }
}

void usbd_event_notify_handler(uint8_t event, void *arg)
{
    switch (event) {
        case USB_EVENT_RESET:
            usbd_set_address(0);
#if USBD_EP_CALLBACK_SEARCH_METHOD == 1
            usbd_ep_callback_register();
#endif

        case USB_EVENT_ERROR:
        case USB_EVENT_SOF:
        case USB_EVENT_CONNECTED:
        case USB_EVENT_CONFIGURED:
        case USB_EVENT_SUSPEND:
        case USB_EVENT_DISCONNECTED:
        case USB_EVENT_RESUME:
        case USB_EVENT_SET_INTERFACE:
        case USB_EVENT_SET_REMOTE_WAKEUP:
        case USB_EVENT_CLEAR_REMOTE_WAKEUP:
        case USB_EVENT_SET_HALT:
        case USB_EVENT_CLEAR_HALT:
            usbd_class_event_notify_handler(event, arg);
            break;

        case USB_EVENT_SETUP_NOTIFY:
            usbd_ep0_setup_handler();
            break;

        case USB_EVENT_EP0_IN_NOTIFY:
            usbd_ep0_in_handler();
            break;

        case USB_EVENT_EP0_OUT_NOTIFY:
            usbd_ep0_out_handler();
            break;

        case USB_EVENT_EP_IN_NOTIFY:
            usbd_ep_in_handler((uint32_t)arg);
            break;

        case USB_EVENT_EP_OUT_NOTIFY:
            usbd_ep_out_handler((uint32_t)arg);
            break;

        default:
            USBD_LOG_ERR("USB unknown event: %d\r\n", event);
            break;
    }
}

void usbd_desc_register(const uint8_t *desc)
{
    usbd_core_cfg.descriptors = desc;
}
/* Register MS OS Descriptors version 1 */
void usbd_msosv1_desc_register(struct usb_msosv1_descriptor *desc)
{
    msosv1_desc = desc;
}

/* Register MS OS Descriptors version 2 */
void usbd_msosv2_desc_register(struct usb_msosv2_descriptor *desc)
{
    msosv2_desc = desc;
}

void usbd_bos_desc_register(struct usb_bos_descriptor *desc)
{
    bos_desc = desc;
}

void usbd_class_register(usbd_class_t *class)
{
    usb_slist_add_tail(&usbd_class_head, &class->list);
    usb_slist_init(&class->intf_list);
}

void usbd_class_add_interface(usbd_class_t *class, usbd_interface_t *intf)
{
    static uint8_t intf_offset = 0;
    intf->intf_num = intf_offset;
    usb_slist_add_tail(&class->intf_list, &intf->list);
    usb_slist_init(&intf->ep_list);
    intf_offset++;
}

void usbd_interface_add_endpoint(usbd_interface_t *intf, usbd_endpoint_t *ep)
{
    usb_slist_add_tail(&intf->ep_list, &ep->list);
}

bool usb_device_is_configured(void)
{
    return usbd_core_cfg.configured;
}
