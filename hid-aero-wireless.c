// SPDX-License-Identifier: GPL-3.0-or-later
/*
 *  HID driver for White Shark Aero Black
 *
 *  Copyright (c) 2024 Moj 
 */

/*
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/hid.h>

#include "hid-ids.h"

/* Fixed Aero Wireless Mouse report descriptor */
/* Button's Usage Maximum changed from 3 to 5 to make side buttons work */
#define AERO_WIRELESS_MOUSE_ORIG_RDESC_LENGTH 149
#define AERO_WIRED_MOUSE_ORIG_RDESC_LENGTH 192
#define AERO_FAULT_OFFSET 17

static const __u8 aero_wireless_faulty_rdesc[] = {
0x05, 0x01,                    // Usage Page (Generic Desktop)        0
0x09, 0x02,                    // Usage (Mouse)                       2
0xa1, 0x01,                    // Collection (Application)            4
0x85, 0x01,                    //  Report ID (1)                      6
0x09, 0x01,                    //  Usage (Pointer)                    8
0xa1, 0x00,                    //  Collection (Physical)              10
0x05, 0x09,                    //   Usage Page (Button)               12
0x19, 0x01,                    //   Usage Minimum (1)                 14
0x29, 0x03,                    //   Usage Maximum (3)                 16
0x15, 0x00,                    //   Logical Minimum (0)               18
0x25, 0x01,                    //   Logical Maximum (1)               20
0x75, 0x01,                    //   Report Size (1)                   22
0x95, 0x05,                    //   Report Count (5)                  24
0x81, 0x02,                    //   Input (Data,Var,Abs)              26
0x75, 0x03,                    //   Report Size (3)                   28
0x95, 0x01,                    //   Report Count (1)                  30
0x81, 0x01,                    //   Input (Cnst,Arr,Abs)              32
0x05, 0x01,                    //   Usage Page (Generic Desktop)      34
0x09, 0x30,                    //   Usage (X)                         36
0x09, 0x31,                    //   Usage (Y)                         38
0x16, 0x01, 0x80,              //   Logical Minimum (-32767)          40
0x26, 0xff, 0x7f,              //   Logical Maximum (32767)           43
0x75, 0x10,                    //   Report Size (16)                  46
0x95, 0x02,                    //   Report Count (2)                  48
0x81, 0x06,                    //   Input (Data,Var,Rel)              50
0x09, 0x38,                    //   Usage (Wheel)                     52
0x15, 0x81,                    //   Logical Minimum (-127)            54
0x25, 0x7f,                    //   Logical Maximum (127)             56
0x75, 0x08,                    //   Report Size (8)                   58
0x95, 0x01,                    //   Report Count (1)                  60
0x81, 0x06,                    //   Input (Data,Var,Rel)              62
0xc0,                          //  End Collection                     64
0xc0,                          // End Collection                      65
0x05, 0x0c,                    // Usage Page (Consumer Devices)       66
0x09, 0x01,                    // Usage (Consumer Control)            68
0xa1, 0x01,                    // Collection (Application)            70
0x85, 0x02,                    //  Report ID (2)                      72
0x75, 0x10,                    //  Report Size (16)                   74
0x95, 0x01,                    //  Report Count (1)                   76
0x15, 0x01,                    //  Logical Minimum (1)                78
0x26, 0x8c, 0x02,              //  Logical Maximum (652)              80
0x19, 0x01,                    //  Usage Minimum (1)                  83
0x2a, 0x8c, 0x02,              //  Usage Maximum (652)                85
0x81, 0x00,                    //  Input (Data,Arr,Abs)               88
0xc0,                          // End Collection                      90
0x05, 0x01,                    // Usage Page (Generic Desktop)        91
0x09, 0x80,                    // Usage (System Control)              93
0xa1, 0x01,                    // Collection (Application)            95
0x85, 0x03,                    //  Report ID (3)                      97
0x09, 0x82,                    //  Usage (System Sleep)               99
0x09, 0x81,                    //  Usage (System Power Down)          101
0x09, 0x83,                    //  Usage (System Wake Up)             103
0x15, 0x00,                    //  Logical Minimum (0)                105
0x25, 0x01,                    //  Logical Maximum (1)                107
0x19, 0x01,                    //  Usage Minimum (1)                  109
0x29, 0x03,                    //  Usage Maximum (3)                  111
0x75, 0x01,                    //  Report Size (1)                    113
0x95, 0x03,                    //  Report Count (3)                   115
0x81, 0x02,                    //  Input (Data,Var,Abs)               117
0x95, 0x05,                    //  Report Count (5)                   119
0x81, 0x01,                    //  Input (Cnst,Arr,Abs)               121
0xc0,                          // End Collection                      123
0x05, 0x01,                    // Usage Page (Generic Desktop)        124
0x09, 0x00,                    // Usage (Undefined)                   126
0xa1, 0x01,                    // Collection (Application)            128
0x85, 0x05,                    //  Report ID (5)                      130
0x15, 0x00,                    //  Logical Minimum (0)                132
0x26, 0xff, 0x00,              //  Logical Maximum (255)              134
0x95, 0x20,                    //  Report Count (32)                  137
0x75, 0x08,                    //  Report Size (8)                    139
0x09, 0x01,                    //  Usage (Pointer)                    141
0x81, 0x02,                    //  Input (Data,Var,Abs)               143
0x09, 0x02,                    //  Usage (Mouse)                      145
0x91, 0x02,                    //  Output (Data,Var,Abs)              147
0xc0,                          // End Collection                      149
};

