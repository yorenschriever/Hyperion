#pragma once

#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
#define ETH_PHY_POWER 12

#include <Arduino.h>
#include <ETH.h>

void NetworkBegin();
boolean networkIsConnected();
