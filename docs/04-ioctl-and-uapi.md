# 04 - IOCTL & UAPI Documentation

## IOCTL Commands

Magic byte: `'v'` (0x76)

| Command | Direction | Type | Description |
|---|---|---|---|
| `VS_IOC_START` | None | `_IO` | Starts periodic sample generation |
| `VS_IOC_STOP` | None | `_IO` | Stops periodic sample generation |
| `VS_IOC_SET_MODE` | Write | `_IOW` | Changes device mode (`enum vs_mode`) |
| `VS_IOC_SET_RATE` | Write | `_IOW` | Sets sample rate in Hz (1 - 1000) |
| `VS_IOC_GET_STATUS` | Read | `_IOR` | Returns current `struct vs_status` |
| `VS_IOC_GET_SAMPLE` | Read | `_IOR` | Peeks latest `struct vs_sample` without pop |
| `VS_IOC_RESET` | None | `_IO` | Resets sequence, counters, and buffer |
| `VS_IOC_INJECT_FAULT` | Write | `_IOW` | Inject error bitmask and switch to ERROR mode |
