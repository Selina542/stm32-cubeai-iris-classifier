/**
  ******************************************************************************
  * @file    network_data_params.c
  * @author  AST Embedded Analytics Research Platform
  * @date    2026-06-12T22:29:31+0800
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#include "network_data_params.h"


/**  Activations Section  ****************************************************/
ai_handle g_network_activations_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(NULL),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};




/**  Weights Section  ********************************************************/
AI_ALIGNED(32)
const ai_u64 s_network_weights_array_u64[70] = {
  0x3f8af1073f220173U, 0xbd657cfcbfb0c24cU, 0xbe9a1d4b3fa4d5d9U, 0xbebce275bf756339U,
  0xbeb490a3bd03661dU, 0x3db72bd23e69f472U, 0x3f6903fd40090035U, 0x3f38852a3ef3b8bbU,
  0x3fa55775bf94b5e0U, 0xbf6efe28bfcbee5aU, 0xbf1b8e3c3eca36b3U, 0x3e4588833f8753abU,
  0x3e13b71e3f0c0c7aU, 0xbf1d83bb3ed1abb3U, 0xbf18dae5be313a34U, 0x3fba27e93ed9e7ebU,
  0xbbb562d83f946ca8U, 0xbedc33793f7aabaaU, 0xbe6e85103e10a8a8U, 0xbf0bd0c33f84e90fU,
  0xbe7252b13fd48a07U, 0x3edd84943e039b20U, 0xbf1923223f82ea77U, 0xbdb13de4be1d343aU,
  0xbf908620bfbb6ba4U, 0x3ff0952f3e996871U, 0x3f88e1c83dbb90e6U, 0x3fc0be6bbef3fc36U,
  0xbe590e063e45c615U, 0xbe0edbd5bf04938aU, 0xbdebf773bedb2448U, 0xbda896cb3e46aa7dU,
  0xc0140d18bee3c88aU, 0x3f87e8283e108e18U, 0x3f6a9197bf8f162fU, 0x3f35d0783e83c1a9U,
  0x4004aaec3f856bfdU, 0xbf8e6ab73f33b2cfU, 0x3d243cc5bfaddec8U, 0xbf178cc53f68868fU,
  0x3fd6c0853eac757dU, 0xbf3547793f8cc159U, 0x3f4433cdbf5dcb68U, 0xbee3dacc3f2928c3U,
  0x402baa9740132289U, 0xbfcf73653efec3c1U, 0xbf14bc13c005ed7cU, 0xbf93813f3f9f4dfdU,
  0xbe17cddbbdf3ead2U, 0xbffa20b03f45fe44U, 0xbeb0663ebe62bb46U, 0xbf280f303f866ef7U,
  0xbf24ffbe3e9e5b6cU, 0xbe382a4ebdb90062U, 0x3f2e6ad13f25e64eU, 0x3d838adc3f9ab497U,
  0xbec825cd3f938e69U, 0xbf8da0acbf42e09bU, 0xbf7a582dbf428b07U, 0x3ed3379fbf6eea4cU,
  0xbfca0f66bf1bff32U, 0xbf2c58c1be59a2abU, 0x3eb1127f3fa678e5U, 0x3f61840d3ff5daf4U,
  0x3fd61904bf5a3b94U, 0x3f482955bef7f09aU, 0xbd2c001bbe866fc0U, 0xc001e9b1bffe6c14U,
  0x3eddbecd3e0cb62fU, 0xbf00a80fU,
};


ai_handle g_network_weights_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(s_network_weights_array_u64),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};