static const __u8 aero_wired_faulty_rdesc[] = {
0x05, 0x01,                    // Usage Page (Generic Desktop)        0
0x09, 0x02,                    // Usage (Mouse)                       2
0xa1, 0x01,                    // Collection (Application)            4
0x85, 0x01,                    //  Report ID (1)                      6
0x09, 0x01,                    //  Usage (Pointer)                    8
0xa1, 0x00,                    //  Collection (Physical)              10
0x05, 0x09,                    //   Usage Page (Button)               12
0x19, 0x01,                    //   Usage Minimum (1)                 14
0x29, 0x03,                    //   Usage Maximum (3)                 16
0x15, 0x00,                    //   Logical Minimum (0)               18
0x25, 0x01,                    //   Logical Maximum (1)               20
0x75, 0x01,                    //   Report Size (1)                   22
0x95, 0x05,                    //   Report Count (5)                  24
0x81, 0x02,                    //   Input (Data,Var,Abs)              26
0x75, 0x03,                    //   Report Size (3)                   28
0x95, 0x01,                    //   Report Count (1)                  30
0x81, 0x01,                    //   Input (Cnst,Arr,Abs)              32
0x05, 0x01,                    //   Usage Page (Generic Desktop)      34
0x09, 0x30,                    //   Usage (X)                         36
0x09, 0x31,                    //   Usage (Y)                         38
0x16, 0x01, 0x80,              //   Logical Minimum (-32767)          40
0x26, 0xff, 0x7f,              //   Logical Maximum (32767)           43
0x75, 0x10,                    //   Report Size (16)                  46
0x95, 0x02,                    //   Report Count (2)                  48
0x81, 0x06,                    //   Input (Data,Var,Rel)              50
0x09, 0x38,                    //   Usage (Wheel)                     52
0x15, 0x81,                    //   Logical Minimum (-127)            54
0x25, 0x7f,                    //   Logical Maximum (127)             56
0x75, 0x08,                    //   Report Size (8)                   58
0x95, 0x01,                    //   Report Count (1)                  60
0x81, 0x06,                    //   Input (Data,Var,Rel)              62
0xc0,                          //  End Collection                     64
0xc0,                          // End Collection                      65
0x05, 0x0c,                    // Usage Page (Consumer Devices)       66
0x09, 0x01,                    // Usage (Consumer Control)            68
0xa1, 0x01,                    // Collection (Application)            70
0x85, 0x02,                    //  Report ID (2)                      72
0x75, 0x10,                    //  Report Size (16)                   74
0x95, 0x01,                    //  Report Count (1)                   76
0x15, 0x01,                    //  Logical Minimum (1)                78
0x26, 0x8c, 0x02,              //  Logical Maximum (652)              80
0x19, 0x01,                    //  Usage Minimum (1)                  83
0x2a, 0x8c, 0x02,              //  Usage Maximum (652)                85
0x81, 0x00,                    //  Input (Data,Arr,Abs)               88
0xc0,                          // End Collection                      90
0x05, 0x01,                    // Usage Page (Generic Desktop)        91
0x09, 0x80,                    // Usage (System Control)              93
0xa1, 0x01,                    // Collection (Application)            95
0x85, 0x03,                    //  Report ID (3)                      97
0x09, 0x82,                    //  Usage (System Sleep)               99
0x09, 0x81,                    //  Usage (System Power Down)          101
0x09, 0x83,                    //  Usage (System Wake Up)             103
0x15, 0x00,                    //  Logical Minimum (0)                105
0x25, 0x01,                    //  Logical Maximum (1)                107
0x19, 0x01,                    //  Usage Minimum (1)                  109
0x29, 0x03,                    //  Usage Maximum (3)                  111
0x75, 0x01,                    //  Report Size (1)                    113
0x95, 0x03,                    //  Report Count (3)                   115
0x81, 0x02,                    //  Input (Data,Var,Abs)               117
0x95, 0x05,                    //  Report Count (5)                   119
0x81, 0x01,                    //  Input (Cnst,Arr,Abs)               121
0xc0,                          // End Collection                      123
0x05, 0x01,                    // Usage Page (Generic Desktop)        124
0x09, 0x00,                    // Usage (Undefined)                   126
0xa1, 0x01,                    // Collection (Application)            128
0x85, 0x05,                    //  Report ID (5)                      130
0x15, 0x00,                    //  Logical Minimum (0)                132
0x26, 0xff, 0x00,              //  Logical Maximum (255)              134
0x95, 0x20,                    //  Report Count (32)                  137
0x75, 0x08,                    //  Report Size (8)                    139
0x09, 0x01,                    //  Usage (Pointer)                    141
0x81, 0x02,                    //  Input (Data,Var,Abs)               143
0x09, 0x02,                    //  Usage (Mouse)                      145
0x91, 0x02,                    //  Output (Data,Var,Abs)              147
0xc0,                          // End Collection                      149
0x05, 0x01,                    // Usage Page (Generic Desktop)        150
0x09, 0x06,                    // Usage (Keyboard)                    152
0xa1, 0x01,                    // Collection (Application)            154
0x85, 0x06,                    //  Report ID (6)                      156
0x05, 0x07,                    //  Usage Page (Keyboard)              158
0x19, 0xe0,                    //  Usage Minimum (224)                160
0x29, 0xe7,                    //  Usage Maximum (231)                162
0x15, 0x00,                    //  Logical Minimum (0)                164
0x25, 0x01,                    //  Logical Maximum (1)                166
0x75, 0x01,                    //  Report Size (1)                    168
0x95, 0x08,                    //  Report Count (8)                   170
0x81, 0x02,                    //  Input (Data,Var,Abs)               172
0x95, 0x06,                    //  Report Count (6)                   174
0x75, 0x08,                    //  Report Size (8)                    176
0x15, 0x00,                    //  Logical Minimum (0)                178
0x26, 0xff, 0x00,              //  Logical Maximum (255)              180
0x05, 0x07,                    //  Usage Page (Keyboard)              183
0x19, 0x00,                    //  Usage Minimum (0)                  185
0x2a, 0xff, 0x00,              //  Usage Maximum (255)                187
0x81, 0x00,                    //  Input (Data,Arr,Abs)               190
0xc0,                          // End Collection                      192
};


