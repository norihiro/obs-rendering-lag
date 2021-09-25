# OBS Rendering Lag Plugin

## Introduction

This plugin is intentionally add lags during video rendering.

## Properties

- Rate:
  This property specifies how often the lag is injected.
- Sleep time:
  This property specifies how long the lag is injected for each.
  If the sleep time is same as your frame time (inverse of the frame rate), OBS Studio tries to avoid rendering lag.
  It is recommended to set around 1.5x of your frame time.