/*
 * we fix the report descriptor by setting byte 17 to 5
 */

static const __u8 *aero_report_fixup(struct hid_device *hdev, __u8 *rdesc,
				 unsigned int *rsize)
{
	switch (hdev->product) {
	case USB_DEVICE_ID_AERO_WIRELESS_MOUSE_C:
		if (*rsize == AERO_WIRED_MOUSE_ORIG_RDESC_LENGTH) {
			hid_info(hdev, "fixing up Aero Mouse report descriptor\n");
			rdesc = aero_wired_faulty_rdesc;
			*rsize = sizeof(aero_wired_faulty_rdesc);
		}
		break;
	case USB_DEVICE_ID_AERO_WIRELESS_MOUSE_W:
		if (*rsize == AERO_WIRELESS_MOUSE_ORIG_RDESC_LENGTH) {
			hid_info(hdev, "fixing up Aero Mouse report descriptor\n");
			rdesc = aero_wireless_faulty_rdesc;
			*rsize = sizeof(aero_wireless_faulty_rdesc);
		}
		break;
	}
	if(memcmp(rdesc, aero_wireless_faulty_rdesc, AERO_FAULT_OFFSET) != 0) return rdesc;
    rdesc[AERO_FAULT_OFFSET] = 0x05;
	return rdesc;
}

static const struct hid_device_id aero_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_AERO_WIRELESS,
            USB_DEVICE_ID_AERO_WIRELESS_MOUSE_C) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_AERO_WIRELESS,
			USB_DEVICE_ID_AERO_WIRELESS_MOUSE_W) },
	{ }
};

MODULE_DEVICE_TABLE(hid, aero_devices);

static struct hid_driver aero_driver = {
	.name = "white_shark_aero",
	.id_table = aero_devices,
	.report_fixup = aero_report_fixup,
};
module_hid_driver(aero_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cviki@gmail.com");
MODULE_DESCRIPTION("Fixing side buttons of White Shark Aero Wireless Mouse");